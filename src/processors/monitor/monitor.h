/***********************************************************************
 * IRMA2: Monitor Process Header
/***********************************************************************/
//*******************************************************************
// Title: monitor.cpp
// Authors: Tomas Arredondo, Renato Agurto, Nicolas Navarro
// Version: v.0.0.1
// Date: 09/15/2009
//
// First aproach of the monitor used on IRMA-2 Project
//
// Version: v.0.0.2
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
//
//*******************************************************************
/******************************************************************************
 *
 *   Copyright (c) 2009 Tomas Arredondo, Renato Agurto, Nicolas Navarro
 *		<tomas.arredondo@usm.cl>
 *
 *   This program is open-source software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License version 3 as
 *   published by the Free Software Foundation.
 *
 *   See README and LICENSE for more details.
 *
 ******************************************************************************/

#ifndef _IRMA2_MONITOR_H_
#define _IRMA2_MONITOR_H_


//*******************************************************************
// Librery Declaration
#include<iostream>
	using namespace std;
#include <string>
#include <stdio.h>
#include <sys/shm.h>
#include "../base/processor.h"
#include "../../common/configFile/configFile.h"


//*******************************************************************
// Global Variables Definition
const int NUM_INPUTS = 5; // Number of infrared sensors



const bool ON = true;
const bool OFF = false;
const bool EXEC = true;
const bool NAV = false;

//*******************************************************************
// MonitorProcessor Class Declaration: BEGIN
//*******************************************************************



class MonitorProcessor : public ProcessorInterface {
private:

    CloseNavData *pCDACloseNav;
    LongNavData *pCDALongNav;
    FeatureNavData *pCDAFeatureNav;
    ExecutiveData *pCDAExecutive;
    MonitorData *pCDAMonitor;

    int test;
    float min_sensor_dist;
    float x[NUM_INPUTS];
    int hysteresis_CRN_steps;
    int MAX_CLOSE_RANGE_HYSTERESIS; //50
    int current_nav;
    int sleep_time;
    int num_steps;
    int max_steps_nr;

    bool flag_fn;
    bool collision_flag;
    bool hysteresis_flag;
    bool status_CRN;
    bool status_LRN;
    bool status_FN;
    bool exec_or_nav;
    bool execute_ready_flag;
    bool mission_accomplished_flag;

    void updateVariables(void);
    void setNavigationMode(void);
    void setNavigationMode2(void);
    void updateExecutiveData(void);
    bool checkSensors(void);
    bool checkFlagFN(void);


public:

    MonitorProcessor() {
        //pCDA=0;
        min_sensor_dist = 0.2;
    }

    int init();
    int step();
    int cleanup();
};
//*******************************************************************
// MonitorProcessor Class Declaration: END
//*******************************************************************

#endif
