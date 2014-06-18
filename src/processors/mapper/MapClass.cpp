/***********************************************************************
* IRMA2: MapClass.cpp
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
#include "MapClass.h"
#include <stdio.h>
/**********************************************************************************************
Constructor: initilization process
**********************************************************************************************/
MapClass::MapClass(int a, int b, float c, float d)
//initialize world and mapping parameters:
:worldSizeX(a),worldSizeY(b),beta(c),rho(d)
{
    //Initially 0 nodes activated
    activeNodes=0;

    //initialize all categories to one
    for (int i = 0; i < nCategories; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            category[i][j] = 1;
        }
    }
}

/**********************************************************************************************
Adds one point to internal map input data int.
/*********************************************************************************************/
void MapClass::AddPoint_Map(int newPoint[2])
{
    float input[4]; //Vector for new input coordinates (corresponding to I)
    float T[nCategories]; //Vector for the results of choice functions for each category
    float min[4]; //short memory for calculation of fuzzy minimum at choice function
    float alpha = 0.00000001; //choice parameter alpha
    int state = 0; //variable to exit the loop
    int count = 0; //variable to count the loop cycles
    int jMax = 0; //variable for the calculation of node with maximum T

    //copy point into input vector
    input[0] = newPoint[0];
    input[1] = newPoint[1];

    //normalize the coordinates relating to the world size
    input[0] /= worldSizeX;
    input[1] /= worldSizeY;
    //complement the Fuzzy coordinates
    input[2] = 1 - input[0];
    input[3] = 1 - input[1];

    //search for the best matching node, and update this node. If no matching node left, creation of new node.
    //check if any node exists:
    if (activeNodes > 0)
    {
        //Calculation of the choice function for each active node, and save it in array T
        for (int node = 0; node<activeNodes; node++)
        {
            //calculation of fuzzy AND operator (minimum of each element)
            for (int i = 0; i<4; i++) //element wise
            {
                if (input[i] < category[node][i]) //this block calculates the minimum of each element pair
                    min[i] = input[i];
                else
                    min[i] = category[node][i];
            }
            //calculation of the choice value:
            T[node] = (min[0]+min[1]+min[2]+min[3])/(alpha+category[node][0]+category[node][1]+category[node][2]+category[node][3]);
        }
        //Selection of active node (node with the highest choice factor) and verification if the input
        //can be added to this category
        while (state==0 && activeNodes-count>0)
        {
            //calculation of the node with the best weighting factor
            for (int j=1; j<activeNodes; j++)
            {
                if (T[j] > T[jMax])
                {
                    jMax=j;
                }
            }

            //calculate fuzzy AND for input and most matching node
            for (int i=0; i<4; i++)
            {
                if (category[jMax][i]<input[i])
                    min[i]=category[jMax][i];
                else
                    min[i]=input[i];
            }

            //check if the point expands category (vigilance criterion valid?)
            if ((min[0]+min[1]+min[2]+min[3])/(input[0]+input[1]+input[2]+input[3]) >= rho)
                state=1; //exit the loop
            else //if not go on with the loop and deactivate node
            {
                T[jMax] = 0; //deactivate node
                count++;
            }
        }

        //update/activate new category
        if (state==1)//update category
        {
            for (int i = 0; i<4; i++)
            {
                category[jMax][i]=beta*min[i]+(1-beta)*category[jMax][i];
            }
        }
        else //activate new category
        {
            for (int i = 0; i<4; i++)
            {
                category[activeNodes][i]=input[i];
            }
            activeNodes++;
        }
    }

    //if input is the first node:
    else
    {
        activeNodes++;
        //set active input as first node
        for (int i = 0; i<4; i++)
        {
            category[0][i] = input[i];
        }
    }

    //print the actual nodes:
    for (int j=0; j<activeNodes; j++)
    {
        fprintf(stdout,"Category[%i]=[%f,%f,%f,%f] \n",j,category[j][0],category[j][1],category[j][2],category[j][3]);
    }
}


/***********************************************************************************************
Saves Map data into text file -- This function is used for testing!
/**********************************************************************************************/
void MapClass::Output(int mapNumber, int output_filter, int max_rectangle_size)
{
    int number_small_rectangles = 0;//number of too small rectangles
    ofstream *fout;
    if (mapNumber == 0)      //choose output file:
        fout = new ofstream("Map0.map");
    else if (mapNumber == 1)
        fout = new ofstream("Map1.map");
    else if (mapNumber == 2)
        fout = new ofstream("Map2.map");
    else if (mapNumber == 3)
        fout = new ofstream("Map3.map");

    for (int j = 0; j<activeNodes; j++)
    {
       int rectangle_size = lround(((1-category[j][2])-category[j][0])*2500+((1-category[j][3])-category[j][1])*2000); //calculation of rectangle size
       if (output_filter==1 && rectangle_size <= max_rectangle_size) //check if output filter activated if yes, check size
        {
           number_small_rectangles++; //calculate number of too small categories
        }
    }

    *fout << (activeNodes-number_small_rectangles) << endl;
     for (int j=0; j<activeNodes; j++)
     {
        int rectangle_size = lround(((1-category[j][2])-category[j][0])*2500+((1-category[j][3])-category[j][1])*2000); //calculation of rectangle size
        if (output_filter==1 && rectangle_size > max_rectangle_size) //check if output filter activated if yes, check size
        {
           *fout << category[j][0] << "," <<  category[j][1] << "," <<  category[j][2] << "," << category[j][3] <<endl;
        }
        else if (output_filter==0) //if output filter deactivatet write all rectangles
        {
           *fout << category[j][0] << "," <<  category[j][1] << "," <<  category[j][2] << "," << category[j][3] <<endl;
        }
     }
    delete fout;
}
void MapClass::Output(const char a_fileName[], int output_filter, int max_rectangle_size)
{
    int number_small_rectangles = 0;//number of too small rectangles
    ofstream *fout;

    fout = new ofstream(a_fileName);

    for (int j = 0; j<activeNodes; j++)
    {
       int rectangle_size = lround(((1-category[j][2])-category[j][0])*2500+((1-category[j][3])-category[j][1])*2000); //calculation of rectangle size
       if (output_filter==1 && rectangle_size <= max_rectangle_size) //check if output filter activated if yes, check size
        {
           number_small_rectangles++; //calculate number of too small categories
        }
    }

    *fout << (activeNodes-number_small_rectangles) << endl;
     for (int j=0; j<activeNodes; j++)
     {
        int rectangle_size = lround(((1-category[j][2])-category[j][0])*2500+((1-category[j][3])-category[j][1])*2000); //calculation of rectangle size
        if (output_filter==1 && rectangle_size > max_rectangle_size) //check if output filter activated if yes, check size
        {
           *fout << category[j][0] << "," <<  category[j][1] << "," <<  category[j][2] << "," << category[j][3] <<endl;
        }
        else if (output_filter==0) //if output filter deactivatet write all rectangles
        {
           *fout << category[j][0] << "," <<  category[j][1] << "," <<  category[j][2] << "," << category[j][3] <<endl;
        }
     }
    delete fout;
}

/***********************************************************************************************
Cleans all obstacles outside the perception area of the robot.
position[2]: actual robot position (in mm)
radius: the radius (in mm) of the given area (perception area = position +- radius (robot length+max sensor range)
***********************************************************************************************/
void MapClass::Clean_Map(int position[2], float radius)
{
    float input[4];//vector which defines the perception area
    float radiusNormalizedX; //variable for normalized radius in x dirction;
    float radiusNormalizedY; //variable for normalized radius in y direction;
    bool checkU; // if checkU=true U lies in perception area
    bool checkV; // if checkV=true V lies in perception area
    bool checkCorner; //if checkCorner=true left upper corner or right downer corner lies in perception area

    //load robot position into perception area vector
    input[0] = position[0];
    input[1] = position[1];

    //Normalize the robot position
    input[0] /= worldSizeX;
    input[1] /= worldSizeY;
    //complement:
    input[2] = 1 - input[0];
    input[3] = 1 - input[1];

    //Normalize the radius in x and y dirction
    radiusNormalizedX = radius / worldSizeX;
    radiusNormalizedY = radius / worldSizeY;


    //calculate the perception area
    if (input[0] - radiusNormalizedX >= 0)//check if given area out of world range
        input[0] -= radiusNormalizedX;
    else
        input[0] = 0;

    if (input[1] - radiusNormalizedY >= 0)//check if given area out of world range
        input[1] -= radiusNormalizedY;
    else
        input[1] = 0;

    if (input[2] - radiusNormalizedX >=0)//check if given area out of world range
        input[2] -= radiusNormalizedX;
    else
        input[2] = 0;

    if (input[3] - radiusNormalizedY >=0)//check if given area out of world range
        input[3] -= radiusNormalizedY;
    else
        input[3] = 0;

//    cout << "u_p1: " <<input[0] << " ,u_p2: " << input[1] << " ,v_p1: " << input[2] << " ,v_p2: " << input[3] << endl;

    //check if categories are lying in the perception area
    for (int j = 0; j < activeNodes; j++)
    {
        //coordinates of the actual category:
        float u_j1 = category[j][0];
        float u_j2 = category[j][1];
        float v_j1 = 1 - category[j][2];
        float v_j2 = 1 - category[j][3];
        //coordinates of the perception area
        float u_p1 = input[0];
        float u_p2 = input[1];
        float v_p1 = 1 - input[2];
        float v_p2 = 1 - input[3];

        //check if U lies in perception area:
        checkU = (u_j1 < v_p1) && (u_j1 > u_p1) && (u_j2 < v_p2) && (u_j2 > u_p2);
        //check if v lies in perception area:
        checkV = (v_j1 > u_p1) && (v_j1 < v_p1) && (v_j2 > u_p2) && (v_j2 < v_p2);
        //check if other corner lies in perception area:
        checkCorner = ((v_j2 > u_p2) && (u_j1 < v_p1) && (v_j1 > v_p1) && (u_j2 < u_p2)) || ((v_j1 > u_p1) && (u_j2 < v_p2) && (v_j2 > v_p2) && (u_j1 < u_p2));

//        cout << "checkU: "<< checkU << ", checkV: " << checkV <<",checkCorner: "<< checkCorner  << endl;
        //check if category has to be erased
        if ((checkU || checkV || checkCorner) == false)
        {
            fprintf(stdout,"category %i must be erased \n ", j);
            //decrement number of active nodes
            activeNodes--;
            //erase/overwrite useless node with the last node
            for (int i = 0; i < 4; i++)
            {
                category[j][i] = category[activeNodes][i];
                category[activeNodes][i] = 1; //reinitialize last node
            }
            //decrement counter j because the new node in this position has to be checked, too.
            j--;

        }

    }

}

