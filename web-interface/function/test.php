<?php

include 'database.php';

$doc = getLastDocument($conn, collTempt);
$lamp = getDocument($conn, collSetup, "title", "lamp");

var_dump($lamp);

// var_dump($tm);

?>