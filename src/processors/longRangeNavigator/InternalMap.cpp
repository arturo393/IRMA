//*******************************************************************
// Title: InternalMap.cpp
//
// Copyright (C) 2009 by IRMA Project
// Contact: tomas.arredondo@usm.cl
//
// Author: Nicolas Navarro
// Contact: nicolas.navarro.guerrero@gmail.com or nicolas.navarro@gmx.de
// Date: November 2009
//
// DISCLAIMER:
// No liability is assumed by the author for any use made of this program.
//
// DISTRIBUTION:
// Any use may be made of this program, as long as the clear
// acknowledgment is made to the author in code and runtime executables
//
//*******************************************************************
/******************************************************************************
*
*   Copyright (c) 2009 Tomas Arredondo, Nicolas Navarro
*		<tomas.arredondo@usm.cl>
*
*   This program is open-source software; you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License version 3 as
*   published by the Free Software Foundation.
*
*   See README and LICENSE for more details.
*
******************************************************************************/

// Declaracion de bibliotecas
#include "InternalMap.h"


//===================================================================



//*******************************************************************
// Internal Map Class Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------
InternalMap::InternalMap()
{
   mapWidth = MAP_WIDHT;
   mapHeight = MAP_HEIGHT;
   mapMeshSize = MAP_MESH_SIZE;
   mapVisitedCells = 0;
   selectedRoom = SLAM_ROOM;

   this->RebuildMap();
}


//-------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------
InternalMap::~InternalMap()
{
//···································································
#ifdef DYNAMIC_MAP // DYNAMIC_MAP Version of the Class
   // Free Memory
   for(int _y = 0; _y < oldYDim; _y++)
      delete [] MAP[_y];
   delete [] MAP;
#endif   // end DYNAMIC_MAP Version of the class
//···································································
}


//-------------------------------------------------------------------
// void initMap(void)
// This function Initializes the map cell's
//-------------------------------------------------------------------
inline void InternalMap::InitMap(void)
{
   // Initialization
   for(int _y = 0; _y < this->mapYDim; _y++)
   {
      for(int _x = 0; _x < this->mapXDim; _x++)
      {
         MAP[_y][_x] = 0;
      }
   }

   for(int _y = this->mapYDim; _y < MAP_CELLS_HEIGHT; _y++)
   {
      for(int _x = this->mapXDim; _x < MAP_CELLS_WIDTH; _x++)
      {
         MAP[_y][_x] = OBSTACLE;
      }
   }
}


//-------------------------------------------------------------------
// void cleanMap(void)
// This function clean the map cell's metadata
//-------------------------------------------------------------------
void InternalMap::CleanMap(void)
{
   this->mapVisitedCells = 0;

   for(int _x = 0; _x < this->mapXDim; _x++)
      for(int _y = 0; _y < this->mapYDim; _y++)
         this->MAP[_y][_x] &= 0x0F;
}


//-------------------------------------------------------------------
// void updateMap(void)
// Make a new MAP with the new seted parameters
//-------------------------------------------------------------------
void InternalMap::RebuildMap(void)
{
   this->mapTotalArea = mapWidth*mapHeight;
   this->mapFreeArea = mapTotalArea;

   this->mapXDim = lround((mapWidth*1.0)/mapMeshSize);
   this->mapYDim = lround((mapHeight*1.0)/mapMeshSize);
   this->mapTotalCells = mapXDim*mapYDim;
   this->mapFreeCells = mapTotalCells;

//···································································
#ifdef DYNAMIC_MAP   // DYNAMIC_MAP Version of the Class
   // Free Memory
   for(int _y = 0; _y < oldYDim; _y++)
      delete [] MAP[_y];
   delete [] MAP;

   // Allocate dynamic memory, Access as matrix
   // Remember: that C create first the rows and then columns
   MAP = new char*[mapYDim];
   for(int _y = 0; _y < mapYDim; _y++)
      MAP[_y] = new char[mapXDim];

   // To make a secure memory release
   oldXDim = mapXDim;   oldYDim = mapYDim;
#endif   // end DYNAMIC_MAP Version of the class
//···································································
   // Initialization
   this->InitMap();
}

//-------------------------------------------------------------------
void InternalMap::set_selected_room(const char _room)
{
   selectedRoom = _room;
   if( selectedRoom != FART_ROOM )  SetTestScenario(_room);
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
void InternalMap::Set_MAP(const int _width, const int _height)
{
   // Rectangular Empty Room (without Obstacles), do nothing
   // It is the default configuration
 selectedRoom = FART_ROOM;
   
   this->mapWidth = _width;
   this->mapHeight = _height;
      this->RebuildMap();
}
void InternalMap::Set_MAP_Dimension(const int _width, const int _height)
{
   this->mapWidth = _width;
   this->mapHeight = _height;
   this->RebuildMap();
}

void InternalMap::Set_SLAM_Map(const int _width, const int _height){
   selectedRoom = SLAM_ROOM;
   this->mapWidth = _width;
   this->mapHeight = _height;
   
   this->mapTotalArea = mapWidth*mapHeight;
   this->mapFreeArea = mapTotalArea;

   this->mapXDim = lround((mapWidth*1.0)/mapMeshSize);
   this->mapYDim = lround((mapHeight*1.0)/mapMeshSize);
   this->mapTotalCells = mapXDim*mapYDim;
   this->mapFreeCells = mapTotalCells;
   
         // Initialization
   for(int _y = 0; _y < this->mapYDim; _y++)
   {
      for(int _x = 0; _x < this->mapXDim; _x++)
      {
         MAP[_y][_x] = 0;
      }
   }
   
   for(int _y = 20; _y < 60; _y++)
      for(int _x = 0; _x < 20; _x++)          
             this->setcCellObstacle(_y,_x);
             
              for(int _y = 100; _y < 120; _y++)
      for(int _x = 90; _x < 100; _x++)          
             this->setcCellObstacle(_y,_x);
   
   for(int _y = this->mapYDim; _y < MAP_CELLS_HEIGHT; _y++)
       for(int _x = this->mapXDim; _x < MAP_CELLS_WIDTH; _x++)
           this->setcCellObstacle(_x,_y);
}

//-------------------------------------------------------------------
// void setTestScenario(int _room)
// Ideal MAP for the four (4) experimental Scenarios
//-------------------------------------------------------------------
void InternalMap::SetTestScenario(char _room)
{
   this->mapWidth = 2500;
   this->mapHeight = 2000;
   this->RebuildMap();
   
//···································································
   if( _room == TEST_ROOM )
   {
      selectedRoom = TEST_ROOM;
      this->RebuildMap();

      int _Xstart = 15;  int _Xend = 25;
      int _Ystart = 0;   int _Yend = 5;

      this->mapFreeArea -= ((_Xend - _Xstart)*(_Yend - _Ystart));
      // Here are the Obstacles
      for(int _x = _Xstart; _x < _Xend; _x++)
         for(int _y = _Ystart; _y < _Yend; _y++)
         {
            this->setcCellObstacle(_x, _y);
            this->mapFreeCells--;
         }  // end for(_y = _Ystart; _y < _Yend; _y++)
   }  // end if( _room == TEST_ROOM )

//···································································
   // L Room (without Obstacles)
   else if( _room == H_ROOM )
   {
      selectedRoom = H_ROOM;
      int auxXS, auxYS, auxXE, auxYE;
      for(int _cat = 0; _cat < CATEGORIES_H_ROOM; _cat++)
      {
         auxXS = lround( (FART_H_ROOM[_cat][0]) * mapXDim );
         auxYS = lround( (FART_H_ROOM[_cat][1]) * mapYDim );
         auxXE = lround( (1-FART_H_ROOM[_cat][2]) * mapXDim );
         auxYE = lround( (1-FART_H_ROOM[_cat][3]) * mapYDim );
         // For Debug Use: BEGIN
         if( auxXS < 0 )   fprintf(stdout, "Ideal Map H: X0 = %d\n", auxXS);
         if( auxYS < 0 )   fprintf(stdout, "Ideal Map H: Y0 = %d\n", auxYS);
         if( auxXE > mapXDim )  fprintf(stdout, "Ideal Map H: X1 = %d\n", auxXE);
         if( auxYE > mapYDim )  fprintf(stdout, "Ideal Map H: Y1 = %d\n", auxYE);
         // For Debug Use: END
         for(int _y = auxYS; _y < auxYE; _y++)
         {
            for(int _x = auxXS; _x < auxXE; _x++)
            {
               this->setcCellObstacle(_x, _y);
            }
         }
      }
   }  // end if( _room == H_ROOM )

//···································································
   // L Room (without Obstacles)
   else if( _room == L_ROOM )
   {
      selectedRoom = L_ROOM;
      int auxXS, auxYS, auxXE, auxYE;
      for(int _cat = 0; _cat < CATEGORIES_L_ROOM; _cat++)
      {
         auxXS = lround( (FART_L_ROOM[_cat][0]) * mapXDim );
         auxYS = lround( (FART_L_ROOM[_cat][1]) * mapYDim );
         auxXE = lround( (1-FART_L_ROOM[_cat][2]) * mapXDim );
         auxYE = lround( (1-FART_L_ROOM[_cat][3]) * mapYDim );
         // For Debug Use: BEGIN
         if( auxXS < 0 )   fprintf(stdout, "Ideal Map L: X0 = %d\n", auxXS);
         if( auxYS < 0 )   fprintf(stdout, "Ideal Map L: Y0 = %d\n", auxYS);
         if( auxXE > mapXDim )  fprintf(stdout, "Ideal Map L: X1 = %d\n", auxXE);
         if( auxYE > mapYDim )  fprintf(stdout, "Ideal Map L: Y1 = %d\n", auxYE);
         // For Debug Use: END
         for(int _y = auxYS; _y < auxYE; _y++)
         {
            for(int _x = auxXS; _x < auxXE; _x++)
            {
               this->setcCellObstacle(_x, _y);
            }
         }
      }
   }  // end if( _room == L_ROOM )

//···································································
   // O Room (without Obstacles)
   else if( _room == O_ROOM )
   {
      selectedRoom = O_ROOM;
      int auxXS, auxYS, auxXE, auxYE;
      for(int _cat = 0; _cat < CATEGORIES_O_ROOM; _cat++)
      {
         auxXS = lround( (FART_O_ROOM[_cat][0]) * mapXDim );
         auxYS = lround( (FART_O_ROOM[_cat][1]) * mapYDim );
         auxXE = lround( (1-FART_O_ROOM[_cat][2]) * mapXDim );
         auxYE = lround( (1-FART_O_ROOM[_cat][3]) * mapYDim );
         // For Debug Use: BEGIN
         if( auxXS < 0 )   fprintf(stdout, "Ideal Map O: X0 = %d\n", auxXS);
         if( auxYS < 0 )   fprintf(stdout, "Ideal Map O: Y0 = %d\n", auxYS);
         if( auxXE > mapXDim )  fprintf(stdout, "Ideal Map O: X1 = %d\n", auxXE);
         if( auxYE > mapYDim )  fprintf(stdout, "Ideal Map O: Y1 = %d\n", auxYE);
         // For Debug Use: END
         for(int _y = auxYS; _y < auxYE; _y++)
         {
            for(int _x = auxXS; _x < auxXE; _x++)
            {
               this->setcCellObstacle(_x, _y);
            }
         }
      }
   }  // end if( _room == O_ROOM )

//···································································
   // S Room (without Obstacles)
   else if( _room == S_ROOM )
   {
      selectedRoom = S_ROOM;
      int auxXS, auxYS, auxXE, auxYE;
      for(int _cat = 0; _cat < CATEGORIES_S_ROOM; _cat++)
      {
         auxXS = lround( (FART_S_ROOM[_cat][0]) * mapXDim );
         auxYS = lround( (FART_S_ROOM[_cat][1]) * mapYDim );
         auxXE = lround( (1-FART_S_ROOM[_cat][2]) * mapXDim );
         auxYE = lround( (1-FART_S_ROOM[_cat][3]) * mapYDim );
         // For Debug Use: BEGIN
         if( auxXS < 0 )   fprintf(stdout, "Ideal Map S: X0 = %d\n", auxXS);
         if( auxYS < 0 )   fprintf(stdout, "Ideal Map S: Y0 = %d\n", auxYS);
         if( auxXE > mapXDim )  fprintf(stdout, "Ideal Map S: X1 = %d\n", auxXE);
         if( auxYE > mapYDim )  fprintf(stdout, "Ideal Map S: Y1 = %d\n", auxYE);
         // For Debug Use: END
         for(int _y = auxYS; _y < auxYE; _y++)
         {
            for(int _x = auxXS; _x < auxXE; _x++)
            {
               this->setcCellObstacle(_x, _y);
            }
         }
      }
   }  // end if( _room == S_ROOM )

//···································································
   // S Room (without Obstacles)
   else if( _room == T_ROOM )
   {
      selectedRoom = T_ROOM;
      int auxXS, auxYS, auxXE, auxYE;
      for(int _cat = 0; _cat < CATEGORIES_T_ROOM; _cat++)
      {
         auxXS = lround( (FART_T_ROOM[_cat][0]) * mapXDim );
         auxYS = lround( (FART_T_ROOM[_cat][1]) * mapYDim );
         auxXE = lround( (1-FART_T_ROOM[_cat][2]) * mapXDim );
         auxYE = lround( (1-FART_T_ROOM[_cat][3]) * mapYDim );
         // For Debug Use: BEGIN
         if( auxXS < 0 )   fprintf(stdout, "Ideal Map T: X0 = %d\n", auxXS);
         if( auxYS < 0 )   fprintf(stdout, "Ideal Map T: Y0 = %d\n", auxYS);
         if( auxXE > mapXDim )  fprintf(stdout, "Ideal Map T: X1 = %d\n", auxXE);
         if( auxYE > mapYDim )  fprintf(stdout, "Ideal Map T: Y1 = %d\n", auxYE);
         // For Debug Use: END
         for(int _y = auxYS; _y < auxYE; _y++)
         {
            for(int _x = auxXS; _x < auxXE; _x++)
            {
               this->setcCellObstacle(_x, _y);
            }
         }
      }
   }  // end if( _room == T_ROOM )
//···································································
#ifdef T_SET_TEST_MAP_PRINT_HERE
printf("\nTEST SET TEST SCENARIO - PRINT THE CHOOSEN MAP HERE:\n");
for(int _y = this->mapYDim-1; _y >= 0; _y--)
{
   printf("%2d)", _y);
   for(int _x = 0; _x < this->mapXDim; _x++)
      printf("%x  ", this->getXYcCellData(_x,_y));

   printf("\n");
}
printf("  ");
for(int _x = 0; _x < this->mapXDim; _x++)
   printf("%2d)", _x);
printf("\n");
#endif   // end T_SET_TEST_MAP_PRINT_HERE
//···································································
}  // end Function setTestScenario(int _room)


//-------------------------------------------------------------------
void InternalMap::PrintMAPtoFile(const char a_fileName[])
{
   printf("PRINTING MAP IN FILE: %s\n", a_fileName);

   FILE *fp;
   if( (fp = fopen(a_fileName, "w")) )
   {
      fprintf(fp, "FREE CELLS: %6d", this->getFreeCellsNumber());
      fprintf(fp, "\nVISITED CELLS: %6d", this->getVisitedCellsNumber());
      fprintf(fp, "\nREVISITED CELLS: %6d", this->getRevisitedCellsNumber());
      fprintf(fp, "\n");

      // PRINT QUESTIONS VISITED
      for(int _y = (mapYDim - 1); _y >= 0; _y--)
      {
         for(int _x = 0; _x < mapXDim; _x++)
         {
            if( cCellRevisited(_x, _y) == true )      {  fprintf(fp, "R"); }
            else if( cCellVisited(_x, _y) == true )   {  fprintf(fp, "V"); }
            else if( cCellObstacle(_x, _y) == true )  {  fprintf(fp, "O"); }
            else if( cCellMission(_x, _y) == true )   {  fprintf(fp, "M"); }
            else                                      {  fprintf(fp, "·"); }
         }
         fprintf(fp, "\n");
      }
      fclose(fp);
   }
   else  {  printf("Error opening FILE in function: PRINTING MAP TO FILE\n");  }
}


//-------------------------------------------------------------------
// Mission in this Cells???
// Cartesian Coordinate System (0, 0) left lower corner
//-------------------------------------------------------------------
bool InternalMap::cCellMission(int _XCoord, int _YCoord) const
{
   if( (_XCoord >= mapXDim) || (_YCoord >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::cCellMission");

   bool aux = false;
   if( (this->MAP[(mapYDim-1)-_YCoord][_XCoord] & MISSION) == MISSION )   aux = true;

   return(aux);
}


//-------------------------------------------------------------------
// Obstacle in this Cells???
// Cartesian Coordinate System (0, 0) left lower corner
//-------------------------------------------------------------------
bool InternalMap::cCellObstacle(int _XCoord, int _YCoord) const
{
   if( (_XCoord >= mapXDim) || (_YCoord >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::cCellObstacle");

   bool aux = false;
   if( (this->MAP[(mapYDim-1)-_YCoord][_XCoord] & OBSTACLE) == OBSTACLE )   aux = true;

   return(aux);
}

//-------------------------------------------------------------------
// Visited Cells???
// Cartesian Coordinate System (0, 0) left lower corner
//-------------------------------------------------------------------
bool InternalMap::cCellRevisited(int _XCoord, int _YCoord) const
{
   if( (_XCoord >= mapXDim) || (_YCoord >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::cCellRevisited");

   bool aux = false;
   if( (this->MAP[(mapYDim-1)-_YCoord][_XCoord] & REVISITED) == REVISITED )   aux = true;

   return(aux);
}

//-------------------------------------------------------------------
// Visited Cells???
// Cartesian Coordinate System (0, 0) left lower corner
//-------------------------------------------------------------------
bool InternalMap::cCellVisited(int _XCoord, int _YCoord) const
{
   if( (_XCoord >= mapXDim) || (_YCoord >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::cCellVisited");

   bool aux = false;
   if( (this->MAP[(mapYDim-1)-_YCoord][_XCoord] & VISITED) == VISITED )   aux = true;

   return(aux);
}


//-------------------------------------------------------------------
// Mission in this Cells???
// Array Coordinate (0, 0) left upper corner
//-------------------------------------------------------------------
bool InternalMap::cellMission(int _col, int _row) const
{
   if( (_col >= mapXDim) || (_row >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::cellMission");

   bool aux = false;
   if( (this->MAP[_row][_col] & MISSION) == MISSION )   aux = true;

   return(aux);
}

//-------------------------------------------------------------------
// Obstacle in this Cells???
// Array Coordinate (0, 0) left upper corner
//-------------------------------------------------------------------
bool InternalMap::cellObstacle(int _col, int _row) const
{
   if( (_col >= mapXDim) || (_row >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::cellObstacle");

   bool aux = false;
   if( (this->MAP[_row][_col] & OBSTACLE) == OBSTACLE )   aux = true;

   return(aux);
}

//-------------------------------------------------------------------
// Visited Cells???
// Array Coordinate (0, 0) left upper corner
//-------------------------------------------------------------------
bool InternalMap::cellRevisited(int _col, int _row) const
{
   if( (_col >= mapXDim) || (_row >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::cellVisited");

   bool aux = false;
   if( (this->MAP[_row][_col] & REVISITED) == REVISITED )   aux = true;

   return(aux);
}

//-------------------------------------------------------------------
// Visited Cells???
// Array Coordinate (0, 0) left upper corner
//-------------------------------------------------------------------
bool InternalMap::cellVisited(int _col, int _row) const
{
   if( (_col >= mapXDim) || (_row >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::cellVisited");

   bool aux = false;
   if( (this->MAP[_row][_col] & VISITED) == VISITED )   aux = true;

   return(aux);
}


//-------------------------------------------------------------------
char InternalMap::get_selected_room(void) const  {  return(this->selectedRoom);}
//-------------------------------------------------------------------
int InternalMap::getMapWidth(void) const     {  return(this->mapWidth);       }
//-------------------------------------------------------------------
int InternalMap::getMapHeight(void) const    {  return(this->mapHeight);      }
//-------------------------------------------------------------------
int InternalMap::getMapTotalArea(void) const {  return(this->mapTotalArea);   }
//-------------------------------------------------------------------
int InternalMap::getMapFreeArea(void) const  {  return(this->mapFreeArea);    }
//-------------------------------------------------------------------
int InternalMap::getMapMeshSize(void) const  {  return(this->mapMeshSize);    }
//-------------------------------------------------------------------
int InternalMap::getMapXDim(void) const      {  return(this->mapXDim);        }
//-------------------------------------------------------------------
int InternalMap::getMapYDim(void) const      {  return(this->mapYDim);        }
//-------------------------------------------------------------------
int InternalMap::getTotalCellsNumber(void) const   {  return(this->mapTotalCells);  }
//-------------------------------------------------------------------
int InternalMap::getFreeCellsNumber(void)
{
   this->mapFreeCells = 0;

   for(int _x = 0; _x < this->mapXDim; _x++)
   {
      for(int _y = 0; _y < this->mapYDim; _y++)
         {  if( cCellObstacle(_x, _y) ) {  this->mapFreeCells++;   }  }
   }

   return(this->mapFreeCells);
}

//-------------------------------------------------------------------
int InternalMap::getRevisitedCellsNumber(void)
{
   this->mapRevisitedCells = 0;

   for(int _x = 0; _x < this->mapXDim; _x++)
   {
      for(int _y = 0; _y < this->mapYDim; _y++)
         {  if( cCellRevisited(_x, _y) ) {  this->mapRevisitedCells++;   }  }
   }

   return(this->mapRevisitedCells);
}

//-------------------------------------------------------------------
int InternalMap::getVisitedCellsNumber(void)
{
   this->mapVisitedCells = 0;

   for(int _x = 0; _x < this->mapXDim; _x++)
   {
      for(int _y = 0; _y < this->mapYDim; _y++)
         {  if( cCellVisited(_x, _y) ) {  this->mapVisitedCells++;   }  }
   }

   return(this->mapVisitedCells);
}

//-------------------------------------------------------------------
// Cartesian Coordinate System (0, 0) left lower corner
char InternalMap::getXYcCellData(int _XCoord, int _YCoord) const
{
   if( (_XCoord >= mapXDim) || (_YCoord >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::getXYcCellData");

   return(MAP[(mapYDim-1)-_YCoord][_XCoord]);
}

//-------------------------------------------------------------------
// Array Coordinate (0, 0) left upper corner
char InternalMap::getXYCellData(int _col, int _row) const
{
   if( (_col >= mapXDim) || (_row >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::getXYCellData");

   return(MAP[_row][_col]);
}

//-------------------------------------------------------------------
void InternalMap::setMapWidth(int _value)    {  this->mapWidth = _value;      }
//-------------------------------------------------------------------
void InternalMap::setMapHeight(int _value)   {  this->mapHeight = _value;     }
//-------------------------------------------------------------------
void InternalMap::setMapTotalArea(int _value){  this->mapTotalArea = _value;  }
//-------------------------------------------------------------------
void InternalMap::setMapFreeArea(int _value) {  this->mapFreeArea = _value;   }
//-------------------------------------------------------------------
void InternalMap::setMapMeshSize(int _value) {  this->mapMeshSize = _value;   }
//-------------------------------------------------------------------
void InternalMap::setMapXDim(int _value)     {  this->mapXDim = _value;       }
//-------------------------------------------------------------------
void InternalMap::setMapYDim(int _value)     {  this->mapYDim = _value;       }
//-------------------------------------------------------------------
void InternalMap::setTotalCellsNumber(int _value)  {  this->mapTotalCells = _value; }
//-------------------------------------------------------------------
void InternalMap::setFreeCellsNumber(int _value)   {  this->mapFreeCells = _value;  }
//-------------------------------------------------------------------
void InternalMap::setVisitedCellsNumber(int _value){  this->mapVisitedCells = _value;}
//-------------------------------------------------------------------
void InternalMap::setRevisitedCellsNumber(int _value) {  this->mapRevisitedCells = _value;}

//-------------------------------------------------------------------
// Cartesian Coordinate System (0, 0) left lower corner
//-------------------------------------------------------------------
void InternalMap::setXYcCellData(const int _XCoord, const int _YCoord, const char _value)
{
   if( (_XCoord >= mapXDim) || (_YCoord >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::setXYcCellData");

   this->MAP[(mapYDim-1)-_YCoord][_XCoord] = _value;
}

//-------------------------------------------------------------------
// Mark the cell as Mission
// Cartesian Coordinate System (0, 0) left lower corner
//-------------------------------------------------------------------
void InternalMap::setcCellMission(const int _XCoord, const int _YCoord)
{
   if( (_XCoord >= mapXDim) || (_YCoord >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::setcCellMission");

   this->MAP[(mapYDim-1)-_YCoord][_XCoord] |= MISSION;
}

//-------------------------------------------------------------------
// Put an obstacle in the cell
// Cartesian Coordinate System (0, 0) left lower corner
//-------------------------------------------------------------------
void InternalMap::setcCellObstacle(const int _XCoord, const int _YCoord)
{
   if( (_XCoord >= mapXDim) || (_YCoord >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::setcCellObstacle");

   this->MAP[(mapYDim-1)-_YCoord][_XCoord] |= OBSTACLE;
}

//-------------------------------------------------------------------
void InternalMap::setcCellRevisited(const int _XCoord, const int _YCoord)
{
   if( (_XCoord >= mapXDim) || (_YCoord >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::setcCellRevisited");

   this->MAP[(mapYDim-1)-_YCoord][_XCoord] |= REVISITED;
}

//-------------------------------------------------------------------
// Mark the cell as visited
// Cartesian Coordinate System (0, 0) left lower corner
//-------------------------------------------------------------------
void InternalMap::setcCellVisited(const int _XCoord, const int _YCoord)
{
   if( (_XCoord >= mapXDim) || (_YCoord >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::setcCellVisited");

   this->MAP[(mapYDim-1)-_YCoord][_XCoord] |= VISITED;
}


//-------------------------------------------------------------------
// Array Coordinate (0, 0) left upper corner
//-------------------------------------------------------------------
void InternalMap::setXYCellData(const int _col, const int _row, const char _value)
{
   if( (_col >= mapXDim) || (_row >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::setXYCellData");

   this->MAP[_row][_col] = _value;
}

//-------------------------------------------------------------------
// Mark the cell as Mission
// Array Coordinate (0, 0) left upper corner
//-------------------------------------------------------------------
void InternalMap::setCellMission(const int _col, const int _row)
{
   if( (_col >= mapXDim) || (_row >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::setCellMission");

   this->MAP[_row][_col] |= MISSION;
}

//-------------------------------------------------------------------
// Put an obstacle in the cell
// Array Coordinate (0, 0) left upper corner
//-------------------------------------------------------------------
void InternalMap::setCellObstacle(const int _col, const int _row)
{
   if( (_col >= mapXDim) || (_row >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::setCellObstacle");

   this->MAP[_row][_col] |= OBSTACLE;
}

//-------------------------------------------------------------------
// Mark the cell as visited
// Array Coordinate (0, 0) left upper corner
//-------------------------------------------------------------------
void InternalMap::setCellRevisited(const int _col, const int _row)
{
   if( (_col >= mapXDim) || (_row >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::setCellRevisited");

   this->MAP[_row][_col] |= REVISITED;
}

//-------------------------------------------------------------------
// Mark the cell as visited
// Array Coordinate (0, 0) left upper corner
//-------------------------------------------------------------------
void InternalMap::setCellVisited(const int _col, const int _row)
{
   if( (_col >= mapXDim) || (_row >= mapYDim) )
      printf("\nERROR: COORDINATES OUTSIDE THE MAP - InternalMap::setCellVisited");

   this->MAP[_row][_col] |= VISITED;
}
//*******************************************************************
// Internal Map Class Implementation: BEGIN
//*******************************************************************
