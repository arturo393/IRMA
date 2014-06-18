/***********************************************************************
 * IRMA2: CONTROL CDA Shared Mem Header
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
#ifndef _CONTROL_COMMON_DATA_AREA_H_
#define _CONTROL_COMMON_DATA_AREA_H_

#include <sys/types.h>
#include "../control.h"

// Pointers NOT ALLOWED in this structure!

struct control {
    pid_t pid;
    enum_loop_cmd loop; //none, step, halt
    int nloop;
    enum_loop_state state; //stopped, running, stepping, error
    long int loopTime;
};

#endif
