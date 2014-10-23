function get_user_info(user_info, room_info) {
    var http = require('http');
    var qs = require('querystring');

    var path = 'user/index';
    var params = {
        lua_version : '1.0.0.0',
        session_id : user_info.session_id,
        uid : user_info.uid,
        version: '1',
        from_where : 2000,
        r: path,
        c: 'userinfo',
        op: 'info',
    };
    
    var sparams = sortObject(params);
    var k, to_sign='';
    for (k in sparams){
        to_sign += k + sparams[k];
    }
    to_sign += user_info.md5Key;
    sparams.urlsign = md5(to_sign);

    http.request({
        hostname: 'www.999com.com',
        port: 80,
        path: '/index.php?r=' + path,
        method: 'POST',
        headers:{'Content-Type': 'application/x-www-form-urlencoded'}
    }, function (res) {
        var body = '';
        res.setEncoding('utf8');
        res.on('data', function (chunk){
            body += chunk;
        });
        res.on('end', function () {
            try {
                var my_info = JSON.parse(body);
                var retcode = my_info.retcode;
                if (retcode === 0){
                    user_info.game_money = my_info.game_money;
                    conn_ws(user_info, room_info);
                }
            } catch (e){
                console.log('exception: ' + e);
            }
        });
    }).on('error', function(e) {
        console.log('error: ' + e.message);
    }).end(qs.stringify(sparams));
    //while(!lock){
    //    console.log(all_room);
    //}
}

function get_roominfo(user_info, sng, ret_room) {
    var http = require('http');
    var qs = require('querystring');

    var sng_path = 'lobby/game/getsngroomlist';
    var normal_path = 'lobby/game/getallroom';
    var params = {
        lua_version : '1.0.0.0',
        session_id : user_info.session_id,
        uid : user_info.uid,
        version: '1',
        from_where : 2000,
        r: (sng === true? sng_path : normal_path),
    };
    
    var sparams = sortObject(params);
    var k, to_sign='';
    for (k in sparams){
        to_sign += k + sparams[k];
    }
    to_sign += user_info.md5Key;
    sparams.urlsign = md5(to_sign);

    //var all_room;
    http.request({
        hostname: 'www.999com.com',
        //hostname: '10.0.1.27',
        port: 80,
        path: '/index.php?r=' + (sng === true? sng_path : normal_path),
        method: 'POST',
        headers:{'Content-Type': 'application/x-www-form-urlencoded'}
    }, function (res) {
        var body = '';
        res.setEncoding('utf8');
        res.on('data', function (chunk){
            body += chunk;
        });
        res.on('end', function () {
            //console.log("-------------"+body);
            try {
                var room_info = JSON.parse(body);
                var retcode = room_info.retCode;
                if (retcode != 0){
                    console.log("get room info error: " + body);
                } else {
                    if (ret_room === undefined){
                        ret_room = [];
                        //ret_room[(sng? 'sng':'normal')] = room_info.data;
                        if (sng){
                            room_info.data.map(function(item){
                                item.match = 'sng';
                                ret_room.push(item);
                            });
                        }else{
                            var k1,k2;
                            for (k1 in room_info.data){
                                for (k2 in room_info.data[k1]){
                                    room_info.data[k1][k2].map(function(item){
                                        item.match = 'normal';
                                        item.level = k1;
                                        item.desk_player_num = k2;
                                        ret_room.push(item);
                                    });
                                }
                            }
                        }
                        get_roominfo(user_info, !sng, ret_room);
                    }else{
                        if (sng){
                            room_info.data.map(function(item){
                                item.match = 'sng';
                                ret_room.push(item);
                            });
                        }else{
                            var k1,k2;
                            for (k1 in room_info.data){
                                for (k2 in room_info.data[k1]){
                                    room_info.data[k1][k2].map(function(item){
                                        item.match = 'normal';
                                        item.level = k1;
                                        item.desk_player_num = k2;
                                        ret_room.push(item);
                                    });
                                }
                            }
                        }
                        get_user_info(user_info, ret_room);
                    }
                }
            } catch (e){
                console.log('exception: ' + e);
            }
        });
    }).on('error', function(e) {
        console.log('error: ' + e.message);
    }).end(qs.stringify(sparams));
    //while(!lock){
    //    console.log(all_room);
    //}
}

function http_login(name, pass, vcode) {
    if (vcode === undefined){
        vcode = '';
    }
    var http = require('http');
    var qs = require('querystring');

    http.request({
        hostname: 'www.999com.com',
        //hostname: '10.0.1.27',
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
            try{
                var uo = JSON.parse(body);
                var retcode = uo.retcode;
                if (retcode != 0){
                    console.log("error: " + body);
                } else {
                    //console.log(uo);
                    upload_public_key(uo);
                }
            }catch(e){
                consolo.log('exception: ' + e);
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
            hostname: 'www.999com.com',
            //hostname: '10.0.1.27',
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
                try{
                    var res = JSON.parse(body);
                    var retcode = res.retCode;
                    if (retcode != 0){
                        console.log("upload private key error: " + body);
                    } else {
                        user_info.md5Key = res.md5Key;

                        var pri_file = 'var/rsa_private_key.pem';
                        var fs = require('fs');
                        var ursa = require('ursa');
                        var pem = fs.readFileSync(pri_file);
                        var key = ursa.createPrivateKey(pem);
                        var benc = new Buffer(user_info.md5Key, 'base64');
                        user_info.md5Key = key.decrypt(benc).toString();

                        get_roominfo(user_info, true); 
                    }
                }catch(e){
                    console.log('exception: ' + e);
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
    mo.from_where = 2000;//parseInt(uo.from_where);
    mo.uid = parseInt(uo.uid);
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

function process_msg(user_info, msg){
    var LC_BROADCAST_ACTION_TYPE_START_GAME = 1; //开局
    var LC_BROADCAST_ACTION_TYPE_HOLE_CARD = 2; //发底牌
    var LC_BROADCAST_ACTION_TYPE_FIGHT_CARD = 3; //斗牌
    var LC_BROADCAST_ACTION_TYPE_FOLD = 4; //弃牌
    var LC_BROADCAST_ACTION_TYPE_CHECK = 5; //过牌
    var LC_BROADCAST_ACTION_TYPE_ALLIN = 6; //allin
    var LC_BROADCAST_ACTION_TYPE_BET = 7; //普通下注
    var LC_BROADCAST_ACTION_TYPE_CALL = 8; //跟注
    var LC_BROADCAST_ACTION_TYPE_RAISE = 9; //加注
    var LC_BROADCAST_ACTION_TYPE_SITDOWN = 10; //坐下
    var LC_BROADCAST_ACTION_TYPE_STANDUP = 11; //站起
    var LC_BROADCAST_ACTION_TYPE_INIT_USERGAMEINFO = 12; //初始化用户游戏信息
    var LC_BROADCAST_ACTION_TYPE_NOTICE_ACTIVE_USER = 13; //通知活动用户
    var LC_BROADCAST_ACTION_TYPE_NOTICE_SIDE_POT = 14; //通知边池更新
    var LC_BROADCAST_ACTION_TYPE_NOTICE_NEW_ROUND = 15; //通知开新轮（里面包含新轮公共牌）
    var LC_BROADCAST_ACTION_TYPE_SMALL_BLIND = 16; //普通小盲下注
    var LC_BROADCAST_ACTION_TYPE_SMALL_BLIND_ALLIN = 17; //小盲allin下注
    var LC_BROADCAST_ACTION_TYPE_BIG_BLIND = 18; //普通大盲下注
    var LC_BROADCAST_ACTION_TYPE_BIG_BLIND_ALLIN = 19; //大盲allin下注
    var LC_BROADCAST_ACTION_TYPE_PRIZE = 20; //派奖
    var LC_BROADCAST_ACTION_TYPE_END_BOARD = 21; //牌局结束
    var LC_BROADCAST_ACTION_TYPE_LEFT_ROOM = 22; //离开房间
    var LC_BROADCAST_ACTION_TYPE_SNG_RANKING_UPDATE = 23; //sng排名更新
    var LC_BROADCAST_ACTION_TYPE_SNG_MATCH_PRIZE = 24; //sng比赛派奖
    var LC_BROADCAST_ACTION_TYPE_SNG_BLIND_UPDATE = 25; //sng大小盲更新
    var LC_BROADCAST_ACTION_TYPE_SYSTEM_EXCEPTION = 26; //系统异常广播
    var LC_BROADCAST_ACTION_TYPE_EXCHANGE_CHIP = 27; //成功兑换筹码
    var LC_BROADCAST_ACTION_TYPE_EMOTICON = 28; //发魔法表情广播
    var LC_BROADCAST_ACTION_TYPE_CHARACTER = 29;//发聊天文字广播
    var LC_BROADCAST_ACTION_TYPE_EXCHANGE_CHIP_NOT_BOARD = 30; //成功兑换筹码(不在牌局中)
    var LC_BROADCAST_ACTION_TYPE_PROMPT = 31;     //牌型提示
    var LC_BROADCAST_ACTION_TYPE_NO_MONEY_STANDUP = 32;   //牌局结束游戏币不足系统使其站起
    var LC_BROADCAST_ACTION_TYPE_BOARD_PAY = 33;  //牌局费广播（因为调整到创建牌局的时候扣，该广播已废弃）
    var LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_CHANGE_DESK = 34; //离开房间(换桌离开)
    var LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_HOSTING = 35; //离开房间(托管站起导致的离开)
    var LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SIT_DOWN_MONITOR = 36; //离开房间(入座超时导致的离开)
    var LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_STANDUP_MONITOR = 37; //离开房间(站起超时导致的离开)
    var LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SNG_MATCH_END = 38; //离开房间(sng比赛结束强制离开)
    var msg_obj;
    try{
        msg_obj = JSON.parse(msg);
        if (msg_obj.retCode !== 0){
            console.log('retcode: ' + msg_obj.retCode);
            console.log(msg);
            return false;
        }
        var action = msg_obj.action.action;
        if (action == LC_BROADCAST_ACTION_TYPE_EMOTICON ||
                action == LC_BROADCAST_ACTION_TYPE_CHARACTER){
            return '';
        }
        if (msg_obj.sequence_id === undefined){
            console.log('no sequence_id');
            return false;
        }
    }catch(e){
        console.log('exception: ' + e);
        return false;
    }
}

function conn_ws(user_info, room_info){
    //var room_id = parseInt(room_info.normal['1']['2'][2].id);
    console.log(user_info, room_info, room_info.length);
    return 0;
    var room_id = 35;
    var WebSocket = require('ws');
    //var ws = new WebSocket('ws://10.0.1.28:7681');
    var ws = new WebSocket('ws://ws.999com.com:17681');
    var zlib = require('zlib');
    ws.on('open', function (){
        sit = {op:"lobby/game/sitdown",roomId:room_id};
        var msg = make_msg(user_info, sit);
        console.log("send: " + msg);
        ws.send(msg, {binary:false, mask: true}, function(err){
            if(err){
                console.log("ws send error: " + err);
            }
        });
    });
    ws.on('close', function (){
        console.log('ws closed');
    });
    ws.on('message', function (data, flags) {
        console.log("recv: " + data);
        process_msg(user_info, data);
        //console.log("ws recv data flag: ", flags);
    });
}

function md5 (text) {
    var crypto = require('crypto');
    return crypto.createHash('md5').update(text).digest('hex');
};

//console.log(md5('from_where2000lua_version1.0.0.0rlobby/game/getsngroomlistsession_idfg7gv9puaqsb69nh15k6k6tkr3uid149version124uaxu3sgrk1ltxlt41kgvkfylyq0mc2'));
http_login('18682006183',md5('123456'), 'ttnm');
