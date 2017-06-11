<?php 

include 'communicate.php';
include 'database.php';

function console_log($data) {
    echo '<script>';
    echo 'console.log('. json_encode( $data ) .')';
    echo '</script>';
}

$MAX_VAL = 200;

if($_REQUEST["request"] === "setup") {

	$name = $_REQUEST["name"];
	$type = $_REQUEST["type"];
	$value = $_REQUEST["value"];

	if($type==="schedule") {
		$data = array_map('intval', explode(',', $value));
	} else {
		if($value==="true")
			$data = true;
		elseif($value==="false")
			$data = false;
		else
			$data = (int)$value;
	}
	
	$cmdResult = sendCommandToMaster($name .":". $type .":". $value);
	// $cmdResult = true;
	if($cmdResult) {
		// updateField($conn, collSetup, "title", $name, $type, $data);
		echo "S: Đã hiết lập giá trị.";
	} else {
		echo "D: Không thể kết nối tới trung tâm";
	}
}

else if($_REQUEST["request"] === "log") {

	$total = getField($conn, $_REQUEST["collection"], "_id", "ob_id", "total");
	$start = 0;
	if($_REQUEST["type"] === "total") {
		if(intval($total) > $MAX_VAL)
			$start = intval(intval($total) - $MAX_VAL);
		// $start =  (intval($total) > $MAX_VAL) ? (intval($total) - $MAX_VAL) : 0;
	} 
	elseif($_REQUEST["type"]==="short") {
		$start = $_REQUEST["index"];
	}

	$property = $_REQUEST["property"];
	$arr = array();
	$count = 0;

	for ($i = $start; $i < intval($total); $i++) { 
		$doc = getDocument($conn, $_REQUEST["collection"], "_id", $i);
		if($doc) {
			$arr[$count][0] = get_object_vars($doc->date)["milliseconds"];
			$arr[$count][1] = $doc->$property;
			$count++;
		}
	}
	echo json_encode($arr, true);
	
}
else if($_REQUEST["request"] === "status") {
	$val = getField($conn, $_REQUEST["collection"], "title", $_REQUEST["name"], $_REQUEST["key"]);
	echo $_REQUEST["name"].":".($val?1:0);
}

?>