// Nguyen Hai Duong
// Date : july 2016
// lib/TCP-IP.h

#ifndef BONE_TCP_IP_H
#define BONE_TCP_IP_H

#include <stdio.h>		// for printf() and fprintf()
#include <sys/socket.h> // for socket(), bind(), and connect()
#include <arpa/inet.h>  // for sockaddr_in and inet_ntoa()
#include <stdlib.h>     // for atoi() and exit()
#include <string.h>		// for memset()
#include <unistd.h>		// for sleep(), close()
#include <sys/types.h>	// for Socket data types
#include <netinet/in.h> // for IP Socket data types
#include <pthread.h>	// for multithread function
#include <sys/wait.h>	// for waitpid()
#include <sys/time.h>   // for struct timeval()
#include <fcntl.h>      // for fcntl()

// for log and error
#include "bone-log.h"

namespace BONE {

#define MAXPENDING 5
#define BUFFSIZE 256

// Create tcp server socket
int createTCPServerSocket(unsigned short port);

// accept tcp connection
// Return server socket
int acceptTCPConnection(int servSock);


}	// end of name space TREE

#endif