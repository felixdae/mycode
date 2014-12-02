module.exports = game;

function req_maker(user_info){//desk
    var self = this;
    self.user_info = user_info;
    self.u = require('./utility');

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

        mo.urlsign = self.u.md5(to_sign);
        return JSON.stringify(mo);
    }

    self.bet = function(chip, desk_id){
        return self.make_msg({op:"lobby/game/callbet", 'chip':chip, action:1, deskId:desk_id});
    }

    self.fold = function(desk_id){
        return self.make_msg({op:"lobby/game/callbet", 'chip':0, action:2, deskId:desk_id});
    }

    self.left = function(desk_id){
        return self.make_msg({op:"lobby/game/left", deskId:desk_id});
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

    self.global_game_info = function(desk_id){
        if (desk_id != undefined)
            return self.make_msg({op:"lobby/game/getUserGameInfo", deskId:desk_id});
        return self.make_msg({op:"lobby/game/getUserGameInfo"});
    }
}

function resp_parser(user_info){
    var self = this;
    self.user_info = user_info;
    self.req_maker = new req_maker(user_info);
    self.u = require('./utility');

    self.resp_global_game_info = function(msg_obj){
        mul_desk = {};
        msg_obj.list.forEach(function(item){
            if (item.action.uid == self.user_info.uid){
                var desk = {};
                desk.match_id = item.inc.desk.match_id;
                desk.desk_id = item.inc.desk.desk_id;
                //desk.desk_id = 1;//item.inc.desk.desk_id;
                desk.sequence_id = 0;//item.sequence_id;
                //desk.sequence_id = 0;//item.sequence_id;
                desk.dSeatNum = item.action.seat_num;
                desk.room_type = item.inc.desk.room_type;

                if(item.inc.desk !== undefined){
                    self.set_desk(item.inc.desk, desk);
                }
                if(item.inc.board !== undefined){
                    self.set_board(item.inc.board, desk);
                }
                if(item.inc.players !== undefined){
                    self.set_players(item.inc.players, desk);
                }
                mul_desk[parseInt(item.desk_id)] = desk;
                //mul_desk[1] = desk;
            }
        });
        return mul_desk;
    };

    self.set_desk = function(deskArr, desk){
        desk.dBoardID=deskArr["board_id"];
        desk.dDeskID=deskArr["desk_id"];
        desk.dDeskStatus=deskArr["desk_status"];
        desk.dDeskMaxPerson=deskArr["max_person"];
        desk.dRoomType=deskArr["room_type"];
        desk.dBoardBigBlindChip=parseInt(deskArr["big_blind_chip"]);
        desk.dBoardSmallBlindChip=deskArr["small_blind_chip"];
    }

    self.set_board = function(boardArr, desk){
        desk.dBoardActiveSeatNum=boardArr["active_seat_num"];
        //self.u.yylog(__FILE__, __LINE__, "desk.dBoardMaxRoundChip: "+desk.dBoardMaxRoundChip);
        desk.dBoardMaxRoundChip=parseInt(boardArr["max_round_chip"]);
        //self.u.yylog(__FILE__, __LINE__, "desk.dBoardMaxRoundChip: "+desk.dBoardMaxRoundChip);
        desk.dBoardRaiseChip=parseInt(boardArr["raise_chip"]);
        desk.dBoardBigBlindSeatNum=boardArr["dealer_seat_num"];
        desk.dBoardBigBlindSeatNum=boardArr["big_blind_seat_num"];
        desk.dBoardSmallBlindSeatNum=boardArr["small_blind_seat_num"];
        desk.dBoardTotalChip=boardArr["total_chip"];
        desk.dBoardRound=boardArr["round"];
    }

    self.set_players = function(players, desk){
        desk.dPlayerArr=players;
        desk.dPlayerArr.forEach(function(item, index, arr){
            if (item.uid == self.user_info.uid){
                desk.dUChip=parseInt(item["chip"]);
                desk.dUBetchips=parseInt(item["betchips"]);
                desk.dUserStatus=item["user_status"];
                if(item["hole_card"] !== undefined){
                    desk.dHoleCardArr=item["hole_card"];
                }
            }
        });
    }

    self.resp_sit_down = function(msg_obj, desk){
        if (self.user_info.uid != msg_obj.action.uid){
            return '';
        }
        desk.dSeatNum = msg_obj.action.seat_num;

        if (msg_obj.action.broadcast_type == 0)
        {
            //入座增量包
            desk.dRoomType=msg_obj.action["room_type"];
            desk.dUserStatus=msg_obj.inc["user_status"];
            desk.dUChip=parseInt(msg_obj.inc["chip"]);
            desk.dUBetchips=parseInt(msg_obj.inc["betchips"]);
        }
        else
        {
            //入座全量包
            if (msg_obj.inc.desk != undefined){
                desk.set_desk(msg_obj.inc.desk);
            }

            if (msg_obj.inc.players != undefined){
                desk.set_players(msg_obj.inc.players);
            }
        }
        return '';
    };

    self.resp_start_game = function(msg_obj, desk){
        //if(self.user_info.uid==msg_obj.action["uid"]){
        //    return '';
        //}
        desk.dRoundCntr = 0;
        desk.dHoleCardArr=[];
        desk.dBoardPublicCardArr=[];

        //desk.dSeatNum=msg_obj.action["seat_num"];
        desk.dDeskStatus=msg_obj.inc["desk_status"];
        desk.dBoardBigBlindChip=parseInt(msg_obj.inc["big_blind_chip"]);
        desk.dBoardSmallBlindChip=msg_obj.inc["small_blind_chip"];
        desk.dBoardDealerSeatNum=msg_obj.inc["dealer_seat_num"];
        desk.dBoardBigBlindSeatNum=msg_obj.inc["big_blind_seat_num"];
        desk.dBoardSmallBlindSeatNum=msg_obj.inc["small_blind_seat_num"];
        //self.u.yylog(__FILE__, __LINE__, "desk.dBoardMaxRoundChip: "+desk.dBoardMaxRoundChip);
        desk.dBoardMaxRoundChip=parseInt(msg_obj.inc["max_round_chip"]);
        //self.u.yylog(__FILE__, __LINE__, "desk.dBoardMaxRoundChip: "+desk.dBoardMaxRoundChip);
        desk.dBoardRaiseChip=parseInt(msg_obj.inc["raise_chip"]);
        desk.dBoardID=msg_obj.inc["board_id"];
        msg_obj.inc["players"].forEach(function(item, index, arr){
            if (item.seat_num == desk.dSeatNum){
                desk.dUChip = parseInt(item.chip);
                desk.dUBetchips = parseInt(item.betchips);
            }
        });
        return self.req_maker.global_game_info(msg_obj.desk_id);
    };

    self.resp_get_holdcard = function(msg_obj, desk){
        if(msg_obj.inc["players"][0]["uid"]==self.user_info.uid){
            desk.dHoleCardArr=msg_obj.inc["players"][0]["hole_card"];
            desk.dSeatNum=msg_obj.inc["players"][0]["seat_num"];
        }
        return '';
    };

    self.check = function(desk){
        return self.req_maker.bet(0, desk.desk_id);
    }

    self.fold = function(desk){
        return self.req_maker.fold(desk.desk_id);
    }

    self.raise = function(desk){
        var mul = 0;
        var r = Math.random();
        if (r<1/8){
            mul = 4;
        }else if(r<1/4){
            mul = 3;
        }else if(r<1/2){
            mul = 2;
        }else{
            mul = 1;
        }
        var rand_chip = parseInt(mul)*desk.dBoardBigBlindChip;
        //self.u.yylog(__FILE__, __LINE__, rand_chip, desk.dBoardBigBlindChip, desk.dBoardMaxRoundChip);
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

        //self.u.yylog(__FILE__, __LINE__, chip, desk.dBoardMaxRoundChip, desk.dUBetchips, desk.dBoardBigBlindChip);
        if(chip>desk.dUChip){
            chip=desk.dUChip;
        }
        return self.req_maker.bet(chip, desk.desk_id);
    }

    self.call = function(desk){
        var chip = 0;
        if (desk.dBoardMaxRoundChip > desk.dBoardBigBlindChip){
            chip = desk.dBoardMaxRoundChip - desk.dUBetchips;
        }else{
            chip = desk.dBoardBigBlindChip - desk.dUBetchips;
        }
        if (chip > desk.dUChip){
            chip = desk.dUChip;
        }
        //self.u.yylog(__FILE__, __LINE__, chip, desk.dBoardBigBlindChip);
        var r = Math.random();
        if (r < 1/Math.pow(2, chip/desk.dBoardBigBlindChip)){
            return self.req_maker.bet(chip, desk.desk_id);
        }
        return self.fold(desk);
    }

    self.think = function(desk){
        if(desk.dBoardMaxRoundChip > 0){
            if (desk.dRoundCntr == 1 && desk.dBoardBigBlindSeatNum == desk.dSeatNum){
                var r = Math.random();
                if (r < 0.02){
                    return self.fold(desk);
                }else{
                    return self.check(desk);
                }
            }else{
                var r = Math.random();
                if (r < 0.05){
                    return self.fold(desk);
                }else if(r < 0.9){
                    return self.call(desk);
                }else{
                    return self.raise(desk);
                }
            }
        }else{
            var r = Math.random();
            if(r < 0.9){
                return self.check(desk);
            }else{
                return self.raise(desk);
            }
        }
    }

    self.resp_notify_active_user = function(msg_obj, desk){
        desk.dRoundCntr+=1;

        desk.dBoardActiveSeatNum=msg_obj.inc["active_seat_num"];
        desk.dBoardRaiseChip=parseInt(msg_obj.inc["raise_chip"]);
        //self.u.yylog(__FILE__, __LINE__, "desk.dBoardMaxRoundChip: "+desk.dBoardMaxRoundChip);
        desk.dBoardMaxRoundChip=parseInt(msg_obj.inc["max_round_chip"]);
        //self.u.yylog(__FILE__, __LINE__, "desk.dBoardMaxRoundChip: "+desk.dBoardMaxRoundChip);

        if(self.user_info.uid == msg_obj.action["uid"]){
            return self.think(desk);
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
        if(self.user_info.uid==msg_obj.action["uid"]){
            desk.dUserStatus=msg_obj.action["user_status"];
            desk.dUChip=0;
            desk.dUBetchips=parseInt(msg_obj.inc["betchips"]);
        }
        desk.dBoardTotalChip=msg_obj.inc["total_chip"];
        return '';
    };

    self.resp_bet = function(msg_obj, desk){
        if(self.user_info.uid==msg_obj.action["uid"]){
            desk.dUserStatus=msg_obj.action["user_status"];
            desk.dUChip=parseInt(msg_obj.inc["chip"]);
            desk.dUBetchips=parseInt(msg_obj.inc["betchips"]);
        }
        desk.dBoardTotalChip=msg_obj.inc["total_chip"];
        return '';
    };

    self.resp_raise = function(msg_obj, desk){
        if(self.user_info.uid==msg_obj.action["uid"]){
            desk.dUserStatus=msg_obj.action["user_status"];
            desk.dUChip=parseInt(msg_obj.inc["chip"]);
            desk.dUBetchips=parseInt(msg_obj.inc["betchips"]);
        }
        desk.dBoardTotalChip=msg_obj.inc["total_chip"];
        return '';
    };

    self.resp_notify_side_pot = function(msg_obj, desk){
        desk.dUBetchips=0;
        desk.dBoardRaiseChip=0;
        //self.u.yylog(__FILE__, __LINE__, "desk.dBoardMaxRoundChip: "+desk.dBoardMaxRoundChip);
        desk.dBoardMaxRoundChip=0;
        //self.u.yylog(__FILE__, __LINE__, "desk.dBoardMaxRoundChip: "+desk.dBoardMaxRoundChip);
        return '';
    };

    self.resp_end_board = function(msg_obj, desk){
        if (msg_obj.inc["players"] !== undefined){
            msg_obj.inc["players"].forEach(function(item, index, arr){
                if (item.seat_num == desk.dSeatNum){
                    desk.dUChip = item.chip;
                }
            });
        }
        //return self.end_board(desk);
    };

    self.resp_stand_up = function(msg_obj, desk){
        return '';
    };

    self.resp_change_left = function(msg_obj, desk){
        if(self.user_info.uid==msg_obj.action["uid"]){
            desk.dSequenceID=0;
        }
        return '';
    };

    self.resp_left = function(msg_obj, desk){
        if(self.user_info.uid==msg_obj.action["uid"]){
            desk.dSequenceID=0;
            //ws.close();
            return false;
        }
        return '';
    };

    self.resp_sng_match_end_left = function(msg_obj, desk){
        return self.resp_host_left(msg_obj, desk);
    };

    self.resp_host_left = function(msg_obj, desk){
        if(self.user_info.uid==msg_obj.action["uid"]){
            desk.dSequenceID=0;
            return self.end_board();
        }
        return '';
    };

    self.resp_monitor_sit_down = function(msg_obj, desk){
        return self.resp_host_left(msg_obj, desk);
    };

    self.resp_monitor_stand_up = function(msg_obj, desk){
        return self.resp_host_left(msg_obj, desk);
    };


    self.resp_buy_chip = function(msg_obj, desk){
        if(msg_obj.inc["seat_num"]==desk.dSeatNum){
            desk.dUChip=msg_obj.inc["chip"];
        }
        return '';
    };
}

function game(setting, user_info, game_type, check_status){
    var self = this;
    self.u = require('./utility');
    self.setting = setting;
    self.user_info = user_info;
    self.game_type = game_type;
    self.end_func = check_status;
    self.ws_state = 'init';

    self.resp_parser = new resp_parser(user_info);
    self.pplog = function(filename, linenumber){
        self.u.yylog.apply(null, [filename, linenumber, "uid: " + self.user_info.uid, "room_id: " + self.room_id].concat(Array.prototype.slice.call(arguments, 2)));
    }

    self.open_ws = function(){
        self.ws_path = 'ws://10.0.1.28:7681';
        if (setting.env == 'test'){
            self.ws_path = 'ws://10.0.1.28:7682';
        }
        if(setting.env == 'release'){
            self.ws_path = 'ws://ws.999com.com:17681';
        }
        var WebSocket = require('ws');
        self.ws = new WebSocket(self.ws_path);

        self.ws.on('open', function (){
            self.pplog(__FILE__, __LINE__, 'ws open');
            self.ws_state = 'open';
            if (self.first_req){
                self.ws_send(self.first_req);
            }
        });
        self.ws.on('close', function (){
            self.pplog(__FILE__, __LINE__, 'ws closed');
            self.ws_state = 'closed';
            clearInterval(self.check_idle_id);
        });
        self.ws.on('message', function (data, flags) {
            self.play(data);
        });
    }

    self.open_ws();
    self.first_req = '';
    self.start_game = function(room_id, duration){
        self.room_id = room_id;

        self.last_send = 2000000000000;
        self.check_idle_id = setInterval(self.check_idle, 60*1000);
        var sng_room_type = 2;
        var normal_room_type = 1;
        var champion_room_type = 4;
        if (self.game_type == 'normal'){
            self.first_req = self.sit_down(self.room_id, normal_room_type);
        }else if(self.game_type == 'sng'){
            self.first_req = self.sit_down(self.room_id, sng_room_type);
        }else if(self.game_type == 'champion'){
            var match_id = self.room_id;
            self.first_req = self.join_match(match_id, champion_room_type);
        }else{
            self.pplog(__FILE__, __LINE__, "unknown game_type");
            return;
        }
        self.pplog(__FILE__, __LINE__, "start game", "duration: " + duration);
        var fd = new Date();
        self.service_end_time = parseInt(fd.getTime()/1000) + duration;
    }

    self.sit_down = function(room_id, room_type){
        return self.resp_parser.req_maker.sit_down(room_id, room_type);
    }

    self.join_match = function(match_id, room_type){
        return self.resp_parser.req_maker.sit_down(match_id, room_type);
    }

    self.global_game_info = function(desk_id){
        return self.resp_parser.req_maker.global_game_info(desk_id);
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
    self.simple_parse = function(msg_obj, desk){
        self.pplog(__FILE__, __LINE__, "action: "+ msg_obj.action.action, "dDeskID: " + desk.dDeskID, "dBoardID: " + desk.dBoardID);
        self.pplog(__FILE__, __LINE__, "left chip: " + desk.dUChip, "big blind: " + desk.dBoardBigBlindChip,
                "board raise chip: " + desk.dBoardRaiseChip, "max round chip: " + desk.dBoardMaxRoundChip, 
                "last bet chip:" + desk.dUBetchips);
        if (msg_obj.retCode !== 0){
            self.pplog(__FILE__, __LINE__, 'retcode: ' + msg_obj.retCode);
            return self.global_game_info(desk.dDeskID);
        }

        var req_msg;
        var action = msg_obj.action.action;
        switch(action)
        {
            case self.LC_BROADCAST_ACTION_TYPE_EMOTICON:
            case self.LC_BROADCAST_ACTION_TYPE_CHARACTER:
                break;
            case self.LC_BROADCAST_ACTION_TYPE_SITDOWN:
                req_msg = self.resp_parser.resp_sit_down(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_START_GAME:
                req_msg = self.resp_parser.resp_start_game(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_HOLE_CARD:
                req_msg = self.resp_parser.resp_get_holdcard(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_NOTICE_ACTIVE_USER:
                req_msg = self.resp_parser.resp_notify_active_user(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_FOLD:
                req_msg = self.resp_parser.resp_fold(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_CHECK:
                req_msg = self.resp_parser.resp_check(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_ALLIN:
                req_msg = self.resp_parser.resp_allin(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_CALL:
            case self.LC_BROADCAST_ACTION_TYPE_BET:
                req_msg = self.resp_parser.resp_bet(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_RAISE:
                req_msg = self.resp_parser.resp_raise(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_NOTICE_SIDE_POT:
                req_msg = self.resp_parser.resp_notify_side_pot(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_END_BOARD:
                req_msg = self.resp_parser.resp_end_board(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_STANDUP:
                req_msg = self.resp_parser.resp_stand_up(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_CHANGE_DESK:
                req_msg = self.resp_parser.resp_change_left(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM:
                req_msg = self.resp_parser.resp_left(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SNG_MATCH_END:
                req_msg = self.resp_parser.resp_sng_match_end_left(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_HOSTING:
                req_msg = self.resp_parser.resp_host_left(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SIT_DOWN_MONITOR:
                req_msg = self.resp_parser.resp_monitor_sit_down(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_STANDUP_MONITOR:
                req_msg = self.resp_parser.resp_monitor_stand_up(msg_obj, desk);
                break;
            case self.LC_BROADCAST_ACTION_TYPE_INIT_USERGAMEINFO:
                self.pplog(__FILE__, __LINE__, "LC_BROADCAST_ACTION_TYPE_INIT_USERGAMEINFO here");
                break;
            case self.LC_BROADCAST_ACTION_TYPE_EXCHANGE_CHIP:
                req_msg = self.resp_parser.resp_buy_chip(msg_obj, desk);
                break;
            default:
                break;
        }
        return req_msg;
    }

    //board end
    //game end
    //need to delay
    self.parse = function(msg){
        //self.pplog(__FILE__, __LINE__, msg);
        var msg_obj;
        try{
            msg_obj = JSON.parse(msg);
        }catch(e){
            self.pplog(__FILE__, __LINE__, 'exception: ' + e);
            self.ws_send(self.global_game_info());
            return false;
        }
        if (msg_obj.retCode !== 0){
            self.pplog(__FILE__, __LINE__, 'retcode: ' + msg_obj.retCode);
            //self.ws.close();
            //self.end_func(self.user_info.uid, self.room_id, 'retcode: ' + msg_obj.retCode);
            self.ws_send(self.resp_parser.req_maker.left(msg_obj.desk_id));
            return false;
        }
        var action = msg_obj.action.action;
        if (action == self.LC_BROADCAST_ACTION_TYPE_INIT_USERGAMEINFO) {
            var t_list = self.resp_parser.resp_global_game_info(msg_obj);
            for (k in t_list){
                self.desk_list[k] = t_list[k];
            }
            return true;
        }

        var desk_id = parseInt(msg_obj.desk_id);
        var desk = self.desk_list[desk_id];
        if (desk == undefined){
            self.pplog(__FILE__, __LINE__, "no desk for desk_id: " + desk_id);
            self.ws_send(self.global_game_info(desk_id));
            return false;
        } 

        if (msg_obj.sequence_id === undefined){
            self.pplog(__FILE__, __LINE__, 'no sequence_id');
            self.ws_send(self.global_game_info(desk.dDeskID));
            return false;
        }
        if (desk.sequence_id != 0){
            if(parseInt(msg_obj.sequence_id) <= parseInt(desk.sequence_id)){
                self.pplog(__FILE__, __LINE__, 'incremently sequence_id error: ' + msg_obj.sequence_id + ' vs ' + desk.sequence_id);
                self.ws_send(self.global_game_info(desk.dDeskID));
                return false;
            }
            if(parseInt(msg_obj.sequence_id) > parseInt(desk.sequence_id) + 1){
                self.pplog(__FILE__, __LINE__, msg_obj.sequence_id + ' vs ' + parseInt(desk.sequence_id));
                self.ws_send(self.global_game_info(desk.dDeskID));
                return false;
            }
        }
        desk.sequence_id = parseInt(msg_obj.sequence_id);

        if (action == self.LC_BROADCAST_ACTION_TYPE_CHARACTER
                || action == self.LC_BROADCAST_ACTION_TYPE_EMOTICON) {
            self.pplog(__FILE__, __LINE__, 'action: ' + action + ' emotion or chat');
            return true;
        }
        if (action == self.LC_BROADCAST_ACTION_TYPE_END_BOARD
                || action == self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_HOSTING
                || action == self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_STANDUP_MONITOR
                || action == self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SIT_DOWN_MONITOR){//board end
            self.resp_parser.resp_end_board(msg_obj, desk);
            self.pplog(__FILE__, __LINE__, 'board end', 'action: ' + action);
            if (self.game_type == 'normal'){
                var now = new Date();
                self.pplog(__LINE__, __FILE__, "should stop at: " + now.toISOString().replace(/T/, ' ').replace(/Z/, ''));
                var ts = parseInt(now.getTime()/1000);
                if (ts > self.service_end_time){
                    self.ws_send(self.resp_parser.req_maker.left(desk.desk_id));
                    return false;
                }else{
                    return true;
                }
            }else if(self.game_type == 'sng'){
                var match_rank = msg_obj.inc.match_rank;
                var lose = false;
                for (rank in match_rank){
                    match_rank[rank].forEach(function(loser){
                        if (loser.uid == self.user_info.uid){
                            lose = true;
                        }
                    });
                }
                if (lose){
                    self.pplog(__FILE__, __LINE__, "failed, will left");
                    self.ws_send(self.resp_parser.req_maker.left(desk.desk_id));
                    return false;
                }
                return true;
            }else{
                self.pplog(__FILE__, __LINE__, "unknown game_type: " + self.game_type);
            }
        } else if (action == self.LC_BROADCAST_ACTION_TYPE_STANDUP || action == self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM){
            if (self.user_info.uid != msg_obj.action["uid"]){
                return false;
            }
            self.pplog(__FILE__, __LINE__, 'action: ' + action, ' LC_BROADCAST_ACTION_TYPE_STANDUP or LC_BROADCAST_ACTION_TYPE_LEFT_ROOM, close');
            delete self.desk_list.desk_id;
            self.ws.close();
            self.end_func(self.user_info.uid, self.room_id, 'action: ' + action + ", exit from: " + self.room_id);
            return true;
        } else if (action == self.LC_BROADCAST_ACTION_TYPE_LEFT_ROOM_SNG_MATCH_END){//game end
            if (self.game_type != 'sng'){
                return true;
            }
            self.ws.close();
            self.end_func(self.user_info.uid, self.room_id, 'exit from sng match: ' + self.room_id);
        } else if (action == self.LC_BROADCAST_ACTION_TYPE_NOTICE_ACTIVE_USER){//need to delay
            self.sleep = 0;
            var r = Math.random();
            if(r<0.1){
                self.sleep = 1;
            }else if(r<0.45){
                self.sleep = 3;
            }else if(r<0.85){
                self.sleep = 5;
            }else if(r<0.95){
                self.sleep = 7;
            }else{
                self.sleep = 8;
            }
            self.delay_send_req(self.simple_parse(msg_obj, desk), self.sleep);
            return true;
        } else {
            self.ws_send(self.simple_parse(msg_obj, desk));
            return true;
        }
    }
    
    self.play = function(msg){
        var ret_code = self.parse(msg);
    }

    self.delay_send_req = function(req, delay){
        setTimeout(self.ws_send, delay*1000, req);
    }

    self.ws_send = function(msg){
        if (!msg){
            return false;
        }
        if (self.ws_state != 'open'){
            self.pplog(__FILE__, __LINE__, "ws not open");
            return false;
        }
        //self.pplog(__FILE__, __LINE__, msg);
        self.ws.send(msg, {binary:false, mask: true}, function(err){
            if(err){
                self.pplog(__FILE__, __LINE__, "ws send error: " + err);
                self.ws.close();
                self.end_func(self.user_info.uid, self.room_id, 'exit for ws error: ' + err);
            }
        });
        self.last_send = new Date().getTime();
    }

    self.check_idle = function(){
        var ts = new Date().getTime();
        if (ts - self.last_send > 540*1000){
            self.pplog(__FILE__, __LINE__, "idle for too long: " + (ts - self.last_send)/1000);
            if (self.game_type == 'normal'){
                self.ws.close();    
                self.end_func(self.user_info.uid, self.room_id, "idle for too long: " + (ts - self.last_send)/1000);
            }else if (self.game_type == 'sng'){
                self.ws_send(self.global_game_info());
            }else{
                self.pplog(__FILE__, __LINE__, "unknown game_type: " + self.game_type);
            }
        }
    }
}

