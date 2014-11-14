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

