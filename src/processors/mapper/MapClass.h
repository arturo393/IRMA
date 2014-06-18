/***********************************************************************
* IRMA2: MapClass.h
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
#ifndef MAPCLASS_H_INCLUDED
#define MAPCLASS_H_INCLUDED

#include <stdio.h>
#include <math.h>
#include "../../common/CDA/mapperDataArea.h"

//only needed for testing:
//Function output:
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/shm.h>


using std::ifstream;
using std::ofstream;
using std::endl;


class MapClass
{
private:
    int worldSizeX;
    int worldSizeY;
    float alpha;
    float beta;
    float rho;

public:
   //Constructor initilization process:
   MapClass(int a, int b, float c, float d);
   //*******************************************************************************************
   //All these Data will be CDA-Data:
   //Datastruct for the categories nCategories number of maximal categories to be defined before
   float category[nCategories][4];
   int activeNodes;
   //*******************************************************************************************
   void AddPoint_Map(int[2]); //adds point into map
   void Clean_Map(int[2], float); //cleans map area outside of sensor range
   void Output(int,int,int); //saves map into text file
   void Output(const char a_fileName[],int,int); //saves map into text file
};

#endif // MAPCLASS_H_INCLUDED
