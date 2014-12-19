function main(){
    var setting = {};

    setting.env = process.argv[2];
    setting.game_type = process.argv[3];
    setting.user_file = process.argv[4];
    if (setting.game_type != 'match'){
        var dispatcher = require('./dispatcher');
        var gamer =  new dispatcher(setting);
        gamer.run();
    } else {
        setting.match_id = process.argv[5];
        setting.room_id = process.argv[6];
        setting.limit = process.argv[7];//人数
        var matcher = require('./match');
        var gamer = new matcher(setting);
        gamer.run();
    }
}

main();
