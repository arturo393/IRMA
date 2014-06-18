/***********************************************************************
* IRMA2: mapper.cpp
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
#include "mapper.h"


#define pi 3.14159265

//Definition of constants for test cases
#define iPoint_1 170 //initaially 20
#define iPoint_2 10 //initially 40

#define wRoom 2500
#define hRoom 2000



/**********************************************************************************************
Initializes the Dynamic Mapper data (final version)
**********************************************************************************************/
int MapperProcessor::init()
{
    pExecutive = (ExecutiveData *)cda.getMemPtr(EXECUTIVE_AREA);
    pMapper = (MapperData *)cda.getMemPtr(MAPPER_AREA);
    pCloseNav = (CloseNavData *)cda.getMemPtr(CLOSE_NAV_AREA);

    pCtrl = &(pMapper->ctrl); // Todos los procesadores tienen que definir pCtrl
    sem_area = MAPPER_AREA;   // Todos los procesadores tienen que definir sem_area

    // At the start, there shouldn't be any previously existing obstacle
    // and there shouldn't be any mapping until told so

    cda.lockArea(MAPPER_AREA);//lock Mapper Area
       pMapper->room_width = wRoom;
       pMapper->room_height = hRoom;
       pMapper->activeNodes = 0;
       pMapper->mapping_flag = false;
       // Categories must be initialized on 1.
       for (int i=0; i<nCategories; i++)
       {
           for (int j=0; j<4; j++)
           {
               pMapper->categories[i][j]=1;
           }
       }
    cda.unlockArea(MAPPER_AREA);// Unlock Mapper Area

    //Instaciation of maps which provides free map area:
    //flexmaps:
    flexMap_1 = new MapClass(2500,2000,1,0.91); //create flexmap which provides free area of the first side perception area
    flexMap_2 = new MapClass(2500,2000,1,0.91); //create flexmap which provides free area of the second side perception area
    flexMap_3 = new MapClass(2500,2000,1,0.95); //create flexmap which provides free area of the front side perception area
    //megamaps:
    megaMap_1 = new MapClass(2500,2000,1,0.6); //create flexmap which provides free area of neigbored free sensors
    megaMap_2 = new MapClass(2500,2000,1,0.6);
    //Instantiation of Internal Map Object
    intMap = new MapClass(2500,2000,1,0.95); //create intMap which saves obstacle data

    /************************ START: Config File reading.**************************/

    ConfigFile config( "config/mapper.conf" );
    int mode;
    string default_input_file = "FART.map";
    string default_output_file = "outputMap.map";

    config.readInto( mode, "mode", 1 );
    config.readInto( input_map, "input_file", default_input_file);
    config.readInto( mapping_mode, "mapping_mode", 0);
    config.readInto( output_map , "output_file", default_output_file);
    config.readInto( max_rectangle_size , "max_rectangle_size", 80);
    config.readInto( output_filter , "output_filter", 0);
    //snprintf(OUT_FILE, sizeof(OUT_FILE),  "%s", output_map);

    if ( mode > 2 || mode < 0)
        mode = 0;

    if (mode == 2)
        mapping = false;
    else
        mapping = true;

    if (mode == 1 || mode == 2)
    {
        //Ya no es necesario, ya que hay archivo de entrada y de salida. Borrar si todo funciona bien. Renato
        //ofstream fo;
        //fo.open(input_map.c_str(), ofstream::app);
        //fo << "0" << endl;
        //fo.close();


        ifstream fi(input_map.c_str(), ifstream::in);
        char line[40];
        int t_active;

        fi >> line;
        sscanf(line,"%d",&t_active);

        cda.lockArea(MAPPER_AREA);
        pMapper->activeNodes = t_active; //Save into Internal Map in CDA
        cda.unlockArea(MAPPER_AREA);
        intMap->activeNodes = t_active; //Save into Internal Map in MapClass

        for (int i=0; i<t_active; i++)
        {
            fi >> line;

            cda.lockArea(MAPPER_AREA);
            sscanf(line,"%f,%f,%f,%f", &pMapper->categories[i][0], &pMapper->categories[i][1], &pMapper->categories[i][2], &pMapper->categories[i][3]);//save categories in internal Map in CDA
            intMap->category[i][0] = pMapper->categories[i][0]; //save categories into internal map in MapClass
            intMap->category[i][1] = pMapper->categories[i][1];
            intMap->category[i][2] = pMapper->categories[i][2];
            intMap->category[i][3] = pMapper->categories[i][3];
            cda.unlockArea(MAPPER_AREA);
        }
        fi.close();
    }

/************************END: Config File reading.**************************/
//if classic map is needed
if (mapping_mode == 2)
   classicMap = new MapClass(2500,2000,1,0.95); //create classicMap which saves obstacle data of classic mapping method

}

/***********************************************************************************************
Controls the mapper algorithm in each step
***********************************************************************************************/
int MapperProcessor::step()
{
    int pointOfOrigin[2]={0,0}; //define point of origin for cleaning maps
    if (pMapper->mapping_flag == true)
    {
        if (mapping)// if mapping activated
        {
           int position[2]={pExecutive->current_X,pExecutive->current_Y};//get actual position out of shared memory
           fprintf(stdout,"Mapping area...\n");

           //get points from shared memory and make sensor data:
           obtainPoints(perceptionValues,pExecutive);
           fprintf(stdout,"Input Vector: \n S0_x: %d, S1_x: %d, S2_x: %d, S3_x: %d, S4_x: %d \n", perceptionValues[0][0], perceptionValues[1][0], perceptionValues[2][0], perceptionValues[3][0], perceptionValues[4][0]);
           fprintf(stdout,"S0_y: %d, S1_y: %d, S2_y: %d, S3_y: %d, S4_y: %d \n", perceptionValues[0][1], perceptionValues[1][1], perceptionValues[2][1], perceptionValues[3][1], perceptionValues[4][1]);
           //Add Points to Flex-, Mega-, and Internal Map
           Add_IntMap(perceptionValues);
           Add_FlexMap(perceptionValues);

           if (mapping_mode == 0 || mapping_mode == 2) //this part only if mapping_mode = "Dynamic Mapping"
           {
              Add_FlexMap(perceptionValues);
              //Update IntMap with Flex and Mega-Maps:
              UpdateCategories(flexMap_1); //update intMap categories with the flexMap_1 data
              UpdateCategories(flexMap_2); //update intMap categories with the flexMap_2 data
              UpdateCategories(flexMap_3); //update intMap categories with the flexMap_3 data
              UpdateCategories(megaMap_1); //update intMap categories with the megaMap_1 data
              UpdateCategories(megaMap_2); //update intMap categories with the megaMap_2 data

              //Clean categories beyond perception area out of Flex- and MegaMap
              flexMap_1->Clean_Map(position,250); //cleans all free map categories of flexMap_1 with radius 250 mm
              flexMap_2->Clean_Map(position,250); //cleans all free map categories of flexMap_2 with radius 250 mm
              flexMap_3->Clean_Map(position,250); //cleans all free map categories of flexMap_3 with radius 250 mm
              megaMap_1->Clean_Map(pointOfOrigin,0); //cleans all free map categories of megaMap_1
              megaMap_2->Clean_Map(pointOfOrigin,0); //cleans all free map categories of megaMap_2

              //Update IntMap with Flex and Mega-Maps:
              UpdateCategories(flexMap_1); //update intMap categories with the flexMap_1 data
              UpdateCategories(flexMap_2); //update intMap categories with the flexMap_2 data
              UpdateCategories(flexMap_3); //update intMap categories with the flexMap_3 data
              UpdateCategories(megaMap_1); //update intMap categories with the megaMap_1 data
              UpdateCategories(megaMap_2); //update intMap categories with the megaMap_2 data
           }
           intMap->Output(output_map.c_str(), output_filter, max_rectangle_size);//output of intMap
           if (mapping_mode == 2)
              classicMap->Output(1, output_filter, max_rectangle_size);//output of classicMap


           //Save actual Internal Map into CDA
           cda.lockArea(MAPPER_AREA);//lock mapper area
           for (int i=0; i<intMap->activeNodes; i++)
           {
               for (int j=0; j<4; j++)//actualize categories
               {
                   pMapper->categories[i][j]=intMap->category[i][j];//MapClass noch ändern!! (Variablen angleichen)
               }
               //check if there are less categories than before:
               if (pMapper->activeNodes>intMap->activeNodes)   //MapClass noch ändern!! (Variablen angleichen)
               {
                   // If yes, initialize new free categories to 1
                   for (int k=pMapper->activeNodes; k < intMap->activeNodes; k++)
                   {
                       for (int j=0; j<4; j++)//actualize categories
                       {
                           pMapper->categories[k][j]=1;//MapClass noch ändern!! (Variablen angleichen)
                       }
                   }
               }
               //Set number of active nodes:
               pMapper->activeNodes=intMap->activeNodes;
           }
           cda.unlockArea(MAPPER_AREA);//unlock mapper area
        }
        //Unset mapping Flag (mapping step finished)
        cda.lockArea(MAPPER_AREA);// lock Mapper Area
         pMapper->mapping_flag = false;
        cda.unlockArea(MAPPER_AREA);// unlock Mapper Area
    }
    usleep(500);

    return 0;
}

int MapperProcessor::cleanup()
{
    //unblock acquired pointer area
    delete intMap;
    delete flexMap_1;
    delete flexMap_2;
    delete flexMap_3;
    delete megaMap_1;
    delete megaMap_2;
    return 0;
}

/**********************************************************************************************
Get sensor values from shared memory and calculates real world coordinates
**********************************************************************************************/
void MapperProcessor::obtainPoints(int realWorldPoints[5][2], ExecutiveData *pExecutive)
{
    // First interprets every sensor readings in order to assign a coordinate relative to the robot (in mm).
    int sensorDistance[5];
    int robotPoints[2];
    long int temp[2];
    float exec_adc[5];
    int sensorNumber;
    int posX;
    int posY;
    int orientation;

    // Saves a copy for the current position, orientation and sensor values out of the CDA.
    cda.lockArea(EXECUTIVE_AREA);
    posX=pExecutive->current_X;
    posY=pExecutive->current_Y;
    orientation=pExecutive->current_orientation;
    for (int i=0;i<5;i++)
    {
        exec_adc[i]=pExecutive->exec_adc[i];
    }
    fprintf(stdout,"0: %f, 1: %f, 2: %f, 3: %f, 4: %f \n", exec_adc[0], exec_adc[1], exec_adc[2], exec_adc[3], exec_adc[4]);
    cda.unlockArea(EXECUTIVE_AREA);
    //***************

    for (int i = 0; i < 5; i++)
    {
        // Checks if coherent information is associated to each sensor
        // 0.6 is an empirically obtained parameter
        if (exec_adc[i]<0.6)
        {
            sensorDistance[i]=adaptMeasurement(exec_adc[i]);
            sensorNumber=i;
        }
        else
            sensorNumber=-1;


        switch (sensorNumber)
        {
            // If no obstacle detected, coordinates=-1
        case -1:
            robotPoints[0]=-1;
            robotPoints[1]=-1;
            break;
            // Adapts the measure depending on the sensor used, which is denoted by case number.
        case 0:
            robotPoints[0]=108;
            robotPoints[1]=(sensorDistance[i]+73);
            break;
        case 1:
            robotPoints[0]=108+8+lround((sensorDistance[i]+75)*cos(49*pi/180));
            robotPoints[1]=lround((sensorDistance[i]+75)*sin(49*pi/180));
            break;
        case 2:
            robotPoints[0]=108+8+lround(sensorDistance[i]+76);
            robotPoints[1]=lround((sensorDistance[i]+76)*sin(1*pi/180));
            break;
        case 3:
            robotPoints[0]=108+8+lround((sensorDistance[i]+75)*cos(45*pi/180));
            robotPoints[1]=-1*lround((sensorDistance[i]+75)*sin(45*pi/180));
            break;
        case 4:
            robotPoints[0]=108;
            robotPoints[1]=-1*(sensorDistance[i]+73);
            break;
        }


    // Once the relative coordinates had been established, the axis mut be rotated
    // so they fit their real position on the map according to the robot's orientation.
    // robotPoints[i][0] -> X coordinate of i-sensor measurement.
    // robotPoints[i][1] -> Y coordinate of i-sensor measurement.
        if(robotPoints[0] != -1 && robotPoints[1] != -1)
        {
            if (orientation<=180)
            {
               temp[0]=lround((robotPoints[0]*cos(-1*orientation*pi/180))+(robotPoints[1]*sin(-1*orientation*pi/180)));
               temp[1]=lround((-1*robotPoints[0]*sin(-1*orientation*pi/180))+(robotPoints[1]*cos(-1*orientation*pi/180)));
            }
           else
           {
               temp[0]=lround((robotPoints[0]*cos((360-orientation)*pi/180))+(robotPoints[1]*sin((360-orientation)*pi/180)));
               temp[1]=lround((-1*robotPoints[0]*sin((360-orientation)*pi/180))+(robotPoints[1]*cos((360-orientation)*pi/180)));
           }
            // Translocate the relative axis to the absolute reference axis.
            realWorldPoints[i][0]=posX+temp[0];
            realWorldPoints[i][1]=posY+temp[1];

            //secludes the sensors range inside the mapping room
           if(realWorldPoints[i][0] < 0)
               realWorldPoints[i][0]=0;

           else if(realWorldPoints[i][0] > wRoom)
               realWorldPoints[i][0]=wRoom;

           else if(realWorldPoints[i][1] < 0)
               realWorldPoints[i][1]=0;

           else if(realWorldPoints[i][1] > hRoom)
               realWorldPoints[i][1]=hRoom;
        }
        else
        {
           realWorldPoints[i][0] = -1; //If sensor free, set value to -1
           realWorldPoints[i][1] = -1;
        }

    }

}
/**********************************************************************************************
Takes sensor values out of array perceptionValues[5][2], uses included obstacle points for
inserting them into the internal Map for world representation.
**********************************************************************************************/
void MapperProcessor::Add_IntMap(int input[5][2])
{
    for (int i = 0; i < 5; i++) //Inserts relevant points into the map
    {
        if (input[i][0] != -1 && input[i][1] != -1) //adds the point only if it is valid
        {
           intMap->AddPoint_Map(input[i]);
           if (mapping_mode == 2)
               classicMap->AddPoint_Map(input[i]);
        }
    }
}

/*************************************************************************************************
Takes snesor values out of array perceptionValues[5][2], uses included "free data" for calculating
and adding the "free points" to the flexmap.
*************************************************************************************************/
void MapperProcessor::Add_FlexMap(int input[5][2])
{
    int insertionPoints_world[2][2]; // array with points (world coordinates) to be inserted (2 points, (x,y))
    int insertionPoints_robot[2][2]; // array with points (robot coordinates) to be inserted (    "   ,   "  )
    int robot_Position_X;
    int robot_Position_Y;
    int robot_Orientation;
    // Saves a copy for the current position, orientation and sensor values out of the CDA.
    cda.lockArea(EXECUTIVE_AREA);
    robot_Position_X=pExecutive->current_X;
    robot_Position_Y=pExecutive->current_Y;
    robot_Orientation=pExecutive->current_orientation;
    cda.unlockArea(EXECUTIVE_AREA);

    for (int i = 0; i < 5; i++) //calculates the insertion points for the FlexMap for each relevant sensor
    {
        if (input[i][0] == -1 && input[i][1] == -1) //adds the "free points" only if nothing was detected
        {
            //calculating the free points:
            switch (i)
            {
                case 0: // left sensor free
                    //Point 1:
                    insertionPoints_robot[0][0]=108;
                    insertionPoints_robot[0][1]=(iPoint_1+73);
                    //Point 2:
                    insertionPoints_robot[1][0]=108;
                    insertionPoints_robot[1][1]=(iPoint_2+73);
                    break;

                case 1: //...
                    //Point 1:
                    insertionPoints_robot[0][0]=108+8+lround((iPoint_1+75)*cos(49*pi/180));
                    insertionPoints_robot[0][1]=lround((iPoint_1+75)*sin(49*pi/180));
                    //Point 2:
                    insertionPoints_robot[1][0]=108+8+lround((iPoint_2+75)*cos(49*pi/180));
                    insertionPoints_robot[1][1]=lround((iPoint_2+75)*sin(49*pi/180));
                    break;

                case 2: // middle sensor free
                    //Point 1:
                    insertionPoints_robot[0][0]=108+8+lround(iPoint_1+76);
                    insertionPoints_robot[0][1]=lround((iPoint_1+76)*sin(1*pi/180))+20;
                    //Point 2:
                    insertionPoints_robot[1][0]=108+8+lround(iPoint_2+76);
                    insertionPoints_robot[1][1]=lround((iPoint_2+76)*sin(1*pi/180))-20;
                    break;

                case 3: //...
                    //Point 1:
                    insertionPoints_robot[0][0]=108+8+lround((iPoint_1+75)*cos(45*pi/180));
                    insertionPoints_robot[0][1]=-1*lround((iPoint_1+75)*sin(45*pi/180));
                    //Point 2:
                    insertionPoints_robot[1][0]=108+8+lround((iPoint_2+75)*cos(45*pi/180));
                    insertionPoints_robot[1][1]=-1*lround((iPoint_2+75)*sin(45*pi/180));
                    break;

                case 4: // right sensor free
                    //Point 1:
                    insertionPoints_robot[0][0]=108;
                    insertionPoints_robot[0][1]=-1*(iPoint_1+73);
                    //Point 2:
                    insertionPoints_robot[1][0]=108;
                    insertionPoints_robot[1][1]=-1*(iPoint_2+73);
                    break;
            }

            //coordinate transformation (robot coordinates -> world coordinates)
            for (int j = 0; j < 2; j++) //transformation for 2 points
            {
                if (robot_Orientation <= 180)
                {
                    insertionPoints_world[j][0]=lround(((insertionPoints_robot[j][0]*cos(-1*robot_Orientation*pi/180))+(insertionPoints_robot[j][1]*sin(-1*robot_Orientation*pi/180))) + robot_Position_X);
                    insertionPoints_world[j][1]=lround(((-1*insertionPoints_robot[j][0]*sin(-1*robot_Orientation*pi/180))+(insertionPoints_robot[j][1]*cos(-1*robot_Orientation*pi/180))) + robot_Position_Y);
                }
                else
                {
                    insertionPoints_world[j][0]=lround(((insertionPoints_robot[j][0]*cos((360-robot_Orientation)*pi/180))+(insertionPoints_robot[j][1]*sin((360-robot_Orientation)*pi/180))) + robot_Position_X);
                    insertionPoints_world[j][1]=lround(((-1*insertionPoints_robot[j][0]*sin((360-robot_Orientation)*pi/180))+(insertionPoints_robot[j][1]*cos((360-robot_Orientation)*pi/180))) + robot_Position_Y);
                }
                //check if the values lie in world range
                //x-value:
                if (insertionPoints_world[j][0] < 0)
                    insertionPoints_world[j][0] = 0;

                else if (insertionPoints_world[j][0] > wRoom)
                    insertionPoints_world[j][0] = wRoom;

                //y-value:
                if (insertionPoints_world[j][1] < 0)
                    insertionPoints_world[j][1] = 0;

                else if (insertionPoints_world[j][1] > hRoom)
                    insertionPoints_world[j][1] = hRoom;
            }

            //insert the calculated points into depending flexmap:
            switch(i)
            {
                case 0: //left sensor
                    //insert points in Megamap:
                    //sensor 0 and 1 free, 2 occupated:
                    if ((input[0][0] == -1) && (input[0][1] == -1) && (input[1][0] == -1) && (input[1][1] == -1) && (input[2][0] != -1) && (input[2][1] != -1))
                    {
                        megaMap_1->AddPoint_Map(insertionPoints_world[0]);
                        megaMap_1->AddPoint_Map(insertionPoints_world[1]);
                    }
                    //sensor 0,1 and 2 free
                    else if ((input[0][0] == -1) && (input[0][1] == -1) && (input[1][0] == -1) && (input[1][1] == -1) && (input[2][0] == -1) && (input[2][1] == -1))
                    {
                        megaMap_1->AddPoint_Map(insertionPoints_world[0]);
                        megaMap_1->AddPoint_Map(insertionPoints_world[1]);
                    }
                    //insert points in regular flexmap
                    flexMap_1->AddPoint_Map(insertionPoints_world[0]);
                    flexMap_1->AddPoint_Map(insertionPoints_world[1]);
                    break;

                case 1: //left/front sensor

                    //inseert points in Megamap
                    //sensor 0 and 1 free, 2 occupated
                    if ((input[0][0] == -1) && (input[0][1] == -1) && (input[1][0] == -1) && (input[1][1] == -1) && (input[2][0] != -1) && (input[2][1] != -1))
                    {
                        megaMap_1->AddPoint_Map(insertionPoints_world[0]);
                        megaMap_1->AddPoint_Map(insertionPoints_world[1]);
                    }
                    //sensor 0,1 and 2 free
                    else if ((input[0][0] == -1) && (input[0][1] == -1) && (input[1][0] == -1) && (input[1][1] == -1) && (input[2][0] == -1) && (input[2][1] == -1))
                    {
                        megaMap_1->AddPoint_Map(insertionPoints_world[0]);
                        megaMap_1->AddPoint_Map(insertionPoints_world[1]);
                    }
                    //sensor 2 and 1 free, 0 occupated
                    else if ((input[0][0] != -1) && (input[0][1] != -1) && (input[1][0] == -1) && (input[1][1] == -1) && (input[2][0] == -1) && (input[2][1] == -1))
                    {
                        megaMap_1->AddPoint_Map(insertionPoints_world[0]);
                        megaMap_1->AddPoint_Map(insertionPoints_world[1]);
                    }

                    //insert Points in regular flexmap
                    flexMap_2->AddPoint_Map(insertionPoints_world[0]);
                    flexMap_2->AddPoint_Map(insertionPoints_world[1]);
                    break;

                case 2: //front sensor

                    //sensor 0,1 and 2 free
                    if ((input[0][0] == -1) && (input[0][1] == -1) && (input[1][0] == -1) && (input[1][1] == -1) && (input[2][0] == -1) && (input[2][1] == -1))
                    {
                        megaMap_1->AddPoint_Map(insertionPoints_world[0]);
                        megaMap_1->AddPoint_Map(insertionPoints_world[1]);
                    }
                    //sensor 2 and 1 free, 0 occupated
                    else if ((input[0][0] != -1) && (input[0][1] != -1) && (input[1][0] == -1) && (input[1][1] == -1) && (input[2][0] == -1) && (input[2][1] == -1))
                    {
                        megaMap_1->AddPoint_Map(insertionPoints_world[0]);
                        megaMap_1->AddPoint_Map(insertionPoints_world[1]);
                    }

                    //sensor 2 and 3 free, 4 occupated
                    if ((input[2][0] == -1) && (input[2][1] == -1) && (input[3][0] == -1) && (input[3][1] == -1) && (input[4][0] != -1) && (input[4][1] != -1))
                    {
                        megaMap_2->AddPoint_Map(insertionPoints_world[0]);
                        megaMap_2->AddPoint_Map(insertionPoints_world[1]);
                    }
                    //sensor 2,3 and 4 free
                    else if ((input[2][0] == -1) && (input[2][1] == -1) && (input[3][0] == -1) && (input[3][1] == -1) && (input[4][0] == -1) && (input[4][1] == -1))
                    {
                        megaMap_2->AddPoint_Map(insertionPoints_world[0]);
                        megaMap_2->AddPoint_Map(insertionPoints_world[1]);
                    }

                    //insert points in regular flexmap
                    flexMap_3->AddPoint_Map(insertionPoints_world[0]);
                    flexMap_3->AddPoint_Map(insertionPoints_world[1]);
                    break;

                case 3: //right/front sensor
                    //sensor 3 and 4 free, 2 occupated
                    if ((input[2][0] != -1) && (input[2][1] != -1) && (input[3][0] == -1) && (input[3][1] == -1) && (input[4][0] == -1) && (input[4][1] == -1))
                    {
                        megaMap_2->AddPoint_Map(insertionPoints_world[0]);
                        megaMap_2->AddPoint_Map(insertionPoints_world[1]);
                    }
                    //sensor 2 and 3 free, 4 occupated
                    else if ((input[2][0] == -1) && (input[2][1] == -1) && (input[3][0] == -1) && (input[3][1] == -1) && (input[4][0] != -1) && (input[4][1] != -1))
                    {
                        megaMap_2->AddPoint_Map(insertionPoints_world[0]);
                        megaMap_2->AddPoint_Map(insertionPoints_world[1]);
                    }
                    //sensor 2,3 and 4 free
                    else if ((input[2][0] == -1) && (input[2][1] == -1) && (input[3][0] == -1) && (input[3][1] == -1) && (input[4][0] == -1) && (input[4][1] == -1))
                    {
                        megaMap_2->AddPoint_Map(insertionPoints_world[0]);
                        megaMap_2->AddPoint_Map(insertionPoints_world[1]);
                    }

                    //add points to regular flexmap
                    flexMap_2->AddPoint_Map(insertionPoints_world[0]);
                    flexMap_2->AddPoint_Map(insertionPoints_world[1]);
                    break;

                case 4: //right sensor
                //sensor 3 and 4 free, 2 occupated
                    if ((input[2][0] != -1) && (input[2][1] != -1) && (input[3][0] == -1) && (input[3][1] == -1) && (input[4][0] == -1) && (input[4][1] == -1))
                    {
                        megaMap_2->AddPoint_Map(insertionPoints_world[0]);
                        megaMap_2->AddPoint_Map(insertionPoints_world[1]);
                    }
                    //sensor 2,3 and 4 free
                    else if ((input[2][0] == -1) && (input[2][1] == -1) && (input[3][0] == -1) && (input[3][1] == -1) && (input[4][0] == -1) && (input[4][1] == -1))
                    {
                        megaMap_2->AddPoint_Map(insertionPoints_world[0]);
                        megaMap_2->AddPoint_Map(insertionPoints_world[1]);
                    }

                    //add points to regular flexmap
                    flexMap_1->AddPoint_Map(insertionPoints_world[0]);
                    flexMap_1->AddPoint_Map(insertionPoints_world[1]);
                    break;
            }
        }
    }
}

/*****************************************************************************************************
This function compares the flexMap (which represents the unocupated area) and the intMap (the real
world internal map including occupated area) and updates the intMap categories.
Possible Update operations are: -Erase
                                -Cut
                                -Divide (creates new category)
*****************************************************************************************************/
void MapperProcessor::UpdateCategories(MapClass *updateMap)
//Compare all Categories with each other:
{
    //Compare all Categories with each other:
    for (int flexCounter = 0; flexCounter < updateMap->activeNodes; flexCounter++)
    {
        //load the actual flexMap category values:
        float x_1 = updateMap->category[flexCounter][0];
        float x_2 = updateMap->category[flexCounter][1];
        float y_1_c = updateMap->category[flexCounter][2];
        float y_2_c = updateMap->category[flexCounter][3];

        for (int intCounter = 0; intCounter < intMap->activeNodes; intCounter++)
        {
            //load the actual intMap category values:
            float u_1 = intMap->category[intCounter][0];
            float u_2 = intMap->category[intCounter][1];
            float v_1_c = intMap->category[intCounter][2];
            float v_2_c = intMap->category[intCounter][3];

            //cout << "flexcount: " << flexCounter << ", Active nodes updateMap: " << updateMap->activeNodes << ", intcount: " << intCounter << ", Active nodes internal Map: " << intMap->activeNodes << endl;
            //check if category has to be erased:
            if (((x_1 <= u_1) && (x_2 <= u_2) && (y_1_c <= v_1_c) && (y_2_c <= v_2_c)))
            {
                fprintf(stdout,"delete \n");
                intMap->activeNodes--; //erase last node
                //overwrite category with last category
                for (int i = 0 ; i < 4; i++)
                {
                    intMap->category[intCounter][i] = intMap->category[intMap->activeNodes][i];
                    intMap->category[intMap->activeNodes][i] = 1; //reinitialize last node
                }

                intCounter--; //check actual (new copied) category by new

            }

            //check if category has to be cut
            //possibility 1 (cut left side):
            else if ((x_1 <= u_1) && (x_2 <= u_2) && ((1 - u_1) > y_1_c) && (y_1_c > v_1_c) && (y_2_c <= v_2_c))
            {
                fprintf(stdout,"Cut left side \n");
                intMap->category[intCounter][0] = 1 - y_1_c; //cut left side
            }
            //possibility 2 (cut lower side):
            else if ((x_1 <= u_1) && (x_2 <= u_2) && (y_1_c <= v_1_c) && ((1 - u_2) > y_2_c) && (y_2_c > v_2_c))
            {
                fprintf(stdout,"cut lower side \n");
                intMap->category[intCounter][1] = 1 - y_2_c; //cut lower side
            }
            //possibility 3 (cut right side):
            else if (((1 - v_1_c) > x_1) && (x_1 > u_1) && (x_2 <= u_2) && (y_1_c <= v_1_c) && (y_2_c <= v_2_c))
            {
                fprintf(stdout,"cut right side");
                intMap->category[intCounter][2] = 1 - x_1; //cut right side
            }
            //possibility 4 (cut upper side):

            else if ((x_1 <= u_1) && ((1 - v_2_c) > x_2) && (x_2 > u_2) && (y_1_c <= v_1_c) && (y_2_c <= v_2_c))
            {
                fprintf(stdout,"cut upper side \n");
                intMap->category[intCounter][3] = 1 - x_2; //cut upper side
            }

            //check if the category has to be divided
            //possibility 1:
            else if ((x_1 > u_1) && (x_2 < u_2) && (y_1_c > v_1_c) && (y_2_c < v_2_c))
            {
                fprintf(stdout,"divide vertically \n");
                //use new category (divide vertically):
                intMap->category[intMap->activeNodes][0] = 1 - y_1_c; //new u_1
                intMap->category[intMap->activeNodes][1] = u_2;       //new u_2
                intMap->category[intMap->activeNodes][2] = v_1_c;     //new v_1 complement
                intMap->category[intMap->activeNodes][3] = v_2_c;     //new v_2 complement

                //Update old Category
                intMap->category[intCounter][2] = 1 - x_1; //new v_1 complement (v_2 complement unchanged)
                intMap->activeNodes++; //add new node to categories
            }
            //possibility 2 (divide horizontally):
            else if ((x_1 < u_1) && (x_2 > u_2) && (y_1_c < v_1_c) && (y_2_c > v_2_c))
            {
                fprintf(stdout,"divide horizontally \n");
                //use new category:
                intMap->category[intMap->activeNodes][0] = u_1;       //new u_1
                intMap->category[intMap->activeNodes][1] = 1 - y_2_c; //new u_2
                intMap->category[intMap->activeNodes][2] = v_1_c;     //new v_1 complement
                intMap->category[intMap->activeNodes][3] = v_2_c;     //new v_2 complement

                //Update old Category
                intMap->category[intCounter][3] = 1 - x_2; //new v_2 complement (v_1 complement unchanged)
                intMap->activeNodes++; //add new node to categories
            }
        }
    }
}

/******************************************************************************************
converts the sensor value into distance
******************************************************************************************/
int MapperProcessor::adaptMeasurement(float measurement)
{
    int result=lround(((36*measurement)+4)*10);
    return result;
}




/*************************************************
Main
*************************************************/
int main()
{
    fprintf(stdout, "\nIRMA2: Mapper Process \n");
    MapperProcessor map;
    map.execute();

    return 0;
}
