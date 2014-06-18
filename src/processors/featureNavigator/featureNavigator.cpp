/************************************************************************************
 * FILE: featureNavigator.cpp, v.1.0.0, 09/15/2009
 * Author: Renato Agurto.
 *
 * IRMA-2: Feature Navigation Process
 *
 * Managment of the feature Database an communication with the CMUcam
 * for the IRMA-2 project
 *
 * DISCLAIMER: No liability is assumed by the author for any use made
 * of this program.
 * DISTRIBUTION: Any use may be made of this program, as long as the
 * clear acknowledgment is made to the author in code and runtime executables
************************************************************************************/
/******************************************************************************
*
*   Copyright (c) 2009 Tomas Arredondo,Renato Agurto   <tomas.arredondo@usm.cl>
*
*   This program is open-source software; you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License version 3 as
*   published by the Free Software Foundation.
*
*   See README and LICENSE for more details.
*
******************************************************************************/
#include "featureNavigator.h"



int FNProcessor::init()
{

    pFeatureNav = (FeatureNavData *)cda.getMemPtr(FEATURE_NAV_AREA);
    pExecutive  = (ExecutiveData *)cda.getMemPtr(EXECUTIVE_AREA);
    pMonitor  = (MonitorData *)cda.getMemPtr(MONITOR_AREA);
    pMapper     = (MapperData *) cda.getMemPtr(MAPPER_AREA);

    pCtrl = &(pFeatureNav->ctrl); // Todos los procesadores tienen que definir pCtrl
    sem_area = FEATURE_NAV_AREA; // Todos los procesadores tienen que definir sem_area
    pCtrl->loop = run_cmd;

    int i, j,activ;
    float ** categories;


    cda.lockArea(MAPPER_AREA);
      activ = pMapper->activeNodes;
      int map_wight = pMapper->room_width;
      int map_height = pMapper->room_height;
    cda.unlockArea(MAPPER_AREA);

    categories = new float * [activ];
    for (i=0;i<activ;i++)
        categories[i] = new float [4];

    cda.lockArea(MAPPER_AREA);
      for (i=0;i<activ;i++)
         for (j=0;j<4;j++)
            categories[i][j] =  pMapper->categories[i][j];
    cda.unlockArea(MAPPER_AREA);

    neighborhood.set(map_wight, map_height, activ, categories);


    feature_num = 0;



    /************************ START: Config File reading.**************************/

    ConfigFile config( "config/featureNavigator.conf" );

    string default_mission_file = "missions";

    config.readInto( op_mode, "op_mode", 0 );
    config.readInto( mission_file, "mission_file", default_mission_file);


    if (op_mode == DO_NOTHING )
        printf("Feature Navigator: Sleeping\n");
    else if (op_mode == RECOGNITION)
        printf("Feature Navigator: Recognition Mode\n");
    else if (op_mode = MISSION)
    {
        printf("Feature Navigator: Mission Mode\n");

        mFile = fopen (mission_file.c_str(),"r");
        if (mFile!=NULL)
        {
            printf("MISSION  : Mission File opened\n");
            fseek(mFile, 0, SEEK_SET);
        }
        else
        {
            printf("MISSION  : Error by opening the Mission File (mission)\n");
            no_more_missions = true;
        }
    }
    /************************END: Config File reading.****************************/

    if (op_mode != DO_NOTHING)
    {
        //Check if there is an already saved DataDase
        if (I2GIS.checkPreviousDB())
        {
            //Request the coordinates from previous saved features
            int ans;
            ans = I2GIS.command("SELECT x,y from features");
            if (ans != -1)
            {
                valueStruct ** previousXY;
                int r,c;

                //Get te answer from the last SQL request.
                //previos XY : Array of coordinates (x,y)
                //r: numer of rows
                //c: number of columns
                previousXY = I2GIS.getSelectAnswer(&r,&c);
                //printf("row = %d - col = %d\n", r, c);

                //Update the neighborhood
                for (i=0;i<r;i++)
                {
                    neighborhood.newPoint(previousXY[i][0].ival,previousXY[i][1].ival);
                    feature_num++;
                    delete [] previousXY[i];
                }

                printf("%d features were loaded.\n",feature_num);

                delete [] previousXY;
                //I2GIS.command("INSERT INTO features VALUES (1,2,3,7,5,6)");
            }
        }
        else
        {
            //Create the table for the features
            I2GIS.command("CREATE TABLE features ( feature_id INT, x INT, y INT, neighborhood INT, form INT, color INT)");
        }
    }
    no_more_missions = false;
    state = 1;

    cda.lockArea(FEATURE_NAV_AREA);
    pFeatureNav->fn_move_ready_flag = false;
    pFeatureNav->fn_move_active_flag = false;
    pFeatureNav->fn_route_obsolete_flag = false;

    pFeatureNav->fn_feature_list_ready_flag = false;
    pFeatureNav->fn_nFeatures = 0;

    pFeatureNav->fn_mission_obsolete_flag = true;

    cda.unlockArea(FEATURE_NAV_AREA);


    return 0;
}

int FNProcessor::cleanup()
{
    return 0;
}

int FNProcessor::step()
{
   readFlags();

    if(op_mode == DO_NOTHING)
        usleep(100000);
    else if (op_mode == RECOGNITION)
        doRecognition();
    else if (op_mode == MISSION)
        doMissions();

    // Go to sleep so robot can execute move
    usleep(1000);

    return 0;
}

void FNProcessor::readFlags()
{
   cda.lockArea(FEATURE_NAV_AREA);

   local_move_ready_flag         = pFeatureNav->fn_move_ready_flag;
   local_move_active_flag        = pFeatureNav->fn_move_active_flag;
   local_route_obsolete_flag     = pFeatureNav->fn_route_obsolete_flag;
   local_feature_list_ready_flag = pFeatureNav->fn_feature_list_ready_flag;
   local_mission_obsolete_flag   = pFeatureNav->fn_mission_obsolete_flag;

   cda.unlockArea(FEATURE_NAV_AREA);
}

void FNProcessor::doRecognition()
{
   //printf("Entre al doRecognition!!\n");

    int oldState;

    bool CLR_isNotForced;

    cda.lockArea(MONITOR_AREA);
    CLR_isNotForced = pMonitor-> monitor_hysteresis;
    cda.unlockArea(MONITOR_AREA);



    if (local_route_obsolete_flag == true)
    {
        if (state != 1)
        {
            cmuCam.cWrite(ESC);
            state = 1;
        }

        if (local_move_ready_flag == true)
        {
            cda.lockArea(FEATURE_NAV_AREA);
            pFeatureNav->fn_move_ready_flag = false;
            cda.unlockArea(FEATURE_NAV_AREA);
        }

        if (local_move_active_flag== true)
        {
            cda.lockArea(FEATURE_NAV_AREA);
            pFeatureNav->fn_move_active_flag = false;
            cda.unlockArea(FEATURE_NAV_AREA);
        }



        if (CLR_isNotForced)
        {
            cda.lockArea(FEATURE_NAV_AREA);
            pFeatureNav->fn_route_obsolete_flag = false;
            pFeatureNav->fn_move_ready_flag     = false;
            pFeatureNav->fn_move_active_flag    = false;
            cda.unlockArea(FEATURE_NAV_AREA);
        }

    }
    else
    {
        //printf("  un paso normal\n");
        if (local_move_ready_flag == false && local_route_obsolete_flag == false)
        {
            bool go =true;
            //printf("    no hay pasos pendientes (a.k.a paso leido)...\n");

            while (go)
            {
                //printf("      camera step\n");
                oldState = state;
                state = cmuCam.stateFunction(oldState);


                if (state == 2)
                {
                    go = false;
                    if (oldState == 1 || oldState == 3)
                     featureChecked = false;
                }

                if (state == 3 && local_move_active_flag == true) // Other Processors can Navigate
                {
                    //printf("         no veo nada\n");
                    local_move_active_flag == false;

                    cda.lockArea(FEATURE_NAV_AREA);
                    pFeatureNav->fn_move_active_flag = false;
                    pFeatureNav->fn_move_ready_flag = false;//por si las dudas
                    cda.unlockArea(FEATURE_NAV_AREA);

                    go = false; //No es necesario,pero puede que se deba dar tiempo al robot para moverse antes de
                    //consultar a la camara nuevamente en busca de features
                }

                if (state == 4 && local_move_active_flag == false) // Camera saw a feature and FeatureNav wants to move.
                {
                    if (!featureChecked)
                     {
                        featureIsInDB = isFeatureAlreadyInDB();
                        featureChecked = true;
                        if (featureIsInDB)
                        {
                           printf("Feature is already in Database\n");
                        }
                        else
                        {
                           printf("New Feature Found\n");
                        }
                     }



                    if(!featureIsInDB)
                    {
                       local_move_active_flag = true;

                       cda.lockArea(FEATURE_NAV_AREA);
                       pFeatureNav->fn_move_active_flag = true;
                       cda.unlockArea(FEATURE_NAV_AREA);
                    }
                    else
                    {
                       state = 3;
                    }

                }

                if (oldState == 5 || oldState == 8)
                {
                    //printf("         dejando un paso para que el executive lo lea...\n");
                    cda.lockArea(FEATURE_NAV_AREA);
                    pFeatureNav->fn_move_ready_flag = true;
                    pFeatureNav->fn_route_obsolete_flag = false;
                    pFeatureNav->fn_speed = cmuCam.getSpeed();
                    pFeatureNav->fn_steering = cmuCam.getSteering();
                    cda.unlockArea(FEATURE_NAV_AREA);

                    go = false;
                }

                if (oldState == 11)
                {


                    //printf("         feature encontrada. ejecutando INSERT\n");
                    int r_posX, r_posY, r_ori, r_nh;
                    const float PI = 3.14159265;


                    cda.lockArea(FEATURE_NAV_AREA);
                    r_posX = pExecutive->current_X;
                    r_posY = pExecutive->current_Y;
                    r_ori = pExecutive->current_orientation;
                    cda.unlockArea(FEATURE_NAV_AREA);

                    r_posX = r_posX + (int)round(130*cos(r_ori*PI/180));
                    r_posY = r_posY + (int)round(130*sin(r_ori*PI/180));

                    r_nh = neighborhood.newPoint(r_posX,r_posY);
                    feature_num++;

                    char r_command[100];
                    sprintf(r_command,"INSERT INTO features VALUES (%d, %d, %d, %d, %d, %d)", feature_num, r_posX, r_posY, r_nh, cmuCam.getRecognizedFeature(), cmuCam.getFeatureColor());
                    I2GIS.command(r_command);

                }

            }
        }
        //else
        //{
        //if (move_ready_flag == true)
        //printf("mov_ready_flag = %d; route_obsolete_flag = %d\n", move_ready_flag, route_obsolete_flag);
        //}
    }
}

void FNProcessor::doMissions()
{
    cda.lockArea(FEATURE_NAV_AREA);
    pFeatureNav->fn_move_active_flag = false; //For Security
    cda.unlockArea(FEATURE_NAV_AREA);

    if (local_mission_obsolete_flag == true)
    {

        if (feof(mFile))
        {
            op_mode = DO_NOTHING;
            fclose (mFile);
        }
        else
        {
            char * mission;
            char n_command[100];
            char noCondition[] = "NO_CONDITION";
            char aux[3];

            int i,r;

            size_t lsize = 0;

            getline(&mission, &lsize, mFile);

            for (i=0;i<strlen(mission)-4;i++)
            {
                r = strncmp( mission+i, "here", 4 );
                if (r == 0)
                {
                    int n_posX, n_posY, n_nh;

                    cda.lockArea(EXECUTIVE_AREA);
                    n_posX = pExecutive->current_X;
                    n_posY = pExecutive->current_Y;
                    cda.unlockArea(EXECUTIVE_AREA);

                    n_nh = neighborhood.newPoint(n_posX, n_posY, RETURN_NEAREST_NH);

                    sprintf(aux,"%d",n_nh);
                    mission[i] = aux[0];
                    mission[i+1] = ' ';
                    mission[i+2] = ' ';
                    mission[i+3] = ' ';

                    break;
                }
            }

            if (strncmp(mission,noCondition,12)==0)
                sprintf(n_command,"SELECT x,y from features");
            else
            {
                //sprintf(n_command,"SELECT x,y from features WHERE form = 5");
                if (mission[strlen(mission)-1] == 10) //if las charcater is "return"
                  mission[strlen(mission)-1] = 32; //replace it with a space, beacuse a "return" character may cause an error.
                sprintf(n_command,"SELECT x,y from features WHERE %s", mission);
            }
            I2GIS.command(n_command);

            valueStruct ** posList;

            int nFeature_in_list, nColumns_in_list;
            posList = I2GIS.getSelectAnswer(&nFeature_in_list,&nColumns_in_list);

            cda.lockArea(FEATURE_NAV_AREA);
            for (i=0;i<nFeature_in_list;i++)
            {
                pFeatureNav->fn_featureArray[i][0] = posList[i][0].ival;
                pFeatureNav->fn_featureArray[i][1] = posList[i][1].ival;
            }
            pFeatureNav->fn_nFeatures = nFeature_in_list;
            pFeatureNav->fn_feature_list_ready_flag = true;
            pFeatureNav->fn_mission_obsolete_flag = false;
            cda.unlockArea(FEATURE_NAV_AREA);
            //mission_obsolete_flag = false;


            for (i=0;i<nFeature_in_list;i++)
            {
                delete [] posList[i];
            }
            delete [] posList;
        }
    }
}

bool FNProcessor::isFeatureAlreadyInDB()
{
   int ans;
   ans = I2GIS.command("SELECT x,y from features");
   bool already_in_DB = false;
   if (ans != -1)
   {
      valueStruct ** XY;
      int r,c;
      int i;

      int r_posX, r_posY, r_ori, f_posX, f_posY;
      int dif_x, dif_y, dist;
      const float PI = 3.14159265;


      //Ask for robot position and orientation
      cda.lockArea(EXECUTIVE_AREA);
      r_posX = pExecutive->current_X;
      r_posY = pExecutive->current_Y;
      r_ori = pExecutive->current_orientation;
      cda.unlockArea(EXECUTIVE_AREA);

      printf("Robot position:  X = %d, Y = %d, Orientation = %d\n", r_posX, r_posY, r_ori);

      //Estimate feaute position
      f_posX = r_posX + (int)round(390*cos(r_ori*PI/180));
      f_posY = r_posY + (int)round(390*sin(r_ori*PI/180));

      printf("Estimated feature position:  X = %d, Y = %d\n", f_posX, f_posY);

       //Get te answer from the last SQL request.
       //XY : Array of coordinates (x,y)
       //r: numer of rows
       //c: number of columns
       XY = I2GIS.getSelectAnswer(&r,&c);
       //printf("row = %d - col = %d\n", r, c);

       //Update the neighborhood
       for (i=0;i<r;i++)
       {
           dif_x = XY[i][0].ival - f_posX;
           dif_y = XY[i][1].ival - f_posY;

           dist = (int)round(sqrt(pow(dif_x,2)+pow(dif_x,2)));

           if (dist < 300)
           {
            already_in_DB = true;
            printf("Position of Feature in DB : x = %d ; Y = %d\n", XY[i][0].ival, XY[i][1].ival);
            printf("X difference = %d ; Y difference = %d ; distance = %d\n", dif_x, dif_y, dist);
           }


           delete [] XY[i];
       }


       delete [] XY;
       //I2GIS.command("INSERT INTO features VALUES (1,2,3,7,5,6)");
   }

   return already_in_DB;
}

int main()
{
    fprintf(stdout, "\n***********************************\n");
    fprintf(stdout, "* IRMA2:Feature Navigator Process *\n");
    fprintf(stdout, "***********************************\n\n");
    FNProcessor fn;

    fn.execute();

    return 0;
}

