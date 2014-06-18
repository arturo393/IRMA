//*******************************************************************
// Title: Utils.h
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


#ifndef UTILS_H
#define UTILS_H


//*******************************************************************
// Librery Declaration
#include <stdio.h>
#include <stdlib.h>
#include <math.h>



//*******************************************************************
//*******************************************************************
//-------------------------------------------------------------------
#define SWAP(type, i, j) {type t = i; i = j; j = t;}



//*******************************************************************
// RANDOM Functions Implementation: BEGIN
//-------------------------------------------------------------------
// int randomRange(int _maxRange)
// Generate Pseudo-Random Numbers into range of [0, _maxRange]
int RandomRange(int _maxRange);

//-------------------------------------------------------------------
// int randomdouble(int _digits)
// Return Pseudo-Random double into range of [0, 1] with _digits of precision
// This function can handle up to 18 _digits of precision
double doubleRandom01(int _digits);

//-------------------------------------------------------------------
// int flip(int __maxRange)
int Flip(void);


//*******************************************************************
// SORT Functions Implementation: BEGIN
//-------------------------------------------------------------------
// Recursive Quicksort: Sort list from greater to lesser
void SortGL(int * list, int first, int last);
//-------------------------------------------------------------------
// Recursive Quicksort: Sort list from lesser to greater
void SortLG(int * list, int first, int last);


//*******************************************************************
// OTHERS Functions Implementation: BEGIN
//-------------------------------------------------------------------
// Return the minimum value between _A and _B
double Min(const double _A, const double _B);

//-------------------------------------------------------------------
// Return the maximum value between _A and _B
double Max(const double _A, const double _B);


//*******************************************************************
// OTHERS Functions Implementation: BEGIN
//-------------------------------------------------------------------
// Angle, X coord, Y coord
int calculate_Distance(const int _1st_position[], const int _2nd_position[]);

#endif   // UTILS_H
