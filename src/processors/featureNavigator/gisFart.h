/***********************************************************************
* IRMA2: gisFart.h
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
#ifndef _GIS_FART_H
#define _GIS_FART_H

#include <stdio.h>
//#include "feature.h"

#define nNeurons 10
#define Alpha 0.000001
#define Beta 1
#define Rho 0.95
#define mapSize 100
#define RETURN_NEAREST_NH true

class gisFart
{
private:

    int activeNeurons;
    float w[nNeurons][4];
    int map_activeNeurons;
    float **map_w;
    float map2fart (int,int);
    int mapWidth, mapHeight;


public:
    // Constructor
    gisFart();
    // Destructor
    ~gisFart();
    // Funciones miembro o metodos
    int set(int, int, int, float ** );
    int newPoint (int x, int y, bool = false);
    int printw(void);
};

#endif

