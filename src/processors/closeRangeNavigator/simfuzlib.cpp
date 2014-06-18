/*
 * FILE: simfuzlib.cpp, v.1.0.0, 6/27/2008
 * Author: Tomas V. Arredondo
 *
 * SimFuzLib: A simple and flexible MIMO Fuzzy implementation in C++.
 *
 * This is NOT a tutorial on Fuzzy as there are many tutorials and books on this
 * subject see http://en.wikipedia.org/wiki/Fuzzy_logic for more information.
 *
 *
 * DISCLAIMER: No liability is assumed by the author for any use made
 * of this program.
 * DISTRIBUTION: Any use may be made of this program, as long as the
 * clear acknowledgment is made to the author in code and runtime executables
 */
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

//#include <iostream>
#include "simfuzlib.h"

using namespace simfuzlib;


void simfuzlib::EvalMF(const float x, const int tmp_fn, float& tmp_mf)
{ // [0]: Low, [1]: Med, [3]: High)
    if ( (x >= 0) && (x < 0.25) )
    {
        if (tmp_fn==0)
            tmp_mf = 1;
        if (tmp_fn==1)
            tmp_mf = 0;
        if (tmp_fn==2)
            tmp_mf = 0;
    }
    else if ( (x >= 0.25) && (x < 0.5) )
    {
        if (tmp_fn==0)
            tmp_mf = 2 - 4*x;
        else if (tmp_fn==1)
            tmp_mf = 4*x - 1;
        else if (tmp_fn==2)
            tmp_mf = 0;
    }
    else if ( (x >= 0.5) && (x < 0.75) )
    {
        if (tmp_fn==0)
            tmp_mf = 0;
        else if (tmp_fn==1)
            tmp_mf = 3 - 4*x;
        else if (tmp_fn==2)
            tmp_mf = 4*x -2;
    }
    else if ( (x >= 0.75) && (x <= 1.0) )
    {
        if (tmp_fn==0)
            tmp_mf = 0;
        if (tmp_fn==1)
            tmp_mf = 0;
        if (tmp_fn==2)
            tmp_mf = 1;
    }
    else if ( x < 0 )
    {
        if (tmp_fn==0)
            tmp_mf = 0;
        if (tmp_fn==1)
            tmp_mf = 0;
        if (tmp_fn==2)
            tmp_mf = 0;
    }
    else if ( x > 1 )
    {
        if (tmp_fn==0)
            tmp_mf = 0;
        if (tmp_fn==1)
            tmp_mf = 0;
        if (tmp_fn==2)
            tmp_mf = 0;
    }
}

