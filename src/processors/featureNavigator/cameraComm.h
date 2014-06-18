/***********************************************************************
* IRMA2: CameraComm.h
************************************************************************/
/******************************************************************************
*
*   Copyright (c) 2009 Tomas Arredondo     <tomas.arredondo@usm.cl>
*
*   This program is open-source software; you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License version 3 as
*   published by the Free Software Foundation.
*
*   See README and LICENSE for more details.
*
******************************************************************************/
#ifndef _IRMA2_FNAV_CAMERACOMM_H_
#define _IRMA2_FNAV_CAMERACOMM_H_

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/signal.h>
#include <queue>

//----------------------------------------------------
#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyS2"
#define _POSIX_SOURCE 1
#define O_BINARY 0x8000
#define FALSE 0
#define TRUE 1
#define STC 1
#define ITF 2
#define SSP 3
#define RTI 4
#define NET 5
#define ESC 6
#define NONE 7

#define MAX_NUM_ASK 15

#define RED 0
#define BLUE 1
#define GREEN 2



class cameraComm
{
private:

        struct termios oldtio,newtio;
        int fd, res;
        int char2int (char);
        int value[3]; //respuesta de la camara en la forma v1:v2:v3
        int cameraPosition, recognizedFeature, featureColor;
        int numAsk;
        float steering, speed;

public:
        // Constructor
        cameraComm();
        // Destructor
        ~cameraComm();
        // Funciones miembro o metodos
        int stateFunction(int);
        int cWrite (int, int = 0);
        int cRead (int);
        int getValue (int);
        float getSpeed (void);
        float getSteering (void);
        int getRecognizedFeature (void);
        int getFeatureColor(void);
};

void signal_handler_IO (int);

#endif
