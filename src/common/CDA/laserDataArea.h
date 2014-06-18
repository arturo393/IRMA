/***********************************************************************
* IRMA2: Feature Navigation Process CDA Shared Mem Header
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
#ifndef _LASER_DATA_AREA_H_
#define _LASER_DATA_AREA_H_

#include "controlDataArea.h"
#define LASER_POINTS 768
#define LASER_START 44
#define LASER_END 725
#define LASER_FRONT 384
#define LASER_SLIT_DIVISION 1024
// Pointers NOT ALLOWED in this structure!
#define MAP_WIDTH 4096
#define MAP_HEIGTH 4096



struct LaserData
{
    struct control ctrl;     // no borrar 
    //Sensar Realiza el sensado cuando este valor es true
    bool sensar;
    //Define el estado del robot, 
    // 1:       Antes de movimiento
    // 2:       Despues de movimiento
    // 3:       Revición
    int estado;
    double read_data[LASER_POINTS];
    //Significado Valores
    // 0:       Obstaculo
    // 127:     Desconocido
    // 255:     Desocupado
    unsigned char map[MAP_WIDTH][MAP_HEIGTH];
    double timestamp;
    float dx;
    float dy;
    float ddir;
    double x;
    double y;
    double dir;
    int vr;
    int vl;
};

#endif
