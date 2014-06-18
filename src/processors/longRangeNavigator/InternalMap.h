//*******************************************************************
// Title: InternalMap.h
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

#ifndef INTERNALMAP_H
#define INTERNALMAP_H


//*******************************************************************
// Librery Declaration
#include <stdlib.h>
#include <math.h>
#include <stdio.h>


//*******************************************************************
// DEFINITIONS FOR TEST CLASS AND FUNCTIONS
//#define DYNAMIC_MAP
//#define T_SET_TEST_MAP_PRINT_HERE


//*******************************************************************
// Define MACROS
const int MAP_CELLS_HEIGHT = 250;  // 2.5 Meters
const int MAP_CELLS_WIDTH  = 250;  // 2.5 Meters

const int MAP_WIDHT   = 4000;
const int MAP_HEIGHT  = 4000;
const int MAP_MESH_SIZE = 50;

const char OBSTACLE  = 0x01;
const char MISSION   = 0x10;
const char VISITED   = 0x20;
const char REVISITED = 0x30;


const char TEST_ROOM = 0;  // TEST ROOM is a small 25x20 cm  L room
const char H_ROOM    = 1;  // Square Empty Room (without Obstacles)
const char L_ROOM    = 2;  // L Room (without extra Obstacles)
const char O_ROOM    = 3;  // T Room (without extra Obstacles)
const char S_ROOM    = 4;  // H Room (without extra Obstacles)
const char T_ROOM    = 5;  // O Room (with two small obstacles)
const char FART_ROOM = 6;  // Square Empty Room (without Obstacles)
const char SLAM_ROOM = 7;  // Mapper Room 

// FART MAP IDEAL - ONLY FOR INTERNAL USE AND GRAPHICAL REPRESENTATION
const int CATEGORIES_H_ROOM = 2;
const double FART_H_ROOM[][4] = {
{0.40, 0.00, 0.40, 0.75},
{0.40, 0.75, 0.40, 0.00},
};

const int CATEGORIES_L_ROOM = 1;
const double FART_L_ROOM[][4] = {
{0.60, 0.50, 0.00, 0.00},
};

const int CATEGORIES_O_ROOM = 4;
const double FART_O_ROOM[][4] = {
{0.300, 0.250, 0.500, 0.500},
{0.500, 0.500, 0.300, 0.250},
{0.000, 0.875, 0.900, 0.000},
{0.900, 0.000, 0.000, 0.875},
};

const int CATEGORIES_S_ROOM = 0;
const double FART_S_ROOM[][4] = {
{0.000, 0.000, 0.000, 0.000},
{0.0000, 0.0000, 0.9996, 0.0000},
{0.0000, 0.0000, 0.0000, 0.9995},
{0.9996, 0.0000, 0.0000, 0.0000},
{0.0000, 0.9995, 0.0000, 0.0000},
};

const int CATEGORIES_T_ROOM = 2;
const double FART_T_ROOM[][4] = {
{0.60, 0.00, 0.00, 0.75},
{0.60, 0.75, 0.00, 0.00},
};

//*******************************************************************
// Internal Map Class Declaration
//*******************************************************************
//-------------------------------------------------------------------
// Only the first 4 bits are asigned to represent the fisical Map
// Obstacles (bit 0), Mission (bit 1) and other
// The 4 more significative bits are used like a buffer to store Map's Metadata
//-------------------------------------------------------------------
// Class Declaration
class InternalMap
{
   private:
      // Variable Declaration
      int mapWidth;      // X: width of the scenario in cms
      int mapHeight;     // Y: height of the scenario in cms
      int mapTotalArea;  // Total Map Area in cms^2
      int mapFreeArea;   // Total Obstacles Free Area in cms^2

      // The Map is divide into cells
      int mapMeshSize;   // in cms
      int mapXDim;         // Total Number of column (X dimension)
      int mapYDim;         // Total Number of Rows (Y dimension)
      int mapTotalCells;   // Total Map Cells in Units
      int mapFreeCells;    // Total Obstacles Free Cells in Units
      int mapVisitedCells;    // Total Number of Visited Cells
      int mapRevisitedCells;  // Total Number of Visited Cells
      int oldXDim, oldYDim;   // to allow a safe array delete

      char selectedRoom;
//···································································
#ifndef DYNAMIC_MAP // STATIC Version of the Class
      // Number of cells or size of the matrix
      char MAP[MAP_CELLS_HEIGHT][MAP_CELLS_WIDTH];
#endif   // end STATIC Version of the class
//···································································
#ifdef DYNAMIC_MAP // DYNAMIC_MAP Version of the Class
// Remember: that C create first the rows and then columns
char **MAP;          // Number of cells or size of the matrix
#endif   // end DYNAMIC_MAP Version of the class
//···································································

   public:
      InternalMap();    // Default Constructor
      ~InternalMap();   // Destructor

      // Function Declaration
      void InitMap(void);
      void CleanMap(void);
      void RebuildMap(void);
      void Set_MAP(const int _XCoord, const int _YCoord);
      void Set_MAP_Dimension(const int _width, const int _height);
      void Set_SLAM_Map(const int _width, const int _height);
      void SetTestScenario(char _room);
      void PrintMAPtoFile(const char a_fileName[]);

      // Cartesian Coordinate System (0, 0) left lower corner
      bool cCellMission(int _XCoord, int _YCoord) const;    // Mission in this Cell???
      bool cCellObstacle(int _XCoord, int _YCoord) const;   // Obstacle in this Cell???
      bool cCellVisited(int _XCoord, int _YCoord) const;    // Visited Cell???
      bool cCellRevisited(int _XCoord, int _YCoord) const;  // Revisited Cell???

      // Array Coordinate (0, 0) left upper corner
      bool cellMission(int _col, int _row) const;     // Mission in this Cell???
      bool cellObstacle(int _col, int _row) const;    // Obstacle in this Cell???
      bool cellVisited(int _col, int _row) const;     // Visited Cell???
      bool cellRevisited(int _col, int _row) const;   // Revisited Cell???

      int getMapWidth(void) const;
      int getMapHeight(void) const;
      int getMapTotalArea(void) const;
      int getMapFreeArea(void) const;
      int getMapMeshSize(void) const;
      int getMapXDim(void) const;
      int getMapYDim(void) const;
      int getTotalCellsNumber(void) const;
      int getFreeCellsNumber(void);
      int getVisitedCellsNumber(void);
      int getRevisitedCellsNumber(void);
      char get_selected_room(void) const;
      // Cartesian Coordinate System (0, 0) left lower corner
      char getXYcCellData(int _XCoord, int _YCoord) const;
      // Array Coordinate (0, 0) left upper corner
      char getXYCellData(int _col, int _row) const;

      void setMapWidth(int _value);
      void setMapHeight(int _value);
      void setMapTotalArea(int _value);
      void setMapFreeArea(int _value);
      void setMapMeshSize(int _value);
      void setMapXDim(int _value);
      void setMapYDim(int _value);
      void setTotalCellsNumber(int _value);
      void setFreeCellsNumber(int _value);
      void setVisitedCellsNumber(int _value);
      void setRevisitedCellsNumber(int _value);
      void set_selected_room(const char _room);

      // Cartesian Coordinate System (0, 0) left lower corner
      void setXYcCellData(const int _XCoord, const int _YCoord, const char _value);
      void setcCellMission(const int _XCoord, const int _YCoord);    // Mark the cell as a Mission
      void setcCellObstacle(const int _XCoord, const int _YCoord);   // Put an obstacle in the cell
      void setcCellVisited(const int _XCoord, const int _YCoord);    // Mark the cell as visited
      void setcCellRevisited(const int _XCoord, const int _YCoord);  // Mark the cell as revisited

      // Array Coordinate (0, 0) left upper corner
      void setXYCellData(const int _col, const int _row, const char _value);
      void setCellMission(const int _col, const int _row);     // Mark the cell as a Mission
      void setCellObstacle(const int _col, const int _row);    // Put an obstacle in the cell
      void setCellVisited(const int _col, const int _row);     // Mark the cell as visited
      void setCellRevisited(const int _col, const int _row);   // Mark the cell as revisited
}; // end Class InternalMap
typedef InternalMap c_Map;
//*******************************************************************
// Internal Map Class Declaration
//*******************************************************************

#endif   // INTERNALMAP_H
