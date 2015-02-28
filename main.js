/*
 * Requirements
 */
var express = require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var fs = require('fs');
var async = require('async');
var radio = require('./build/Release/addon');

/* 
 * Configurations
 */
var sensors = { 
  160: {
    name: "Chaudière bois",
    offset: 0,
    radioId: 10100000,
    currentTemp: -1,
  },
  161: {
    name: "Chaudière fioul",
    offset: 5,
    radioId: 10100001,
    currentTemp: -1,
  },
  10100010: { 
    name: "Ballon tampon",
    offset: 0,
    radioId: 10100010,
    currentTemp: -1,
  }
};

radio.start(2, function(identifier, temperature) {
  if (sensors[identifier])
    sensors[identifier].currentTemp = temperature;
  else
    console.log('Unknown sensor : ', identifier);
});

/*
 * Security
 */
process.setgid('pi');
process.setuid('pi');

var dashboardConfig = { 
  dashboardTitle: 'Température Chaudière',
  nbSensors: 3,
};

app.use(express.static(__dirname + '/public'));

function readSensors(s) {
  var data = {
    time: Date.now(),
    sensors: [],
  };

  for (var i in sensors)
    data.sensors.push({ name: sensors[i].name, temp: sensors[i].currentTemp});

  io.emit('data', data);
}

function logTemp(interval) {
  readSensors(sensors);
  setInterval(readSensors, interval, sensors);
}

app.get('/', function(req, res){
  res.sendfile('index.html');
});

app.get('/test', function(req, res){
  res.sendfile('test.html');
});

io.on('connection', function(socket){
  console.log('a user connected');
  socket.emit('config', dashboardConfig);
});

http.listen(8080, function(){
  console.log('listening on *:8080');
});

logTemp(5000);

