/***********************************************************************
* IRMA2: CameraComm.c
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
#include "cameraComm.h"

// Constructor
cameraComm::cameraComm()
{

    struct termios attr;
    int przer=0;

    fd_set readset;
    struct sigaction saio;

    if (( fd = open( MODEMDEVICE, O_RDWR | O_EXCL )) < 0 )
    {
        perror(MODEMDEVICE);
        exit(-1);
    }

    tcgetattr( fd, &attr );

    attr.c_iflag = 0;
    attr.c_oflag = 0;
    attr.c_cflag = CLOCAL | CREAD | CS8;
    attr.c_lflag = 0;
    attr.c_cc[ VTIME ] = 0; // timeout in tenths of a second
    attr.c_cc[ VMIN ] = 0;  // Only wait for a single char

    cfsetispeed( &attr, BAUDRATE );
    cfsetospeed( &attr, BAUDRATE );

    tcsetattr( fd, TCSAFLUSH, &attr );

    // Put stdin & stdout in unbuffered mode.

    setbuf( stdin, NULL );
    setbuf( stdout, NULL );

    // Put stdin in raw mode (i.e. turn off canonical mode). Canonical mode
    // causes the driver to wait for the RETURN character so that line editing
    // can take place. We also want to turn off ECHO.

    {
        struct termios tio;

        tcgetattr( fileno( stdin ), &tio );


        tio.c_lflag &= ~( ICANON | ECHO );
        tio.c_cc[VTIME] = 0;
        tio.c_cc[VMIN] = 1;

        tcsetattr( fileno( stdin ), TCSANOW, &tio );
    }

    saio.sa_handler = signal_handler_IO;
    sigemptyset(&saio.sa_mask);
    saio.sa_flags = 0;
    saio.sa_flags = SA_NODEFER;
    saio.sa_restorer = NULL;
    sigaction(SIGIO,&saio,NULL);


    fcntl(fd, F_SETOWN, getpid());
    fcntl(fd, F_SETFL, FASYNC);

    tcgetattr(fd,&oldtio);
}

// Destructor
cameraComm::~cameraComm()
{

    //tcsetattr(fd,TCSANOW,&oldtio);
    close( fd );
}

int cameraComm::cWrite (int query, int pos)
{

    // Read(clean) Buffer
    char cmd[10];
    cRead(NONE);
    //sleep(1);
    switch ( query )
    {

    case STC  :
        write( fd, "stc\n", 4);
        break;

    case ITF  :
        write( fd, "itf\n", 4);
        break;

    case SSP  :
        sprintf(cmd,"ssp %d\n",pos);
        write( fd, cmd, strlen(cmd));
        break;

    case RTI  :
        write( fd, "rti\n", 4);
        break;

    case NET  :
        write( fd, "net\n", 4);
        break;

    case ESC  :
        write( fd, "esc\n", 4);
        break;
        //default   : printf( "???" );
    }
}

int cameraComm::cRead (int query)
{
    unsigned char buf[255];
    char answer[50];

    int i=0, j=0, imax=0;
    res = read(fd,buf,1);
    buf[res]=0;
    answer[i] = buf[0];
    //printf("ans=%d i=%d\n", answer[i],i);
    if (res!=1)
    {
        return 1;
    }
    //else
    //printf("buf(0)=%c \n", buf[0]);

    //printf("durmiendo...\n");
    sleep(1);
    //printf("wake up!\n");
    while (res)
    {

        //if ((int)buf[0] != 13)
        i++;
        res = read(fd,buf,1);
        buf[res]=0;
        answer[i] = buf[0];
        /*if (res>0){
            printf("buf(0)=%c \n", buf[0]);
        }*/
    }

    imax=i-2;
    //printf("1ro: %s \n", answer);
    if ( query == ITF || query == RTI)
    {
        sscanf(answer,"%d:%d:%d",&value[0],&value[1],&value[2]);
    }

    else if ( query == NET )
    {
        value[0] = char2int(answer[0]);
    }

    return 0;
}

int cameraComm::char2int (char c)
{
    switch ( c )
    {

    case 48  :
        return 0;
    case 49  :
        return 1;
    case 50  :
        return 2;
    case 51  :
        return 3;
    case 52  :
        return 4;
    case 53  :
        return 5;
    case 54  :
        return 6;
    case 55  :
        return 7;
    case 56  :
        return 8;
    case 57  :
        return 9;
    default  :
        return -1;
    }
}

int cameraComm::getValue (int i)
{
    return value[i-1];
}

float cameraComm::getSpeed()
{
    return speed;
}

float cameraComm::getSteering ()
{
    return steering;
}

int cameraComm::getRecognizedFeature ()
{
    return recognizedFeature;
}

int cameraComm::getFeatureColor ()
{
    return featureColor;
}

int cameraComm::stateFunction(int actualState)
{
    int nextState = 0;
    int i =0;
    int r;

    switch (actualState)
    {
    case 1:
        printf("CAMERA   : Sending STC\n");
        cWrite(STC);
        cameraPosition = 134;
        numAsk = MAX_NUM_ASK;
        nextState = 2;
        break;

    case 2:
        printf("CAMERA   : Is there a Feature?... ");
        usleep (500000);
        r=cRead(ITF);// free buffer
        cWrite(ITF);

        r = 1;
        //r = cRead(ITF);
        while (r == 1 &&  i < 5)
        {
            usleep (500000);
            r=cRead(ITF);
            i++;
            //printf("    i = %d ; r = %d\n", i, r);
        }
        if (r != 1)
        {
            if (value[0] == 0)
            {
                printf("No");
                if (numAsk < MAX_NUM_ASK)
                {
                    numAsk++;;
                    nextState = 2;
                    usleep(1000000);
                    printf("( %d/15)", numAsk);
                }
                else
                    nextState = 3;

                printf("\n");
            }
            else if (value[0]==1)
            {
                printf("Yes\n");
                nextState = 4;
            }
        }
        else
        {
            printf("CAMERA   : No answer from CMUcam\n");
            nextState = 1;
        }
        break;

    case 3:
        //Other Processor Movement
        nextState = 2;
        break;

    case 4:
        //printf("x =%d\n", cmuCam.getValue(2));

        numAsk = 0;
        if (value[1] >= 82 && value[1] <= 92)
        {
            printf("CAMERA   : Is feature is in the horizontal range (82 - 92)? Yes (%d)\n", value[1]);
            nextState = 6;
        }
        else
        {
            printf("CAMERA   : Is feature is in the horizontal range (82 - 92)? No (%d)\n", value[1]);
            nextState = 5;
        }
        break;

    case 5:

        speed = 0.5;
        if (value[1] > 92)
        {
            if (cameraPosition < 50)
                steering = 0.55;
            else
                steering = 0.6;
            printf("CAMERA   : Turning to the right (steering=%f)\n", steering);

        }
        else if (value[1] < 82)
        {
            if (cameraPosition < 50)
                steering = 0.43;
            else
                steering = 0.4;
            printf("CAMERA   : Turning to the left (steering=%f)\n", steering);
        }

        //GIRAR
        nextState = 2;

        break;

    case 6:

        if (value[2] >= 138 && value[2] <= 148)
        {
            printf("CAMERA   : Is the feature in the vertical range (138 - 148): Yes (%d)\n", value[2]);
            nextState = 8;
        }
        else
        {
            printf("CAMERA   : Is the feature in the vertical range (138 - 148): No (%d)\n", value[2]);
            nextState = 7;
        }
        break;

    case 7:

        int newCameraPosition;
        newCameraPosition = cameraPosition + (int)(0.3*(float)(143-value[2]));
        if (newCameraPosition < 15)
        {
            newCameraPosition = 0;
        }
        if (newCameraPosition > 134)
        {
            newCameraPosition = 134;
        }

        if (newCameraPosition < cameraPosition + 20 && newCameraPosition > cameraPosition - 20 && newCameraPosition != 0 )//newCameraPosition == cameraPosition)
        {
            printf("CAMERA   : CMUcam is in optimal position: %d\n", cameraPosition);
            nextState = 8;
        }
        else
        {
            printf("CAMERA   : CMUcam is not in optimal position: %d\n", cameraPosition);
            cameraPosition = newCameraPosition;
            nextState = 9;
        }

        break;

    case 8:

        speed = 0.6 + 0.5*((float)cameraPosition/134);//old Value: 0.4 (now 0.5)
        if (speed > 0.8)
            speed = 0.8;
        steering = 0.5;
        printf("CAMERA   : Moving forward: speed = %f\n", speed);
        //AVANZAR
        nextState = 2;
        break;

    case 9:

        printf("CAMERA   : Setting new camera position: %d. \n", cameraPosition);
        cWrite(SSP,cameraPosition);

        if (cameraPosition == 0)
        {
            numAsk = 0;
            nextState = 10;
        }
        else
            nextState = 2;
        break;

    case 10:

        printf("CAMERA   : Is the camera ready to identify the feature?\n");
        cWrite(ITF);

        r = cRead(ITF);
        while (r == 1 &&  i < 3)
        {
            usleep (500000);
            r=cRead(ITF);
            i++;
            //printf("    i = %d ; r = %d\n", i, r);
        }
        if (r != 1)
        {
            if (value[0] == 0)
            {
                printf("\tNo\n");
                nextState = 2;
            }
            else if (value[0]==1)
            {
                printf("\t(%d,%d) ", value[1], value[2]);
                nextState = 11;
            }
        }

        usleep(1000000);

        cWrite(RTI);

        r = 1;
        while (r == 1 &&  i < 6)
        {
            usleep (500000);
            r=cRead(RTI);
            i++;
            //printf("    i = %d ; r = %d\n", i, r);
        }
        if (r != 1)
        {
            if (value[0] == 0)
            {
                printf("\tNo\n");
                if (numAsk < MAX_NUM_ASK)
                {
                    numAsk++;;
                    nextState = 10;
                    printf("numAsk = %d", numAsk);
                }
                else
                    nextState = 2;
            }
            else if (value[0]==1)
            {
                printf("\tYes\n");
                nextState = 11;
            }
        }
        else
        {
            printf("CAMERA   : No answer from CMUcam\n");
            nextState = 1;
        }
        break;

    case 11:

        cWrite(NET);

        r = cRead(NET);
        while (r == 1 &&  i < 3)
        {
            usleep (500000);
            r=cRead(NET);
            i++;
            //printf("    i = %d ; r = %d\n", i, r);
        }
        if (r != 1)
        {

            recognizedFeature = value[0];
            featureColor = RED;
            printf("CAMERA   : Feature found: %d\n",recognizedFeature);
            nextState = 1;
            numAsk = MAX_NUM_ASK;
        }

        usleep (500000);
        break;
    }

    return nextState;
}

void signal_handler_IO (int status)
{

    //printf("Llego\n");
    /*
    int i;
    unsigned char buf[255];
    res = read(fd,buf,1);

    while (res){
    buf[res]=0;

      if ((int)buf[0] != 13)
        printf("%c",buf[0]);
      else
        printf("\n");
    res = read(fd,buf,1);
    }
    */
//printf("Hola\n");
}
