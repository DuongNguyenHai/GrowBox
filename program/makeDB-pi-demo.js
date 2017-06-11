var logTempt = "logTempt";
var logHumy = "logHumidity";
var logCamera = "logCamera";
var logLamp = "logLamp";
var logProgressTemperature = "logProgressTemperature";
var logProgressHumidity = "logProgressHumidity";
var setup = "setup";
db = connect("localhost:27017");
db = db.getSiblingDB('GrowBox');
// remove database if its exist
if(db)
	db.dropDatabase();

function getTime(GMT) {
	return new Date( new Date().getTime() + GMT * 3600 * 1000);
}

function getTimeExpand(GMT, expand) {
	return new Date( new Date().getTime() + GMT * 3600 * 1000 + 1000*expand);
}

function hour2second(hour) {
	return hour*3600;
}

var localDate = getTime(7);

db[logTempt].insert({_id: "ob_id", total: 10, infor: "log temperature", date: localDate});
db[logProgressTemperature].insert({_id: "ob_id", total: 20, infor: "log all temperatures for progress of setting tempt", date: localDate});
db[logProgressHumidity].insert({_id: "ob_id", total: 20, infor: "log all humidity for progress of setting humidity", date: localDate});
db[logHumy].insert({_id: "ob_id", total: 20, infor: "log humidity in box", date: localDate});
db[logLamp].insert({_id: "ob_id", total: 10, infor: "log lamp", date: localDate});
db[logCamera].insert({_id: "ob_id", total: 0, infor: "log image of plant in box", date: localDate});
db[setup].insert({_id: "ob_id", total: 4, infor: "setup for grow box", date: localDate });


for (var i = 0; i < 10; i++) {
	db[logTempt].insert(
		{
			_id: i, 
			temptInBox: 20 + (Math.floor(Math.random() * 10) - 5), 
			date:  getTimeExpand(7, i)
		}
	);
}

for (var i = 0; i < 10; i++) {
	db[logTempt].insert({
		_id: i,
		temptInBox: 20 + (Math.floor(Math.random() * 10) - 5),
		date:  getTimeExpand(7, i)
	});
}

var hum = 80;
for (var i = 0; i < 20; i++) {
	db[logProgressHumidity].insert({
		_id: i,
		humidity: hum,
		date:  getTimeExpand(7, i)
	});
	hum += 1;
}

hum = 80;
for (var i = 0; i < 20; i++) {
	db[logHumy].insert({
		_id: i,
		humidity: hum,
		date:  getTimeExpand(7, i)
	});
	hum += (Math.floor(Math.random() * 10) + 5)/10;
}

for (var i = 0; i < 10; i++) {
	db[logProgressTemperature].insert({
		_id: i,
		temptInBox: 20 + (Math.floor(Math.random() * 10) - 5),
		temptIn: 20 + (Math.floor(Math.random() * 10) - 5),
		temptOut: 20 + (Math.floor(Math.random() * 10) - 5),
		humidity: 80 + (Math.floor(Math.random() * 40) - 20),
		date:  getTimeExpand(7, i)
	});
}

for (var i = 0; i < 10; i++) {
	db[logLamp].insert({
		_id: i,
		lightLevel: 5000 + (Math.floor(Math.random() * 5000) - 2500),
		date:  getTimeExpand(7, i)
	});
}

db[setup].insert(
	{
	   _id: 0,
	   title: "temptAndHumy",
	   enable: true,			// enable reading and saving tempt and humy
	   enable_tempt: false,		// nealbe automatic controlling tempt
	   enable_humy: false,		// enalbe automatic controlling humy
	   temptInBox: 25,
	   humidity: 80,
	   schedule: [hour2second(2), hour2second(7), hour2second(12), hour2second(17)]  // time by the second
	}
);

db[setup].insert(
	{
		_id: 1,
		title: "lamp",
		enable: true,
		state: false,
		schedule: [hour2second(7), hour2second(17)] // time by the second
	}
);

db[setup].insert(
	{
		_id: 2,
	   title: "ventilation",
	   enable: true,
	   state: false,
	   cycle: [120, 1800]  // [open time, close time]
	}
);

db[setup].insert(
	{
	   _id: 3,
	   title: "camera",
	   enable: true,
	   state: false,
	   schedule: [hour2second(18)+600,hour2second(18) + 620] // time by the second
	}
);