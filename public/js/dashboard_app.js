// Welcome to the RazorFlow Dashbord Quickstart. Simply copy this "dashboard_quickstart"
// to somewhere in your computer/web-server to have a dashboard ready to use.
// This is a great way to get started with RazorFlow with minimal time in setup.
// However, once you're ready to go into deployment consult our documentation on tips for how to 
// maintain the most stable and secure 

var socket = io();

socket.on('config', function(config) {

  StandaloneDashboard(function(tdb){
  
    tdb.setTabbedDashboardTitle(config.dashboardTitle);
  
    var db1 = new Dashboard();
  	db1.setDashboardTitle("Temps réel");
  
    var db2 = new Dashboard();
    db2.setDashboardTitle("Historique");
    
    var sensorsGauges = [];
    
    for(var i=0; i<config.nbSensors; i++) {
      var s = new GaugeComponent();
      s.setDimensions(6,4);
      s.setLimits(0, 120);
      s.lock();
      db1.addComponent(s);
      sensorsGauges.push(s);
    }

    socket.on('data', function(d) {
      for (var i=0; i<d.sensors.length; i++) {
        sensorsGauges[i].setValue(d.sensors[i].temp, {numberSuffix: '° C', numberDecimalPoints: 1});
        if (sensorsGauges[i].isLocked()) {
          sensorsGauges[i].setCaption(d.sensors[i].name);
          sensorsGauges[i].unlock();
        }
      }
    });
  
    tdb.addDashboardTab(db1, {
      active: true
    });
    tdb.addDashboardTab(db2, {
      active: true
    });
  }, {tabbed: true});

});

