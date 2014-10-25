Object.defineProperty(global, '__stack', {
    get: function(){
             var orig = Error.prepareStackTrace;
             Error.prepareStackTrace = function(_, stack){ return stack; };
             var err = new Error;
             Error.captureStackTrace(err, arguments.callee);
             var stack = err.stack;
             Error.prepareStackTrace = orig;
             return stack;
         }
});

Object.defineProperty(global, '__line', {
    get: function(){
             return __stack[1].getLineNumber();
         }
});

function md5 (text) {
    var crypto = require('crypto');
    return crypto.createHash('md5').update(text).digest('hex');
};

//console.log(__line);
//console.log(__stack);

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

function msg_maker(user_info)
{
    this.user_info = user_info;

    this.make_msg = function (mo){
        mo.session_id = this.user_info.session_id;
        mo.from_where = 2000;
        mo.uid = parseInt(this.user_info.uid);
        var smo = sortObject(mo);
        var k, to_sign='';
        for (k in smo){
            to_sign += k + smo[k];
        }
        to_sign += this.user_info.md5Key;

        mo.urlsign = md5(to_sign);
        return JSON.stringify(mo);
    }

    this.bet = function(chip){
        return this.make_msg({op:"lobby/game/callbet", chip:chip, action:1});
    }

    this.fold = function(){
        return this.make_msg({op:"lobby/game/callbet", chip:0, action:2});
    }

    this.change_desk = function(){
        return this.make_msg({op:"lobby/game/changedesk"});
    }

    this.left = function(){
        return this.make_msg({op:"lobby/game/left"});
    }

    this.sit_down = function(room_id){
        return this.make_msg({op:"lobby/game/sitdown",roomId:room_id});
    }

    this.global_game_info = function(){
        return this.make_msg({op:"lobby/game/getUserGameInfo"});
    }
}

function msg_handler(user_info, ws){
    this.LC_BROADCAST_ACTION_TYPE_START_GAME = 1; //开局
    this.LC_BROADCAST_ACTION_TYPE_HOLE_CARD = 2; //发底牌
    this.LC_BROADCAST_ACTION_TYPE_FIGHT_CARD = 3; //斗牌
    this.LC_BROADCAST_ACTION_TYPE_FOLD = 4; //弃牌
    this.LC_BROADCAST_ACTION_TYPE_CHECK = 5; //过牌
    this.LC_BROADCAST_ACTION_TYPE_ALLIN = 6; //allin
    this.LC_BROADCAST_ACTION_TYPE_BET = 7; //普通下注
    this.LC_BROADCAST_ACTION_TYPE_CALL = 8; //跟注
    this.LC_BROADCAST_ACTION_TYPE_RAISE = 9; //加注
    this.LC_BROADCAST_ACTION_TYPE_SITDOWN = 10; //坐下
    this.LC_BROADCAST_ACTION_TYPE_STANDUP = 11; //站起
    this.LC_BROADCAST_ACTION_TYPE_INIT_USERGAMEINFO = 12; //初始化用户游戏信息
    this.LC_BROADCAST_ACTION_TYPE_NOTICE_ACTIVE_USER = 13; //通知活动用户
    this.LC_BROADCAST_ACTION_TYPE_NOTICE_SIDE_POT = 14; //通知边池更新
    this.LC_BROADCAST_ACTION_TYPE_NOTICE_NEW_ROUND = 15; //通知开新轮（里面包含新轮公共牌）
    this.LC_BROADCAST_ACTION_TYPE_SMALL_BLIND = 16; //普通小盲下注
    this.LC_BROADCAST_ACTION_TYPE_SMALL_BLIND_ALLIN = 17; //小盲allin下注
    this.LC_BROADCAST_ACTION_TYPE_BIG_BLIND = 18; //普通大盲下注
    this.LC_BROADCAST_ACTION_TYPE_BIG_BLIND_ALLIN = 19; //大盲allin下注
    this.LC_BROADCAST_ACTION_TYPE_PRIZE = 20; //派奖
    this.LC_BROADCAST_ACTION_TYPE_END_BOARD = 21; //牌局结束
    this.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM = 22; //离开房间
    this.LC_BROADCAST_ACTION_TYPE_SNG_RANKING_UPDATE = 23; //sng排名更新
    this.LC_BROADCAST_ACTION_TYPE_SNG_MATCH_PRIZE = 24; //sng比赛派奖
    this.LC_BROADCAST_ACTION_TYPE_SNG_BLIND_UPDATE = 25; //sng大小盲更新
    this.LC_BROADCAST_ACTION_TYPE_SYSTEM_EXCEPTION = 26; //系统异常广播
    this.LC_BROADCAST_ACTION_TYPE_EXCHANGE_CHIP = 27; //成功兑换筹码
    this.LC_BROADCAST_ACTION_TYPE_EMOTICON = 28; //发魔法表情广播
    this.LC_BROADCAST_ACTION_TYPE_CHARACTER = 29;//发聊天文字广播
    this.LC_BROADCAST_ACTION_TYPE_EXCHANGE_CHIP_NOT_BOARD = 30; //成功兑换筹码(不在牌局中)
    this.LC_BROADCAST_ACTION_TYPE_PROMPT = 31;     //牌型提示
    this.LC_BROADCAST_ACTION_TYPE_NO_MONEY_STANDUP = 32;   //牌局结束游戏币不足系统使其站起
    this.LC_BROADCAST_ACTION_TYPE_BOARD_PAY = 33;  //牌局费广播（因为调整到创建牌局的时候扣，该广播已废弃）
    this.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_CHANGE_DESK = 34; //离开房间(换桌离开)
    this.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_HOSTING = 35; //离开房间(托管站起导致的离开)
    this.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SIT_DOWN_MONITOR = 36; //离开房间(入座超时导致的离开)
    this.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_STANDUP_MONITOR = 37; //离开房间(站起超时导致的离开)
    this.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SNG_MATCH_END = 38; //离开房间(sng比赛结束强制离开)

    var self = this;
    this.user_info = user_info;
    this.maker = new msg_maker(this.user_info);
    this.sequence_id = 0;
    this.parse = function (msg){
        var msg_obj;
        try{
            msg_obj = JSON.parse(msg);
        }catch(e){
            console.log('exception: ' + e);
            ws.close();
        }
        if (msg_obj.retCode !== 0){
            console.log('retcode: ' + msg_obj.retCode);
            console.log(msg);
            ws.close();
        }else{
            if (msg_obj.sequence_id === undefined){
                console.log('no sequence_id');
                ws.close();
            }else{
                var action = msg_obj.action.action;
                if(action == this.LC_BROADCAST_ACTION_TYPE_INIT_USERGAMEINFO){
                    if (msg_obj.sequence_id < this.sequence_id){
                        console.log('full sequence_id error: ' + msg_obj.sequence_id + ' vs ' + this.sequence_id);
                        ws.close();
                    }else{
                        this.sequence_id = msg_obj.sequence_id;
                    }
                }else{//incremently
                    var req_msg = '';
                    if (this.sequence_id === 0){
                        req_msg  = this.global_game_info();
                    }
                    else if(msg_obj.sequence_id <= this.sequence_id){
                        console.log('incremently sequence_id error: ' + msg_obj.sequence_id + ' vs ' + this.sequence_id);
                        ws.close();
                    }else if(msg_obj.sequence_id != this.sequence_id + 1){
                        req_msg  = this.global_game_info();
                    }else{
                        switch(action)
                        {
                            case this.LC_BROADCAST_ACTION_TYPE_EMOTICON:
                            case this.LC_BROADCAST_ACTION_TYPE_CHARACTER:
                                console.log('emotion or chat');
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_SITDOWN:
                                req_msg = this.resp_sit_down(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_START_GAME:
                                req_msg = this.resp_start_game(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_HOLE_CARD:
                                req_msg = this.resp_get_holdcard(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_NOTICE_ACTIVE_USER:
                                req_msg = this.resp_notify_active_user(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_FOLD:
                                req_msg = this.resp_fold(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_CHECK:
                                req_msg = this.resp_check(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_ALLIN:
                                req_msg = this.resp_allin(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_CALL:
                            case this.LC_BROADCAST_ACTION_TYPE_BET:
                                req_msg = this.resp_bet(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_RAISE:
                                req_msg = this.resp_raise(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_NOTICE_SIDE_POT:
                                req_msg = this.resp_notify_side_pot(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_END_BOARD:
                                req_msg = this.resp_end_board(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_STANDUP:
                                req_msg = this.resp_stand_up(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_CHANGE_DESK:
                                req_msg = this.resp_change_left(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM:
                                req_msg = this.resp_left(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SNG_MATCH_END:
                                req_msg = this.resp_sng_match_end_left(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_HOSTING:
                                req_msg = this.resp_host_left(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SIT_DOWN_MONITOR:
                                req_msg = this.resp_monitor_sit_down(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_STANDUP_MONITOR:
                                req_msg = this.resp_monitor_stand_up(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_INIT_USERGAMEINFO:
                                req_msg = this.resp_global_game_info(msg_obj);
                                break;
                            case this.LC_BROADCAST_ACTION_TYPE_EXCHANGE_CHIP:
                                req_msg = this.resp_buy_chip(msg_obj);
                                break;
                            default:
                                break;
                        }
                    }

                    if (req_msg !== ''){
                    }
                }
            }
        }
    }

    this.send_msg = function(msg){
        ws.send(req_msg, {binary:false, mask: true}, function(err){
            if(err){
                console.log("ws send error: " + err);
            }
        });
    }

    this.set_desk = function(desk){
        var deskArr=msg_obj.inc.desk;;
        this.dDeskID=deskArr["desk_id"];
        this.dDeskStatus=deskArr["desk_status"];
        this.dDeskMaxPerson=deskArr["max_person"];
        this.dRoomType=deskArr["room_type"];
    }

    this.set_board = function(board){
        var boardArr=msg_obj.inc.board;
        this.dBoardActiveSeatNum=boardArr["active_seat_num"];
        this.dBoardMaxRoundChip=boardArr["max_round_chip"];
        this.dBoardRaiseChip=boardArr["raise_chip"];
        this.dBoardBigBlindSeatNum=boardArr["dealer_seat_num"];
        this.dBoardBigBlindSeatNum=boardArr["big_blind_seat_num"];
        this.dBoardSmallBlindSeatNum=boardArr["small_blind_seat_num"];
        this.dBoardBigBlindChip=boardArr["big_blind_chip"];
        this.dBoardSmallBlindChip=boardArr["small_blind_chip"];
        this.dBoardTotalChip=boardArr["total_chip"];
        this.dBoardRound=boardArr["round"];
    }

    this.set_players = function(players){
        this.dPlayerArr=msg_obj.inc.players;
        this.dPlayerArr.forEach(function(item, index, arr){
            if (item.uid == self.user_info.uid){
                this.dUChip=item["chip"];
                this.dUBetchips=item["betchips"];
                this.dUserStatus=item["user_status"];
                if(item["hole_card"] !== undefined){
                    this.dHoleCardArr=item["hole_card"];
                }
            }
        });
    }

    this.resp_sit_down = function(msg_obj){
        if (this.user_info != msg_obj.action.uid){
            return '';
        }
        this.dSeatNum = msg_obj.action.seat_num;

        if (msg_obj.action.broadcast_type == 0)
        {
            //入座增量包
            this.dRoomType=msg_obj.action["room_type"];
            this.dUserStatus=msg_obj.inc["user_status"];
            this.dUChip=msg_obj.inc["chip"];
            this.dUBetchips=msg_obj.inc["betchips"];
        }
        else
        {
            //入座全量包
            if (msg_obj.inc.desk != undefined){
                this.set_desk(msg_obj.inc.desk);
            }

            if (msg_obj.inc.players != undefined){
                this.set_players(msg_obj.inc.players);
            }
        }
        return '';
    };

    this.resp_start_game = function(msg_obj){
        this.dHoleCardArr=[];
        this.dBoardPublicCardArr=[];
        
        this.dDeskStatus=msg_obj.inc["desk_status"];
        this.dBoardBigBlindChip=msg_obj.inc["big_blind_chip"];
        this.dBoardSmallBlindChip=msg_obj.inc["small_blind_chip"];
        this.dBoardDealerSeatNum=msg_obj.inc["dealer_seat_num"];
        this.dBoardBigBlindSeatNum=msg_obj.inc["big_blind_seat_num"];
        this.dBoardSmallBlindSeatNum=msg_obj.inc["small_blind_seat_num"];
        this.dBoardMaxRoundChip=msg_obj.inc["max_round_chip"];
        this.dBoardRaiseChip=msg_obj.inc["raise_chip"];
        msg_obj.inc["players"].forEach(function(item, index, arr){
            if (item.seat_num == self.dSeatNum){
                this.dUChip = item.chip;
                this.dUBetchips = item.betchips;
            }
        });
        return this.maker.global_game_info();
    };

    this.resp_get_holdcard = function(msg_obj){
        if(msg_obj.inc["players"][0]["uid"]==this.user_info.uid){
           this.dHoleCardArr=msg_obj.inc["players"][0]["hole_card"];
        }
        return '';
    };

    this.check = function(){
        return this.maker.bet(0);
    }

    this.fold = function(){
        return this.maker.fold();
    }

    this.raise = function(){
        var rand_chip = parseInt(1+9*Math.random())*this.dBoardBigBlindChip;
        var chip;
        if(this.dBoardRaiseChip == 0){
            if(this.dBoardMaxRoundChip<=this.dBoardBigBlindChip){
                chip = this.dBoardBigBlindChip-this.dUBetchips+this.dBoardBigBlindChip+rand_chip;
            }else if(this.dBoardMaxRoundChip > this.dBoardBigBlindChip){
                chip = this.dBoardMaxRoundChip-this.dUBetchips+this.dBoardBigBlindChip+rand_chip;
            }
        }else{
            chip = this.dBoardMaxRoundChip-this.dUBetchips+this.dBoardRaiseChip+rand_chip;
        }

        if(chip>this.dUChip){
            chip=this.dUChip;
        }
        return this.maker.bet(chip);
    }

    this.call = function(){
        var chip = 0;
        if (this.dBoardMaxRoundChip > this.dBoardBigBlindChip){
            chip = this.dBoardMaxRoundChip - this.dUBetchips;
        }else{
            chip = this.dBoardBigBlindChip - this.dUBetchips;
        }
        if (chip > this.dUChip){
            chip = this.dUChip;
        }
        return this.maker.bet(chip);
    }

    this.think = function(){
        var r = Math.random();
        if(r<0.4){
            if (this.dBoardMaxRoundChip > 0){
                return this.call();
            }else{
                return this.check();
            }
        }else if(r<0.6){
            this.call();
        }else if(r<0.95){
            if (this.dBoardMaxRoundChip > 0){
                return this.fold();
            }else{
                return this.check();
            }
        }else{
            return this.raise();
        }
    }

    this.resp_notify_active_user = function(msg_obj){
        
        this.dBoardActiveSeatNum=msg_obj.inc["active_seat_num"];
        this.dBoardRaiseChip=msg_obj.inc["raise_chip"];
        this.dBoardMaxRoundChip=msg_obj.inc["max_round_chip"];

        if(this.user_info.uid == msg_obj.action["uid"]){
            return this.think();
        }
        return '';
    };

    this.resp_fold = function(msg_obj){
        if(this.user_info.uid==msg_obj.action["uid"]){
            this.dUserStatus=msg_obj.action["user_status"];
        }
        return '';
    };

    this.resp_check = function(msg_obj){
        if(this.user_info.uid==msg_obj.action["uid"]){
            this.dUserStatus=msg_obj.action["user_status"];
        }
        return '';
    };

    this.resp_allin = function(msg_obj){
        if(this.user_info.uid==msg_obj.action["uid"]){
            this.dUserStatus=msg_obj.action["user_status"];
            this.dUChip=0;
            this.dUBetchips=msg_obj.action["betchips"];
        }
        this.dBoardTotalChip=msg_obj.action["total_chip"];
        return '';
    };

    this.resp_bet = function(msg_obj){
        if(this.user_info.uid==msg_obj.action["uid"]){
            this.dUserStatus=msg_obj.action["user_status"];
            this.dUChip=0;
            this.dUBetchips=msg_obj.action["betchips"];
        }
        this.dBoardTotalChip=msg_obj.action["total_chip"];
        return '';
    };

    this.resp_raise = function(msg_obj){
        if(this.user_info.uid==msg_obj.action["uid"]){
            this.dUserStatus=msg_obj.action["user_status"];
            this.dUChip=0;
            this.dUBetchips=msg_obj.action["betchips"];
        }
        this.dBoardTotalChip=msg_obj.action["total_chip"];
        return '';
    };

    this.resp_notify_side_pot = function(msg_obj){
        this.dUBetchips=0;
        this.dBoardRaiseChip=0;
        this.dBoardMaxRoundChip=0;
        return '';
    };

    this.end_board = function(){
        var d = new Date();
        var ts = (d.getTime()-d.getMilliseconds())/1000;
        if (ts > this.sevice_end_time){
            return this.maker.left();
        }else{
            if (this.dRoomType == 1){
                if (Math.random()<0.1){
                    return this.maker.change_desk();
                }
                return '';
            }
            return this.maker.sit_down(this.dRoomId);
        }
    }
    this.resp_end_board = function(msg_obj){
        msg_obj.inc["players"].forEach(function(item, index, arr){
            if (item.seat_num == self.dSeatNum){
                this.dUChip = item.chip;
            }
        });
        return this.end_board();
    };

    this.resp_stand_up = function(msg_obj){
        return '';
    };

    this.resp_change_left = function(msg_obj){
        if(this.user_info.uid==msg_obj.action["uid"]){
            this.dSequenceID=0;
        }
        return '';
    };

    this.resp_left = function(msg_obj){
        if(this.user_info.uid==msg_obj.action["uid"]){
            this.dSequenceID=0;
            ws.close();
        }
        return '';
    };

    this.resp_sng_match_end_left = function(msg_obj){
        return this.resp_host_left();
    };

    this.resp_host_left = function(msg_obj){
        if(this.user_info.uid==msg_obj.action["uid"]){
            this.dSequenceID=0;
            return this.end_board();
        }
        return '';
    };

    this.resp_monitor_sit_down = function(msg_obj){
        return this.resp_host_left();
    };

    this.resp_monitor_stand_up = function(msg_obj){
        return this.resp_host_left();
    };

    this.resp_global_game_info = function(msg_obj){
        if (msg_obj.uid != this.user_info.uid){
            return '';
        }
        this.dSeatNum = msg_obj.action.seat_num;
        if(msg_obj.inc.desk !== undefined)){
            this.set_desk(msg_obj.inc.desk);
        }
        if(msg_obj.inc.board !== undefined)){
            this.set_board(msg_obj.inc.board);
        }
        if(msg_obj.inc.players !== undefined){
            this.set_players(msg_obj.inc.players);
        }
        return '';
    };

    this.resp_buy_chip = function(msg_obj){
        if(msg_obj.inc["seat_num"]==this.dSeatNum){
            this.dUChip=msg_obj.inc["chip"];
        }
        return '';
    };

    this.global_game_info = function(){
        return this.maker.global_game_info();
    };
}


http_login('18682006183',md5('123456'), 'ttnm');
