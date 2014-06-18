/***********************************************************************
* IRMA2: IRMA Errors Header
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
#ifndef _IRMA2_ERRORS_H_
#define _IRMA2_ERRORS_H_

#define IRMA2_OK              0

// CDA ERRORS: CDA_             = 10-19
#define IRMA2_CDA_ERR_SHMGET  10
#define IRMA2_CDA_ERR_SHMAT   11
#define IRMA2_CDA_ERR_SHMCTL  12
#define IRMA2_CDA_ERR_SHMDT   13

// CNTRL ERRORS: CTRL_          = 20-49

// CNTRL ERRORS: SEM_           = 20-49

// CLOSE NAV ERRORS: CLNAV_     = 150-199

// LONG NAV ERRORS: LNGNAV_     = 200-249

// CAMERA ERRORS: CAM_          = 250-299
#define IRMA2_CAM_ERR_C

// NETWORK PROC ERRORS: NET_    = 300-349


#endif
