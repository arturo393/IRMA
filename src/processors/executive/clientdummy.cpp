/* 
 * File:   clientdummy.cpp
 * Author: root
 * 
 * Created on 13 de mayo de 2013, 03:07 PM
 */

#include "clientdummy.h"
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/wait.h> 
#include <netdb.h>

clientdummy::clientdummy() {
    init();
}

clientdummy::clientdummy(const clientdummy& orig) {
}

clientdummy::~clientdummy() {

    close(sockfd);
}
//Centimetros de distancia
float clientdummy::sense(int channel) {
    fprintf(stdout, "\n sense(%d) In \n",channel);
    float tmp;
    ssize_t bytesreceived = 0;
    const size_t bufflen = 4096;
    char buff[bufflen];
    const char * question = "Hello how are you?";
    if (-1 == send(sockfd, question, (size_t) strlen(question) + 1, 0)) {
        perror("send");
        return 1;
    }
    
    bytesreceived = recv(sockfd, buff, bufflen, 0);
    fprintf(stdout, "\n  bytesreceived (%zd)\n",bytesreceived);
    if (-1 == bytesreceived) {
        perror("recv");
        return 1;
    }
    printf("%s\n", buff);
    fprintf(stdout, "\n sense(%d) Out \n",channel);
    return tmp;
}
uint16_t socketdata(const char* gCmdStr, const char* gVal)
{
    
}

void clientdummy::move(float speed, float steering){
    
}


float clientdummy::giro(){
    return 2;
}

int clientdummy::encoder(int channel)
{
    return 1;
}

void clientdummy::init() {
    fprintf(stdout, "\n init \n");
    struct sockaddr_in server;
    struct addrinfo *serverinfo;
    
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd) {
        perror("socket");
        fprintf(stdout, "\n socket \n");
        //return 1;
    }
    //if (0 != getaddrinfo("192.168.1.2", NULL, NULL, &serverinfo)) {
    if (0 != getaddrinfo("mipc.magana.cl", NULL, NULL, &serverinfo)) {
        perror("getaddrinfo");
        fprintf(stdout, "\n getaddrinfo \n");
        //return 1;
    }
    /*Copy size of sockaddr_in b/c res->ai_addr to big for this example*/ memcpy(&server, serverinfo->ai_addr, sizeof (struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
    freeaddrinfo(serverinfo);
    if (-1 == connect(sockfd, (const struct sockaddr *) &server, sizeof (struct sockaddr_in))) {
        perror("connect");
        fprintf(stdout, "\n connect \n");
        //return 1;
    }else
    {
        fprintf(stdout, "\n connect ok \n");
    }

    //return 0;
}

