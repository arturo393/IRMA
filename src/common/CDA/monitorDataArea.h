/***********************************************************************
* IRMA2: Monitor CDA Shared Mem Header
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

#ifndef _MONITOR_COMMON_DATA_AREA_H_
#define _MONITOR_COMMON_DATA_AREA_H_

//*******************************************************************
// Librery Declaration
#include "controlDataArea.h"

//*******************************************************************

// Global Variables Definition
const char CRN	= 1;	// 1: Close Range Navigator
const char LRN	= 2;	// 2: Long Range Navigator
const char RTN	= 3;	// 3: Real Time Navigator
const char FN  = 4;	// 4: Feature Navigator

// Pointers NOT ALLOWED in this structure!
struct MonitorData {
    struct control ctrl;     // no borrar
    int monitor_int;
    int monitor_current_nav;
    bool monitor_hysteresis;
};

#endif
