exports.yylog=yylog;
exports.shuffle=shuffle;
exports.md5=md5;
exports.clone=clone;
exports.sortObject=sortObject;

function md5 (text) {
    var crypto = require('crypto');
    return crypto.createHash('md5').update(text).digest('hex');
};

function clone(a) {
       return JSON.parse(JSON.stringify(a));
}

function sortObject(o) {
    var sorted = {},
        key, a = [];

    for (key in o) {
        if (o.hasOwnProperty(key)) {
            a.push(key);
        }
    }

    a.sort();

    for (key = 0; key < a.length; key++) {
        sorted[a[key]] = o[a[key]];
    }
    return sorted;
}

function shuffle(array) {
    var currentIndex = array.length, temporaryValue, randomIndex ;

    // While there remain elements to shuffle...
    while (0 !== currentIndex) {
        // Pick a remaining element...
        randomIndex = Math.floor(Math.random() * currentIndex);
        currentIndex -= 1;

        // And swap it with the current element.
        temporaryValue = array[currentIndex];
        array[currentIndex] = array[randomIndex];
        array[randomIndex] = temporaryValue;
    }
    return array;
}
        
Object.defineProperty(global, '__STACK__', {
    get: function(){
             var orig = Error.prepareStackTrace;
             Error.prepareStackTrace = function(_, stack){ return stack; };
             var err = new Error;
             Error.captureStackTrace(err, arguments.callee);
             var stack = err.stack;
             Error.prepareStackTrace = orig;
             return stack;
         }
});

Object.defineProperty(global, '__LINE__', {
    get: function(){
             return __STACK__[1].getLineNumber();
         }
});

Object.defineProperty(global, '__FILE__', {
    get: function(){
             return __STACK__[1].getFileName();
         }
});

function yylog(filename, linenumber){
    var now = new Date(new Date().getTime() + 8*3600*1000);
    var iso = now.toISOString().replace(/T/, ' ').replace(/Z/, '');

    var msg = iso + ":" + filename + ":" + linenumber;
    for (var i = 2; i < arguments.length; i++) {
        msg += "|" + arguments[i];
    }
    console.log(msg);
}
