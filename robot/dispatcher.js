module.exports = dispatcher;

function dispatcher(setting){
    var self = this;
    self.setting = setting;
    var u = require('./utility');
    var authenticator = require('./http_login');
    self.loginer = new authenticator(self.setting.env);
    self.roomer = new authenticator(self.setting.env);
    self.fs = require('fs');

    self.sync_user_info = function(robot_list){
        u.yylog(__FILE__, __LINE__);
        var file_data = JSON.stringify(robot_list, null, 2);
        self.fs.writeFile(self.setting.user_file, file_data, function (err) {
            if (err) throw err;
            u.yylog(__FILE__, __LINE__, 'robot user file synced');
        });
        setTimeout(self.sync_user_info, 31*60*1000, robot_list);
    }

    self.robot2room = {};
    self.room2robot = {};
    self.pairing = false;
    self.pair_robot = function (){
        //u.yylog(__FILE__, __LINE__);
        if (self.game_type == 'sng'){
            var num = 0;
            for(k in self.robot2room){
                num++;
            }
            if (num > 20){
                u.yylog(__FILE__, __LINE__, "sng robot num: " + num);
                return false;
            }
        }
        if (self.pairing == true){
            setTimeout(self.pair_robot, 107*1000);
            return true;
        }
        self.pairing = true;
        var robot_pass = 'ASD123@$888qwe';
        self.room_list = u.shuffle(self.room_list);
        self.room_list.forEach(function(room){
            if (self.room2robot[room.id] == undefined){
                self.room2robot[room.id] = [];
            }
            if (self.room2robot[room.id].length >= self.quota[room.id]){
                return false;
            }
            self.robot_list = u.shuffle(self.robot_list);
            self.robot_list.forEach(function(robot, idx, arr){
                if (self.room2robot[room.id].length >= self.quota[room.id]){
                    return true;
                }
                if (self.setting.game_type == 'normal' && robot[2] < room.min_chip){
                    return false;
                }
                if (self.setting.game_type == 'sng'){
                    if (room.apply_chip >= 10000 || robot[2] < room.apply_chip){
                        return false;
                    }
                }
                if (self.robot2room[robot[0]] != undefined){
                    return false;
                }
                self.robot2room[robot[0]] = room.id;
                if (self.room2robot[room.id] == undefined){
                    self.room2robot[room.id] = [];
                }
                self.room2robot[room.id].push(robot[0]);
                u.yylog(__FILE__, __LINE__, robot[1], 'login');
                self.loginer.login(robot[1], robot_pass, '', function(user_info){
                    var my_user_info = u.clone(user_info);
                    self.robot_list.forEach(function(robot, idx, arr){
                        if (robot[0] == my_user_info.uid){
                            arr[idx][2] = my_user_info.game_money;
                        }
                    });
                    var game = require('./champion_player');
                    var player = new game(self.setting, my_user_info, self.setting.game_type, function(uid, room_id, exit_reason){
                        u.yylog(__FILE__, __LINE__, "uid: " + uid + " exit room: " + room_id + " for reason: " + exit_reason);
                        if (!(exit_reason == 'sng start' && self.game_type == 'sng')){
                            delete self.robot2room[uid];
                        }
                        var t_arr = self.room2robot[room_id].filter(function(item){
                            return uid != item;
                        });
                        self.room2robot[room_id] = t_arr;
                        u.yylog(__FILE__, __LINE__, JSON.stringify(self.room2robot));
                        u.yylog(__FILE__, __LINE__, JSON.stringify(self.robot2room));
                    });
                    player.start_game(self.robot2room[user_info.uid], parseInt(60*(3+27*Math.random())));
                });
            });
        });
        u.yylog(__FILE__, __LINE__, JSON.stringify(self.room2robot));
        u.yylog(__FILE__, __LINE__, JSON.stringify(self.robot2room));
        setTimeout(self.pair_robot, 77*1000);
        self.pairing = false;
    }

    self.room_list = {};
    self.started = false;
    self.quota = {};
    self.room_success = function (room_list){
        self.room_list = room_list;
        self.room_list.forEach(function(room){
            if (self.setting.game_type == 'normal'){
                self.quota[room.id] = parseInt(2+3*Math.random());
            }else{
                self.quota[room.id] = room.desk_person-1;
            }
        });
        if (self.started == false){
            self.pair_robot();
            self.started = true;
        }
    }

    self.refresh_room_list = function(){
        self.roomer.get_game_room(self.setting.game_type, '18682006183', u.md5('yzq123456'), '', self.room_success);
        setTimeout(self.refresh_room_list, 7*60*1000);
    }

    self.run = function(){
        var file_data = self.fs.readFileSync(self.setting.user_file);
        self.robot_list = JSON.parse(file_data);
        self.refresh_room_list();
        setTimeout(self.sync_user_info, 31*60*1000, self.robot_list);
    }
}

