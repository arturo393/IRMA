/***********************************************************************
* IRMA2: CONTROL Defines Header
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
#ifndef _CONTROL_H_
#define _CONTROL_H_

enum enum_loop_cmd {
  step_cmd,
  stop_cmd,
  run_cmd,
  halt_cmd
};

enum enum_loop_state {
  stopped_state,
  running_state,
  stepping_state,
  error_state
};


#endif
