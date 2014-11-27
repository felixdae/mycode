function main(){
    var setting = {};

    setting.env = process.argv[2];
    setting.game_type = process.argv[3];
    setting.user_file = process.argv[4];
    var dispatcher = require('./dispatcher');
    var gamer =  new dispatcher(setting);
    gamer.run();
}

main();
