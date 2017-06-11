// Nguyen Hai Duong
// Date : july 2016
// lib/TCP-IP.cc

#include "TCP-IP.h"

namespace BONE { 

int createTCPServerSocket(unsigned short port) {
    int servSock;                           /* socket to create */
    struct sockaddr_in serv_addr;           /* Local address */

    // Create socket for incoming connections 
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        BONE_ERROR << "ERROR opening socket";
    
    int reuse = 1;
    if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        BONE_ERROR << "setsockopt(SO_REUSEADDR) failed";

    #ifdef SO_REUSEPORT
        if (setsockopt(servSock, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
            BONE_ERROR << "setsockopt(SO_REUSEPORT) failed";
    #endif

    // Construct local address structure 
    memset(&serv_addr, 0, sizeof(serv_addr));       /* Zero out structure */
    serv_addr.sin_family = PF_INET;                 /* Internet address family */
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */
    serv_addr.sin_port = htons(port);               /* Local port */

    // Bind to the local address
    if (bind(servSock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
        BONE_ERROR << "ERROR on binding";

    // Mark the socket so it will listen for incoming connections 
    if (listen(servSock, MAXPENDING) < 0)
        BONE_ERROR << "ERROR on listening";

    return servSock;
}

int acceptTCPConnection(int servSock){
    int clntSock;                       /* Socket descriptor for client */
    struct sockaddr_in cli_addr;        /* Client address */
    unsigned int clntLen;               /* Length of client address data structure */

    // Set the size of the in-out parameter 
    clntLen = sizeof(cli_addr);
    
    // Wait for a client to connect 
    if ((clntSock = accept(servSock, (struct sockaddr *) &cli_addr, &clntLen)) < 0)
        BONE_ERROR << "accept() failed";
    
    // BONE_VLOG   <<"Got a connection from " << inet_ntoa(cli_addr.sin_addr)
    //             <<" on port " 
    //             << htons(cli_addr.sin_port);

    return clntSock;
}

}   // end of namespace TREE
