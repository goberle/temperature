var express = require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var fs = require('fs');
var async = require('async');

/* 
 * Configurations
 */
var sensors = [ 
  {
    name: "Sensor1",
    path: "/sys/bus/w1/devices/28-00000626d601/w1_slave"
  },
  {
    name: "Sensor2",
    path: "/sys/bus/w1/devices/28-000006281950/w1_slave"
  },
  {
    name: "Sensor3",
    path: "/sys/bus/w1/devices/28-00000626e75a/w1_slave"
  },
  {
    name: "Sensor4",
    path: "/sys/bus/w1/devices/28-00000626a49a/w1_slave"
  },
  { 
    name: "Sensor5",
    path: "/sys/bus/w1/devices/28-000006284772/w1_slave"
  }
];

var dashboardConfig = { 
  dashboardTitle: 'Température Chaudière',
  nbSensors: sensors.length,
}

app.use(express.static(__dirname + '/public'));

/*
 * Read temperature from sensor
 */
function extractTempFromDevice(sensor, callback) {
  fs.readFile(sensor.path, function (err, buffer) {
    if (err)
      if (err.code == 'ENOENT')
        callback(null, { name: sensor.name, temp: -1 });
      else
        callback(err);
    else {
      var data = buffer.toString('ascii').split(" ");
      var temp = parseFloat(data[data.length-1].split("=")[1])/1000.0;
      callback(err, { name: sensor.name, temp: temp });
    }
  });
}

function readSensors(s) {
  async.waterfall([
    function (callback) {
      async.map(s, extractTempFromDevice, function (err, res) {
        callback(err, res);
      });
    },
    function (temps, callback) {
      var data = {
        time: Date.now(),
        sensors: temps,
      };
      callback(null, data);
    }
  ], function (err, res) {
    if (err)
      console.log(err);
    else {
      io.emit('data', res);
    }
  });
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

logTemp(1000);

