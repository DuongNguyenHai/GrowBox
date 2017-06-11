
// show all collections
// db.getCollectionNames();

function nextIndex(sequenceName, docName){
   	var sequenceDocument = db[docName].findAndModify({
    	query:{_id: sequenceName },
      	update: {$inc:{total:1}},
      	new:true
   	});
   	return sequenceDocument.total -1;
}

function getTime(GMT) {
	return new Date( new Date().getTime() + GMT * 3600 * 1000);
}

function getTimeExpand(GMT, expand) {
	return new Date( new Date().getTime() + GMT * 3600 * 1000 + 1000*expand);
}
// convert hour to second
function hour2second(hour) {
	return hour*3600;
}

var localDate = getTime(7);	// time zone is 7, add + 3 seconds

// insert order document in collection
db[logTempt].insert({_id: "ob_id", total: 0, infor: "log temperature", date: localDate});
db[logProgress].insert({_id: "ob_id", total: 0, infor: "log all temperatures for progress of setting tempt", date: localDate});
db[logHumy].insert({_id: "ob_id", total: 0, infor: "log humidity in box", date: localDate});
db[logLamp].insert({_id: "ob_id", total: 0, infor: "log lamp", date: localDate});
db[logCamera].insert({_id: "ob_id", total: 0, infor: "log image of plant in box", date: localDate});
db[setup].insert({_id: "ob_id", total: 0, infor: "setup for grow box", date: localDate });

for (var i = 0; i < 10; i++) {
	db[logTempt].insert({
		_id: nextIndex("ob_id", logTempt),
		temptInBox: 20 + (Math.floor(Math.random() * 10) - 5),
		date:  getTimeExpand(7, i)
	});
}

for (var i = 0; i < 10; i++) {
	db[logHumy].insert({
		_id: nextIndex("ob_id", logHumy),
		humidity: 80 + (Math.floor(Math.random() * 40) - 20),
		date:  getTimeExpand(7, i)
	});
}

for (var i = 0; i < 10; i++) {
	db[logProgress].insert({
		_id: nextIndex("ob_id", logProgress),
		temptInBox: 20 + (Math.floor(Math.random() * 10) - 5),
		temptIn: 20 + (Math.floor(Math.random() * 10) - 5),
		temptOut: 20 + (Math.floor(Math.random() * 10) - 5),
		humidity: 80 + (Math.floor(Math.random() * 40) - 20),
		date:  getTimeExpand(7, i)
	});
}

for (var i = 0; i < 3; i++) {
	db[logLamp].insert({
		_id: nextIndex("ob_id", logLamp),
		lightLevel: 5000 + (Math.floor(Math.random() * 5000) - 2500),
		date:  getTimeExpand(7, i)
	});
}

db[setup].insert(
	{
	   _id: nextIndex("ob_id", setup),
	   title: "temptAndHumy",
	   enable: true,
	   temptInBox: 25,
	   humidity: 80,
	   schedule: [hour2second(2), hour2second(7), hour2second(12), hour2second(17)]  // time by the second
	}
);

db[setup].insert(
	{
		_id: nextIndex("ob_id", setup),
		title: "lamp",
		enable: true,
		state: false,
		schedule: [hour2second(7), hour2second(17)] // time by the second
	}
);

db[setup].insert(
	{
		_id: nextIndex("ob_id", setup),
	   title: "ventilation",
	   enable: true,
	   state: false,
	   interval: 1800	// time by the minute
	}
);

db[setup].insert(
	{
	   _id: nextIndex("ob_id", setup),
	   title: "camera",
	   enable: true,
	   state: false,
	   schedule: [hour2second(18)+600,hour2second(18) + 620] // time by the second
	}
);


show all collections
db.getCollectionNames();