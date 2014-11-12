require('./utility');
module.exports = ws_init;

function ws_init(setting, user_info, room_info){
    var self = this;

    self.setting = setting;
    self.ws_path = 'ws://10.0.1.28:7681';
    if (setting.env == 'test'){
        self.ws_path = 'ws://10.0.1.28:7682';
    }
    if(setting.env == 'release'){
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
        var maker;
        if (self.setting.tour == true){
            var player = require('./tour_player');
            var maker = new player.msg_maker(self.user_info, 4);
        }else{
            var player = require('./player');
            var maker = new player.msg_maker(self.user_info);
        }
        var handler = new player.msg_handler(self.user_info, ws);
        ws.on('open', function (){
            if (self.setting.tour == true){
                var msg = maker.join_match(1);
            }else{
                var msg = maker.sit_down(room_id);
            }
            console.log('send :' + JSON.parse(msg).op);
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

