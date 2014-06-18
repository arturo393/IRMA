/***********************************************************************
 IRMA2: Mapper Prcess Header
 /***********************************************************************/
 /******************************************************************************
*
*   Copyright (c) 2009 Tomas Arredondo, Gabriel Cisternas
*		<tomas.arredondo@usm.cl>
*
*   This program is open-source software; you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License version 3 as
*   published by the Free Software Foundation.
*
*   See README and LICENSE for more details.
*
******************************************************************************/
#ifndef _IRMA2_MAPPER_H_
#define _IRMA2_MAPPER_H_

#include <stdio.h>
#include "../base/processor.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>



#define REMOTE_SERVER_PORT 12345
#define MAX_MSG 64
#define SERVER_IP "172.21.21.2"
#define SOCKET_ERROR -1

class NetworkProcessor : public ProcessorInterface
{
private:
    int n, s, len;
    struct sockaddr_in remoteServAddr;
    struct hostent *hp;
    char buf[MAX_MSG];

    ExecutiveData *pExecutive;
    MapperData *pMapper;
    NetworkData *pNetwork;

public:
    NetworkProcessor() : ProcessorInterface()
    {
        ;
    }

    int init();
    int step();
    int cleanup();

    void crearSocket();
    void sendMap();
    void sendMsg(char *);
    void recvMsg();
    void cerrarSocket();

    int strToInt(char *);
};

#endif
