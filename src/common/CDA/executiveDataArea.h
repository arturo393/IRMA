/***********************************************************************
 * IRMA2: EXECUTIVE CDA Shared Mem Header
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


#ifndef _EXECUTIVE_COMMON_DATA_AREA_H_
#define _EXECUTIVE_COMMON_DATA_AREA_H_


//*******************************************************************
// Librery Declaration
#include "controlDataArea.h"

//*******************************************************************
// Global Variables Definition


// Pointers NOT ALLOWED in this structure!

struct ExecutiveData {
    struct control ctrl; // no borrar

    bool on_moving;
    bool exec_ready_flag;
    bool exec_move_ready_flag;
    float exec_speed;
    float exec_steering;
    int exec_speed_percent;
    int exec_movement;
    float exec_adc[5];

    // posX and posY are measured in milimeters.
    int current_X;
    int current_Y;
    // orientation is measured in degrees. 0 degrees means east, 90 means north, etc.
    float current_orientation;

    // Buffers for gps acquired positions
    int posX_previa;
    int posY_previa;
    int orientacion_previa;

    // array to keep track of irma's position
    // path[step_no][var]
    // var:     0-> rotation
    //          1-> posX
    //          2-> posY
    int path_length;
    int path[5000][3];
    int steps_number;
    //Grilla de ocupacion
    //int grid[1024*5][1024*5];
    //Grilla ultima lectura
    //int laserSense[400][400];
};

#endif
