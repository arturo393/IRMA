/************************************************************************************
 * FILE: network.cpp, v.1.0.0, 07/30/2009
 * Author: Gabriel Cisternas.
 *
 * IRMA-2: Network Process
 *
 * Creates the network interface used on IRMA-2 Proyect
 *
 * DISCLAIMER: No liability is assumed by the author for any use made
 * of this program.
 * DISTRIBUTION: Any use may be made of this program, as long as the
 * clear acknowledgment is made to the author in code and runtime executables
************************************************************************************/
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
#include <stdio.h>
#include "network.h"
#include <sys/shm.h>
#include <math.h>


int NetworkProcessor::init()
{
    pExecutive = (ExecutiveData *)cda.getMemPtr(EXECUTIVE_AREA);
    pNetwork = (NetworkData *)cda.getMemPtr(NETWORK_AREA);
    pMapper = (MapperData *)cda.getMemPtr(MAPPER_AREA);

    pCtrl = &(pNetwork->ctrl); // Todos los procesadores tienen que definir pCtrl
    sem_area = NETWORK_AREA;   // Todos los procesadores tienen que definir sem_area

    // Initialize the flags values
    cda.lockArea(NETWORK_AREA);
       pNetwork->get_position_flag = false;
       pNetwork->send_map_flag = false;
       pNetwork->start_log_flag = false;
    cda.unlockArea(NETWORK_AREA);

    pNetwork->ctrl.loop = run_cmd;

    hp = gethostbyname(SERVER_IP);
    if (hp == NULL)
    {
        printf("Network Process: unknown host\n");
        exit(1);
    }

    crearSocket();
}

int NetworkProcessor::step()
{
    bool send_map_flag, get_position_flag, start_log_flag;
    int i, tempX, tempY;
    char message[15];

    // local copy of variables
    cda.lockArea(NETWORK_AREA);
       send_map_flag = pNetwork->send_map_flag;
       get_position_flag = pNetwork->get_position_flag;
       start_log_flag = pNetwork->start_log_flag;
    cda.unlockArea(NETWORK_AREA);

    if (start_log_flag)
    {
        printf("Network Process: Starting log...\n");
        sprintf(message,"start.log");
        sendMsg(message);

        cda.lockArea(NETWORK_AREA);
         pNetwork->start_log_flag = false;
        cda.unlockArea(NETWORK_AREA);
    }

    if (send_map_flag)
    {
        printf("Network Process: sending map...\n");
        sprintf(message,"recieve.map");
        sendMsg(message);
        sendMap();

        cda.lockArea(NETWORK_AREA);
        pNetwork->send_map_flag=false;
        cda.unlockArea(NETWORK_AREA);
        printf("Network Process: map sent...\n");
    }

    if (get_position_flag)
    {
      printf("Network Process: asking position to GPS...\n");
      sprintf(message,"send.location");
      sendMsg(message);

      usleep(50000);

      printf("Network Process: retrieving location data...\n");
      recvMsg();

      tempX = strToInt(strtok(buf,"."));
      tempY = strToInt(strtok(NULL,"."));

      while(tempX==5000 && tempY==5000)
      {
         sprintf(message,"send.location");
         sendMsg(message);

         usleep(50000);

         recvMsg();

         tempX=strToInt(strtok(buf,"."));
         tempY=strToInt(strtok(NULL,"."));
      }

      printf("Network Process: verified location is:\n");
      printf("X coordinate = %i\n",tempX);
      printf("Y coordinate = %i\n",tempY);

      cda.lockArea(NETWORK_AREA);
         pExecutive->current_X=tempX;
         pExecutive->current_Y=tempY;
      cda.unlockArea(NETWORK_AREA);

      cda.lockArea(NETWORK_AREA);
         pNetwork->get_position_flag=false;
      cda.unlockArea(NETWORK_AREA);
    }

    usleep(5000);
    return 0;
}


int NetworkProcessor::cleanup()
{
    cerrarSocket();
    return 0;
}

int main()
{
    fprintf(stdout, "\nIRMA2: Network Process \n");
    NetworkProcessor net;
    net.execute();

    return 0;
}

// Creates an TCP client socket
void NetworkProcessor::crearSocket()
{
    remoteServAddr.sin_family = AF_INET;
    remoteServAddr.sin_port = htons(REMOTE_SERVER_PORT);
    memcpy((char *) &remoteServAddr.sin_addr, hp->h_addr_list[0], hp->h_length);
    len = sizeof(struct sockaddr_in);

    s=socket(AF_INET, SOCK_STREAM, 0);
    if (s<0)
    {
        printf("Network Process: cannot open socket \n");
        exit(1);
    }

    connect(s, (struct sockaddr *) &remoteServAddr, (socklen_t) len);
}

void NetworkProcessor::cerrarSocket()
{
    close(s);
}

// Sends the map to the main computer
void NetworkProcessor::sendMap()
{
    int i, j;
    cda.lockArea(EXECUTIVE_AREA);
    printf("Network Process: Sending %i map objects\n",pMapper->activeNodes);

    sprintf(buf,"%i\n",pExecutive->steps_number);

    printf("Numbers of steps to send: %s",buf);

    // sizeof() returns the same value (4) for types int and float
    send(s,buf,(sizeof(char)+sizeof(int)),0);
    sleep(1);

    for (i=0; i<pExecutive->steps_number; i++)
    {
        sprintf(buf,"%i,%i,%i,%i\n",i,pExecutive->path[i][0],pExecutive->path[i][1],
                pExecutive->path[i][2]);

        printf("%s",buf);

        // sizeof() returns the same value (4) for types int and float
        send(s,buf,(4*sizeof(char))+(4*sizeof(int)),0);
        usleep(50000);
    }

    for (i=0; i<pMapper->activeNodes; i++)
    {
        sprintf(buf,"%f,%f,%f,%f\n",pMapper->categories[i][0],pMapper->categories[i][1],
                pMapper->categories[i][2],pMapper->categories[i][3]);

        printf("%s",buf);

        // sizeof() returns the same value (4) for types int and float
        send(s,buf,(5*sizeof(char))+(8*sizeof(float)),0);
        usleep(50000);
    }
    cda.unlockArea(EXECUTIVE_AREA);

    sprintf(buf,"end");
    send(s,buf,3*sizeof(char),0);

}

// Ask the main computer for the GPS location info
void NetworkProcessor::sendMsg(char *msg)
{
    sprintf(buf,"%s",msg);
    send(s,buf,(13*sizeof(char)),0);
}

// Recieves data
void NetworkProcessor::recvMsg()
{
    // Clears I/O buffer
    memset(buf,0x0,MAX_MSG);
    n = recv(s, buf, MAX_MSG, 0);
    if (n>=0)
    {
        // print received message
        printf("Network Process: Message recieved...\n");
    }
    else
        printf("Network Process: cannot receive data \n");


}

// Converts the number given in 'tempBuffer' to a string
int NetworkProcessor::strToInt(char *tempBuffer)
{
    int number=0;
    int i=0;

    while (*(tempBuffer+i) != '\0')
    {
        // ASCII related issues
        if (*(tempBuffer+i)>47 && *(tempBuffer+i)<58)
        {
            number*=10;
            number+=((int)*(tempBuffer+i)-48);
        }
        i++;
    }
    return number;
}
