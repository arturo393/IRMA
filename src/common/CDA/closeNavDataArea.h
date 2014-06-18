/***********************************************************************
* IRMA2: CLOSE NAV CDA Shared Mem Header
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

#ifndef _CLOSE_NAV_COMMON_DATA_AREA_H_
#define _CLOSE_NAV_COMMON_DATA_AREA_H_

#include "controlDataArea.h"

// Pointers NOT ALLOWED in this structure!
struct CloseNavData {
    struct control ctrl;     // no borrar
    int crn_execute;
    float crn_speed;
    float crn_steering;
    bool crn_move_ready_flag;
};

#endif
