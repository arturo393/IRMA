//***********************************************************************
// * IRMA2: Laser Process Header
//***********************************************************************/
//*******************************************************************
// Title: laser.cpp
// Authors: Tomas Arredondo, Victor Magaña
// Version: v.0.0.1
// Date: 09/15/2009
//
// First aproach of the laser used on IRMA-2 Project
//
// Version: v.0.0.2
// Copyright (C) 2013 by Victor Magaña
// Contact: vmagana@gmail.com
// Date: Agosto 2013
//
// DISCLAIMER:
// No liability is assumed by the author for any use made of this program.
//
// DISTRIBUTION:
// Any use may be made of this program, as long as the clear
// acknowledgment is made to the author in code and runtime executables
//
//*******************************************************************
/******************************************************************************
 *
 *   Copyright (c) 2009 Tomas Arredondo, Víctor Magaña
 *		<tomas.arredondo@usm.cl>
 *
 *   This program is open-source software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License version 3 as
 *   published by the Free Software Foundation.
 *
 *   See README and LICENSE for more details.
 *
 ******************************************************************************/

#ifndef _IRMA2_LASER_H_
#define _IRMA2_LASER_H_


//*******************************************************************
// Librery Declaration
#include <vector>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <math.h>
#include "urg-0.8.18/include/cpp/UrgDevice.h"
#include "polar_matching/polar_matching/polar_match.h"
#include <iostream>
#include <cassert>
#include <locale>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "CImg.h"
#include "ekf.h"
#include "view.h"
#include "calculos.h"

#include <cstdlib>
#include <cstdio>
#include <sys/types.h>

#include "../base/processor.h"
#include "../../common/configFile/configFile.h"



// Configurable values
#define INTERP  1      // 0 - simple ICP, 1 - ICP with interpolation
#define NIT  10        // number of ICP iterations for each scan-match
#define GATE1  0.5     //; % 1st data association gate for each point in scan
#define GATE2  0.05    //;% 2nd data association gate for each point in scan
// Constants
#define MAXR = 7.8      //; % this laser data times-out at ~7.9 m


//********************-***********************************************
// Global Variables Definition
const int N_ANGULOS = 726; // Number of infrared sensors


//*******************************************************************
// MonitorProcessor Class Declaration: BEGIN
//*******************************************************************
using namespace qrk;
using namespace std;
int n_pose = 0;
class LaserProcessor : public ProcessorInterface {
private:

    ExecutiveData *pCDAExecutive;
    LaserData *pCDALaser;
    double x[N_ANGULOS];
    UrgDevice urg;
    int sleep_time;
    bool primera_medicion;
    PMScan *primero;
    PMScan *segundo;
    PMScan *img_scan;
public:

    LaserProcessor() {
        //pCDA=0;
        printf("Constructor\n");
        primera_medicion = true;
        primero = (PMScan *) malloc(sizeof (PMScan));
        segundo = (PMScan *) malloc(sizeof (PMScan));
        img_scan = (PMScan *) malloc(sizeof (PMScan));
    }

    int init();
    int step();
    int cleanup();
};
//*******************************************************************
// MonitorProcessor Class Declaration: END
//*******************************************************************

#endif
