require('./utility');
exports.req_maker = req_maker;
exports.resp_parser = resp_parser;
exports.hall = hall;

function req_maker(user_info){//desk
    var self = this;
    self.u = require('./utility');
    self.user_info = user_info;

    self.make_msg = function (mo){
        mo.session_id = self.user_info.session_id;
        mo.from_where = 2000;
        mo.uid = parseInt(self.user_info.uid);
        if (self.match_id != undefined){
            mo.matchId = self.match_id;
        }
        if (self.room_type != undefined){
            mo.roomType = self.room_type;
        }
        if (self.desk_id != undefined){
            mo.deskId = self.desk_id;
        }
        
        var smo = self.u.sortObject(mo);
        var k, to_sign='';
        for (k in smo){
            to_sign += k + smo[k];
        }
        to_sign += self.user_info.md5key;
        console.log(user_info, to_sign);

        mo.urlsign = self.u.md5(to_sign);
        return JSON.stringify(mo);
    }

    self.bet = function(chip){
        return self.make_msg({op:"lobby/game/callbet", 'chip':chip, action:1});
    }

    self.fold = function(){
        return self.make_msg({op:"lobby/game/callbet", 'chip':0, action:2});
    }

    self.left = function(){
        return self.make_msg({op:"lobby/game/left"});
    }
    
    self.sit_down = function(room_id, room_type){
        self.room_type = room_type;
        return self.make_msg({op:"lobby/game/sitdown",roomId:room_id});
    }

    self.join_match = function(match_id, room_type){
        self.match_id = match_id;
        self.room_type = room_type;
        return self.make_msg({op:"lobby/game/JoinMatch"});
    }

    self.global_game_info = function(){
        return self.make_msg({op:"lobby/game/getUserGameInfo"});
    }
}

function resp_parser(user_info){
    var self = this;
    self.maker = new req_maker(user_info);

    self.set_desk = function(deskArr, desk){
        desk.dBoardID=deskArr["board_id"];
        desk.dDeskID=deskArr["desk_id"];
        desk.dDeskStatus=deskArr["desk_status"];
        desk.dDeskMaxPerson=deskArr["max_person"];
        desk.dRoomType=deskArr["room_type"];
        desk.dBoardBigBlindChip=deskArr["big_blind_chip"];
        desk.dBoardSmallBlindChip=deskArr["small_blind_chip"];
    }

    self.set_board = function(boardArr, desk){
        desk.dBoardActiveSeatNum=boardArr["active_seat_num"];
        desk.dBoardMaxRoundChip=boardArr["max_round_chip"];
        desk.dBoardRaiseChip=boardArr["raise_chip"];
        desk.dBoardBigBlindSeatNum=boardArr["dealer_seat_num"];
        desk.dBoardBigBlindSeatNum=boardArr["big_blind_seat_num"];
        desk.dBoardSmallBlindSeatNum=boardArr["small_blind_seat_num"];
        desk.dBoardTotalChip=boardArr["total_chip"];
        desk.dBoardRound=boardArr["round"];
    }

    self.set_players = function(players, desk){
        desk.dPlayerArr=players;
        desk.dPlayerArr.forEach(function(item, index, arr){
            if (item.uid == desk.user_info.uid){
                desk.dUChip=item["chip"];
                desk.dUBetchips=item["betchips"];
                desk.dUserStatus=item["user_status"];
                if(item["hole_card"] !== undefined){
                    desk.dHoleCardArr=item["hole_card"];
                }
            }
        });
    }

    self.resp_sit_down = function(msg_obj){
        console.log(__LINE__, "resp_sit_down?");
        return false;
    //    if (self.user_info != msg_obj.action.uid){
    //        return '';
    //    }
    //    self.dSeatNum = msg_obj.action.seat_num;

    //    if (msg_obj.action.broadcast_type == 0)
    //    {
    //        //入座增量包
    //        self.dRoomType=msg_obj.action["room_type"];
    //        self.dUserStatus=msg_obj.inc["user_status"];
    //        self.dUChip=msg_obj.inc["chip"];
    //        self.dUBetchips=msg_obj.inc["betchips"];
    //    }
    //    else
    //    {
    //        //入座全量包
    //        if (msg_obj.inc.desk != undefined){
    //            self.set_desk(msg_obj.inc.desk);
    //        }

    //        if (msg_obj.inc.players != undefined){
    //            self.set_players(msg_obj.inc.players);
    //        }
    //    }
    //    return '';
    };

    self.resp_start_game = function(msg_obj, desk){
        desk.dHoleCardArr=[];
        desk.dBoardPublicCardArr=[];

        desk.dDeskStatus=msg_obj.inc["desk_status"];
        desk.dBoardBigBlindChip=msg_obj.inc["big_blind_chip"];
        desk.dBoardSmallBlindChip=msg_obj.inc["small_blind_chip"];
        desk.dBoardDealerSeatNum=msg_obj.inc["dealer_seat_num"];
        desk.dBoardBigBlindSeatNum=msg_obj.inc["big_blind_seat_num"];
        desk.dBoardSmallBlindSeatNum=msg_obj.inc["small_blind_seat_num"];
        desk.dBoardMaxRoundChip=msg_obj.inc["max_round_chip"];
        desk.dBoardRaiseChip=msg_obj.inc["raise_chip"];
        desk.dBoardID=msg_obj.inc["board_id"];
        msg_obj.inc["players"].forEach(function(item, index, arr){
            if (item.seat_num == desk.dSeatNum){
                desk.dUChip = item.chip;
                desk.dUBetchips = item.betchips;
            }
        });
        return desk.maker.global_game_info();
    };

    self.resp_get_holdcard = function(msg_obj, desk){
        if(msg_obj.inc["players"][0]["uid"]==self.user_info.uid){
            desk.dHoleCardArr=msg_obj.inc["players"][0]["hole_card"];
        }
        return '';
    };

    self.check = function(){
        return self.maker.bet(0);
    }

    self.fold = function(){
        return self.maker.fold();
    }

    self.raise = function(){
        var rand_chip = parseInt(1+9*Math.random())*desk.dBoardBigBlindChip;
        var chip = 0;
        if(desk.dBoardRaiseChip == 0){
            if(desk.dBoardMaxRoundChip<=desk.dBoardBigBlindChip){
                chip = desk.dBoardBigBlindChip-desk.dUBetchips+desk.dBoardBigBlindChip+rand_chip;
            }else if(desk.dBoardMaxRoundChip > desk.dBoardBigBlindChip){
                chip = desk.dBoardMaxRoundChip-desk.dUBetchips+desk.dBoardBigBlindChip+rand_chip;
            }
        }else{
            chip = desk.dBoardMaxRoundChip-desk.dUBetchips+desk.dBoardRaiseChip+rand_chip;
        }

        if(chip>desk.dUChip){
            chip=desk.dUChip;
        }
        return self.maker.bet(chip);
    }

    self.call = function(){
        var chip = 0;
        if (desk.dBoardMaxRoundChip > desk.dBoardBigBlindChip){
            chip = desk.dBoardMaxRoundChip - desk.dUBetchips;
        }else{
            chip = desk.dBoardBigBlindChip - desk.dUBetchips;
        }
        if (chip > desk.dUChip){
            chip = desk.dUChip;
        }
        return desk.maker.bet(chip);
    }

    self.think = function(){
        var r = Math.random();
        if(r<0.4){
            if (desk.dBoardMaxRoundChip > 0){
                return self.call();
            }else{
                //console.log(__LINE__);
                return self.check();
            }
        }else if(r<0.6){
            return self.call();
        }else if(r<0.95){
            if (desk.dBoardMaxRoundChip > 0){
                return self.fold();
            }else{
                //console.log(__LINE__);
                return self.check();
            }
        }else{
            return self.raise();
        }
    }

    self.resp_notify_active_user = function(msg_obj, desk){
        //console.log(JSON.stringify(msg_obj, null, 2));

        desk.dBoardActiveSeatNum=msg_obj.inc["active_seat_num"];
        desk.dBoardRaiseChip=msg_obj.inc["raise_chip"];
        desk.dBoardMaxRoundChip=msg_obj.inc["max_round_chip"];

        if(desk.user_info.uid == msg_obj.action["uid"]){
            return self.think();
        }
        return '';
    };

    self.resp_fold = function(msg_obj, desk){
        if(self.user_info.uid==msg_obj.action["uid"]){
            desk.dUserStatus=msg_obj.action["user_status"];
        }
        return '';
    };

    self.resp_check = function(msg_obj, desk){
        if(self.user_info.uid==msg_obj.action["uid"]){
            desk.dUserStatus=msg_obj.action["user_status"];
        }
        return '';
    };

    self.resp_allin = function(msg_obj, desk){
        //console.log(__LINE__, JSON.stringify(msg_obj, null, 2));
        if(self.user_info.uid==msg_obj.action["uid"]){
            desk.dUserStatus=msg_obj.action["user_status"];
            desk.dUChip=0;
            desk.dUBetchips=msg_obj.inc["betchips"];
        }
        desk.dBoardTotalChip=msg_obj.inc["total_chip"];
        return '';
    };

    self.resp_bet = function(msg_obj, desk){
        if(self.user_info.uid==msg_obj.action["uid"]){
            desk.dUserStatus=msg_obj.action["user_status"];
            desk.dUChip=msg_obj.inc["chip"];
            desk.dUBetchips=msg_obj.inc["betchips"];
        }
        desk.dBoardTotalChip=msg_obj.inc["total_chip"];
        return '';
    };

    self.resp_raise = function(msg_obj, desk){
        if(desk.user_info.uid==msg_obj.action["uid"]){
            desk.dUserStatus=msg_obj.action["user_status"];
            desk.dUChip=msg_obj.inc["chip"];
            desk.dUBetchips=msg_obj.inc["betchips"];
        }
        desk.dBoardTotalChip=msg_obj.inc["total_chip"];
        return '';
    };

    self.resp_notify_side_pot = function(msg_obj, desk){
        //console.log(__LINE__, JSON.stringify(msg_obj, null, 2));
        desk.dUBetchips=0;
        desk.dBoardRaiseChip=0;
        desk.dBoardMaxRoundChip=0;
        return '';
    };

    self.end_board = function(){
        var d = new Date();
        var ts = (d.getTime()-d.getMilliseconds())/1000;
        if (ts > self.service_end_time){
            //console.log('------------------left');
            return self.maker.left();
        }else{
            return '';
            if (desk.dRoomType == 1){
                //if (Math.random()<0.1){
                //    return self.maker.change_desk();
                //}
                return '';
            }
            //console.log('------------------sit_down');
            return self.maker.sit_down(self.dRoomId);
        }
    }

    self.resp_end_board = function(msg_obj, desk){
        if (msg_obj.inc["players"] !== undefined){
            msg_obj.inc["players"].forEach(function(item, index, arr){
                if (item.seat_num == desk.dSeatNum){
                    desk.dUChip = item.chip;
                }
            });
        }
        return self.end_board();
    };

    self.resp_stand_up = function(msg_obj){
        return '';
    };

    self.resp_change_left = function(msg_obj){
        if(self.user_info.uid==msg_obj.action["uid"]){
            desk.dSequenceID=0;
        }
        return '';
    };

    self.resp_left = function(msg_obj){
        if(self.user_info.uid==msg_obj.action["uid"]){
            desk.dSequenceID=0;
            //ws.close();
            return false;
        }
        return '';
    };

    self.resp_sng_match_end_left = function(msg_obj){
        return self.resp_host_left(msg_obj);
    };

    self.resp_host_left = function(msg_obj){
        if(self.user_info.uid==msg_obj.action["uid"]){
            desk.dSequenceID=0;
            return self.end_board();
        }
        return '';
    };

    self.resp_monitor_sit_down = function(msg_obj){
        return self.resp_host_left(msg_obj);
    };

    self.resp_monitor_stand_up = function(msg_obj){
        return self.resp_host_left(msg_obj);
    };


    self.resp_buy_chip = function(msg_obj){
        if(msg_obj.inc["seat_num"]==desk.dSeatNum){
            desk.dUChip=msg_obj.inc["chip"];
        }
        return '';
    };

    self.global_game_info = function(){
        return self.maker.global_game_info();
    };
}

function game(setting, user_info, game_type){
    var self = this;
    self.setting = setting;
    self.user_info = user_info;
    self.game_type = game_type;

    self.resp_parser = new resp_parser(user_info);

    self.start_game = function(room_id){
        self.ws_path = 'ws://10.0.1.28:7681';
        if (setting.env == 'test'){
            self.ws_path = 'ws://10.0.1.28:7682';
        }
        if(setting.env == 'release'){
            self.ws_path = 'ws://ws.999com.com:17681';
        }
        var WebSocket = require('ws');
        var ws = new WebSocket(self.ws_path);

        ws.on('open', function (){
            var sng_room_type = 2;
            var normal_room_type = 1;
            var champion_room_type = 4;
            var first_req = '';
            if (self.game_type == 'normal'){
                first_req = self.sit_down(room_id, normal_room_type);
            }else if(self.game_type == 'sng'){
                first_req = self.sit_down(room_id, sng_room_type);
            }else if(self.game_type == 'champion'){
                var match_id = room_id;
                first_req = self.join_match(match_id, champion_room_type);
            }else{
                console.log(__LINE__, "unknown game_type");
                return;
            }
            console.log('send :' + first_req);
            ws.send(first_req, {binary:false, mask: true}, function(err){
                if(err){
                    console.log("ws send error: " + err);
                    ws.close();
                }
            });
        });
        ws.on('close', function (){
            console.log('ws closed');
        });
        ws.on('message', function (data, flags) {
            self.parse(data);
        });
    }

    self.sit_down = function(room_id, room_type){
        return self.resp_parser.req_maker.sitdown(room_id, room_type);
    }

    self.join_match = function(match_id, room_type){
        return self.resp_parser.req_maker.sitdown(match_id, room_type);
    }

    self.global_game_info = function(){
        return self.resp_parser.req_maker.global_game_info();
    }

    self.LC_BROADCAST_ACTION_TYPE_START_GAME = 1; //开局
    self.LC_BROADCAST_ACTION_TYPE_HOLE_CARD = 2; //发底牌
    self.LC_BROADCAST_ACTION_TYPE_FIGHT_CARD = 3; //斗牌
    self.LC_BROADCAST_ACTION_TYPE_FOLD = 4; //弃牌
    self.LC_BROADCAST_ACTION_TYPE_CHECK = 5; //过牌
    self.LC_BROADCAST_ACTION_TYPE_ALLIN = 6; //allin
    self.LC_BROADCAST_ACTION_TYPE_BET = 7; //普通下注
    self.LC_BROADCAST_ACTION_TYPE_CALL = 8; //跟注
    self.LC_BROADCAST_ACTION_TYPE_RAISE = 9; //加注
    self.LC_BROADCAST_ACTION_TYPE_SITDOWN = 10; //坐下
    self.LC_BROADCAST_ACTION_TYPE_STANDUP = 11; //站起
    self.LC_BROADCAST_ACTION_TYPE_INIT_USERGAMEINFO = 12; //初始化用户游戏信息
    self.LC_BROADCAST_ACTION_TYPE_NOTICE_ACTIVE_USER = 13; //通知活动用户
    self.LC_BROADCAST_ACTION_TYPE_NOTICE_SIDE_POT = 14; //通知边池更新
    self.LC_BROADCAST_ACTION_TYPE_NOTICE_NEW_ROUND = 15; //通知开新轮（里面包含新轮公共牌）
    self.LC_BROADCAST_ACTION_TYPE_SMALL_BLIND = 16; //普通小盲下注
    self.LC_BROADCAST_ACTION_TYPE_SMALL_BLIND_ALLIN = 17; //小盲allin下注
    self.LC_BROADCAST_ACTION_TYPE_BIG_BLIND = 18; //普通大盲下注
    self.LC_BROADCAST_ACTION_TYPE_BIG_BLIND_ALLIN = 19; //大盲allin下注
    self.LC_BROADCAST_ACTION_TYPE_PRIZE = 20; //派奖
    self.LC_BROADCAST_ACTION_TYPE_END_BOARD = 21; //牌局结束
    self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM = 22; //离开房间
    self.LC_BROADCAST_ACTION_TYPE_SNG_RANKING_UPDATE = 23; //sng排名更新
    self.LC_BROADCAST_ACTION_TYPE_SNG_MATCH_PRIZE = 24; //sng比赛派奖
    self.LC_BROADCAST_ACTION_TYPE_SNG_BLIND_UPDATE = 25; //sng大小盲更新
    self.LC_BROADCAST_ACTION_TYPE_SYSTEM_EXCEPTION = 26; //系统异常广播
    self.LC_BROADCAST_ACTION_TYPE_EXCHANGE_CHIP = 27; //成功兑换筹码
    self.LC_BROADCAST_ACTION_TYPE_EMOTICON = 28; //发魔法表情广播
    self.LC_BROADCAST_ACTION_TYPE_CHARACTER = 29;//发聊天文字广播
    self.LC_BROADCAST_ACTION_TYPE_EXCHANGE_CHIP_NOT_BOARD = 30; //成功兑换筹码(不在牌局中)
    self.LC_BROADCAST_ACTION_TYPE_PROMPT = 31;     //牌型提示
    self.LC_BROADCAST_ACTION_TYPE_NO_MONEY_STANDUP = 32;   //牌局结束游戏币不足系统使其站起
    self.LC_BROADCAST_ACTION_TYPE_BOARD_PAY = 33;  //牌局费广播（因为调整到创建牌局的时候扣，该广播已废弃）
    self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_CHANGE_DESK = 34; //离开房间(换桌离开)
    self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_HOSTING = 35; //离开房间(托管站起导致的离开)
    self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SIT_DOWN_MONITOR = 36; //离开房间(入座超时导致的离开)
    self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_STANDUP_MONITOR = 37; //离开房间(站起超时导致的离开)
    self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SNG_MATCH_END = 38; //离开房间(sng比赛结束强制离开)

    self.desk_list = {};
    self.simple_parse(msg_obj, desk){
        console.log("desk id: " + desk.dDeskID + " uid: " + self.user_info.uid + " left chip: "
                + desk.dUChip + " board id: " + desk.dBoardID);
        console.log("action: "+msg_obj.action.action);
        if (msg_obj.retCode !== 0){
            console.log('retcode: ' + msg_obj.retCode);
            return self.global_game_info();
        }
        if (msg_obj.sequence_id === undefined){
            console.log('no sequence_id');
            return self.global_game_info();
        }
        if(parseInt(msg_obj.sequence_id) <= parseInt(desk.sequence_id)){
            console.log('incremently sequence_id error: ' + msg_obj.sequence_id + ' vs ' + desk.sequence_id);
            return self.global_game_info();
        }
        if(parseInt(msg_obj.sequence_id) > parseInt(desk.sequence_id) + 1){
            return self.global_game_info();
        }

        var req_msg;
        var action = msg_obj.action.action;
        desk.sequence_id = parseInt(msg_obj.sequence_id);
        switch(action)
        {
            case self.LC_BROADCAST_ACTION_TYPE_EMOTICON:
            case self.LC_BROADCAST_ACTION_TYPE_CHARACTER:
                break;
            case self.LC_BROADCAST_ACTION_TYPE_SITDOWN:
                req_msg = self.resp_sit_down(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_START_GAME:
                req_msg = self.resp_start_game(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_HOLE_CARD:
                req_msg = self.resp_get_holdcard(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_NOTICE_ACTIVE_USER:
                req_msg = self.resp_notify_active_user(msg_obj);
                console.log("big blind: " + self.dBoardBigBlindChip + " board raise chip: " + self.dBoardRaiseChip +
                        " max round chip: " + self.dBoardMaxRoundChip + " last bet chip:" + self.dUBetchips);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_FOLD:
                req_msg = self.resp_fold(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_CHECK:
                req_msg = self.resp_check(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_ALLIN:
                req_msg = self.resp_allin(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_CALL:
            case self.LC_BROADCAST_ACTION_TYPE_BET:
                req_msg = self.resp_bet(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_RAISE:
                req_msg = self.resp_raise(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_NOTICE_SIDE_POT:
                req_msg = self.resp_notify_side_pot(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_END_BOARD:
                req_msg = self.resp_end_board(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_STANDUP:
                req_msg = self.resp_stand_up(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_CHANGE_DESK:
                req_msg = self.resp_change_left(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM:
                req_msg = self.resp_left(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SNG_MATCH_END:
                req_msg = self.resp_sng_match_end_left(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_HOSTING:
                req_msg = self.resp_host_left(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SIT_DOWN_MONITOR:
                req_msg = self.resp_monitor_sit_down(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_STANDUP_MONITOR:
                req_msg = self.resp_monitor_stand_up(msg_obj);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_INIT_USERGAMEINFO:
                console.log(__LINE__, "self.LC_BROADCAST_ACTION_TYPE_INIT_USERGAMEINFO here");
                break;
            case self.LC_BROADCAST_ACTION_TYPE_EXCHANGE_CHIP:
                req_msg = self.resp_buy_chip(msg_obj);
                break;
            default:
                break;
        }
        return req_msg;
    }

    //board end
    //game end
    //need to delay
    self.parse = function(resp){
        var msg_obj;
        try{
            msg_obj = JSON.parse(msg);
        }catch(e){
            console.log('exception: ' + e);
            ws.close();
            return false;
        }
        if (msg_obj.retCode !== 0){
            console.log(__LINE__, 'retcode: ' + msg_obj.retCode);
            return self.global_game_info();
        }
        var action = msg_obj.action.action;
        if (action == self.LC_BROADCAST_ACTION_TYPE_INIT_USERGAMEINFO) {
            self.desk_list = self.resp_global_game_info(msg_obj);
            return true;
        }

        var desk_id = parseInt(msg_obj.desk_id);
        var desk = self.desk_list[desk_id];
        if (desk == undefined){
            console.log(__LINE__, "no desk for desk_id: " + desk_id + ", " + JSON.stringify(self.desk_list));
            return self.global_game_info();
        } else {
            if (action == LC_BROADCAST_ACTION_TYPE_CHARACTER
                    || action == LC_BROADCAST_ACTION_TYPE_EMOTICON) {
                console.log('emotion or chat');
                return '';
            }
            var sleep = 0;
            if (action == self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM){//board end
            } else if (action == self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SNG_MATCH_END){//game end
            } else if (action == self.LC_BROADCAST_ACTION_TYPE_NOTICE_ACTIVE_USER){//need to delay
                var r = Math.random();
                if(r<0.1){
                    sleep = 1;
                }else if(r<0.45){
                    sleep = 3;
                }else if(r<0.85){
                    sleep = 5;
                }else if(r<0.95){
                    sleep = 7;
                }else{
                    sleep = 8;
                }
            } else {
                return self.simple_parse(msg, desk);
            }
        }
    }
}
//setTimeout(self.send_msg, sleep*1000, req_msg);

