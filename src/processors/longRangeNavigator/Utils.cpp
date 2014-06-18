//*******************************************************************
// Title: Utils.cpp
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
#include "Utils.h"


//===================================================================


//*******************************************************************
// RANDOM Functions Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
// Generate Pseudo-Random Numbers into range of [0, _maxRange]
int RandomRange(int _maxRange)
{
   int temp = rand()% (_maxRange+1);
   return(temp);
}

//-------------------------------------------------------------------
// Return Pseudo-Random double into range of [0, 1] with _digits of precision
// This function can handle up to 18 _digits of precision
double doubleRandom01(int _digits)
{
   int range = lround(pow(10,_digits));
   double precision = 1.0/range;

   double number = 0;
   number = RandomRange(range)*precision;

   return(number);
}

//-------------------------------------------------------------------
int Flip(void)
{
   return(RandomRange(1));
}
//*******************************************************************
// RANDOM Functions Implementation: END
//*******************************************************************


//*******************************************************************
// SORT Functions Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
// Recursive Quicksort: Sort ARRAY from greater to lesser
//-------------------------------------------------------------------
void SortGL(int * list, int first, int last)
{
   int left = first;
   int right = last;
   int pivot = list[(first+last)/2];
   int aux;

   do{
      while( list[left] > pivot ) left++;
      while( list[right] < pivot ) right--;
      if( left <= right )
      {
         aux = list[right];
         list[right] = list[left];
         list[left] = aux;
         left++;
         right--;
      }
   } while( left <= right );

   if( first < right ) SortGL(list, first, right);
   if( last > left ) SortGL(list, left, last);
}


//-------------------------------------------------------------------
// Recursive Quicksort: Sort ARRAY from lesser to greater
//-------------------------------------------------------------------
void SortLG(int * list, int first, int last)
{
   int left = first;
   int right = last;
   int pivot = list[(first+last)/2];
   int aux;

   do{
      while( list[left] < pivot ) left++;
      while( list[right] > pivot ) right--;
      if( left <= right )
      {
         aux = list[right];
         list[right] = list[left];
         list[left] = aux;
         left++;
         right--;
      }
   } while( left <= right );

   if( first < right ) SortLG(list, first, right);
   if( last > left ) SortLG(list, left, last);
}
//*******************************************************************
// SORT Functions Implementation: END
//*******************************************************************


//*******************************************************************
// OTHERS Functions Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
// double min(double _A, double _B)
// Return the minimum value between _A and _B
//-------------------------------------------------------------------
double Min(const double _A, const double _B)
{
   if( _A <= _B ) {  return(_A); }
   else           {  return(_B); }
}


//-------------------------------------------------------------------
// double max(double _A, double _B)
// Return the maximum value between _A and _B
//-------------------------------------------------------------------
double Max(const double _A, const double _B)
{
   if( _A <= _B ) {  return(_B); }
   else           {  return(_A); }
}
//*******************************************************************
// OTHERS Functions Implementation: END
//*******************************************************************


//*******************************************************************
// OTHERS Functions Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
int calculate_Distance(const int _1st_position[], const int _2nd_position[])
{
   int _distance = 0;
   double temp = 0.0;
   double _delta_x = (_2nd_position[1] - _1st_position[1])*1.0;
   double _delta_y = (_2nd_position[2] - _1st_position[2])*1.0;

   temp = pow(_delta_x, 2.0) + pow(_delta_y, 2.0);
   _distance = lround(sqrt(temp));

   return(_distance);
}
