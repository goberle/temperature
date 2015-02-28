process.stdin.resume();

process.on('SIGINT', function () {
  console.log('Got SIGINT.');
  process.exit();
});

var addon = require('./build/Release/addon');

addon.start(2, function(a, b) {
  if (a == 160) {
    if (b > 25.0) 
      addon.lightOff();
    else
      addon.lightOn();
  }
	console.log(a + ' ' + b);
});

console.log('Started!');

