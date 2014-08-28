/****************************************************************************
 *
 *   Copyright (c) 2006 Dave Hylands     <dhylands@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation.
 *
 *   Alternatively, this software may be distributed under the terms of BSD
 *   license.
 *
 *   See README and COPYING for more details.
 *
 ****************************************************************************/
/**
 *
 *   @file   uart-io.c
 *
 *   @brief  This stdintfile implements commands for performing I/O to the msp 
 *           via the usart bus.
 *
 ****************************************************************************/

// ---- Include Files -------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <termios.h>
#include <regex.h>
#include <sys/types.h>
#include <stdint.h>

//#include "BootLoader-api.h"
#include "Log.h"
#include "svn-version.h"

#include <iostream>

#define UART_WAIT 100
#define LEDM_WAIT 100000

using namespace std;

// ---- Public Variables ----------------------------------------------------
// ---- Private Constants and Types -----------------------------------------

// ---- Private Variables ---------------------------------------------------

enum {
    OPT_MEM_DEFAULT = 0,

    // Options assigned a single character code can use that charater code
    // as a short option.

    OPT_BASE_DECIMAL = 'd',
    OPT_BASE_HEX = 'x',

    // Options from this point onwards don't have any short option equivalents

    OPT_FIRST_LONG_OPT = 0x80,

    OPT_VERSION,

    OPT_HELP,
};

enum {
    CMD_DEFAULT,
    CMD_INFO,
    CMD_GET,
    CMD_SET,
    CMD_GET_DIR,
    CMD_SET_DIR,
    CMD_READ_REG,
    CMD_WRITE_REG,
    CMD_WHEEL_L,
    CMD_WHEEL_R,
    CMD_GET_DIST,
    CMD_GET_GIRO,
};

struct cmdmap {
    int cmd;
    const char *cmdStr;
    regex_t re;

} gCmdMap[] = {
    { CMD_INFO, "Info"},
    { CMD_GET, "Get"},
    { CMD_SET, "Set"},
    { CMD_GET_DIR, "GetDir"},
    { CMD_SET_DIR, "SetDir"},
    { CMD_READ_REG, "ReadReg"},
    { CMD_WRITE_REG, "WriteReg"},
    { CMD_READ_REG, "rr"},
    { CMD_WRITE_REG, "wr"},
    { CMD_WHEEL_L, "WheelLeft"},
    { CMD_WHEEL_R, "WheelRight"},
    { CMD_GET_DIST, "dist"},
    { CMD_GET_GIRO, "giro"},
};

int gNumCmds = sizeof ( gCmdMap) / sizeof ( gCmdMap[ 0 ]);

#define REG_IO8(   name, offset )   { name, 0, offset + 0x20 }
#define REG_IO16(  name, offset )   { name, 1, offset + 0x20 }
#define REG_MEM8(  name, offset )   { name, 0, offset }
#define REG_MEM16( name, offset )   { name, 1, offset }

// The following register map is only valid for the ATMega 128

typedef struct {
    const char *regStr;
    uint8_t is16Bit;
    uint8_t reg;

} RegMap_t;

typedef struct {
    uint16_t signature;
    uint16_t numRegs;
    RegMap_t *regMap;

} Regs_t;
#define ARRAY_LEN( x )  ( sizeof( x ) / sizeof( x[0] ))
/*
Regs_t gRegs[] = {
    { AVR_ATMEGA_8_SIGNATURE, ARRAY_LEN(gRegMapATMega8), gRegMapATMega8},
    { AVR_ATMEGA_128_SIGNATURE, ARRAY_LEN(gRegMapATMega128), gRegMapATMega128},
};
 */
int gI2cAddr = -1;

const char *gCmdStr;
int gCmd = CMD_DEFAULT;
regex_t gCmdRExp;
const char *gPortPinStr = NULL;
const char *gValStr = NULL;
const char *gRegStr = NULL;
//int gBase = OPT_BASE_HEX;

struct option gOption[] = {
    { "decimal", no_argument, NULL, OPT_BASE_DECIMAL},
    { "hex", no_argument, NULL, OPT_BASE_HEX},
    { "version", no_argument, NULL, OPT_VERSION},
    { "verbose", no_argument, &gVerbose, 1},
    { "debug", no_argument, &gDebug, 1},
    { "help", no_argument, NULL, OPT_HELP},
    { NULL}
};

#define TRUE    1
#define FALSE   0

typedef enum {
    NoADC,
    AllowADC,

} AllowADC_t;

#define ADC_PORT    8

// ---- Private Function Prototypes -----------------------------------------

//static int ParseByte(const char *byteStr, uint8_t *byte);
//static int ParseUInt16(const char *str, uint16_t *val16);
//static int ParsePortPinStr(const char *portPinStr, uint8_t *portNum, uint8_t *pin, AllowADC_t allowAdc);
//static int ParseReg(int uartDev, const char *regStr, RegMap_t *regFound);
/*
static void ProcessInfoCommand(int uartDev);
uint16_t ProcessGetCommand(int uartDev, const char *portPinStr);
static void ProcessSetCommand(int uartDev, const char *portPinStr, const char *valStr);
static void ProcessGetDirCommand(int uartDev, const char *portPinStr);
static void ProcessSetDirCommand(int uartDev, const char *portPinStr, const char *valStr);
static void ProcessReadRegCommand(int uartDev, const char *regStr);
static void ProcessWriteRegCommand(int uartDev, const char *regStr, const char *valStr);
static void Usage(void);
 */
void GetFromMSP(char* rec, int len, int uartDev);
// ---- Functions -----------------------------------------------------------

uint16_t uart(const char* gCmdStr, const char* gVal, int uartDev);

//void start_pwm(int uartDev);

void set_M_left(float gVal, int uartDev); // gVal de 0 a 1
void set_M_right(float gVal, int uartDev); // gVal de 0 a 1

float sense_adc(int channel, int uartDev);
//int sense_c(int channel, int uartDev);
int start_msp(const char *uartDevName);
void stop_msp(int uartDev);

//***************************************************************************

/**
 *   Main entry point
 */
struct termios uartOptionsBkp;

#define BAUDRATE B9600

int start_msp(const char *uartDevName) {
    int uartDev;
    struct termios uartOptions;
    int cmdIdx;
    LogInit(stdout);

    // Try to open the uart device

    if ((uartDev = open(uartDevName, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) {
        LogError("Error  opening '%s': %s\n", uartDevName, strerror(errno));
        exit(1);
    }
    if (tcgetattr(uartDev, &uartOptions) == -1) {
        LogError("Can't get attributes from '%s': %s\n", uartDevName, strerror(errno));
        exit(1);
    }
    uartOptionsBkp = uartOptions;

    // Setting baudrate
    if (cfsetispeed(&uartOptions, BAUDRATE) == -1) {
        LogError("Unable to set input-baudrate for '%s': %s\n", uartDevName, strerror(errno));
        exit(1);
    }
    if (cfsetospeed(&uartOptions, BAUDRATE) == -1) {
        LogError("Unable to set output-baudrate for '%s': %s\n", uartDevName, strerror(errno));
        exit(1);
    }
    // Setting the rawmode

    uartOptions.c_cflag &= ~(CSIZE | PARENB | CSTOPB);
    uartOptions.c_cflag |= (CS8 | CREAD | CLOCAL); //8 Bits (CS8)
    //uartOptions.c_cflag |= (CS7 | CREAD | CLOCAL); //7 Bits (CS7)
    uartOptions.c_cflag &= ~(CRTSCTS);
    uartOptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    uartOptions.c_lflag |= (FLUSHO);
    uartOptions.c_iflag &= ~(IXON | IXOFF | IXANY | INPCK | ISTRIP); //~(ICRNL | BRKINT );
    uartOptions.c_iflag |= (IGNBRK);
    uartOptions.c_oflag &= ~(OPOST);
    uartOptions.c_cc[VMIN] = 1; // Only 1 char to read!
    uartOptions.c_cc[VTIME] = 0;


    if (uartOptions.c_lflag & (ICANON | ECHO | ECHOE | ISIG)) {
        LogError("Unable to set terminalmode for '%s': %s\n", uartDevName, strerror(errno));
        exit(1);

    }
    if (uartOptions.c_iflag & (IXON | IXOFF | IXANY | INPCK | ISTRIP)) {
        LogError("Unable to set terminalmode for '%s': %s\n", uartDevName, strerror(errno));
        exit(1);
    }
    // now clean the  line ...
    if (tcflush(uartDev, TCIOFLUSH) == -1) {
        LogError("Unable to flush data from '%s': %s\n", uartDevName, strerror(errno));
        exit(1);
    }
    // now clean the  line ...
    if (tcflow(uartDev, TCOON) == -1) {
        LogError("Unable to flush data from '%s': %s\n", uartDevName, strerror(errno));
        exit(1);
    }

    // ... and activate the settings for the port
    if (tcsetattr(uartDev, TCSANOW, &uartOptions) == -1) {
        LogError("Unable to set attributes for '%s': %s\n", uartDevName, strerror(errno));
        exit(1);
    }

    //start_pwm(uartDev);
    int status;
    for (cmdIdx = 0; cmdIdx < gNumCmds; cmdIdx++) {
        switch (gCmdMap[ cmdIdx ].cmd) {
            case CMD_GET_GIRO:
                if ((status = regcomp(&(gCmdMap[ cmdIdx ].re), ".*\\|g\\|>([0-9]{1,4})<.*", REG_EXTENDED)) != 0) {
                    printf("ERROR:REGEXP:%d\n", status);
                }
                break;
            case CMD_GET_DIST:
                if ((status = regcomp(&(gCmdMap[ cmdIdx ].re), ".*\\|ADC([0-9])\\|>([0-9]{1,4})<.*", REG_EXTENDED)) != 0) {
                    printf("ERROR:REGEXP:%d\n", status);
                }
                break;
            default: break;
        }
    }
    return uartDev;
}

void stop_msp(int uartDev) {
    if (tcsetattr(uartDev, TCSANOW, &uartOptionsBkp) == -1) {
        LogError("Unable to restore attributes for '%s': %s\n", uartDev, strerror(errno));
        exit(1);
    }
    close(uartDev);
}

/*
int sense_c(int channel, int uartDev) {
    char tmp[10];
    uint16_t data;
    sprintf(tmp, "c.%d", channel);
    data = uart("get", tmp, 0, "", uartDev);
    return data;
}
 */
float sense_adc(int channel, int uartDev) {

    char tmp[10];
    int data = 0;
    float tmp_return = -1;
    float data2 = 0;
    int i;
    int medicion[40];
    int medicion_max;
    int numMed = 3;

    sprintf(tmp, "%d", channel - 1);

    for (i = 0; i < numMed; i++) {

        data += uart("dist", tmp, uartDev);
        //fprintf(stdout, "\n  Medicion (%d,%)(%d)\n",channel,i,data);
        usleep(1000);
    }

    data2 = (double) data / (double) numMed;
    //fprintf(stdout, "\n  Medicion (%d,%d)(%f)\n",channel,i,data2);
    medicion[4] = 560; //  4cm
    medicion[5] = 475; //  5cm
    medicion[6] = 422; //  6cm
    medicion[7] = 362; //  7cm
    medicion[8] = 320; //  8cm
    medicion[9] = 290; //  9cm
    medicion[10] = 262; // 10cm
    medicion[11] = 240; // 11cm
    medicion[12] = 220; // 12cm
    medicion[13] = 205; // 13cm
    medicion[14] = 190; // 14cm
    medicion[15] = 175; // 15cm
    medicion[16] = 165; // 16cm
    medicion[17] = 157; // 17cm
    medicion[18] = 147; // 18cm
    medicion[19] = 142; // 19cm
    medicion[20] = 135; // 20cm
    medicion[21] = 128; // 21cm
    medicion[22] = 122; // 22cm
    medicion[23] = 117; // 23cm
    medicion[24] = 110; // 24cm
    medicion[25] = 104; // 25cm
    medicion[26] = 100; // 26cm
    medicion[27] = 96; // 27cm
    medicion[28] = 92; // 28cm
    medicion[29] = 90; // 29cm
    medicion[30] = 87; // 30cm
    medicion[31] = 84; // 31cm
    medicion[32] = 82; // 32cm
    medicion[33] = 80; // 33cm
    medicion[34] = 78; // 34cm
    medicion[35] = 76; // 35cm
    medicion[36] = 74; // 36cm
    medicion[37] = 72; // 37cm
    medicion[38] = 70; // 38cm
    medicion[39] = 68; // 39cm
    medicion[40] = 60; // 40cm
    medicion_max = 40; // hasta cuantos centimetros quieres sensar

    if (data2 > medicion[4]) {
        tmp_return = 0.0;
    } else {
        for (i = 5; i < medicion_max; i++)
            if (data2 > medicion[i]) {
                tmp_return = (float) (i + (float) (data2 - medicion[i - 1]) / (float) (medicion[i] - medicion[i - 1]));
                break;
            }

        if (tmp_return == -1) tmp_return = (float) medicion_max;
    }

    if (tmp_return == 0.0) tmp_return = 4.0;

    tmp_return -= 4.0; // 0 a 6.0
    tmp_return /= (float) medicion_max - 4.0; // 0 a 1.0
    return tmp_return;
}

float sense_giro(int uartDev) {
    float retval = 0;
    int data = 0;
    int numMed = 3;
    int i = 0;

    for (i = 0; i < numMed; i++) {
        data += uart("giro", "", uartDev);
        usleep(UART_WAIT);
        //printf("DATA: %d\n",data);
    }
    retval = data / numMed; //printf("DATA: %d\n",data);
    retval = (25 / 12) * retval + 400;
    retval /= 1000;
    retval = ((retval - 0.5) / (4))*(300) - 150;
    retval = retval + 9;
    //printf("SENSEGIRO:-->%f<--\n",retval);
    return retval; //retval;
}

void set_M_left(float gVal, int uartDev) {
    char gValStr[5];
    //printf("ORIG izq: %f\n", (float) gVal);
    gVal -= 0.5; // -0.5 a 0.5
    gVal *= 200; // -100   a 100

    sprintf(gValStr, "%d", (int) gVal);
    //printf("izq: %d\n", (int) gVal);
    uart("WheelLeft", gValStr, uartDev);
    usleep(UART_WAIT);
}

void set_M_right(float gVal, int uartDev) {
    char gValStr[5];
    //printf("ORIG der: %f\n", (float) gVal);
    gVal -= 0.5; // -0.5 a 0.5
    gVal *= 200; // -100   a 100

    sprintf(gValStr, "%d", (int) gVal);
    //printf("der: %d\n", (int) gVal);
    uart("WheelRight", gValStr, uartDev);
    usleep(UART_WAIT);
}

void set_M_left_v(float gVal, int uartDev) {
    char gValStr[5];
    sprintf(gValStr, "%d", (int) gVal);
    //printf("izq: %d\n", (int) gVal);
    uart("WheelLeft", gValStr, uartDev);
    usleep(UART_WAIT);
}

void set_M_right_v(float gVal, int uartDev) {
    char gValStr[5];
    sprintf(gValStr, "%d", (int) gVal);
    //printf("der: %d\n", (int) gVal);
    uart("WheelRight", gValStr, uartDev);
    usleep(UART_WAIT);
}

void set_ml(float gVal, int uartDev) {
    char TxStr[30];
     sprintf(TxStr, "mov ml %d\n", (int)gVal);
            fprintf(stdout, "%s", TxStr);
            write(uartDev, TxStr, strlen(TxStr));
            usleep(UART_WAIT);
}

void set_mr(float gVal, int uartDev) {
    char TxStr[30];

     sprintf(TxStr, "mov mr %d\n", (int)gVal);
            fprintf(stdout, "%s", TxStr);
            write(uartDev, TxStr, strlen(TxStr));
            usleep(UART_WAIT);

}
void set_l(float gVal, int uartDev) {
    char TxStr[30];

     sprintf(TxStr, "mov l %d\n", (int)gVal);
            fprintf(stdout, "%s", TxStr);
            write(uartDev, TxStr, strlen(TxStr));
            usleep(UART_WAIT);
}

void set_r(float gVal, int uartDev) {
    char TxStr[30];

     sprintf(TxStr, "mov r %d\n", (int)gVal);
            fprintf(stdout, "%s", TxStr);
            write(uartDev, TxStr, strlen(TxStr));
            usleep(UART_WAIT);

}
void set_f(float gVal, int uartDev) {
    char TxStr[30];
    sprintf(TxStr, "mov f %d\n", (int) gVal);
    fprintf(stdout, "%s", TxStr);
    write(uartDev, TxStr, strlen(TxStr));
    usleep(UART_WAIT);
}
void set_b(float gVal, int uartDev) {
    char TxStr[30];

            sprintf(TxStr, "mov b %d\n", (int)gVal);
            fprintf(stdout, "%s", TxStr);
            write(uartDev, TxStr, strlen(TxStr));
            usleep(UART_WAIT);
}

/*
void start_pwm(int uartDev) {

      uart("writereg", "", 40000, "ICR3", uartDev);
      uart("writereg", "", 3060, "OCR3A", uartDev);
      uart("writereg", "", 3060, "OCR3B", uartDev);
      uart("writereg", "", 162, "TCCR3A", uartDev);
      uart("writereg", "", 26, "TCCR3B", uartDev);
      uart("writereg", "", 24, "DDRE", uartDev);
    
}
 */
uint16_t uart(const char* gCmdStr, const char* gVal, int uartDev) {
    int cmdIdx;
    char gValStr[20] = "0";
    char TxStr[30];

    sprintf(gValStr, "%s", gVal);

    //gBase = (int) 'd'; // o h para hexadecimal

    // Verify that a command has been specified

    for (cmdIdx = 0; cmdIdx < gNumCmds; cmdIdx++) {
        if (strcasecmp(gCmdStr, gCmdMap[ cmdIdx ].cmdStr) == 0) {
            gCmd = gCmdMap[ cmdIdx ].cmd;
            gCmdRExp = gCmdMap[ cmdIdx ].re;
            break;
        }
    }
    if (gCmd == CMD_DEFAULT) {
        LogError("Unrecognized command '%s'\n", gCmdStr);
        exit(1);
    }

    // Process command specific arguments
    /*
        if (gCmd == CMD_INFO) {
            if (argc != 2) {
                LogError("Unexpected extra parameters\n");
                Usage();
                exit(1);
            }
        } else
            if ((gCmd == CMD_GET)
                || (gCmd == CMD_GET_DIR)) {
            if (argc < 3) {
                LogError("Expecting port.pin\n");
                Usage();
                exit(1);
            }

        } else
            if ((gCmd == CMD_SET)
                || (gCmd == CMD_SET_DIR)) {
            if (argc < 4) {
                LogError("port.pin followed by value\n");
                Usage();
                exit(1);
            }
        } else
            if (gCmd == CMD_READ_REG) {
            if (argc < 3) {
                LogError("Expecting register index\n");
                Usage();
                exit(1);
            }
        } else
            if (gCmd == CMD_WRITE_REG) {
            if (argc < 4) {
                LogError("Expecting register index and value\n");
                Usage();
                exit(1);
            }
        }

        if (gDebug) {
            Log("uartAddr:0x%02x Cmd: %s (%d)", gI2cAddr, gCmdStr, gCmd);
            if (gPortPinStr != NULL) {
                Log(" Pin: %s", gPortPinStr);
            }
            if (gValStr != NULL) {
                Log(" Val: %s", gValStr);
            }
            Log("\n");
        }
     */

    switch (gCmd) {

        case CMD_INFO:
        {
            //ProcessInfoCommand(uartDev);
            break;
        }

        case CMD_GET:
        {
            //return ProcessGetCommand(uartDev, gPortPinStr);
            break;
        }

        case CMD_SET:
        {
            //ProcessSetCommand(uartDev, gPortPinStr, gValStr);
            break;
        }

        case CMD_GET_DIR:
        {
            //ProcessGetDirCommand(uartDev, gPortPinStr);
            break;
        }

        case CMD_SET_DIR:
        {
            //ProcessSetDirCommand(uartDev, gPortPinStr, gValStr);
            break;
        }

        case CMD_READ_REG:
        {
            //ProcessReadRegCommand(uartDev, gRegStr);
            break;
        }

        case CMD_WRITE_REG:
        {
            //ProcessWriteRegCommand(uartDev, gRegStr, gValStr);
            break;
        }
        case CMD_WHEEL_L:
        {
            //fprintf(stdout, "Rueda Izquierda %s\n",gVal);
            write(uartDev, "ledm 1\n", 7);
            usleep(LEDM_WAIT);

            sprintf(TxStr, "mov ml %s\n", gVal);
            fprintf(stdout, "%s\n", TxStr);
            write(uartDev, TxStr, strlen(TxStr));
            usleep(UART_WAIT);

            write(uartDev, "ledm 2\n", 7);
            usleep(LEDM_WAIT);
            break;
        }
        case CMD_WHEEL_R:
        {
            write(uartDev, "ledm 1\n", 7);
            usleep(LEDM_WAIT);

            //fprintf(stdout, "Rueda Derecha %s\n",gVal);
            sprintf(TxStr, "mov mr %s\n", gVal);
            write(uartDev, TxStr, strlen(TxStr));
            fprintf(stdout, "%s\n", TxStr);
            usleep(UART_WAIT);

            write(uartDev, "ledm 2\n", 7);
            usleep(LEDM_WAIT);
            break;
        }
        case CMD_GET_DIST:
        {
            char rec[16];
            char *ret;
            sprintf(TxStr, "m %s\n", gVal);
            char buf[256];
            int retval;
            size_t nmatch = 3;
            regmatch_t pmatch[nmatch];
            int channelOut = atoi(gVal);
            int channelIn;
            do {

                do {
                    for (int i = 0; i < 13; i++) {
                        rec[i] = 0;
                    }
                    if (tcflush(uartDev, TCIOFLUSH) == -1) {
                        LogError("Unable to flush data from '%s': %s\n", uartDev, strerror(errno));
                        exit(1);
                    }
                    //write(uartDev, "ledm 1\n", 7);
                    usleep(LEDM_WAIT);

                    write(uartDev, TxStr, strlen(TxStr));
                    usleep(UART_WAIT);

                    //write(uartDev, "ledm 2\n", 7);
                    usleep(LEDM_WAIT);
                    GetFromMSP(rec, 13, uartDev);

                } while (rec[0] != '|' || strlen(rec) < 5);


                //fprintf(stdout, "\nGetFromMSP %s\n %d", rec,strlen(rec));
                //fprintf(stdout, "GetFromMSP |%s| \n", rec);

                retval = regexec(&gCmdRExp, rec, nmatch, pmatch, 0);
                //fprintf(stdout, "regexec %d\n",retval);
                ret = rec;
                ret = ret + pmatch[1].rm_so;
                *(ret + pmatch[1].rm_eo - pmatch[1].rm_so) = '\0';
                channelIn = atoi(ret);
            } while (retval != 0 || channelIn != channelOut);
            ret = rec;
            ret = ret + pmatch[2].rm_so;
            *(ret + pmatch[2].rm_eo - pmatch[2].rm_so) = '\0';
            return atoi(ret);
            break;
        }
        case CMD_GET_GIRO:
        {
            char rec[10];
            char *ret;
            sprintf(TxStr, "giro ");
            size_t nmatch = 2;
            regmatch_t pmatch[nmatch];
            do {
                if (tcflush(uartDev, TCIOFLUSH) == -1) {
                    LogError("Unable to flush data from '%s': %s\n", uartDev, strerror(errno));
                    exit(1);
                }
                write(uartDev, "led 1 1\n", 8);
                usleep(UART_WAIT);
                write(uartDev, TxStr, strlen(TxStr));
                usleep(UART_WAIT);
                write(uartDev, "led 1 0\n", 8);
                usleep(UART_WAIT);
                GetFromMSP(rec, 10, uartDev);
            } while (regexec(&gCmdRExp, rec, nmatch, pmatch, 0) != 0);
            ret = rec;
            ret = ret + pmatch[1].rm_so;
            *(ret + pmatch[1].rm_eo - pmatch[1].rm_so) = '\0';
            return atoi(ret);
            break;
        }
    }

    return 0;

} // main

//***************************************************************************

/**
 *   Get data from the MSP
 */

void GetFromMSP(char* rec, int len, int uartDev) {
    int i = 0;
    fd_set rfds;
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    FD_ZERO(&rfds);
    FD_SET(uartDev, &rfds);
    for (i = 0; i < len; i++) {
        if (select(uartDev + 1, &rfds, NULL, NULL, &tv) > 0) {
            read(uartDev, &(rec[i]), 1);
            if (rec[i] == '\n') {
                break;
            }
        } else {
            LogError("Unable to read from uart\n");
            break;
        }
    }
    //    while (i < len) {
    //        int leidos = read(uartDev, &(rec[i]), 1);
    //        if (leidos == 0)
    //            usleep(1000);
    //        else if (leidos > 0) {
    //            if (rec[i] == '\n') {
    //                break;
    //            }
    //            i += leidos;
    //        } else {
    //            LogError("Unable to read from uart\n");
    //            //fprintf(stdout, "Rec (%s)\n", rec);
    //            usleep(1000);
    //            break;
    //        }
    //    }
    //  fprintf(stdout, "Rec (%s)\n", rec);
    //    for(i = 0;i<len;i++)
    //    {
    //                if (select(uartDev + 1, &rfds, NULL, NULL, &tv) > 0) {
    //                    read(uartDev, &(rec[i]), 1);
    //                    if (rec[i] == '\n') {
    //                        break;
    //                    }
    //                } else {
    //                    LogError("Unable to read from uart\n");
    //                    fprintf(stdout, "Rec (%s)\n", rec);
    //                    usleep(1000);
    //                    break;
    //                }
    //    }
} // GetFromMSP
