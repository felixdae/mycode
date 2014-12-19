module.exports = match;

function match(setting){
    var self = this;
    self.setting = setting;
    var u = require('./utility');
    var authenticator = require('./http_login');
    self.loginer = new authenticator(self.setting.env);
    self.fs = require('fs');

    self.run = function(){
        var robot_pass = 'ASD123@$888qwe';
        var file_data = self.fs.readFileSync(self.setting.user_file);
        self.robot_list = JSON.parse(file_data);
        self.robot_list = u.shuffle(self.robot_list);
        self.robot_list = self.robot_list.slice(0, self.setting.limit);
        self.robot_list.forEach(function(robot){
            self.loginer.apply(robot[1], robot_pass, '', self.setting.match_id, self.setting.room_id, function(user_info){
            //self.loginer.apply('13661314855', u.md5('123456'), '', self.setting.match_id, self.setting.room_id, function(user_info){
                u.yylog(__FILE__, __LINE__);

                return;
                var my_user_info = u.clone(user_info);

                var game = require('./champion_player');
                var player = new game(self.setting, my_user_info, self.setting.game_type, function(uid, room_id, exit_reason){
                    u.yylog(__FILE__, __LINE__, "uid: " + uid + " exit room: " + room_id + " for reason: " + exit_reason);
                });
                player.start_game(self.setting.room_id, 0);
            });
        });
    }
}

