/***********************************************************************
* IRMA2: Feature Navigation Process Header
/***********************************************************************/
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
#ifndef _IRMA2_FNAV_H_
#define _IRMA2_FNAV_H_

#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>
#include <math.h>
#include "cameraComm.h"
#include "sql.h"
#include "database.h"
#include "gisFart.h"
#include "../base/processor.h"


#define DO_NOTHING  0
#define RECOGNITION 1
#define MISSION     2

class FNProcessor : public ProcessorInterface
{
private:
    FeatureNavData *pFeatureNav;
    ExecutiveData *pExecutive;
    MonitorData *pMonitor;
    MapperData * pMapper;
    sql I2GIS;
    gisFart neighborhood;
    cameraComm cmuCam;
    int op_mode;  // DO_NOTHING, RECOGNITION or MISSION
    string mission_file;
    int state;
    int feature_num; //Number of features in the DataBase
    bool list_generated_flag;
    bool no_more_missions;
    FILE * mFile;

    //Local Flags
      //RECOGNITION MISSION FLAGS
    bool local_move_ready_flag; // Read and Write steps Flag
    bool local_move_active_flag; // true: fetureNavigator takes control
    bool local_route_obsolete_flag; // Monitor is no longer listening to the FeautreNavigator(eg. CloseRange is active)

    //NAVIGATION MISSION FLAGS
    bool local_feature_list_ready_flag; // true: fetureNavigator has more feature position to return

    //GENERAL FLAGS
    bool local_mission_obsolete_flag; //true: Feature must quit the actual mission


    void readFlags(void);

    void doRecognition(void);
    void doMissions(void);


    bool isFeatureAlreadyInDB(void);
    bool featureIsInDB;
    bool featureChecked;


public:
    FNProcessor() : ProcessorInterface()
    {
       ;
    }

    int init();
    int step();
    int cleanup();

};

#endif
