/***********************************************************************
* IRMA2: Mapper CDA Shared Mem Header
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

#ifndef _MAPPER_COMMON_DATA_AREA_H_
#define _MAPPER_COMMON_DATA_AREA_H_

#include "controlDataArea.h"

// nCategories determines the maximun number of categories allowed for the internal map saved in CDA.
// Each category represents one obstacle.
const int nCategories = 400;

// hRoom and wRoom each respectively defines the height and the width (in mm) of the testing room.
const int hRoom = 2000;
const int wRoom = 2500;


// Pointers NOT ALLOWED in this structure!
struct MapperData {
	struct control ctrl;     // no borrar
	int mapper_int;
	//bool mapper_execute;

	float categories[nCategories][4];
	int activeNodes;
	bool mapping_flag;
	int room_width;
	int room_height;
};

#endif
