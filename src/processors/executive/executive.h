/***********************************************************************
 * IRMA2: Executive Process Header
/***********************************************************************/
//*******************************************************************
// Title: executive.h
// Authors: Tomas Arredondo, Gabriel Cisternas, Renato Agurto, Nicolas Navarro, Linus Casassa.
//
// First aproach of the IRMA-2 Executive Process
// Temporal management of the blackboard scheme, and software-hardware
// interaction layer for IRMA-2 proyect
//
// Authors: Tomas Arredondo, Gabriel Cisternas, Renato Agurto, Linus Casassa.
// Version: v.1.0.0
// Date: 07/30/2009
//
//
// Version: v.1.0.1
// Copyright (C) 2009 by Nicolas Navarro
// Contact: nicolas.navarro.guerrero@gmail.com or nicolas.navarro@gmx.de
// Date: November 2009
//
// DISCLAIMER:
// No liability is assumed by the author for any use made of this program.
//
// DISTRIBUTION:
// Any use may be made of this program, as long as the clear
// acknowledgment is made to the author in code and runtime executables
//*******************************************************************

#ifndef _IRMA2_EXECUTIVE_H_
#define _IRMA2_EXECUTIVE_H_


//*******************************************************************
// Librery Declaration

#include <unistd.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include "robostix.h"
#include "msp.h"
#include "../base/processor.h"
#include "../../common/configFile/configFile.h"
#include "clientdummy.h"
#include <time.h>
#include <sys/time.h>
#include <iostream>
using namespace std;

//#define MANUAL_MOVE  // Para movimiento manual
#define LOG          // Para habilitar log
#ifdef LOG
#include <iostream>
#include <fstream>
#endif
//*******************************************************************
// Global Variables Definition

// Definition of time lenght for each step in usec
const int STEP_SIZE = 250000;

// Definition for the maximum correction allowed
// for orientation using the GPS data
const int MAX_DEV = 360;

const double PI = 3.14159265;

// Definition of inital position and orientation (in mm and degrees)
const int InitPosX = 280;
const int InitPosY = 185;
const int InitPosOrientation = 90;

//#define InitPosX 225
//#define InitPosY 1685
//#define InitPosOrientation 0

// Selection of localization model. LSQ = 0, Splines = 1.
const int LocModel = 1;


//*******************************************************************
// ExecutiveProcessor Class Declaration: BEGIN
//*******************************************************************

class ExecutiveProcessor : public ProcessorInterface {
private:
    int test;
    //       Robostix rs;
    //       MSP msp;
    DriverSerial* serial; // driver comun para comunicacion con microcontroladores

    int rightWheelCounter;
    int leftWheelCounter;
    int rightEncoder;
    int leftEncoder;
    int leftEncoderPS;
    int rightEncoderPS;
    std::ofstream fout; //ARCHIVO DE SALIDA PARA ANALISIS
    ExecutiveData *pExecutive;
    LaserData *pLaser;
    NetworkData *pNetwork;
    MapperData *pMapper;
    MonitorData *pMonitor;
    bool use_manual_to_move;
    int num_executed_steps;
    int orient_steps;
    bool free_steps;
    bool move_ready_flag;

    float modelo4[3][15], modelo3[3][10], modelo2[3][6], modelo1[3][3];
    float modeloSpline[61][61][3];

    int deltaXtotal, deltaYtotal;

    //Configuration file variables.
    int MAX_STEPS;
    int GPS_STEPS;
    int InitPosX;
    int InitPosY;
    int InitPosOrientation;
    int correction_mode; //1 interpolation, 2 constant

    int current_X;
    int current_Y;
    // orientation is measured in degrees. 0 degrees means east, 90 means north, etc.
    int current_orientation;

    void callMapper();
    float getNormalized(float f);
    void saveLog(string string);

public:
      int times;
      int movement_times;
      int temp_speed_percent;
      int temp_movement ;

    ~ExecutiveProcessor() {
#ifdef LOG
        fout << "FInalizado";
        fout.close();
#endif
    }

    ExecutiveProcessor() : ProcessorInterface() {
#ifdef MSP_
        
        serial = (DriverSerial*) new MSP(); //hacerlo condicional dependiendo del micro
        //serial = (DriverSerial*) new clientdummy();
        // fprintf(stdout, "\n clientdummy \n");
#else
        serial = (DriverSerial*) new Robostix(); //hacerlo condicional dependiendo del micro
#endif
        use_manual_to_move = true;
        rightWheelCounter = 0;
        leftWheelCounter = 0;

        leftEncoder = serial->encoder(0);
        leftEncoderPS = leftEncoder;
        rightEncoder = serial->encoder(1);
        rightEncoderPS = rightEncoder;

    }

    int init();
    int step();
    int cleanup();
    int move();
    void manual_move();

    void getEncoderState();
    void getSensorReadings(ExecutiveData *);
    void getLaserReading(ExecutiveData *, LaserData *,int estado,int vr,int vl);
    void localizationLSQ(float, float, int);
    void localizationSpline(float, float);
    void localization(float *);
    void loadLSQmodels();
    void loadSplineModel();

    void getGPSPosition();
    void getCoordinate(int *, int *);
    void adjust_constant(int *, int *);
    void adjust_interpolated(int*, int*, int);
    void setManualMove(bool manual_move);
};
//*******************************************************************
// ExecutiveProcessor Class Declaration: END
//*******************************************************************
#endif
