<?php 

define("PORT", 8888);

// Connect to master via tcp/ip
function connectToMaster() {

	if(!($sock = socket_create(AF_INET, SOCK_STREAM, 0))) {
	    // $errorcode = socket_last_error();
	    // $errormsg = socket_strerror($errorcode);
	    // echo ("Couldn't create socket: [$errorcode] $errormsg \n");
	}
	 
	if(!socket_connect($sock , '127.0.0.1' , PORT)) {
	    // $errorcode = socket_last_error();
	    // $errormsg = socket_strerror($errorcode);
	    
	    // echo ("Could not connect to Master: [$errorcode] $errormsg \n");
	}
	// echo "Connection established \n";
	return $sock;
}

// Send the message to the server
function sendCommandToMaster($message) {
	$sock = connectToMaster();
	if( ! socket_send ( $sock , $message , strlen($message) , 0)) {
	    // $errorcode = socket_last_error();
	    // $errormsg = socket_strerror($errorcode);
	    // echo ("Could not send data to Master: [$errorcode] $errormsg \n");
	    return false;
	}
	// echo "Message send successfully \n";
	disconnectToMaster($sock);
	return true;
}
 
function disconnectToMaster($sock) {
	socket_close($sock);
}



?>
