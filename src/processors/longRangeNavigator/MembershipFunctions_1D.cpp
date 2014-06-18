//*******************************************************************
// Title: MembershipFunctions_1D.cpp
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
#include "MembershipFunctions_1D.h"


//===================================================================



//*******************************************************************
// MembershipFunctions_1D Class Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
// Default Constructor
//-------------------------------------------------------------------
MembershipFunctions_1D::MembershipFunctions_1D()
{
   linguisticVNumber = 1;

   // Allocate dynamic memory, Access as matrix
   a_DOM = new double[linguisticVNumber];
}


//-------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------
MembershipFunctions_1D::MembershipFunctions_1D(int _linguistics)
{
   linguisticVNumber = _linguistics;

   // Allocate dynamic memory, Access as matrix
   a_DOM = new double[linguisticVNumber];
}


//-------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------
MembershipFunctions_1D::~MembershipFunctions_1D()
{
   // Free Memory
   delete [] a_DOM;
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------
void MembershipFunctions_1D::setLinguisticVNumber(int _value)
{
   linguisticVNumber = _value;

   // Free Memory
   delete [] a_DOM;

   // Allocate dynamic memory, Access as matrix
   a_DOM = new double[linguisticVNumber];
}


//-------------------------------------------------------------------
int MembershipFunctions_1D::getLinguisticVNumber(void) const
{
   return(linguisticVNumber);
}

//-------------------------------------------------------------------
// void DOM_5Triangular(double * _DOMarray, double _input)
// Triangular Membership Function \/\/\/\/ (5 Functions) => 5 Outputs
// _DOMarray: This pointer contain the "Degree Of Membership" of an _input
// _input: Input parameter
//-------------------------------------------------------------------
void MembershipFunctions_1D::Triangular_5(double _input)
{
   // It is necessary to avoid the double precision problem
   if( (_input >= 0.999999) && (_input <= 1.000001) )   _input = 1.0;

   if( ((_input > 0.0) && (_input < 0.25)) || (_input == 0.0) )
	{
//      printf("\t 0.0 - 0.25\t _input: %1.3f \n", _input); // DEBUG PURPOSE
      this->a_DOM[0] = 1.0 - (4.0*_input);
      this->a_DOM[1] = 4.0*_input;
      this->a_DOM[2] = 0.0;
      this->a_DOM[3] = 0.0;
      this->a_DOM[4] = 0.0;
	}
	else if( (_input >= 0.25) && (_input < 0.5) )
   {
//      printf("\t 0.25 - 0.5\t _input: %1.3f \n", _input); // DEBUG PURPOSE
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 2.0 - (4.0*_input);
      this->a_DOM[2] = (4.0*_input) - 1;
      this->a_DOM[3] = 0.0;
      this->a_DOM[4] = 0.0;
	}
   else if( (_input >= 0.5) && (_input < 0.75) )
	{
//      printf("\t 0.5 - 0.75\t _input: %1.3f \n", _input); // DEBUG PURPOSE
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 0.0;
      this->a_DOM[2] = 3.0 - (4.0*_input);
      this->a_DOM[3] = (4.0*_input) - 2.0;
      this->a_DOM[4] = 0.0;
	}
   else if( (_input >= 0.75) && (_input <= 1.0) )
	{
//      printf("\t 0.75 - 1.0\t _input: %1.3f \n", _input); // DEBUG PURPOSE
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 0.0;
      this->a_DOM[2] = 0.0;
      this->a_DOM[3] = 4.0 - (4.0*_input);
      this->a_DOM[4] = (4.0*_input) - 3.0;
	}

   // Condition to Out of Range values [0.0, 1.0]
   else if( (_input < 0.0) || (_input > 1.0) )
	{
//      printf("\t else \t _input: %1.3f \n", _input);   // DEBUG PURPOSE
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 0.0;
      this->a_DOM[2] = 0.0;
      this->a_DOM[3] = 0.0;
      this->a_DOM[4] = 0.0;
	}
}


//-------------------------------------------------------------------
// void DOM_3Triangular(double * _DOMarray, double _input)
// Triangular Membership Function (3 Functions) => 3 Outputs
// _DOMarray: This pointer contain the "Degree Of Membership" of an _input
// _input: Input parameter
//  _    _
// | \/\/ |
//-------------------------------------------------------------------
void MembershipFunctions_1D::Triangular_3(double _input)
{
   // It is necessary to avoid the double precision problem
   if( (_input >= 0.999999) && (_input <= 1.000001) )   _input = 1.0;

   if( (_input >= 0.0) && (_input < 0.25) )
   {
      this->a_DOM[0] = 1.0;
      this->a_DOM[1] = 0.0;
      this->a_DOM[2] = 0.0;
   }
   else if( (_input >= 0.25) && (_input < 0.50) )
   {
      this->a_DOM[0] = 2 - (4*_input);
      this->a_DOM[1] = (4*_input) - 1;
      this->a_DOM[2] = 0.0;
   }
   else if( (_input >= 0.50) && (_input < 0.75) )
   {
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 3 - (4*_input);
      this->a_DOM[2] = (4*_input) -2;
   }
   else if( (_input >= 0.75) && (_input <= 1.0) )
   {
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 0.0;
      this->a_DOM[2] = 1.0;
   }
   // Condition to Out of Range values [0.0, 1.0]
   else if( (_input < 0.0) || (_input > 1.0) )
	{
//      printf("\t else \t _input: %1.3f \n", _input);   // DEBUG PURPOSE
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 0.0;
      this->a_DOM[2] = 0.0;
	}
}


//-------------------------------------------------------------------
// void DOM_9Triangular(double * _DOMarray, double _input)
// Triangular Membership Function \/\/\/\/\/\/\/\/ (9 Functions) => 9 Outputs
// _DOMarray: This pointer contain the "Degree Of Membership" of an _input
// _input: Input parameter
//-------------------------------------------------------------------
void MembershipFunctions_1D::Triangular_9(double _input)
{
   // It is necessary to avoid the double precision problem
   if( (_input >= 0.999999) && (_input <= 1.000001) )   _input = 1.0;

   if( (_input >= 0.000) && (_input < 0.125) )
   {
//      printf("\t 0.0 - 0.25\t _input: %1.3f \n", _input); // DEBUG PURPOSE
      this->a_DOM[0] = 1.0 - (8.0*_input);
      this->a_DOM[1] = 8.0*_input - 0;
      this->a_DOM[2] = 0.0;
      this->a_DOM[3] = 0.0;
      this->a_DOM[4] = 0.0;
      this->a_DOM[5] = 0.0;
      this->a_DOM[6] = 0.0;
      this->a_DOM[7] = 0.0;
      this->a_DOM[8] = 0.0;
   }
   else if( (_input >= 0.125) && (_input < 0.250) )
   {
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 2 - (8.0*_input);
      this->a_DOM[2] = (8.0*_input) - 1;
      this->a_DOM[3] = 0.0;
      this->a_DOM[4] = 0.0;
      this->a_DOM[5] = 0.0;
      this->a_DOM[6] = 0.0;
      this->a_DOM[7] = 0.0;
      this->a_DOM[8] = 0.0;
   }
   else if( (_input >= 0.250) && (_input < 0.375) )
   {
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 0.0;
      this->a_DOM[2] = 3 - (8.0*_input);
      this->a_DOM[3] = (8.0*_input) - 2;
      this->a_DOM[4] = 0.0;
      this->a_DOM[5] = 0.0;
      this->a_DOM[6] = 0.0;
      this->a_DOM[7] = 0.0;
      this->a_DOM[8] = 0.0;
   }
   else if( (_input >= 0.375) && (_input < 0.500) )
   {
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 0.0;
      this->a_DOM[2] = 0.0;
      this->a_DOM[3] = 4 - (8.0*_input);
      this->a_DOM[4] = (8.0*_input) - 3;
      this->a_DOM[5] = 0.0;
      this->a_DOM[6] = 0.0;
      this->a_DOM[7] = 0.0;
      this->a_DOM[8] = 0.0;
   }
   else if( (_input >= 0.500) && (_input < 0.625) )
   {
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 0.0;
      this->a_DOM[2] = 0.0;
      this->a_DOM[3] = 0.0;
      this->a_DOM[4] = 5 - (8.0*_input);
      this->a_DOM[5] = (8.0*_input) - 4;
      this->a_DOM[6] = 0.0;
      this->a_DOM[7] = 0.0;
      this->a_DOM[8] = 0.0;
   }
   else if( (_input >= 0.625) && (_input < 0.750) )
   {
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 0.0;
      this->a_DOM[2] = 0.0;
      this->a_DOM[3] = 0.0;
      this->a_DOM[4] = 0.0;
      this->a_DOM[5] = 6 - (8.0*_input);
      this->a_DOM[6] = (8.0*_input) - 5;
      this->a_DOM[7] = 0.0;
      this->a_DOM[8] = 0.0;
   }
   else if( (_input >= 0.750) && (_input < 0.875) )
   {
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 0.0;
      this->a_DOM[2] = 0.0;
      this->a_DOM[3] = 0.0;
      this->a_DOM[4] = 0.0;
      this->a_DOM[5] = 0.0;
      this->a_DOM[6] = 7 - (8.0*_input);
      this->a_DOM[7] = (8.0*_input) - 6;
      this->a_DOM[8] = 0.0;
   }
   else if( (_input >= 0.875) && (_input <= 1.000) )
   {
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 0.0;
      this->a_DOM[2] = 0.0;
      this->a_DOM[3] = 0.0;
      this->a_DOM[4] = 0.0;
      this->a_DOM[5] = 0.0;
      this->a_DOM[6] = 0.0;
      this->a_DOM[7] = 8 - (8.0*_input);
      this->a_DOM[8] = (8.0*_input) - 7;
   }
   // Condition to Out of Range values [0.0, 1.0]
   else if( (_input < 0.0) || (_input > 1.0) )
	{
//      printf("\t else \t _input: %1.3f \n", _input);   // DEBUG PURPOSE
      this->a_DOM[0] = 0.0;
      this->a_DOM[1] = 0.0;
      this->a_DOM[2] = 0.0;
      this->a_DOM[3] = 0.0;
      this->a_DOM[4] = 0.0;
      this->a_DOM[5] = 0.0;
      this->a_DOM[6] = 0.0;
      this->a_DOM[7] = 0.0;
      this->a_DOM[8] = 0.0;
	}
}
//*******************************************************************
// MembershipFunctions_1D Class Implementation: END
//*******************************************************************
