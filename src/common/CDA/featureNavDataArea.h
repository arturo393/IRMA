/***********************************************************************
* IRMA2: Feature Navigation Process CDA Shared Mem Header
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
#ifndef _FEATURE_COMMON_DATA_AREA_H_
#define _FEATURE_COMMON_DATA_AREA_H_

#include "controlDataArea.h"

#define max_N_Features 15

// Pointers NOT ALLOWED in this structure!
struct FeatureNavData
{
    struct control ctrl;     // no borrar

    //GENERAL FLAGS
    bool fn_mission_obsolete_flag; //true: Feature must quit the actual mission

    //RECOGNITION MISSION FLAGS
    float fn_speed;
    float fn_steering;
    bool fn_move_ready_flag; // Read and Write steps Flag
    bool fn_move_active_flag; // true: fetureNavigator takes control
    bool fn_route_obsolete_flag; // Monitor is no longer listening to the FeautreNavigator(eg. CloseRange is active)

    //NAVIGATION MISSION FLAGS
    int fn_featureArray[max_N_Features][2]; // coordenates are in [mm] . E.g. Map is 2500 x 2000
    int fn_nFeatures;
    bool fn_feature_list_ready_flag; // true: fetureNavigator has more feature position to return
};

#endif
