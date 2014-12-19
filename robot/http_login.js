module.exports = http_login;

function http_login(env){
    var self = this;

    //self.setting = setting;
    self.http_hostname = '10.0.1.27';
    self.http_port = 80;
    self.from_where = 2000;
    if (env == 'test'){
        self.http_hostname = '10.0.1.23';
    }else if(env == 'release'){
        self.http_hostname = 'www.999com.com';
    }

    self.u = require('./utility');

    //self.user_info = {};
    //self.room_info = [];

    self.pub_file = './var/rsa_public_key.pem';
    self.pri_file = './var/rsa_private_key.pem';

    self.rsa_cipher = function(plain_text){//base64 input
        var fs = require('fs');

        var ursa = require('ursa');
        var pem = fs.readFileSync(self.pub_file);
        var key = ursa.createPublicKey(pem);
        var benc = new Buffer(plain_text, 'base64');
        return key.encrypt(benc).toString();
    }

    self.rsa_decipher = function(encrypted){//base64 input
        var fs = require('fs');

        var ursa = require('ursa');
        var pem = fs.readFileSync(self.pri_file);
        var key = ursa.createPrivateKey(pem);
        var benc = new Buffer(encrypted, 'base64');
        return key.decrypt(benc).toString();
    }

    self.make_urlsign = function(params, md5key){
        var sparams = self.u.sortObject(params);
        var k, to_sign='';
        for (k in sparams){
            to_sign += k + sparams[k];
        }
        to_sign += md5key;
        //self.u.yylog(__FILE__, __LINE__, to_sign);
        return self.u.md5(to_sign);
    }

    self.req_ok = function(doing, response, user_info){
        var resp_obj;
        try{
            resp_obj = JSON.parse(response);
        }catch(e){
            self.u.yylog(__FILE__, __LINE__, 'exception: ' + e + response);
            return false;
        }
        var retcode = (resp_obj.retcode===undefined? resp_obj.retCode:resp_obj.retcode);
        if (retcode != 0){
            self.u.yylog(__FILE__, __LINE__, 'error response: ' + response);
            return false;
        }
        if (doing == 1){//login
            user_info = resp_obj;
            user_info.from_where = self.from_where;
            //self.user_info = resp_obj;
            //self.user_info.from_where = self.from_where;
            if (resp_obj.retcode!=undefined){
                delete resp_obj.retcode;
            }
            if (resp_obj.retCode!=undefined){
                delete resp_obj.retCode;
            }
            //if (!self.do_upload && self.type == 'login'){
            //    self.login_success(self.user_info);
            //}else{
            self.upload_key(user_info);
            //}
        }else if(doing == 2){
            user_info.md5key = self.rsa_decipher(resp_obj.md5Key);

            if (self.type == 'login'){
                self.login_success(user_info);
            }else if (self.type == 'room'){
                self.get_room_info(self.game, user_info);
            }else{
                self.apply_match(user_info);
            }
        }else if(doing == 3){
            var room_info = [];
            if (self.game == 'sng'){
                resp_obj.data.forEach(function(item, index, arr){
                    item.match = 'sng';
                    room_info.push(item);
                });
            }else{
                var k1,k2;
                for (k1 in resp_obj.data){
                    for (k2 in resp_obj.data[k1]){
                        resp_obj.data[k1][k2].forEach(function(item, index, arr){
                            item.match = 'normal';
                            item.level = k1;
                            item.desk_player_num = k2;
                            room_info.push(item);
                        });
                    }
                }
            }
            self.room_success(room_info);
        }else if(doing == 6){
            self.u.yylog(__FILE__, __LINE__, doing, response);
            setTimeout(self.query_match, 1*1000, user_info);
        }else if(doing == 7){
            self.u.yylog(__FILE__, __LINE__, doing, response);
            self.apply_success(user_info);
        }else{
            self.u.yylog(__FILE__, __LINE__, doing, response);
            throw 'unknown doing';
        }
    }

    self.comm_request = function(req, params, doing, success, user_info){
        var http = require('http');
        var qs = require('querystring');
        http.request(req, function (res) {
            var body = '';
            res.setEncoding('utf8');
            res.on('data', function (chunk){
                body += chunk;
            });
            res.on('end', function () {
                success(doing, body, user_info);
            });
        }).on('error', function(e) {
            self.u.yylog(__FILE__, __LINE__, 'http error: ' + e.message);
        }).end(qs.stringify(params));
    }

    self.get_game_room = function(game, name, pass, vcode, success){
        self.type = 'room';
        self.game = game;
        self.room_success = success;
        self.do_login(name, pass, vcode);
    }

    //self.do_upload = false;
    self.login = function(name, pass, vcode, success){
        //self.do_upload = upload_key;
        self.type = 'login';
        self.login_success = success;
        self.do_login(name, pass, vcode);
    }

    self.apply = function(name, pass, vcode, match_id, room_id, success){
        self.type = 'apply';
        //self.u.yylog(__FILE__, __LINE__, name, pass, vcode, match_id, room_id);
        self.match_id = match_id;
        self.room_id = room_id;
        self.apply_success = success;
        self.do_login(name, pass, vcode);
    }

    self.do_login = function(name, pass, vcode){
        //self.u.yylog(__FILE__, __LINE__, name, pass, vcode);
        if (vcode == undefined){
            vcode = '';
        }
        var req = {
            hostname: self.http_hostname,
            port: self.http_port,
            path: '/index.php?r=user/index',
            method: 'POST',
            headers:{'Content-Type': 'application/x-www-form-urlencoded'}
        }
        var params = {
            c : 'login',
            from_where : self.from_where,
            loginname : name,
            password : pass,
            verifycode: vcode,
            login_mode : 1
        }
        var user_info = {};
        self.comm_request(req, params, 1, self.req_ok, user_info);
    }

    self.upload_key = function(user_info){
        var fs = require('fs');
        var pub_key = fs.readFileSync(self.pub_file).toString();
        var req={
            hostname: self.http_hostname,
            port: self.http_port,
            path: '/index.php?r=user/rsa/pubkey',
            method: 'POST',
            headers:{'Content-Type': 'application/x-www-form-urlencoded'}
        }
        var params = {
            from_where : self.from_where,
            uid: user_info.uid,
            session_id: user_info.session_id,
            pubKey: pub_key
        }
        self.comm_request(req, params, 2, self.req_ok, user_info);
    }

    self.get_room_info = function(game, user_info){
        var sng_route = 'lobby/game/getsngroomlist';
        var normal_route = 'lobby/game/getallroom';

        var req={
            hostname: self.http_hostname,
            port: self.http_port,
            method: 'POST',
            headers:{'Content-Type': 'application/x-www-form-urlencoded'}
        }

        var params = {
            lua_version : '1.0.0.0',
            session_id : user_info.session_id,
            uid : user_info.uid,
            version: '1',
            from_where : self.from_where,
        };

        if (game == 'sng'){
            params.r = sng_route;
            req.path='/index.php?r=' + sng_route;
            if (params.urlsign != undefined){
                delete params.urlsign;
            }
        }else{
            params.r = normal_route;
            req.path='/index.php?r=' + normal_route;
            if (params.urlsign != undefined){
                delete params.urlsign;
            }
        }
        params.urlsign = self.make_urlsign(params, user_info.md5key);
        self.comm_request(req, params, 3, self.req_ok, user_info);
    }

    //currently useless
    self.get_user_info = function(user_info){
        return true;
        var route = 'user/index';
        var req={
            hostname: self.http_hostname,
            port: self.http_port,
            path: '/index.php?r=' + route,
            method: 'POST',
            headers:{'Content-Type': 'application/x-www-form-urlencoded'}
        }

        var params = {
            lua_version : '1.0.0.0',
            session_id : user_info.session_id,
            uid : user_info.uid,
            version: '1',
            from_where : self.from_where,
            r: route,
            c: 'userinfo',
            op: 'info',
        };
        params.urlsign = self.make_urlsign(params, user_info.md5key);
        self.comm_request(req, params, 5, self.req_ok, user_info);
    }

    self.apply_match = function(user_info){
        var route = 'tournament/application/apply';
        var req={
            hostname: self.http_hostname,
            port: self.http_port,
            path: '/index.php?r=' + route,
            method: 'POST',
            headers:{'Content-Type': 'application/x-www-form-urlencoded'}
        }

        var params = {
            lua_version : '1.0.0.0',
            session_id : user_info.session_id,
            uid : user_info.uid,
            version: '1',
            from_where : self.from_where,
            room_id:self.room_id,
            r:route
        };
        params.urlsign = self.make_urlsign(params, user_info.md5key);
        self.comm_request(req, params, 6, self.req_ok, user_info);
    }

    self.query_match = function(user_info){
        var route = 'tournament/application/getApplyState';
        var req={
            hostname: self.http_hostname,
            port: self.http_port,
            path: '/index.php?r=' + route,
            method: 'POST',
            headers:{'Content-Type': 'application/x-www-form-urlencoded'}
        }

        var params = {
            lua_version : '1.0.0.0',
            session_id : user_info.session_id,
            version: '1',
            uid : user_info.uid,
            from_where : self.from_where,
            room_id:self.room_id,
            r:route
        };
        params.urlsign = self.make_urlsign(params, user_info.md5key);
        self.comm_request(req, params, 7, self.req_ok, user_info);
    }
}

