module.exports = http_login;

function http_login(env){
    var self = this;

    self.http_hostname = '10.0.1.27';
    self.http_port = 80;
    self.from_where = 2000;
    if (env == 'test'){
        self.http_hostname = '10.0.1.23';
    }else if(env == 'release'){
        self.http_hostname = 'www.999com.com';
    }

    self.u = require('./utility');

    self.login_done = false;
    self.user_info = {};
    self.room_info = [];
    self.upload_key_done = false;
    self.get_sng_room_info_done = false;
    self.get_normal_room_info_done = false;
    self.get_user_info_done = false;

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
        return self.u.md5(to_sign);
    }

    self.req_ok = function(doing, response){
        var resp_obj;
        try{
            resp_obj = JSON.parse(response);
        }catch(e){
            console.log('exception: ' + e);
            return false;
        }
        var retcode = (resp_obj.retcode===undefined? resp_obj.retCode:resp_obj.retcode);
        if (retcode != 0){
            console.log('error response: ' + response);
            return false;
        }
        if (doing == 1){//login
            //console.log('--------------',resp_obj.game_money);
            self.user_info = resp_obj;
            self.user_info.from_where = self.from_where;
            if (resp_obj.retcode!=undefined){
                delete resp_obj.retcode;
            }
            if (resp_obj.retCode!=undefined){
                delete resp_obj.retCode;
            }
            self.login_done = true;
            self.upload_key();
        }else if(doing == 2){
            self.user_info.md5key = self.rsa_decipher(resp_obj.md5Key);
            self.upload_key_done = true;
            self.get_room_info();
            self.get_user_info();
        }else if(doing == 3||doing == 4||doing == 5){
            if (doing == 3){
                self.get_sng_room_info_done = true;
                resp_obj.data.forEach(function(item, index, arr){
                    item.match = 'sng';
                    self.room_info.push(item);
                });
            }
            if (doing == 4){
                var k1,k2;
                for (k1 in resp_obj.data){
                    for (k2 in resp_obj.data[k1]){
                        resp_obj.data[k1][k2].forEach(function(item, index, arr){
                            item.match = 'normal';
                            item.level = k1;
                            item.desk_player_num = k2;
                            self.room_info.push(item);
                        });
                    }
                }
                self.get_normal_room_info_done = true;
            }
            if (doing == 5){
                self.user_info.game_money = resp_obj.game_money;
                self.get_user_info_done = true;
            }
            if (self.get_user_info_done
                    && self.get_normal_room_info_done
                    && self.get_sng_room_info_done){
                //console.log(JSON.stringify(self.user_info,null,2));
                //console.log(JSON.stringify(self.room_info,null,2));
                self.login_success(self.user_info, self.room_info);
                //console.log('http all done');
            }
        }else{
            console.log(doing, response);
            throw 'unknown doing';
        }
    }

    self.comm_request = function(req, params, doing, success){
        var http = require('http');
        var qs = require('querystring');
        http.request(req, function (res) {
            var body = '';
            res.setEncoding('utf8');
            res.on('data', function (chunk){
                body += chunk;
            });
            res.on('end', function () {
                success(doing, body);
            });
        }).on('error', function(e) {
            console.log('http error: ' + e.message);
        }).end(qs.stringify(params));
    }

    self.login = function(name, pass, vcode, success){
        self.login_success = success;
        self.do_login(name, pass, vcode);
    }

    self.do_login = function(name, pass, vcode){
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
        self.comm_request(req, params, 1, self.req_ok);
    }

    self.upload_key = function(){
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
            uid: self.user_info.uid,
            session_id: self.user_info.session_id,
            pubKey: pub_key
        }
        self.comm_request(req, params, 2, self.req_ok);
    }

    self.get_room_info = function(){
        var sng_route = 'lobby/game/getsngroomlist';
        var normal_route = 'lobby/game/getallroom';

        var req={
            hostname: self.http_hostname,
            port: self.http_port,
            //path: '/index.php?r=' + route,
            method: 'POST',
            headers:{'Content-Type': 'application/x-www-form-urlencoded'}
        }

        var params = {
            lua_version : '1.0.0.0',
            session_id : self.user_info.session_id,
            uid : self.user_info.uid,
            version: '1',
            from_where : self.from_where,
            //r: route,
        };

        params.r = sng_route;
        req.path='/index.php?r=' + sng_route;
        if (params.urlsign != undefined){
            delete params.urlsign;
        }
        params.urlsign = self.make_urlsign(params, self.user_info.md5key);
        self.comm_request(req, params, 3, self.req_ok);

        params.r = normal_route;
        req.path='/index.php?r=' + normal_route;
        if (params.urlsign != undefined){
            delete params.urlsign;
        }
        params.urlsign = self.make_urlsign(params, self.user_info.md5key);
        self.comm_request(req, params, 4, self.req_ok);
    }

    self.get_user_info = function(){
        self.get_user_info_done = true;
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
            session_id : self.user_info.session_id,
            uid : self.user_info.uid,
            version: '1',
            from_where : self.from_where,
            r: route,
            c: 'userinfo',
            op: 'info',
        };
        params.urlsign = self.make_urlsign(params, self.user_info.md5key);
        self.comm_request(req, params, 5, self.req_ok);
    }
}

