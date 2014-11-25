function normal_dispatcher(setting){
    var self = this;
    self.setting = setting;
    var u = require('./utility');
    var authenticator = require('./http_login');
    self.loginer = new authenticator(self.setting.env);
    self.roomer = new authenticator(self.setting.env);
    var game_module = require('./champion_player');
    
    self.sync_user_info = function(robot_list){
        var now = new Date();
        console.log(__LINE__, now.toString());
        var file_data = JSON.stringify(robot_list, null, 2);
        fs.writeFile(self.setting.user_file, file_data, function (err) {
            if (err) throw err;
            console.log(__LINE__, 'robot user file synced');
        });
        setTimeout(self.sync_user_info, 31*60*1000, robot_list);
    }

    self.robot2room = {};
    self.room2robot = {};
    self.pair_robot = function (){
        var now = new Date();
        console.log(__LINE__, now.toString());
        //var robot_pass = 'ASD123@$888qwe';
        var robot_pass = '123456';
        self.room_list.forEach(function(room){
            //console.log(__LINE__, room);
            if (self.room2robot[room.id] == undefined){
                self.room2robot[room.id] = [];
            }
            if (self.room2robot[room.id].length >= 5){
                return false;
            }
            for (k in self.robot_list){
                robot = self.robot_list[k];
                if (self.room2robot[room.id].length >= 5){
                    break;
                }
                if (robot[1] < room.min_chip){
                    continue;
                }
                if (self.robot2room[k] != undefined){
                    continue;
                }
                self.robot2room[k] = room.id;
                if (self.room2robot[room.id] == undefined){
                    self.room2robot[room.id] = [];
                }
                self.room2robot[room.id].push(k);
                self.loginer.login(robot[0], u.md5(robot_pass), '', function(user_info){
                    self.robot_list[k][1] = user_info.game_money;
                    var player = new game_module.game(self.setting, user_info, 'normal', function(uid, room_id){
                        delete self.robot2room[uid];
                        var t_arr = self.room2robot[room_id].filter(function(item){
                            return uid != item;
                        });
                        self.room2robot[room_id] = t_arr;
                    });
                    //var duration = parseInt(600*(1 + Math.random()));
                    var duration = parseInt(150*(1 + Math.random()));
                    player.start_game(room.id, duration);
                });
            }
        });
        console.log(__LINE__, self.room2robot, self.robot2room);
        setTimeout(self.pair_robot, 3*60*1000);
    }

    self.room_list = {};
    self.started = false;
    self.normal_room_success = function (room_list){
        //console.log(__LINE__, room_list);
        self.room_list = room_list;
        if (self.started == false){
            self.pair_robot();
            self.started = true;
        }
    }

    self.refresh_room_list = function(){
        var now = new Date();
        console.log(__LINE__, now.toString());
        self.roomer.get_game_room('normal', '18682006183', u.md5('123456'), '', self.normal_room_success);
        setTimeout(self.refresh_room_list, 7*60*1000);
    }

    self.run = function(){
        var fs = require('fs');
        var file_data = fs.readFileSync(self.setting.user_file);
        self.robot_list = JSON.parse(file_data);
        self.refresh_room_list();
        setTimeout(self.sync_user_info, 31*60*1000, self.robot_list);
    }
}

function main(){
    var setting = {};
    setting.env = process.argv[2];
    setting.game_type = process.argv[3];
    setting.user_file = process.argv[4];
    var dispatcher =  new normal_dispatcher(setting);
    dispatcher.run();
}

main();
