/***********************************************************************
* IRMA2: Network CDA Shared Mem Header
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
#ifndef _NETWORK_COMMON_DATA_AREA_H_
#define _NETWORK_COMMON_DATA_AREA_H_

#include "controlDataArea.h"

// Pointers NOT ALLOWED in this structure!
struct NetworkData {
    struct control ctrl;     // no borrar
    int net_int;

    bool send_map_flag;
    bool get_position_flag;
    bool start_log_flag;
};

#endif
