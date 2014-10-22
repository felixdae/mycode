function http_login(name, pass, vcode) {
    if (vcode === undefined){
        vcode = '';
    }
    var http = require('http');
    var qs = require('querystring');

    http.request({
        //hostname: 'www.999com.com',
        hostname: '10.0.1.27',
        port: 80,
        path: '/index.php?r=user/index',
        method: 'POST',
        headers:{'Content-Type': 'application/x-www-form-urlencoded'}
    }, function (res) {
        var body = '';
        res.setEncoding('utf8');
        res.on('data', function (chunk){
            body += chunk;
        });
        res.on('end', function () {
            var uo = JSON.parse(body);
            var retcode = uo.retcode;
            if (retcode != 0){
                console.log("error: " + body);
            } else {
                //console.log(uo);
                upload_public_key(uo);
            }
        });
    }).on('error', function(e) {
        console.log('error: ' + e.message);
    }).end(qs.stringify({
        c : 'login',
        from_where : 2000,
        loginname : name,
        password : pass,
        verifycode: vcode,
        login_mode : 1
    }));
}

function upload_public_key(user_info){
    //console.log('-----------------------');
    var uid = user_info.uid;
    var session_id = user_info.session_id;
    var pri_file = 'var/rsa_private_key.pem';
    var pub_file = 'var/rsa_public_key.pem';
    fs = require('fs');
    fs.readFile(pub_file, 'utf8', function (err,data) {
        if (err) {
            return console.log(err);
        }
        var pub_key = data;
        var http = require('http');
        var qs = require('querystring');

        http.request({
            //hostname: 'www.999com.com',
            hostname: '10.0.1.27',
            port: 80,
            path: '/index.php?r=user/rsa/pubkey',
            method: 'POST',
            headers:{'Content-Type': 'application/x-www-form-urlencoded'}
        }, function (res) {
            var body = '';
            res.setEncoding('utf8');
            res.on('data', function (chunk){
                body += chunk;
            });
            res.on('end', function () {
                var res = JSON.parse(body);
                var retcode = res.retCode;
                if (retcode != 0){
                    console.log("error: " + body);
                } else {
                    user_info.md5Key = res.md5Key;
                    conn_ws(user_info);
                }
            });
        }).on('error', function(e) {
            console.log('error: ' + e.message);
        }).end(qs.stringify({
            from_where : 2000,
            uid: uid,
            session_id: session_id,
            pubKey: pub_key
        }));
    });
}

function make_msg(uo, mo){
    mo.session_id = uo.session_id;
    mo.from_where = uo.from_where;
    mo.uid = uo.uid;
    var smo = sortObject(mo);
    var k, to_sign='';
    for (k in smo){
        to_sign += k + smo[k];
    }
    to_sign += uo.md5Key;

    mo.urlsign = md5(to_sign);
    return JSON.stringify(mo);
}

function sortObject(o) {
    var sorted = {},
    key, a = [];

    for (key in o) {
        if (o.hasOwnProperty(key)) {
            a.push(key);
        }
    }

    a.sort();

    for (key = 0; key < a.length; key++) {
        sorted[a[key]] = o[a[key]];
    }
    return sorted;
}

function conn_ws(user_info){
    var pri_file = 'var/rsa_private_key.pem';
    fs = require('fs');
    ursa = require('ursa');
    var pem = fs.readFileSync(pri_file);
    var key = ursa.createPrivateKey(pem);
    var benc = new Buffer(user_info.md5Key, 'base64');
    user_info.md5Key = key.decrypt(benc).toString();
    var WebSocket = require('ws');
    var ws = new WebSocket('ws://10.0.1.28:7681');
    var zlib = require('zlib');
    ws.on('open', function (){
        sit = {op:"lobby/game/sitdown",roomId:15};
        var msg = make_msg(user_info, sit);
        ws.send(msg, {binary:false, mask: true}, function(err){
            if(err){
                console.log(err);
            }
        });
        //zlib.deflate(msg, function(err, buffer){
        //    if (err) {
        //        console.log(err);
        //    } else {
        //        ws.send(buffer, {binary:false, mask: true}, function(err){
        //            if(err){
        //                console.log(err);
        //            }
        //        });
        //    }
        //});
    });
    ws.on('close', function (){
        console.log('ws closed');
    });
    ws.on('message', function (data, flags) {
        console.log(data, flags);
    });
}

function md5 (text) {
    var crypto = require('crypto');
    return crypto.createHash('md5').update(text).digest('hex');
};

http_login('18682006183','123456', 'ttnm');
