/*
 * FILE: simfuzlib.h, v.1.0.0, 6/27/2008
 * Author: Tomas V. Arredondo
 *
 * SimFuzLib: A simple and flexible MIMO Fuzzy implementation in C++.
 *
 * This is NOT a tutorial on Fuzzy as there are many tutorials and books on this
 * subject see http://en.wikipedia.org/wiki/Fuzzy_logic for more information.
 *
 *
 * DISCLAIMER: No liability is assumed by the author for any use made
 * of this program.
 * DISTRIBUTION: Any use may be made of this program, as long as the
 * clear acknowledgment is made to the author in code and runtime executables
 */
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

#ifndef SIMFUZLIB_H
#define SIMFUZLIB_H

#include "stdio.h"

namespace simfuzlib
{


    using namespace std;

#define NUM_MEMBER_FNS  3
#define NUM_INPUTS      5
#define NUM_OUTPUTS     2
#define NUM_FUZZY_RULES 243 // 3^5
#define NUM_ARENA_POINTS 100 // fuzzy granularity

    enum FuzzyFNS3 {L=0, M=1, H=2};

    class Drive
    {
    public:
        float steer; // 0: left, 0.5: straight, 1.0: right
        float speed;  // 0: back, 0.5: no motion, 1.0: forward

    public:
        Drive()
        {
            steer=0.5;
            speed=0.5;
        }
    };
    typedef Drive drive_t;

    class MemberFns3
    {
    public:
        float mf[NUM_INPUTS][3];  // [0]: Low, [1]: Med, [3]: High

        MemberFns3()
        {
            for (int i=0;i<NUM_INPUTS;i++)
            {
                for (int j=0;j<3;j++)
                    mf[i][j]=0;
            }
        }

        void Display()
        {
            for (int i=0;i<NUM_INPUTS;i++)
            {
                fprintf(stdout,"Member fns: %f:%f:%f\n",mf[i][0],mf[i][1],mf[i][2]);

                //std::cout << "Member fns: "<< mf[i][0] << ":" << mf[i][1] << ":" << mf[i][2];
                //std::cout << endl;
            }
        }

        void EvalInputs(float x[NUM_INPUTS])
        {
            for (int i=0;i < NUM_INPUTS;i++)
            {
                if ( (x[i] >= 0) && (x[i] < 0.25) )
                {
                    mf[i][0] = 1;
                    mf[i][1] = 0;
                    mf[i][2] = 0;
                }
                else if ( (x[i] >= 0.25) && (x[i] < 0.5) )
                {
                    mf[i][0] = 2 - 4*x[i];
                    mf[i][1] = 4*x[i] - 1;
                    mf[i][2] = 0;
                }
                else if ( (x[i] >= 0.5) && (x[i] < 0.75) )
                {
                    mf[i][0] = 0;
                    mf[i][1] = 3 - 4*x[i];
                    mf[i][2] = 4*x[i] -2;
                }
                else if ( (x[i] >= 0.75) && (x[i] <= 1.0) )
                {
                    mf[i][0] = 0;
                    mf[i][1] = 0;
                    mf[i][2] = 1;
                }
                else if ( x[i] < 0 )
                {
                    mf[i][0] = 0;
                    mf[i][1] = 0;
                    mf[i][2] = 0;
                }
                else if ( x[i] > 1 )
                {
                    mf[i][0] = 0;
                    mf[i][1] = 0;
                    mf[i][2] = 0;
                }
            }
        }

    };
    typedef MemberFns3 member_fns3_t;

    class FuzzyRules
    {
    public:
        int in[NUM_FUZZY_RULES][NUM_INPUTS];
        int out[NUM_FUZZY_RULES][NUM_OUTPUTS];
        FuzzyRules()
        {
            ClearFuzzyRules();
        }
        void ClearFuzzyRules()
        {
            for (int i=0;i<NUM_FUZZY_RULES;i++)
            {
                for (int j=0;j<NUM_INPUTS;j++)
                    in[i][j]=0;
                for (int j=0;j<NUM_INPUTS;j++)
                    out[i][j]=0;
            }
        }
    };
    typedef FuzzyRules fuzzy_rules_t;

    extern void EvalMF(const float x, const int tmp_fn, float& tmp_mf);

    class Mamdani
    {
    private:
        float arena1[NUM_ARENA_POINTS]; // steer
        float arena2[NUM_ARENA_POINTS]; // forward
        float min[NUM_FUZZY_RULES];

        void ClearMin()
        {
            for (int i=0;i<NUM_FUZZY_RULES;i++)
            {
                min[i]=0;
            }
        }

    public:
        Mamdani()
        {
            ClearArena1();
            ClearArena2();
            ClearMin();
        }
        void ClearArena1()
        {
            for (int i=0;i<NUM_ARENA_POINTS;i++)
            {
                arena1[i]=0;
            }
        }
        void ClearArena2()
        {
            for (int i=0;i<NUM_ARENA_POINTS;i++)
            {
                arena2[i]=0;
            }
        }

        float minVal(const float v,const float w, const float x, const float y, const float z)
        {
            float m = 1;
            if ( v < m ) m = v;
            if ( w < m ) m = w;
            if ( x < m ) m = x;
            if ( y < m ) m = y;
            if ( z < m ) m = z;
            return(m);
        }

        float maxVal(const float v,const float w, const float x, const float y, const float z)
        {
            float m = 0;
            if ( v > m ) m = v;
            if ( w > m ) m = w;
            if ( x > m ) m = x;
            if ( y > m ) m = y;
            if ( z > m ) m = z;
            return(m);
        }


        void calcMin(const fuzzy_rules_t& rules, const member_fns3_t member_fns )
        {
            for (int i=0;i<NUM_FUZZY_RULES;i++)
            {   // determine minimum value for all input functions given the inputs
                min[i]=minVal(  member_fns.mf[0][rules.in[i][0]],
                                member_fns.mf[1][rules.in[i][1]],
                                member_fns.mf[2][rules.in[i][2]],
                                member_fns.mf[3][rules.in[i][3]],
                                member_fns.mf[NUM_INPUTS-1][rules.in[i][NUM_INPUTS-1]]);  // make sure to use all rules here!
            }
        }
        void FillArenas(const fuzzy_rules_t& rules, const member_fns3_t member_fns)
        {
            float x=0;
            float x_inc=1.0/NUM_ARENA_POINTS;
            float tempMax;
            float tempEvalOutputMF;

            for (int i=0;i<NUM_ARENA_POINTS;i++)
            {  // find max of all output functions for each arena point (bounded by min[i] from inputs)
                arena1[i]=0;
                arena2[i]=0;
                x=x+x_inc;

                // Do arena1: steer for point x
                tempMax=0;  // set maximum value of arena point x to 0

                // steer first: rules.out[x][0]
                for (int j=0;j<NUM_FUZZY_RULES;j++)
                {
                    simfuzlib::EvalMF(x, rules.out[j][0], tempEvalOutputMF );

                    if (tempEvalOutputMF > min[j])
                    {  // output mf for x is > min for rule i so check if min[i] is max so far and set max
                        if (min[j] > tempMax)
                            tempMax = min[j];
                    }
                    else
                    {  // output mf for x is < min so check if output mf is max so far and set max
                        if (tempEvalOutputMF > tempMax)
                            tempMax = tempEvalOutputMF;
                    }
                }
                arena1[i]=tempMax;  // set arena point x to maximum value for all rules

                // Do arena2: drive for point x
                tempMax=0;  // set maximum value of arena point x to 0

                // drive : rules.out[1][x]
                for (int j=0;j<NUM_FUZZY_RULES;j++)
                {

                    simfuzlib::EvalMF(x, rules.out[j][1], tempEvalOutputMF );

                    if (tempEvalOutputMF > min[j])
                    {  // output mf for x is > min for rule i so check if min[i] is max so far and set max
                        if (min[j] > tempMax)
                            tempMax = min[j];
                    }
                    else
                    {  // output mf for x is < min so check if output mf is max so far and set max
                        if (tempEvalOutputMF > tempMax)
                            tempMax = tempEvalOutputMF;
                    }
                }
                arena2[i]=tempMax;  // set arena point x to maximum value for all rules

            }
        }
        void Centroids(drive_t &drive)
        {
            float prodSum1=0;
            float Sum1=0;
            float prodSum2=0;
            float Sum2=0;
            float x=0;
            float x_inc=1.0/NUM_ARENA_POINTS;

            for (int i=0;i<NUM_ARENA_POINTS;i++)
            {  // find the prod sum
                x=x+x_inc;
                Sum1+=arena1[i];
                prodSum1+=arena1[i]*x;
                Sum2+=arena2[i];
                prodSum2+=arena2[i]*x;
            }
            //assert(Sum1);
            //assert(Sum2);
            drive.steer=prodSum2/Sum2;
            drive.speed=prodSum1/Sum1;
        }
        void calc(const fuzzy_rules_t& rules, const member_fns3_t& member_fns, drive_t& drive )
        {
            // update min membership values for all rules and store in min
            calcMin(rules, member_fns);

            // fill the arenas for outputs one and two
            FillArenas(rules, member_fns);

            // calculate centroids of both arenas and store in drive
            Centroids(drive);
        }

    };
    typedef Mamdani mamdani_t;


} // namespace simfuzlib

#endif  // SIMFUZLIB_H

