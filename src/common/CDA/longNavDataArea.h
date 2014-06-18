/***********************************************************************
* IRMA2: LONG NAVIGATION CDA Shared Mem Header
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

#ifndef _LONG_NAV_COMMON_DATA_AREA_H_
#define _LONG_NAV_COMMON_DATA_AREA_H_


//*******************************************************************
// Librery Declaration
#include "controlDataArea.h"

//*******************************************************************
// Global Variables Definition
const int MAX_STEP_NUMBER = 200;

// Pointers NOT ALLOWED in this structure!
struct LongNavData {
	struct control ctrl;				// NO borrar - NO delete
//	int lrn_execute;				// Tell who is making the robot moves
	bool lrn_move_ready_flag;		// False if the move is done
	bool lrn_route_obsolete_flag;	// True if the route obsolete
	bool lrn_mission_accomplished;	// True if the route obsolete

	// Information for EXECUTIVE
	float lrn_speed;				//	LRN - Speed command to the motors
	float lrn_steering;			// LRN - Steering command to the motors
	int lrn_speed_percent;
        int lrn_movement;
	// Information for MONITOR
	float fitness_curiosity;		// Normalized values [0, 1]
	float fitness_energy;			// Normalized values [0, 1]
	float fitness_homing;			// Normalized values [0, 1]
	float fitness_missions;			// Normalized values [0, 1]
	int route_lenght;
	int route[MAX_STEP_NUMBER][3];//
};

#endif
