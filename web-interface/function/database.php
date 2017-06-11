<?php

require 'vendor/autoload.php'; // include Composer's autoloader

define("dbName","GrowBox");
define("collTempt", "logTempt");
define("collHumy", "logHumidity");
define("collLamp", "logLamp");
define("collSetup", "setup");

function connectDatabase($databaseName) {
	$client = new MongoDB\Client("mongodb://localhost:27017");
	return $client->$databaseName;
}

function getSchedule($conn, $collName, $type) {
	$coll = $conn->$collName;
	$document = $coll->findOne(["title"=>$type]);
	$schedule = iterator_to_array($document->schedule, true);
	return $schedule;
}

function getDocument($conn, $collName, $key_select, $val_select) {
	$coll = $conn->$collName;
	$doc = $coll->findOne([$key_select=>$val_select]);
	return $doc;
}

function getField($conn, $collName, $key_select, $val_select, $key_get) {
	$coll = $conn->$collName;
	$doc = $coll->findOne([$key_select=>$val_select]);
	return $doc->$key_get;
}

function getLastDocument($conn, $collName) {
	$coll = $conn->$collName;
	$cursor = $coll->find([], [
        'limit' => 1,
        'sort' => ['$natural' => -1],
    ]);

	foreach($cursor as $document) {
		$doc = $document;
	}

    return $doc;
}

function updateField($conn, $collName, $key_select, $val_select, $key_change, $val_change) {
	$coll = $conn->$collName;
	$updateResult = $coll->updateOne(
		[$key_select => $val_select], 
		['$set'=>[$key_change => $val_change]]
	); 
}

$GLOBALS["conn"] = connectDatabase(dbName);

?>