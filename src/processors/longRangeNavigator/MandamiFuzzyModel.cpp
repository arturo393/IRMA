//*******************************************************************
// Title: MandamiFuzzyModel.cpp
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
#include "MandamiFuzzyModel.h"


//===================================================================



//*******************************************************************
// FuzzyRules Class Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
// Default Constructor
//-------------------------------------------------------------------
FuzzyRules::FuzzyRules()
{
   rulesNumber = RULES_NUMBER;
   inputsNumber = INPUTS_NUMBER;
   outputsNumber = OUTPUTS_NUMBER;

#ifdef DINAMIC    // DYNAMIC_MANDAMI Version of the class
   // Allocate dynamic memory, Access as matrix
   // Remember: that C create first the rows and then columns
   a_input = new int*[rulesNumber];
   a_output = new int*[rulesNumber];
   for(int _yr = 0; _yr < rulesNumber; _yr++)
   {
      a_input[_yr] = new int[inputsNumber];
      a_output[_yr] = new int[outputsNumber];
   }  // end for rulesNumber
#endif   // end DYNAMIC_MANDAMI Version of the class

   // Initialization
   ClearFuzzyRules();
}


#ifdef DYNAMIC_MANDAMI // DYNAMIC_MANDAMI Version of the class
//-------------------------------------------------------------------
// Constructor Class FuzzyRules
//-------------------------------------------------------------------
FuzzyRules::FuzzyRules(int _rules, int _inputs)
{
   rulesNumber = _rules;
   inputsNumber = _inputs;
   outputsNumber = OUTPUTS_NUMBER;

   // Allocate dynamic memory, Access as matrix
   // Remember: that C create first the rows and then columns
   a_input = new int*[rulesNumber];
   a_output = new int*[rulesNumber];
   for(int _yr = 0; _yr < rulesNumber; _yr++)
   {
      a_input[_yr] = new int[inputsNumber];
      a_output[_yr] = new int[outputsNumber];
   }  // end for rulesNumber

   // Initialization
   ClearFuzzyRules();
}


//-------------------------------------------------------------------
// Constructor Class FuzzyRules
//-------------------------------------------------------------------
FuzzyRules::FuzzyRules(int _rules, int _inputs, int _outputs)
{
   rulesNumber = _rules;
   inputsNumber = _inputs;
   outputsNumber = _outputs;

   // Allocate dynamic memory, Access as matrix
   // Remember: that C create first the rows and then columns
   a_input = new int*[rulesNumber];
   a_output = new int*[rulesNumber];
   for(int _yr = 0; _yr < rulesNumber; _yr++)
   {
      a_input[_yr] = new int[inputsNumber];
      a_output[_yr] = new int[outputsNumber];
   }  // end for rulesNumber

   // Initialization
   ClearFuzzyRules();
}
#endif   // end DYNAMIC_MANDAMI Version of the class

//-------------------------------------------------------------------
// Destructor Class FuzzyRules
//-------------------------------------------------------------------
FuzzyRules::~FuzzyRules()
{
#ifdef DYNAMIC_MANDAMI // DYNAMIC_MANDAMI Version of the class
   // Free Memory
   for(int _yr = 0; _yr < rulesNumber; _yr++)
   {
      delete [] a_input[_yr];
      delete [] a_output[_yr];
   }
   delete [] a_input;
   delete [] a_output;
#endif   // end DYNAMIC_MANDAMI Version of the class
   ;
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------
void FuzzyRules::ClearFuzzyRules(void)
{
   // Initialization
   for(int _yr = 0; _yr < rulesNumber; _yr++)
   {
      for(int _xi = 0; _xi < inputsNumber; _xi++)
         a_input[_yr][_xi] = 0;
      for(int _xo = 0; _xo < outputsNumber; _xo++)
         a_output[_yr][_xo] = 0;
   }
}


//-------------------------------------------------------------------
void FuzzyRules::setRule(const int _rule, const int _premise[])
{
   for(int _i = 0; _i < inputsNumber; _i++)
      a_input[_rule][_i] = _premise[_i];
}

//-------------------------------------------------------------------
void FuzzyRules::setPremise(const int _rule, const int _input, const int _premise)
{
   a_input[_rule][_input] = _premise;
}

//-------------------------------------------------------------------
void FuzzyRules::setConsequence(const int _rule, const int _output, const int _consequence)
{
   a_output[_rule][_output] = _consequence;
}

//-------------------------------------------------------------------
void FuzzyRules::getRule(const int _rule, int _premise[])
{
   for(int _i = 0; _i < inputsNumber; _i++)
      _premise[_i] = a_input[_rule][_i];
}

//-------------------------------------------------------------------
int FuzzyRules::getPremise(const int _rule, const int _input) const
{
   return(a_input[_rule][_input]);
}

//-------------------------------------------------------------------
int FuzzyRules::getConsequence(const int _rule, const int _output) const
{
   return(a_output[_rule][_output]);
}

//*******************************************************************
// FuzzyRules Class Implementation: END
//*******************************************************************


//*******************************************************************
// Mandami Fuzzy Model Class Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
// Default Constructor MANDAMIfuzzy Class
//-------------------------------------------------------------------
MANDAMIfuzzy::MANDAMIfuzzy()
{
   areaSamples = SAMPLES_PER_AREA;  // Fuzzy Granularity
   outputMinValue = MIN_OUTPUT;     // Minimum output
   outputMaxValue = MAX_OUTPUT;     // Maximum input
   rulesNumber = RULES_NUMBER;
   inputsNumber = INPUTS_NUMBER;
   outputsNumber = OUTPUTS_NUMBER;
   linguisticVNumber = LINGUISTIC_NUMBER;
   o_DOMin = new c_MF_1D(linguisticVNumber);
//   o_DOMout = new c_MF_1D(linguisticVNumber);
   o_DOMout = new c_MF_1D(9);

//···································································
#ifdef DYNAMIC_MANDAMI // DYNAMIC_MANDAMI Version of the class
   a_Tnorm = new double[rulesNumber];
   a_AreaTconorm = new double*[areaSamples];
   for(int _yr = 0; _yr < areaSamples; _yr++)
      a_AreaTconorm[_yr] = new double[outputsNumber];

   a_Outputs = new double[outputsNumber];

   // Allocate dynamic memory, Access as matrix
   // Remember: that C create first the rows and then columns
   a_DOMinputs = new double*[inputsNumber];
   for(int _yr = 0; _yr < inputsNumber; _yr++)
      a_DOMinputs[_yr] = new double[linguisticVNumber];

   o_Rules = new c_FuzzyRules(rulesNumber, inputsNumber);
#endif   // end DYNAMIC_MANDAMI Version of the class
//···································································

   this->InitRulesInputs(o_Rules);
   this->InitRulesOutputs(o_Rules);
}


//-------------------------------------------------------------------
// MANDAMIfuzzy Class Destructor
//-------------------------------------------------------------------
MANDAMIfuzzy::~MANDAMIfuzzy()
{
   delete o_DOMin;
   delete o_DOMout;

//···································································
#ifdef DYNAMIC_MANDAMI // DYNAMIC_MANDAMI Version of the Class
   // Free Memory
   delete [] a_Tnorm;
   for(int _yr = 0; _yr < areaSamples; _yr++)
      delete [] a_AreaTconorm[_yr];

   delete [] a_AreaTconorm;
   delete [] a_Outputs;

   for(int _yr = 0; _yr < inputsNumber; _yr++)
      delete [] a_DOMinputs[_yr];

   delete [] a_DOMinputs;

   delete o_Rules;
#endif   // end DYNAMIC_MANDAMI Version of the class
//···································································
}


//-------------------------------------------------------------------
// The Mandami Fuzzy Inference System: Tnorm - MIN
//-------------------------------------------------------------------
int MANDAMIfuzzy::CalcOutputs(const double _inputs[])
{
   int output_shortcut = 0;
   // Correction over the output value, it is needed because of
   // the membership function shape (perhaps not)
   for(int _ii = 0; _ii < inputsNumber; _ii++)
      if( _inputs[_ii] == 0.0 )
         output_shortcut++;

   if( output_shortcut >= inputsNumber )
   {
      for(int _io = 0; _io < outputsNumber; _io++)
         a_Outputs[_io] = 0.0;
      return(0);
   }

   output_shortcut = 0;
   for(int _ii = 0; _ii < inputsNumber; _ii++)
   if( _inputs[_ii] == 1.0 )
      output_shortcut++;

   if( output_shortcut >= inputsNumber )
   {
      for(int _io = 0; _io < outputsNumber; _io++)
         a_Outputs[_io] = 1.0;
      return(0);
   }

   ClearAll();

   this->CalcDOMinputs(_inputs);

   this->Aggregator(TNORM_MIN, TCONORM_MAX);

   // The Outputs values are stored in an array (a_Outputs)
   this->Defuzzifier(DEFUZZ_zCOA);

//···································································
#ifdef T_MANDAMI_OUTPUTS
printf("\nTEST MANDAMI OUTPUTS");
for(int _io = 0; _io < outputsNumber; _io++)
   printf("OUT-%3d: %1.6f\t", _io, a_Outputs[_io]);

FILE *fp;
if( (fp = fopen("mandami.OUTPUTS", "a")) )
{
   printf("\nTEST MANDAMI OUTPUTS - PRINTING TO FILE");
   for(int _io = 0; _io < outputsNumber; _io++)
      fprintf(fp, "%3d\t%f\n", _io, a_Outputs[_io]);
   fclose(fp);
}
else  {  printf("Error opening FILE in function: CalcOutputs\n");  }
#endif   // end T_MANDAMI_OUTPUTS
//···································································
   return(0);
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------
void MANDAMIfuzzy::ClearAll(void)
{
   ClearTnorm();
   ClearAreaTconorm();
   ClearOutputs();
   ClearDOMinputs();
}
//-------------------------------------------------------------------
void MANDAMIfuzzy::ClearTnorm(void)
{
   for(int _i = 0; _i < rulesNumber; _i++)
      a_Tnorm[_i] = 0.0;
}
//-------------------------------------------------------------------
void MANDAMIfuzzy::ClearAreaTconorm(void)
{
   for(int _is = 0; _is < areaSamples; _is++)
      for(int _io = 0; _io < outputsNumber; _io++)
         this->a_AreaTconorm[_is][_io] = 0.0;
}
//-------------------------------------------------------------------
void MANDAMIfuzzy::ClearOutputs(void)
{
   for(int _i = 0; _i < outputsNumber; _i++)
      a_Outputs[_i] = 0.0;
}
//-------------------------------------------------------------------
void MANDAMIfuzzy::ClearDOMinputs(void)
{
   for(int _ii = 0; _ii < inputsNumber; _ii++)
      for(int _il = 0; _il < linguisticVNumber; _il++)
         this->a_DOMinputs[_ii][_il] = 0.0;
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
void MANDAMIfuzzy::FillTnorm(const char _method)
{
   double auxTnorm = 1;

   if( _method == TNORM_MIN )
   {
      for(int _ir = 0; _ir < rulesNumber; _ir++)
      {
         a_Tnorm[_ir] = 1;
         for(int _ii = 0; _ii < inputsNumber; _ii++)
         {
//            auxTnorm = a_DOMinputs[_ii][o_Rules.a_input[_ir][_ii]];
            auxTnorm = a_DOMinputs[_ii][o_Rules.getPremise(_ir, _ii)];
            if( auxTnorm < a_Tnorm[_ir] )
               a_Tnorm[_ir] = auxTnorm;   // Set minimum
         }  // end for inputsNumber
      }  // end for rulesNumber
   }  // end if TNORM_MIN
   else if( _method == TNORM_PROD )
   {
      ;
   }  // end if TNORM_PROD

//···································································
#ifdef T_FILL_TNORM_ALL
printf("\nTEST INFERENCE SYSTEM - TNORM");
for(int _ir = 0; _ir < rulesNumber; _ir++)
   printf("R(%3d)   %1.3f\t", _ir, a_Tnorm[_ir]);
#endif   // end T_FILL_TNORM_ALL
#ifdef T_FILL_TNORM_SHORT
printf("\nTEST INFERENCE SYSTEM - TNORM");
for(int _ir = 0; _ir < rulesNumber; _ir++)
   if( a_Tnorm[_ir] != 0.0 )
      printf("R(%3d)   %1.3f\t", _ir, a_Tnorm[_ir]);
#endif   // end T_FILL_TNORM_SHORT
//···································································
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------
void MANDAMIfuzzy::FillAreaTconorm(const char _method)
{
   double auxTconorm = 0.0; // Temporal Variable for the Output value
   double auxDOMout = 0.0;  // Temporal Variable for the Degree Of Membership
   double _z = 0.0;         // Samples of Output to build the Area
   // (Max Samples - 1) because I want to consider the two interval border
   double _deltaZ = 1.0/(areaSamples-1);   // Distance between samples

   if( _method == TCONORM_MAX )
   {
      for(int _is = 0; _is < areaSamples; _is++)
      {
         this->o_DOMout->Triangular_9(_z);
//         this->o_DOMout->Triangular_5(_z);
         for(int _io = 0; _io < outputsNumber; _io++)
         {
            auxTconorm = 0;
            a_AreaTconorm[_is][_io] = 0;
            for(int _ir = 0; _ir < rulesNumber; _ir++)
            {
               // Only the Membership Function indicated
               // in the rule _ir for the output _io it is considerate
               if( o_Rules.getConsequence(_ir, _io) > 8 )
                  fprintf(stdout, "%d   %d   --->   %d\n", _ir, _io, o_Rules.getConsequence(_ir, _io));
//               if( _ir == 610 )  o_Rules.setConsequence(_ir, _io, 1);
//               if( _ir == 611 )  o_Rules.setConsequence(_ir, _io, 2);
//               if( _ir == 616 )  o_Rules.setConsequence(_ir, _io, 3);
//               if( _ir == 617 )  o_Rules.setConsequence(_ir, _io, 3);
//               if( _ir == 618 )  o_Rules.setConsequence(_ir, _io, 4);
//               if( _ir == 619 )  o_Rules.setConsequence(_ir, _io, 4);
               auxDOMout = o_DOMout->a_DOM[o_Rules.getConsequence(_ir, _io)];
               // Membership Function's Output for _z is major than Tnorm value
               // for Rule _ir, So check if Tnorm[_ir] is max so far and set max
               if( auxDOMout > a_Tnorm[_ir] )
               {
                  if( a_Tnorm[_ir] > auxTconorm )
                     auxTconorm = a_Tnorm[_ir];
               }  // end if( auxDOMout > a_Tnorm[_ir] )
               // Membership Function's Output (MFO) for _z is smaller than Tnorm value
               // for Rule _ir, So check if MFO is max so far and set max
               else
               {
                  if( auxDOMout > auxTconorm )
                     auxTconorm = auxDOMout;
               }  // end else for if( auxDOMout > a_Tnorm[_ir] )
            }  // for outputsNumber
            a_AreaTconorm[_is][_io] = auxTconorm;
         }  // end for rulesNumber
         _z += _deltaZ;
      }  // end for areaSamples
   }  // end if TNORM_MIN

//···································································
#ifdef T_FILL_TCONORM
printf("\nTEST INFERENCE SYSTEM - TCONORM - AREA");
for(int _is = 0; _is < areaSamples; _is++)
   for(int _io = 0; _io < outputsNumber; _io++)
      printf("S-%3d: %1.3f\t", _is, a_AreaTconorm[_is][_io]);
#endif   // end T_FILL_TCONORM

#ifdef T_FILL_TCONORM_FILE
FILE *fp;
if( (fp = fopen("../Debug.Outputs/mandami.TCONORM", "a")) )
{
   printf("\nTEST INFERENCE SYSTEM - TCONORM - AREA - PRINTING TO FILE");
   for(int _is = 0; _is < areaSamples; _is++)
      for(int _io = 0; _io < outputsNumber; _io++)
         fprintf(fp, "%1.6f\n", a_AreaTconorm[_is][_io]);
   fclose(fp);
}
else  {  printf("Error opening FILE in function: FillAreaTconorm\n");  }
#endif   // end T_FILL_TCONORM_FILE
//···································································
//···································································
}


//-------------------------------------------------------------------
// Defuzzification Methods: Centroid of Area
// zCOA = ( sum(uA(z)*z) ) / ( sum(uA(z)) )
// where uA(z) = (Area Tconorm)
//-------------------------------------------------------------------
void MANDAMIfuzzy::Centroid(double zCOA[])
{
   double _numerator;
   double _denominator;
   double _z = outputMinValue; // Samples of Output to build the Area
   // (Max Samples - 1) because I want to consider the two interval border
   double _deltaZ = outputMaxValue/(areaSamples-1); // Distance between samples

   for(int _io = 0; _io < outputsNumber; _io++)
   {
      _numerator = 0.0;
      _denominator = 0.0;
      _z = 0.0;
      for(int _is = 0; _is < areaSamples; _is++)
      {
         _numerator += (a_AreaTconorm[_is][_io]*_z);
         _denominator += a_AreaTconorm[_is][_io];
         _z += _deltaZ;
      }  // end for areaSamples
      zCOA[_io] = 0.0;
      if( _denominator != 0.0 )
         zCOA[_io] = _numerator/_denominator;
   }  // end for outputsNumber

//···································································
#ifdef T_CENTROID_OUTS
printf("\nTEST DEFFUZIFIER - CENTROID");
for(int _io = 0; _io < outputsNumber; _io++)
   printf("OUT-%3d: %1.6f\t", _io, zCOA[_io]);
#endif   // end T_CENTROID_OUTS
//···································································
}


//-------------------------------------------------------------------
// Defuzzification Methods: Bisector of Area
//-------------------------------------------------------------------
void MANDAMIfuzzy::Bisector(double zBOA[])
{
   ;
}


//-------------------------------------------------------------------
// Defuzzification Methods: Mean of maximun
//-------------------------------------------------------------------
void MANDAMIfuzzy::MeanMax(double zMOM[])
{
   ;
}


//-------------------------------------------------------------------
// void CalcDOMinputs(const double _inputs[])
// Calculate the Degree Of Membership for every Input
//-------------------------------------------------------------------
void MANDAMIfuzzy::CalcDOMinputs(const double _inputs[])
{
   // Depend on number of linguistic variables, kind and
   // number of Membership Functions
   for(int _ii = 0; _ii < inputsNumber; _ii++)
   {
      if( this->o_DOMin->getLinguisticVNumber() == 5 )
      {
         this->o_DOMin->Triangular_5(_inputs[_ii]);
      }  // end if getLinguisticVNumber
      for(int _ilv = 0; _ilv < linguisticVNumber; _ilv++)
         this->a_DOMinputs[_ii][_ilv] = this->o_DOMin->a_DOM[_ilv];
   }  // end for inputsNumber

//···································································
#ifdef T_DOM_INPUTS
printf("\nTEST OUTPUT DEGREE OF MEMBERSHIP");
for(int _ii = 0; _ii < inputsNumber; _ii++)
{
   printf("\nINPUT (%d):  ", _ii);
   for(int _ilv = 0; _ilv < linguisticVNumber; _ilv++)
      printf("%1.3f  ", a_DOMinputs[_ii][_ilv]);
}
#endif   // end T_DOM_INPUTS
//···································································
}


//-------------------------------------------------------------------
// The Mandami Fuzzy Inference System
//-------------------------------------------------------------------
void MANDAMIfuzzy::Aggregator(const char _methodNorm, const char _methodConorm)
{
   this->FillTnorm(_methodNorm);
   this->FillAreaTconorm(_methodConorm);
}


//-------------------------------------------------------------------
// Defuzzification Methods
//-------------------------------------------------------------------
void MANDAMIfuzzy::Defuzzifier(const char _method)
{
   // Centroid Of Area
   if( _method == DEFUZZ_zCOA )     {  this->Centroid(a_Outputs); }

   // Bisector Of Area
   else if( _method == DEFUZZ_zBOA ){  this->Bisector(a_Outputs); }

   // Mean Of Max
   else if( _method == DEFUZZ_zMOM ){  this->MeanMax(a_Outputs);  }
}


//-------------------------------------------------------------------
void MANDAMIfuzzy::setRulesNumber(int _value)   {  rulesNumber = _value;   }
//-------------------------------------------------------------------
void MANDAMIfuzzy::setInputsNumber(int _value)  {  inputsNumber = _value;  }
//-------------------------------------------------------------------
void MANDAMIfuzzy::setOutputsNumber(int _value) {  outputsNumber = _value; }
//-------------------------------------------------------------------
void MANDAMIfuzzy::setLinguisticVNumber(int _value){  linguisticVNumber = _value;}
//-------------------------------------------------------------------
void MANDAMIfuzzy::setAreaSamples(int _value)   {  areaSamples = _value;   }
//-------------------------------------------------------------------
void MANDAMIfuzzy::setOutMinValue(double _value){  outputMinValue = _value;}
//-------------------------------------------------------------------
void MANDAMIfuzzy::setOutMaxValue(double _value){  outputMaxValue = _value;}

//-------------------------------------------------------------------
int MANDAMIfuzzy::getRulesNumber(void) const    {  return(rulesNumber);    }
//-------------------------------------------------------------------
int MANDAMIfuzzy::getInputsNumber(void) const   {  return(inputsNumber);   }
//-------------------------------------------------------------------
int MANDAMIfuzzy::getOutputsNumber(void) const  {  return(outputsNumber);  }
//-------------------------------------------------------------------
int MANDAMIfuzzy::getLinguisticVNumber(void) const {  return(linguisticVNumber);}
//-------------------------------------------------------------------
int MANDAMIfuzzy::getAreaSamples(void) const    {  return(areaSamples);    }
//-------------------------------------------------------------------
double MANDAMIfuzzy::getOutMinValue(void) const {  return(outputMinValue); }
//-------------------------------------------------------------------
double MANDAMIfuzzy::getOutMaxValue(void) const {  return(outputMaxValue); }
//-------------------------------------------------------------------
double MANDAMIfuzzy::getDOM(const int _input, const int _lv) const
{
   return(a_DOMinputs[_input][_lv]);
}
//-------------------------------------------------------------------
double MANDAMIfuzzy::getOutput(const int _output) const   {  return(a_Outputs[_output]);   }

//-------------------------------------------------------------------
// void initInputs(c_FuzzyRules& o_rls)
// very inefficient method
// n = rules
// O(n)^(inputs)
//-------------------------------------------------------------------
void MANDAMIfuzzy::InitRulesInputs(c_FuzzyRules& o_rls)
{
   int _ilv = 0;  // Index for linguistic variables
   int _aux = 0;  // Contain the value of (linguistic ^ input)

   // Init fuzzy rules
   for(int _ii = (inputsNumber-1); _ii >= 0 ; _ii--)
   {
      _aux = lround(pow(linguisticVNumber, _ii)); // ceil???
      for(int _ir = 0; _ir < rulesNumber; _ir++)
      {
         o_rls.setPremise(_ir, _ii, _ilv);

         if( (_ir % _aux) == 0 )
            _ilv++;
         if( _ilv == linguisticVNumber )
            _ilv = 0;
      }  // end for rulesNumber
   }  // end for inputsNumber

//···································································
#ifdef T_INIT_RINPUTS
printf("\nTEST (COMBINATORIAL) INPUTS FOR EVERY RULE");
for(int _ir = 0; _ir < rulesNumber; _ir++)
{
   printf("(R%3d) ", _ir);
   for(int _ii = 0; _ii < inputsNumber; _ii++)
      printf("%2d\t", o_rls.getPremise(_ir, _ii));
}
#endif   // end T_INIT_RINPUTS

#ifdef T_INIT_RINPUTS_FILE
printf("\nTEST (COMBINATORIAL) INPUTS FOR EVERY RULE - PRINTING TO FILE");
FILE *fp;
if( (fp = fopen("MANDAMI.RULES.625in", "w")) )
{
   for(int _ir = 0; _ir < rulesNumber; _ir++)
   {
      fprintf(fp, "\n(R%3d)\t", _ir);
      for(int _ii = 0; _ii < inputsNumber; _ii++)
         fprintf(fp, "%2d\t", o_rls.getPremise(_ir, _ii));
   }
   fclose(fp);
}
else  {  printf("Error opening FILE in function: InitRulesInputs\n");  }
#endif   // end T_INIT_RINPUTS_FILE
//···································································
}


//-------------------------------------------------------------------
// void InitRulesOutputs(c_FuzzyRules& o_rls)
// very inefficient method
// n = rules
// O(n)^(inputs)
//-------------------------------------------------------------------
void MANDAMIfuzzy::InitRulesOutputs(c_FuzzyRules& o_rls)
{
   int _premises[inputsNumber];
   double auxSum = 0.0;
   double auxOut = 0.0;

   for(int _ir = 0; _ir < rulesNumber; _ir++)
   {
      o_rls.getRule(_ir, _premises);
      auxSum = 0.0;
      for(int _it = 0; _it < inputsNumber; _it++)
      {
         auxSum += _premises[_it];
         _premises[_it] = 0;
      }  // end for inputsNumber

      for(int _io = 0; _io < outputsNumber; _io++)
      {
         auxOut = (auxSum/2);
         if( (auxOut < 0.5) )   {  o_rls.setConsequence(_ir, _io, lround(floor(auxOut))); }
         else if( (auxOut >= 0.5) && (auxOut < 1.0) ) {  o_rls.setConsequence(_ir, _io, lround(ceil(auxOut)));    }
         else if( (auxOut >= 1.0) && (auxOut < 1.5) ) {  o_rls.setConsequence(_ir, _io, lround(floor(auxOut)));   }
         else if( (auxOut >= 1.5) && (auxOut < 2.0) ) {  o_rls.setConsequence(_ir, _io, lround(ceil(auxOut)));    }
         else if( (auxOut >= 2.0) && (auxOut < 2.5) ) {  o_rls.setConsequence(_ir, _io, lround(floor(auxOut)));   }
         else if( (auxOut >= 2.5) && (auxOut < 3.0) ) {  o_rls.setConsequence(_ir, _io, lround(ceil(auxOut)));    }
         else if( (auxOut >= 3.0) && (auxOut < 3.5) ) {  o_rls.setConsequence(_ir, _io, lround(floor(auxOut)));   }
         else if( (auxOut >= 3.5) && (auxOut < 4.0) ) {  o_rls.setConsequence(_ir, _io, lround(ceil(auxOut)));    }
         else if( (auxOut >= 4.0) && (auxOut < 4.5) ) {  o_rls.setConsequence(_ir, _io, lround(floor(auxOut)));   }
         else if( (auxOut >= 4.5) && (auxOut < 5.0) ) {  o_rls.setConsequence(_ir, _io, lround(ceil(auxOut)));    }
         else if( (auxOut >= 5.0) && (auxOut < 5.5) ) {  o_rls.setConsequence(_ir, _io, lround(floor(auxOut)));   }
         else if( (auxOut >= 5.5) && (auxOut < 6.0) ) {  o_rls.setConsequence(_ir, _io, lround(ceil(auxOut)));    }
         else if( (auxOut >= 6.0) && (auxOut < 6.5) ) {  o_rls.setConsequence(_ir, _io, lround(floor(auxOut)));   }
         else if( (auxOut >= 6.5) && (auxOut < 7.0) ) {  o_rls.setConsequence(_ir, _io, lround(ceil(auxOut)));    }
         else if( (auxOut >= 7.0) && (auxOut < 7.5) ) {  o_rls.setConsequence(_ir, _io, lround(floor(auxOut)));   }
         else if( (auxOut >= 7.5) && (auxOut <= 8.0) ){  o_rls.setConsequence(_ir, _io, lround(ceil(auxOut)));    }
      }  // end fot outputsNumber
   }  // end for rulesNumber

//···································································
#ifdef T_INIT_ROUTPUTS
printf("\nTEST CONSEQUENSE OR OUTPUTS FOR EACH RULE");
for(int _ir = 0; _ir < rulesNumber; _ir++)
{
   printf("R(%3d)  ", _ir);
   for(int _io = 0; _io < outputsNumber; _io++)
   {
      printf("%d\t", o_rls.getConsequence(_ir, _io));
   }
}
#endif   // end T_INIT_ROUTPUTS

#ifdef T_INIT_ROUTPUTS_FILE
printf("\nTEST CONSEQUENSE OR OUTPUTS FOR EACH RULE - PRINTING TO FILE\n\n");
FILE *fp;
if( (fp = fopen("MANDAMI.RULES.625out", "w")) )
{
   for(int _ir = 0; _ir < rulesNumber; _ir++)
   {
      fprintf(fp, "\nR(%3d)\t", _ir);
      for(int _io = 0; _io < outputsNumber; _io++)
      {
         fprintf(fp, "%d\t", o_rls.getConsequence(_ir, _io));
      }
   }
   fclose(fp);
}
else  {  printf("Error opening FILE in function: InitRulesOutputs\n");  }
#endif   // end T_INIT_ROUTPUTS_FILE
//···································································
}
//*******************************************************************
// Mandami Fuzzy Model Class Implementation: END
//*******************************************************************
