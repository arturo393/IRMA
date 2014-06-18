/***********************************************************************
* IRMA2: mapper.h
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
#ifndef DYNMAPPER_H_INCLUDED
#define DYNMAPPER_H_INCLUDED

#include <stdio.h>
#include <math.h>
#include "MapClass.h"
#include "../base/processor.h"
#include <unistd.h>
#include <sys/shm.h>
#include "../../common/configFile/configFile.h"

class MapperProcessor : public ProcessorInterface
{
private:
   //Define pointers for CDA
   ExecutiveData *pExecutive;
   MapperData *pMapper;
   CloseNavData *pCloseNav;
   //perception values. Is filled with function ObtainPoints() if values: 0,0 -> nothing detected
   //                                                                     x,y -> obstacle
   int perceptionValues[5][2];

   //Flex and MegaMaps which are representing free area
   MapClass *flexMap_1; //flexible map 1
   MapClass *flexMap_2; //flexible map 2
   MapClass *flexMap_3; //flexible map 3
   MapClass *megaMap_1; //megaMap 1
   MapClass *megaMap_2; //megaMap 2
   //Internal Map which represents occupied area
   MapClass *intMap;
   MapClass *classicMap;

   //Configuration variables.
   bool mapping;
   bool saveCategories();
   int mapping_mode;
   string output_map;
   string input_map;
   int max_rectangle_size;
   int output_filter;

public:

    MapperProcessor() : ProcessorInterface()
    {
       ;
    }
    //Interface functions:
    int init();
    int step();
    int cleanup();
    //Dynamic Mapper Functions
    void MapperExecutive();
    void UpdateCategories(MapClass*);
    int adaptMeasurement(float);
    void obtainPoints(int[5][2], ExecutiveData*);
    void Add_IntMap(int[5][2]);
    void Add_FlexMap(int[5][2]);

};

#endif // DYNMAPPER_H_INCLUDED
