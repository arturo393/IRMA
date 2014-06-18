//*******************************************************************
// Title: MembershipFunctions_1D.h
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

#ifndef MEMBERSHIPFUNCTIONS_1D_H
#define MEMBERSHIPFUNCTIONS_1D_H


//*******************************************************************
// Librery Declaration



//===================================================================
// Global Variables Definition



//*******************************************************************
// 1 Dimension Membership Functions Class Declaration
//*******************************************************************
class MembershipFunctions_1D
{
   private:
      int linguisticVNumber;
   public:
      double *a_DOM;         // To store the rules (if ...)

      // Constructors
      MembershipFunctions_1D();
      MembershipFunctions_1D(int _linguistics);
      // Destructor
      ~MembershipFunctions_1D();

      void setLinguisticVNumber(int _value);
      int getLinguisticVNumber(void) const;

      void Triangular_3(double _input);
      void Triangular_5(double _input);
      void Triangular_9(double _input);
      //void Gaussian(double _input);
      //void Sigmoidal(double _input);
};
typedef MembershipFunctions_1D c_MF_1D;


#endif   // MEMBERSHIPFUNCTIONS_1D_H
