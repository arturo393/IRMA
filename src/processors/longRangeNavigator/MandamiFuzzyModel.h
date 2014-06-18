//*******************************************************************
// Title: MandamiFuzzyModel.h
// MANDAMI FUZZY MODEL
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


#ifndef MANDAMI_FUZZY_MODEL_H
#define MANDAMI_FUZZY_MODEL_H


//*******************************************************************
// Librery Declaration
#include <stdio.h>
#include <math.h>
#include "MembershipFunctions_1D.h"


//*******************************************************************
// DEFINE TO DEBUG
//#define DYNAMIC_MANDAMI

//#define T_INIT_RINPUTS
//#define T_INIT_RINPUTS_FILE
//#define T_INIT_ROUTPUTS
//#define T_INIT_ROUTPUTS_FILE
//#define T_DOM_INPUTS
//#define T_FILL_TNORM_ALL
//#define T_FILL_TNORM_SHORT
//#define T_FILL_TCONORM
//#define T_FILL_TCONORM_FILE
//#define T_CENTROID_OUTS
//#define T_MANDAMI_OUTPUTS


//*******************************************************************
// Global Variables Definition
const int RULES_NUMBER = 625;    // 625 = 5^4 = (Linguistic Variables) ^ (Inputs)
const int INPUTS_NUMBER = 4;     // 4 Motivations
const int OUTPUTS_NUMBER = 1;    // 1 Fitness Value
const int LINGUISTIC_NUMBER = 5; // 5 = Linguistic Variables Number
const int SAMPLES_PER_AREA = 100;// Fuzzy Granularity

const int MIN_OUTPUT = 0;  // Minimum Output value
const int MAX_OUTPUT = 1;  // Maximum Output value

const char TNORM_MIN = 0;      // T-norm Minimum
const char TNORM_PROD = 1;     // T-norm Product
const char TCONORM_MAX = 2;    // T-conorm Maximum

const char DEFUZZ_zCOA = 0;    // Centroid Of Area
const char DEFUZZ_zBOA = 1;    // Bisector Of Area
const char DEFUZZ_zMOM = 2;    // Mean Of Maximum



//*******************************************************************
// Rules Class Declaration: BEGIN
//*******************************************************************
class FuzzyRules
{
   private:
      int rulesNumber;
      int inputsNumber;
      int outputsNumber;

//···································································
#ifndef DYNAMIC_MANDAMI   // STATIC Version of the Class
      int a_input[RULES_NUMBER][INPUTS_NUMBER];    // To store the rules (if ...)
      int a_output[RULES_NUMBER][OUTPUTS_NUMBER];  // To store the outputs or consequences
#endif   // end STATIC Version of the Class
//···································································
#ifdef DYNAMIC_MANDAMI // DYNAMIC_MANDAMI Version of the Class
      int **a_input;         // To store the rules (if ...)
      int **a_output;        // To store the outputs or consequences
#endif   // end DYNAMIC_MANDAMI Version of the Class
//···································································

   public:
      // Constructors
      FuzzyRules();

//···································································
#ifdef DYNAMIC_MANDAMI // DYNAMIC_MANDAMI Version of the Class
      FuzzyRules(int _rules, int _inputs);
      FuzzyRules(int _rules, int _inputs, int _outputs);
#endif   // end DYNAMIC_MANDAMI Version of the Class
//···································································

      // Destructor
      ~FuzzyRules();

      void ClearFuzzyRules(void);

      void setRule(const int _rule, const int _premise[]);
      void setPremise(const int _rule, const int _input, const int _premise);
      void setConsequence(const int _rule, const int _output, const int _consequence);

      // The Function Outputs are _linguistics[]
      void getRule(const int _rule, int _linguistics[]);
      int getPremise(const int _rule, const int _input) const;
      int getConsequence(const int _rule, const int _output) const;
}; // end Class FuzzyRules
typedef FuzzyRules c_FuzzyRules;
//*******************************************************************
// Rules Class Declaration: END
//*******************************************************************


//*******************************************************************
// Mandami Fuzzy Model Class Declaration: BEGIN
//*******************************************************************
class MANDAMIfuzzy
{
   private:
      int rulesNumber;
      int inputsNumber;
      int outputsNumber;
      int linguisticVNumber;

      // The Tconorm Area is formed like a sum of rectangles
      // Like a definition for a integral
      int areaSamples;        // Fuzzy Granularity
      double outputMinValue;   // output or z
      double outputMaxValue;   // output or z

//···································································
#ifndef DYNAMIC_MANDAMI   // STATIC Version of the Class
      double a_Tnorm[RULES_NUMBER];
      double a_AreaTconorm[SAMPLES_PER_AREA][OUTPUTS_NUMBER];
      double a_Outputs[OUTPUTS_NUMBER];
      // Degree Of Membership for every input
      double a_DOMinputs[INPUTS_NUMBER][LINGUISTIC_NUMBER];

      // Object Rules Type: Class FuzzyRules
      c_FuzzyRules o_Rules;
#endif   // end STATIC Version of the Class
//···································································
#ifdef DYNAMIC_MANDAMI    // DYNAMIC_MANDAMI Version of the Class
      double *a_Tnorm;
      double **a_AreaTconorm;
      double *a_Outputs;
      double **a_DOMinputs;    // Degree Of Membership for every input

      // Object Rules Type: Class FuzzyRules
      c_FuzzyRules *o_Rules;
#endif   // end DYNAMIC_MANDAMI Version of the Class
//···································································

      // Object of Degree Of Memberships for inputs
      // Type: Class MembershipFunctions_1D
      c_MF_1D *o_DOMin;
      // Object of Degree Of Memberships for outputs
      // Type: Class MembershipFunctions_1D
      c_MF_1D *o_DOMout;

      // Rules
      void InitRulesInputs(c_FuzzyRules& o_rls);
      void InitRulesOutputs(c_FuzzyRules& o_rls);

      void ClearOutputs(void);
      void ClearDOMinputs(void);
      // Mandami Fuzzy Inference System
      void ClearTnorm(void);
      void FillTnorm(const char _method);  // Only Min is implemted
      void ClearAreaTconorm(void);
      void FillAreaTconorm(const char _method);

      // Defuzzification Methods
      void Centroid(double zCOA[]);  // Centroid of Area
      void Bisector(double zBOA[]);  // Bisector of Area - Method Not Implemented
      void MeanMax(double zMOM[]);   // Mean of maximun - Method Not Implemented

   public:
      // Constructors
      MANDAMIfuzzy();

//···································································
#ifdef DYNAMIC_MANDAMI // DYNAMIC_MANDAMI Version of the Class
      MANDAMIfuzzy(int _rulesN, int _inputsN);
      MANDAMIfuzzy(int _rulesN, int _inputsN, int _outputsN, int _linguisticsN);
#endif   // end DYNAMIC_MANDAMI Version of the Class
//···································································

      // Destructor
      ~MANDAMIfuzzy();

      void ClearAll(void);
      int CalcOutputs(const double _inputs[]);

      // Calculate Degree Of Membership for every Input
      void CalcDOMinputs(const double _inputs[]);

      // The Mandami Fuzzy Inference System
      void Aggregator(const char _methodNorm, const char _methodConorm);
      // Defuzzification Methods
      void Defuzzifier(const char _method);

      void setRulesNumber(int _value);
      void setInputsNumber(int _value);
      void setOutputsNumber(int _value);
      void setLinguisticVNumber(int _value);
      void setAreaSamples(int _value);
      void setOutMinValue(double _value);
      void setOutMaxValue(double _value);

      int getRulesNumber(void) const;
      int getInputsNumber(void) const;
      int getOutputsNumber(void) const;
      int getLinguisticVNumber(void) const;
      int getAreaSamples(void) const;
      double getOutMinValue(void) const;
      double getOutMaxValue(void) const;

      double getDOM(const int _input, const int _lv) const;
      double getOutput(const int _output) const;
}; // end Class MANDAMIfuzzy
typedef MANDAMIfuzzy c_FMandami;
//*******************************************************************
// Mandami Fuzzy Model Class Declaration: END
//*******************************************************************



#endif   // end MANDAMI_FUZZY_MODEL_H
