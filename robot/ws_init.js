require('./utility');
module.exports = ws_init;

function ws_init(setting, user_info, room_id){
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

    self.play = function (game){

        var room_type = 1;
        if (game == 'sng'){
            room_type = 2;
        } else if(game == 'champion'){
            room_type = 4;
        }
        var WebSocket = require('ws');
        var ws = new WebSocket(self.ws_path);
        var maker;
        var handler;
        //if (self.setting.champion== true){
        var player = require('./champion_player');
        maker = new player.msg_maker(self.user_info);
        handler = new player.hall(self.user_info, ws);
        //}else{
        //    var player = require('./player');
        //    maker = new player.msg_maker(self.user_info);
        //    handler = new player.msg_handler(self.user_info, ws);
        //}
        ws.on('open', function (){
            if (self.setting.champion== true){
                var msg = handler.join_match(1, room_type);
            }else{
                var msg = maker.sit_down(room_id, room_type);
            }
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
            console.log(data);
            handler.parse(data);
        });
    }
}

