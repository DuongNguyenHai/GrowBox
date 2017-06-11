// command line : mongo < file.sh
// mongo command : load("path of file")
var logTempt = "logTempt"
var logHumy = "logHumidity"
var logCamera = "logCamera"
var setup = "setup"

// db.runCommand({whatsmyuri : 1});

// connect to database
db = connect("localhost:27017");

// get user name
// db.getUsers();

// show all databases
// db.adminCommand('listDatabases');

// use database : "GrowBox"
db = db.getSiblingDB('GrowBox');

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
function secs(hour) {
	return hour*3600;
}

var localDate = getTime(7);	// time zone is 7, add + 3 seconds


for (var i = 0; i < 1; i++) {
	db[logTempt].insert({
		_id: nextIndex("ob_id", logTempt),
		temptInBox: 20 + (Math.floor(Math.random() * 10) - 5),
		date:  getTimeExpand(7, i)
	});
}

for (var i = 0; i < 1; i++) {
	db[logHumy].insert({
		_id: nextIndex("ob_id", logHumy),
		humidity: 80 + (Math.floor(Math.random() * 40) - 20),
		date:  getTimeExpand(7, i)
	});
}

for (var i = 0; i < 1; i++) {
	db[logTemptProgress].insert({
		_id: nextIndex("ob_id", logTemptProgress),
		temptInBox: 20 + (Math.floor(Math.random() * 10) - 5),
		temptIn: 20 + (Math.floor(Math.random() * 10) - 5),
		temptOut: 20 + (Math.floor(Math.random() * 10) - 5),
		date:  getTimeExpand(7, i)
	});
}
