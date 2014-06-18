/***********************************************************************
* IRMA2: PROCESS Defines Header
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
#ifndef _IRMA2_PROCESS_H_
#define _IRMA2_PROCESS_H_

#define NUM_PROCESS_AREAS 11

// Do not CHANGE the ORDER of this enum!
// Add new entries very carefully into the CDA code :-)
enum enum_process_areas {
  CLOSE_NAV_AREA=0,
  LONG_NAV_AREA=1,
  FEATURE_NAV_AREA=2,
  EXECUTIVE_AREA=3,
  MAPPER_AREA=4,
  MONITOR_AREA=5,
  NETWORK_AREA=6,
  OBJ_COGNITIVE_AREA=7,
  NUM_COGNITIVE_AREA=8,
  CAM_BLOB_AREA=9,
  LASER_AREA = NUM_PROCESS_AREAS -1
};

#endif
