function choose_room(room_info){
    var rooms = room_info.filter(function(item, index, arr){
        return item.match != 'sng' && item.min_chip<10000;
    });
    var item = rooms[Math.floor(Math.random()*rooms.length)];
    return item;
}

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

    var robot_start = function(user_info,room_info){
        var ws_init = require('./ws_init');
        var player = new ws_init(setting, user_info, room_info);
        player.play();
    }

    var loginer = new hl(setting);
    loginer.login(setting.name, u.md5(setting.pass), '', robot_start);
}

//main();
function main_multi(){
    var u = require('./utility');
    var authenticator = require('./http_login');

    var setting = {champion:false};
    setting.env = process.argv[2];
    setting.name = process.argv[3];
    setting.pass = process.argv[4];
    if (process.argv.length > 5 && process.argv[5] == 'champion'){
        setting.champion= true;
    }
    
    var fs = require('fs');
    var robot_buf = fs.readFileSync('./user.json');
    var robot_list = JSON.parse(robot_buf);
    var robot_in_room = {};//robot=>room
    for (k in robot_list){
        robot_in_room[k] = false;
    }
    
    var dispatch_robot = function(game, room_info){
        var robot_pass = 'ASD123@$888qwe';
    }
    var sng_room_success = function(room_info){
        dispatch_robot('sng', room_info);
    }
    var room_id = 0;
    var normal_room_success = function(room_info){
        //dispatch_robot('normal', room_info);
        var room = choose_room(room_info);
        room_id = room.id;
        var loginer = new authenticator(setting);
        loginer.login(setting.name, u.md5(setting.pass), '', robot_start);
    }
    
    var robot_start = function(user_info){
        //console.log(__LINE__, user_info);
        var game_module = require('./champion_player');
        var player = new game_module.game(setting, user_info, 'normal', function(){
            console.log(__LINE__, 'hello there, end function');
        });
        player.start_game(room_id);
    }

    var gamer = new authenticator(setting);
    //gamer.get_game_room('sng', setting.name, u.md5(setting.pass), '', sng_room_success);
    gamer.get_game_room('normal', setting.name, u.md5(setting.pass), '', normal_room_success);
}

main_multi();
