function http_login(name, pass, vcode) {
    if (vcode === undefined){
        vcode = '';
    }
    var http = require('http');
    var qs = require('querystring');

    http.request({
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

function make_msg(mo, md5Key){
16         $commonArr["session_id"]=self::$sessionId;
 17         $commonArr["from_where"]=self::$fromWhere;
  18         $commonArr["uid"]=$uid;
  for (k in mo){
      s
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
    ws.on('open', function (){
        sit = {op:"lobby/game/sitdown",roomId:15};
        ws.send(JSON.stringify(sit));
    });
    ws.on('close', function (){
        console.log('ws closed');
    });
    ws.on('message' = function (data, flags) {
        console.log(data, flags);
    });
}

var crypto = require('crypto');
function md5 (text) {
      return crypto.createHash('md5').update(text).digest('hex');
};

http_login('18682006183','123456');
