//exports.ws_player = ws_player;
require('./utility');
module.exports = ws_player;

function msg_maker(user_info)
{
    var self = this;
    self.u = require('./utility');
    self.user_info = user_info;

    self.make_msg = function (mo){
        mo.session_id = self.user_info.session_id;
        mo.from_where = 2000;
        mo.uid = parseInt(self.user_info.uid);
        
        var smo = self.u.sortObject(mo);
        var k, to_sign='';
        for (k in smo){
            to_sign += k + smo[k];
        }
        to_sign += self.user_info.md5key;

        mo.urlsign = self.u.md5(to_sign);
        return JSON.stringify(mo);
    }

    self.bet = function(chip){
        return self.make_msg({op:"lobby/game/callbet", 'chip':chip, action:1});
    }

    self.fold = function(){
        return self.make_msg({op:"lobby/game/callbet", 'chip':0, action:2});
    }

    self.change_desk = function(){
        return self.make_msg({op:"lobby/game/changedesk"});
    }

    self.left = function(){
        return self.make_msg({op:"lobby/game/left"});
    }

    self.sit_down = function(room_id){
        return self.make_msg({op:"lobby/game/sitdown",roomId:room_id});
    }

    self.global_game_info = function(){
        return self.make_msg({op:"lobby/game/getUserGameInfo"});
    }
}

function msg_handler(user_info, ws){
    var self = this;
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

    var fd=new Date('2015-01-01');
    self.service_end_time = parseInt(fd.getTime()/1000);
    self.user_info = user_info;
    self.maker = new msg_maker(self.user_info);
    self.sequence_id = 0;
    self.parse = function (msg){
        var msg_obj;
        try{
            msg_obj = JSON.parse(msg);
        }catch(e){
            console.log('exception: ' + e);
            ws.close();
        }
        //console.log(JSON.stringify(msg_obj, null, 2));
        console.log(self.sequence_id + " vs "  +msg_obj.sequence_id);
        console.log(__LINE__, self.dBoardBigBlindChip, self.dBoardRaiseChip, self.dBoardMaxRoundChip, self.dUBetchips, self.dUChip);
        console.log("action: "+msg_obj.action.action);
        if (msg_obj.retCode !== 0){
            console.log('retcode: ' + msg_obj.retCode);
            ws.close();
        }else{
            if (msg_obj.sequence_id === undefined){
                console.log('no sequence_id');
                ws.close();
            }else{
                var action = msg_obj.action.action;
                var req_msg = '';
                if(action == self.LC_BROADCAST_ACTION_TYPE_INIT_USERGAMEINFO){
                    if (parseInt(msg_obj.sequence_id) < parseInt(self.sequence_id)){
                        console.log('full sequence_id error: ' + msg_obj.sequence_id + ' vs ' + self.sequence_id);
                        ws.close();
                    }else{
                        self.sequence_id = parseInt(msg_obj.sequence_id);
                        req_msg = self.resp_global_game_info(msg_obj);
                    }
                }else{//incremently
                    if (self.sequence_id === 0){
                        //console.log(__LINE__);
                        req_msg  = self.global_game_info();
                    }else if(parseInt(msg_obj.sequence_id) <= parseInt(self.sequence_id)){
                        //console.log(__LINE__);
                        console.log('incremently sequence_id error: ' + msg_obj.sequence_id + ' vs ' + self.sequence_id);
                        ws.close();
                    }else if(parseInt(msg_obj.sequence_id) > parseInt(self.sequence_id) + 1){
                        //console.log(msg_obj.sequence_id, " vvss ", parseInt(self.sequence_id) + 1, parseInt(msg_obj.sequence_id) > parseInt(self.sequence_id) + 1);
                        //console.log(__LINE__);
                        req_msg  = self.global_game_info();
                    }else{
                        //console.log(__LINE__);
                        self.sequence_id = parseInt(msg_obj.sequence_id);
                        //console.log("self.sequence_id updated: " + self.sequence_id);
                        switch(action)
                        {
                            case self.LC_BROADCAST_ACTION_TYPE_EMOTICON:
                            case self.LC_BROADCAST_ACTION_TYPE_CHARACTER:
                                console.log('emotion or chat');
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
                                //console.log('------------'+self.service_end_time);
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
                                req_msg = self.resp_global_game_info(msg_obj);
                                break;
                            case self.LC_BROADCAST_ACTION_TYPE_EXCHANGE_CHIP:
                                req_msg = self.resp_buy_chip(msg_obj);
                                break;
                            default:
                                break;
                        }
                    }
                }
                if (req_msg !== ''){
                    var sleep = 0;
                    if (action == self.LC_BROADCAST_ACTION_TYPE_NOTICE_ACTIVE_USER){
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
                    }
                    setTimeout(self.send_msg, sleep*1000, req_msg);
                }
            }
        }
    }

    self.send_msg = function(msg){
        if (!msg){
            console.log(__LINE__, "error msg: " + msg);
            return false;
        }
        //console.log('send :' + JSON.stringify(JSON.parse(msg), null, 2));
        console.log('send :' + msg);
        ws.send(msg, {binary:false, mask: true}, function(err){
            if(err){
                console.log("ws send error: " + err);
                ws.close();
            }
        });
    }

    self.set_desk = function(desk){
        var deskArr=desk;;
        self.dDeskID=deskArr["desk_id"];
        self.dDeskStatus=deskArr["desk_status"];
        self.dDeskMaxPerson=deskArr["max_person"];
        self.dRoomType=deskArr["room_type"];
    }

    self.set_board = function(board){
        var boardArr=board;
        self.dBoardActiveSeatNum=boardArr["active_seat_num"];
        self.dBoardMaxRoundChip=boardArr["max_round_chip"];
        self.dBoardRaiseChip=boardArr["raise_chip"];
        self.dBoardBigBlindSeatNum=boardArr["dealer_seat_num"];
        self.dBoardBigBlindSeatNum=boardArr["big_blind_seat_num"];
        self.dBoardSmallBlindSeatNum=boardArr["small_blind_seat_num"];
        self.dBoardBigBlindChip=boardArr["big_blind_chip"];
        self.dBoardSmallBlindChip=boardArr["small_blind_chip"];
        self.dBoardTotalChip=boardArr["total_chip"];
        self.dBoardRound=boardArr["round"];
    }

    self.set_players = function(players){
        self.dPlayerArr=players;
        //console.log(__LINE__);
        //console.log(JSON.stringify(players, null, 2));
        self.dPlayerArr.forEach(function(item, index, arr){
            //console.log(__LINE__);
            //console.log(item.uid,self.user_info.uid,self.dUBetchips);
            if (item.uid == self.user_info.uid){
                self.dUChip=item["chip"];
                self.dUBetchips=item["betchips"];
                self.dUserStatus=item["user_status"];
                if(item["hole_card"] !== undefined){
                    self.dHoleCardArr=item["hole_card"];
                }
            }
        });
        //console.log(__LINE__);
        //console.log(self.dUBetchips);
    }

    self.resp_sit_down = function(msg_obj){
        if (self.user_info != msg_obj.action.uid){
            return '';
        }
        self.dSeatNum = msg_obj.action.seat_num;
        //console.log(__LINE__, JSON.stringify(msg_obj, null, 2));

        if (msg_obj.action.broadcast_type == 0)
        {
            //入座增量包
            self.dRoomType=msg_obj.action["room_type"];
            self.dUserStatus=msg_obj.inc["user_status"];
            self.dUChip=msg_obj.inc["chip"];
            self.dUBetchips=msg_obj.inc["betchips"];
        }
        else
        {
            //入座全量包
            if (msg_obj.inc.desk != undefined){
                self.set_desk(msg_obj.inc.desk);
            }

            if (msg_obj.inc.players != undefined){
                self.set_players(msg_obj.inc.players);
            }
        }
        return '';
    };

    self.resp_start_game = function(msg_obj){
        self.dHoleCardArr=[];
        self.dBoardPublicCardArr=[];

        //console.log(__LINE__, JSON.stringify(msg_obj, null, 2));
        self.dDeskStatus=msg_obj.inc["desk_status"];
        self.dBoardBigBlindChip=msg_obj.inc["big_blind_chip"];
        self.dBoardSmallBlindChip=msg_obj.inc["small_blind_chip"];
        self.dBoardDealerSeatNum=msg_obj.inc["dealer_seat_num"];
        self.dBoardBigBlindSeatNum=msg_obj.inc["big_blind_seat_num"];
        self.dBoardSmallBlindSeatNum=msg_obj.inc["small_blind_seat_num"];
        self.dBoardMaxRoundChip=msg_obj.inc["max_round_chip"];
        self.dBoardRaiseChip=msg_obj.inc["raise_chip"];
        msg_obj.inc["players"].forEach(function(item, index, arr){
            if (item.seat_num == self.dSeatNum){
                self.dUChip = item.chip;
                self.dUBetchips = item.betchips;
            }
        });
        return self.maker.global_game_info();
    };

    self.resp_get_holdcard = function(msg_obj){
        if(msg_obj.inc["players"][0]["uid"]==self.user_info.uid){
            self.dHoleCardArr=msg_obj.inc["players"][0]["hole_card"];
        }
        return '';
    };

    self.check = function(){
        //console.log(__LINE__);
        return self.maker.bet(0);
    }

    self.fold = function(){
        //console.log(__LINE__);
        return self.maker.fold();
    }

    self.raise = function(){
        //console.log(__LINE__);
        var rand_chip = parseInt(1+9*Math.random())*self.dBoardBigBlindChip;
        var chip = 0;
        if(self.dBoardRaiseChip == 0){
            if(self.dBoardMaxRoundChip<=self.dBoardBigBlindChip){
                chip = self.dBoardBigBlindChip-self.dUBetchips+self.dBoardBigBlindChip+rand_chip;
            }else if(self.dBoardMaxRoundChip > self.dBoardBigBlindChip){
                chip = self.dBoardMaxRoundChip-self.dUBetchips+self.dBoardBigBlindChip+rand_chip;
            }
        }else{
            chip = self.dBoardMaxRoundChip-self.dUBetchips+self.dBoardRaiseChip+rand_chip;
        }

        if(chip>self.dUChip){
            chip=self.dUChip;
        }
        return self.maker.bet(chip);
    }

    self.call = function(){
        //console.log(__LINE__);
        //console.log(__LINE__, self.dBoardBigBlindChip, self.dBoardRaiseChip, self.dBoardMaxRoundChip, self.dUBetchips, self.dUChip);
        var chip = 0;
        if (self.dBoardMaxRoundChip > self.dBoardBigBlindChip){
            chip = self.dBoardMaxRoundChip - self.dUBetchips;
        }else{
            chip = self.dBoardBigBlindChip - self.dUBetchips;
        }
        if (chip > self.dUChip){
            chip = self.dUChip;
        }
        //console.log(__LINE__, chip);
        return self.maker.bet(chip);
    }

    self.think = function(){
        var r = Math.random();
        if(r<0.4){
            if (self.dBoardMaxRoundChip > 0){
                return self.call();
            }else{
                //console.log(__LINE__);
                return self.check();
            }
        }else if(r<0.6){
            return self.call();
        }else if(r<0.95){
            if (self.dBoardMaxRoundChip > 0){
                return self.fold();
            }else{
                //console.log(__LINE__);
                return self.check();
            }
        }else{
            return self.raise();
        }
    }

    self.resp_notify_active_user = function(msg_obj){
        //console.log(JSON.stringify(msg_obj, null, 2));

        self.dBoardActiveSeatNum=msg_obj.inc["active_seat_num"];
        self.dBoardRaiseChip=msg_obj.inc["raise_chip"];
        self.dBoardMaxRoundChip=msg_obj.inc["max_round_chip"];

        if(self.user_info.uid == msg_obj.action["uid"]){
            return self.think();
        }
        return '';
    };

    self.resp_fold = function(msg_obj){
        if(self.user_info.uid==msg_obj.action["uid"]){
            self.dUserStatus=msg_obj.action["user_status"];
        }
        return '';
    };

    self.resp_check = function(msg_obj){
        if(self.user_info.uid==msg_obj.action["uid"]){
            self.dUserStatus=msg_obj.action["user_status"];
        }
        return '';
    };

    self.resp_allin = function(msg_obj){
        //console.log(__LINE__, JSON.stringify(msg_obj, null, 2));
        if(self.user_info.uid==msg_obj.action["uid"]){
            self.dUserStatus=msg_obj.action["user_status"];
            self.dUChip=0;
            self.dUBetchips=msg_obj.inc["betchips"];
        }
        self.dBoardTotalChip=msg_obj.inc["total_chip"];
        return '';
    };

    self.resp_bet = function(msg_obj){
        //console.log(__LINE__, JSON.stringify(msg_obj, null, 2));
        if(self.user_info.uid==msg_obj.action["uid"]){
            self.dUserStatus=msg_obj.action["user_status"];
            self.dUChip=msg_obj.inc["chip"];
            self.dUBetchips=msg_obj.inc["betchips"];
        }
        self.dBoardTotalChip=msg_obj.inc["total_chip"];
        return '';
    };

    self.resp_raise = function(msg_obj){
        //console.log(__LINE__, JSON.stringify(msg_obj, null, 2));
        if(self.user_info.uid==msg_obj.action["uid"]){
            self.dUserStatus=msg_obj.action["user_status"];
            self.dUChip=msg_obj.inc["chip"];
            self.dUBetchips=msg_obj.inc["betchips"];
        }
        self.dBoardTotalChip=msg_obj.inc["total_chip"];
        return '';
    };

    self.resp_notify_side_pot = function(msg_obj){
        //console.log(__LINE__, JSON.stringify(msg_obj, null, 2));
        self.dUBetchips=0;
        self.dBoardRaiseChip=0;
        self.dBoardMaxRoundChip=0;
        return '';
    };

    self.end_board = function(){
        var d = new Date();
        var ts = (d.getTime()-d.getMilliseconds())/1000;
        if (ts > self.service_end_time){
            //console.log('------------------left');
            return self.maker.left();
        }else{
            if (self.dRoomType == 1){
                //if (Math.random()<0.1){
                //    return self.maker.change_desk();
                //}
                return '';
            }
            //console.log('------------------sit_down');
            return self.maker.sit_down(self.dRoomId);
        }
    }

    self.resp_end_board = function(msg_obj){
        if (msg_obj.inc["players"] !== undefined){
            msg_obj.inc["players"].forEach(function(item, index, arr){
                if (item.seat_num == self.dSeatNum){
                    self.dUChip = item.chip;
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
            self.dSequenceID=0;
        }
        return '';
    };

    self.resp_left = function(msg_obj){
        if(self.user_info.uid==msg_obj.action["uid"]){
            self.dSequenceID=0;
            ws.close();
        }
        return '';
    };

    self.resp_sng_match_end_left = function(msg_obj){
        return self.resp_host_left(msg_obj);
    };

    self.resp_host_left = function(msg_obj){
        if(self.user_info.uid==msg_obj.action["uid"]){
            self.dSequenceID=0;
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

    self.resp_global_game_info = function(msg_obj){
        if (msg_obj.action.uid != self.user_info.uid){
            return '';
        }
        self.dSeatNum = msg_obj.action.seat_num;
        if(msg_obj.inc.desk !== undefined){
            self.set_desk(msg_obj.inc.desk);
        }
        if(msg_obj.inc.board !== undefined){
            //console.log(JSON.stringify(msg_obj.inc.board, null, 2));
            self.set_board(msg_obj.inc.board);
        }
        if(msg_obj.inc.players !== undefined){
            self.set_players(msg_obj.inc.players);
        }
        return '';
    };

    self.resp_buy_chip = function(msg_obj){
        if(msg_obj.inc["seat_num"]==self.dSeatNum){
            self.dUChip=msg_obj.inc["chip"];
        }
        return '';
    };

    self.global_game_info = function(){
        return self.maker.global_game_info();
    };
}

function ws_player(env, user_info, room_info){
    var self = this;
    self.ws_path = 'ws://10.0.1.28:7681';
    if (env == 'test'){
        self.ws_path = 'ws://10.0.1.28:7682';
    }
    if(env == 'release'){
        self.ws_path = 'ws://ws.999com.com:17681';
    }
    self.user_info = user_info;
    self.room_info = room_info;

    self.choose_room = function (user_info, room_info){
        var rooms = room_info.filter(function(item, index, arr){
            return item.match != 'sng' && item.min_chip<10000;
        });
        var item = rooms[Math.floor(Math.random()*rooms.length)];
        return item;
    }

    self.play = function (){
        var room = self.choose_room(self.user_info, self.room_info);
        var room_id = room.id;

        var WebSocket = require('ws');
        var ws = new WebSocket(self.ws_path);
        var maker = new msg_maker(self.user_info);
        var handler = new msg_handler(self.user_info, ws);
        ws.on('open', function (){
            var msg = maker.sit_down(room_id);
            console.log('send :' + msg);
            ws.send(msg, {binary:false, mask: true}, function(err){
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
            handler.parse(data);
        });
    }
}

