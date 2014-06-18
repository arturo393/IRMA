/***********************************************************************
* IRMA2: Object Cognitive CDA Shared Mem Header
************************************************************************/
/******************************************************************************
*
*   Copyright (c) 2010 Pablo Benapres     <pbenapres@gmail.com>
*
*   This program is open-source software; you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License version 3 as
*   published by the Free Software Foundation.
*
*   See README and LICENSE for more details.
*
******************************************************************************/

#ifndef _NUM_COGNITIVE_COMMON_DATA_AREA_H_
#define _NUM_COGNITIVE_COMMON_DATA_AREA_H_

#include "controlDataArea.h"


// Pointers NOT ALLOWED in this structure!
struct NumCogData {
	struct control ctrl;     // no borrar
	//bool mapper_execute;

	int test;

};

#endif
