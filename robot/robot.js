function main(){
    var u = require('./utility');
    var hl= require('./http_login');
    var loginer = new hl('release');

    var robot_start = function(user_info,room_info){
        var ws_player = require('./ws_player');
        var player = new ws_player('release', user_info, room_info);
        player.play();
    }

    loginer.login('18682006183', u.md5('123456'), '', robot_start);
}

main();
