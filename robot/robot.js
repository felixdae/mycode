function main(){
    var u = require('./utility');
    var hl= require('./http_login');

    var setting = {champion:false};
    setting.env = process.argv[2];
    setting.name = process.argv[3];
    setting.pass = process.argv[4];
    if (process.argv.length > 5 && process.argv[5] == 'champion'){
        setting.champion= true;
    }
    //console.log(process.argv, setting.champion);
    var robot_start = function(user_info,room_info){
        var ws_init = require('./ws_init');
        //console.log(JSON.stringify(user_info, null, 2));
        //console.log(JSON.stringify(room_info, null, 2));
        var player = new ws_init(setting, user_info, room_info);
        player.play();
    }

    var loginer = new hl(setting);
    loginer.login(setting.name, u.md5(setting.pass), '', robot_start);
}

main();
