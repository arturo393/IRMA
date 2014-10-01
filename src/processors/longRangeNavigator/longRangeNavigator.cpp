//*******************************************************************
// Copyright (C) 2009 by IRMA Project
// Contact: tomas.arredondo@usm.cl
//
// Author: Nicolas Navarro
// Contact: nicolas.navarro.guerrero@gmail.com or nicolas.navarro@gmx.de
// Date: December 2009
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
#include "longRangeNavigator.h"


//===================================================================

// Patricio: Note: if appears the following error: "*** glibc detected *** double free or corruption (!prev): 0x00024018"
// use "export MALLOC_CHECK_=0" instead of update glibc

//*******************************************************************
// LONG RANGE NAV PROCESSOR: LRNProcessor Class Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
//-------------------------------------------------------------------
int LRNProcessor::init()
{
    // Initialization: pointers to the Share Memory
    pCDALongNav = (LongNavData *)cda.getMemPtr(LONG_NAV_AREA);
    pCDAExecutive = (ExecutiveData *)cda.getMemPtr(EXECUTIVE_AREA);
    pCDAMonitor = (MonitorData *)cda.getMemPtr(MONITOR_AREA);
    pCDALaser = (LaserData *)cda.getMemPtr(LASER_AREA);

    /* IRMA 3 no necessary
     * pCDAMapper = (MapperData *)cda.getMemPtr(MAPPER_AREA);
     * pCDAMap = (MapperData *)cda.getMemPtr(MAPPER_AREA); not use map
     * pCDAFeatureNav = (FeatureNavData *)cda.getMemPtr(FEATURE_NAV_AREA); not use feature nav
     */

    // Initialization: class ProcessorInterface Variables
    pCtrl = &(pCDALongNav->ctrl);
    sem_area = LONG_NAV_AREA;
    pCtrl->loop = run_cmd;


    // Initialization: Variables of the share memory  the long range navigator
    cda.lockArea(LONG_NAV_AREA);
    pCDALongNav->lrn_move_ready_flag = false;
    pCDALongNav->lrn_route_obsolete_flag = true;
    pCDALongNav->lrn_mission_accomplished = false;
    cda.unlockArea(LONG_NAV_AREA);

    /* No feature navigator
     * new_features_flag = false;
     * 	cda.lockArea(FEATURE_NAV_AREA);
     * 	pCDAFeatureNav->fn_feature_list_ready_flag = false;
     * 	cda.unlockArea(FEATURE_NAV_AREA);
     */

    // config parameters
    if( !Read_Configuration_File() )
        printf("Parameters updated from configuration file\n");
    else
        printf("Error Updating Parameters from file, default parameters will be used instead\n");

    /* Path planner config o_routes
     * GA settings.
     * Virtual Executive start and goal coord.
     * Set experimientation rom
     * Set fitness motivations
     */
    Configure_path_planner();

    /* Final path o_final
     * Set robot diameter
     * Set final steps number (genes)
     * Set start and home coord.
     */

    //Configure_final_path();

    /* Get mapper width and height 
     * Get obstacle map from mapper
     * Set obstacle map to InternalMap
     * Print ap
     */

//    Update_Slam_Map();

    /* Setting number of missions
     * Set mission's coord o_final
     * Calculate mission distance between start and mission coord
     */
    //	Update_Start_position();
    //	Update_Missions_list();

    visited_feature_nr = 0;
    route_nr = 0;
    current_route_action = 0;

    return(0);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
int LRNProcessor::cleanup()
{
    int total_moves = N_FORWARD+N_TURN_RIGHT_1+N_TURN_LEFT_1+N_TURN_RIGHT_3+N_TURN_LEFT_3+N_TURN_RIGHT_2+N_TURN_LEFT_2+N_REVERSE+N_FREEZE;

    fprintf(fp_moves, "\n================= TOTAL MOVEMENTS ================\n");
    fprintf(fp_moves, "FORWARD :\t\t%d\t(%2.2f)\n", N_FORWARD, ((double) N_FORWARD/(double) total_moves*100) );
    fprintf(fp_moves, "TURN_RIGHT_1 :\t\t%d\t(%2.2f)\n", N_TURN_RIGHT_1, ((double) N_TURN_RIGHT_1/(double) total_moves*100) );
    fprintf(fp_moves, "TURN_LEFT_1 :\t\t%d\t(%2.2f)\n", N_TURN_LEFT_1, ((double) N_TURN_LEFT_1/(double) total_moves*100) );
    fprintf(fp_moves, "TURN_RIGHT_3 :\t\t%d\t(%2.2f)\n", N_TURN_RIGHT_3, ((double) N_TURN_RIGHT_3/(double) total_moves*100) );
    fprintf(fp_moves, "TURN_LEFT_3 :\t\t%d\t(%2.2f)\n", N_TURN_LEFT_3, ((double) N_TURN_LEFT_3/(double) total_moves*100) );
    fprintf(fp_moves, "TURN_RIGHT_2 :\t\t%d\t(%2.2f)\n", N_TURN_RIGHT_2, ((double) N_TURN_RIGHT_2/(double) total_moves*100) );
    fprintf(fp_moves, "TURN_LEFT_2 :\t\t%d\t(%2.2f)\n", N_TURN_LEFT_2, ((double) N_TURN_LEFT_2/(double) total_moves*100) );
    fprintf(fp_moves, "REVERSE :\t\t%d\t(%2.2f)\n", N_REVERSE, ((double) N_REVERSE/(double) total_moves*100) );
    fprintf(fp_moves, "FREEZE :\t\t%d\t(%2.2f)\n", N_FREEZE, ((double) N_FREEZE/(double) total_moves*100) );
    fprintf(fp_moves, "TOTAL :\t%d\n", total_moves);

    if(fp_moves != NULL)
        fclose(fp_moves);

    fprintf(fp_fitness, "\n=============================== AVERAGE FITNESS ==============================\n");
    fprintf(fp_fitness, "Curiosity: %.3f\t Energy: %.3f\t Homing: %.3f\t Missions: %.3f\n",
            (SUM_FITNESS_C/(double) N_FITNESS), (SUM_FITNESS_E/(double) N_FITNESS), (SUM_FITNESS_H/(double) N_FITNESS), (SUM_FITNESS_M/(double) N_FITNESS) );

    if(fp_fitness != NULL)
        fclose(fp_fitness);

    fprintf(stdout, "\n********************************************************\n");
    fprintf(stdout, "LONG RANGE NAVIGATOR: EXPERIMENT WAS COMPLETED\n");
    fprintf(stdout, "********************************************************\n\n");

    exit(0);
    //return(0);

}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
int LRNProcessor::step(){       

    Read_Flags();  // Read Flags from the Common Data Area
    if( (monitor_current_nav == LRN) && (is_route_obsolete) ){
        /* IRMA 3 not neccesary
         * if( experimentation_room == FART_ROOM )         Update_Internal_Map();
         * Check posibility of implementation
         * if (experimentation_room = SLAM_ROOM) Update_Internal_Map();
         */	 
        //Update_Step_Lenght();	

        Update_Slam_Map();
        // Update the Start coordinates with the current one
        Update_Start_position();
        // Update Missions
        //if( (m_missions > 0) && (new_features_flag) )   Update_Missions_list();
        // for each missions, it calculates the distantance to the mission with the current starting point
        if( m_missions > 0 )   Update_Missions_list();

        // Then Compute route and Tell everybody when it's ready
        Compute_new_path();
        current_route_action = 0;
    }  // end if

    // Once I have a new route, I deliver the respective commands to the motors
    // If there is no moves available in the CDA then deliver a new one
    Read_Flags();  // Read Flags from the Common Data Area

    if( (!is_move_ready) && (!is_route_obsolete) && (monitor_current_nav == LRN) )
    {
        /* registrar el paso del robot y hacer un promedio y 
         * ajustar el valor del step*/
        Action_To_Motor(o_final_route.get_gen(current_route_action));
        current_route_action++;
    }

    return(0);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
void LRNProcessor::Read_Flags(void)
{
    // Read CDA: LONG RANGE NAV AREA
    cda.lockArea(LONG_NAV_AREA);
    is_move_ready = pCDALongNav->lrn_move_ready_flag;
    is_route_obsolete = pCDALongNav->lrn_route_obsolete_flag;
    mission_accomplished_flag = pCDALongNav->lrn_mission_accomplished;
    ctrl_current_loop = pCDALongNav->ctrl.loop;  //Patricio: check if current loop is stop
    cda.unlockArea(LONG_NAV_AREA);

    cda.lockArea(MONITOR_AREA);
    monitor_current_nav = pCDAMonitor->monitor_current_nav;
    cda.unlockArea(MONITOR_AREA);

    /*
       cda.lockArea(FEATURE_NAV_AREA);
       new_features_flag = pCDAFeatureNav->fn_feature_list_ready_flag;
       cda.unlockArea(FEATURE_NAV_AREA);
       */
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
/* c_pop o_routes class perform all GA operations
 * FitnessFuntion o_fftiness evaluates each organism fitness function with o_virtualMotion data.
 * c_vExecutive o_virtualMotion emulates the robots movement in the internal map representation class InternalMap
 * 
 */
int LRNProcessor::Configure_path_planner(void)
{
    o_routes.setGenesNumber(actions_nr);
    o_routes.setPopulationSize(population_size);
    o_routes.setGenerationsNumber(generations_nr);
    o_routes.setCrossPointsNumber(cross_points_nr);
    o_routes.setSelectionMethod(selection_method);

    /* VirtualMotion startcoordinates are updated in the LRNProccess::step()
     * o_routes.o_ffitness.o_virtualMotion.Set_StartCoordinates(start_coord);
     */
    o_routes.o_ffitness.o_virtualMotion.Set_GoalCoordinates(home_coord);    // Home

    /* IRMA 3 not necessary
       o_routes.o_ffitness.o_virtualMotion.set_selected_room(experimentation_room);
       */

    o_routes.o_ffitness.a_motivations[0] = m_curiosity;  
    o_routes.o_ffitness.a_motivations[1] = m_energy;
    o_routes.o_ffitness.a_motivations[2] = m_homing;     
    o_routes.o_ffitness.a_motivations[3] = m_missions;

}


//-------------------------------------------------------------------
int LRNProcessor::Configure_final_path(void)
{

    /* cambiar esta parte a IRMA3*/
    /* All this values  are sets in the o_final_route consturctor
       o_final_route.Clean_All();


       o_final_route.set_robot_diameter(o_routes.o_ffitness.o_virtualMotion.get_robot_diameter());
       o_final_route.set_robot_diameter_sensors(o_routes.o_ffitness.o_virtualMotion.get_robot_diameter_sensors());
       o_final_route.set_genes_nr(actions_nr);

       o_final_route.Set_StartCoordinates(start_coord);
       o_final_route.set_home_coord(home_coord);
       */

    /* IRMA3 not necessary
     * if( experimentation_room == FART_ROOM ) Update_Internal_Map();      
     */

    /* the missions are updated in LRNProcess::step()
     * if( ((m_missions > 0) && (new_features_flag)) || (mission_op_mode == TEST) )   Update_Missions_list();
     */
    return(0);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
void LRNProcessor::Compute_new_path(void)
{
    // CALCULATE THE TIME USED TO COMPUTE THE PATH
    struct timeb tp;     ftime(&tp);

    // Set Elite_Individual fitness value to zero
    o_final_route.Clean_All();

    fprintf(stdout, "LRN start = (%d,%d,%3d°) -- ", start_coord[1], start_coord[2], start_coord[0]);
    fprintf(stdout, "LRN home = (%d,%d,%d°)", home_coord[1], home_coord[2],home_coord[0]);
    //···································································
    // COMPUTE ROUTES WITH MISSIONS
    if( (m_missions > 0) && (missions_nr > 0) ){

        fprintf(stdout, " -- LRN actual mission = (%d,%d,%d°)\n", \
                mission_coord[visited_feature_nr][1], mission_coord[visited_feature_nr][2],mission_coord[visited_feature_nr][0]);
        /* Set mission coord to VirtualExecutive
         * Calculate de distance to current position
         * Mark the cells in the InternalMap as missions */
        o_routes.o_ffitness.o_virtualMotion.Set_Mission(mission_coord[visited_feature_nr]);

        /* Set crossing point at the same distance
         * Set population random genes, fitness -1 and ranking -1  on every organism */               
        o_routes.Reinit();
        o_routes.Evolve(VERBOSE_MEDIUM);

        fprintf(stdout,"NUMBER OF ACTIONS BEFORE CLEAN : %d\n",o_routes.get_e_useful_steps_nr());
        o_routes.CleanGenes();
        fprintf(stdout,"NUMBER OF ACTIONS AFTER  CLEAN : %d\n\n",o_routes.get_e_useful_steps_nr());
        /* Set distance from the start_coord to the misssion_coord */
        Update_missions_extra_data(visited_feature_nr);
        /* Set the EliteIndividual updated values*/
        Update_elite_data(visited_feature_nr);

        fprintf(stdout, "Looking for feature: %2d\n", visited_feature_nr);
        if( mission_coord[visited_feature_nr][3] < TOLERANCE_GOAL_DISTANCE )
        {
            fprintf(stdout, "Long Range Navigator: Feature: %2d found\n", visited_feature_nr);
            visited_feature_nr++;
            if( visited_feature_nr < missions_nr )
                Update_missions_extra_data(visited_feature_nr);
        }

        if( visited_feature_nr >= missions_nr )
        {
            visited_feature_nr = 0;
            mission_accomplished_flag = true;
        }
        is_route_obsolete = false;
        //is_move_ready = false;
    }  // End Compute route with mission
    //···································································
    // COMPUTE ROUTES WITH HOMING
    else if( m_homing > 0 )
    {
        o_routes.Reinit();
        o_routes.Evolve(VERBOSE_MEDIUM);

        printf("NUMBER OF ACTIONS BEFORE CLEAN : %d\n",o_routes.get_e_useful_steps_nr());
        o_routes.CleanGenes();

        // Update Elite Organism Output Data
        Update_elite_data(0);  // missions_nr = 0;

        printf("NUMBER OF ACTIONS AFTER  CLEAN : %d\n\n",o_routes.get_e_useful_steps_nr());

        Update_Start_position();
        int _home_coord[3];
        o_final_route.get_home_coord(_home_coord);
        int _distance = o_routes.o_ffitness.o_virtualMotion.Calculate_Distance(start_coord, _home_coord);

        if( (_distance < TOLERANCE_GOAL_DISTANCE) && (route_nr > 0) )
        {
            mission_accomplished_flag = true;
        }
        is_route_obsolete = false;
        //is_move_ready = false;
    }  // End Compute route with homing
    //···································································
    // COMPUTE ROUTES WITHOUT HOMING OR MISSIONS
    else
    {
        o_routes.Reinit();
        o_routes.Evolve(VERBOSE_MEDIUM);

        // Update Elite Organism Output Data
        printf("NUMBER OF ACTIONS BEFORE CLEAN : %d\n",o_routes.get_e_useful_steps_nr());
        o_routes.CleanGenes();

        Update_elite_data(0);  // missions_nr = 0;

        printf("NUMBER OF ACTIONS AFTER  CLEAN : %d\n\n",o_routes.get_e_useful_steps_nr());
        //      mission_accomplished_flag = true;
        is_route_obsolete = false;
        //is_move_ready = false;
    }  // End Compute route without homing or missions


    //···································································
    if( (is_route_obsolete == false) && (is_move_ready == false) ){
    /* Compute next position of the elite individual */
        Update_elite_phenotype();

        // Print best solution
        Print_output_data(VERBOSE_MEDIUM);
        Print_elite_output_data();
        // CALCULATE THE TIME USED TO COMPUTE THE PATH
        Compute_time_difference(tp.time, tp.millitm);
        //usleep(5000000);
        int _temp_pos[3];
        //usleep(5000000);
        cda.lockArea(LONG_NAV_AREA);
        //usleep(5000000);
        pCDALongNav->fitness_curiosity = o_final_route.a_fitness[0];
        pCDALongNav->fitness_energy = o_final_route.a_fitness[1];
        pCDALongNav->fitness_homing = o_final_route.a_fitness[2];
        pCDALongNav->fitness_missions = o_final_route.a_fitness[3];
        pCDALongNav->route_lenght = o_final_route.get_elite_genes_nr();
        for(int _i = 0; _i < pCDALongNav->route_lenght; _i++)
        {
            o_final_route.get_current_ideal_coord(_i, pCDALongNav->route[_i]);
            //            o_final_route.get_current_ideal_coord(_i, _temp_pos);
            //            pCDALongNav->route[_i][0] = _temp_pos[0];
            //            pCDALongNav->route[_i][1] = _temp_pos[1];
            //            pCDALongNav->route[_i][2] = _temp_pos[2];
        }
        pCDALongNav->lrn_route_obsolete_flag = is_route_obsolete;
        //pCDALongNav->lrn_move_ready_flag = is_move_ready;
        cda.unlockArea(LONG_NAV_AREA);

        /*
           if( mission_accomplished_flag )
           {
           fprintf(stdout, "Long Range Navigator: Mission was successfully Accomplished\n");
           fprintf(stdout, "***********************************************************\n\n");

           cda.lockArea(LONG_NAV_AREA);
           pCDALongNav->lrn_mission_accomplished = true;
           cda.unlockArea(LONG_NAV_AREA);
           }*/

        route_nr++;
    }
    else
    {
        fprintf(stdout, "\n*******************************************************************\n");
        fprintf(stdout, "LONG RANGE NAVIGATOR: I DON'T HAVE ALL THE NECESSARY INFORMATION!!!\n");
        fprintf(stdout, "*******************************************************************\n\n");
    }
}

void LRNProcessor::Action_To_Motor(char _action)
{
    //Renato
    bool go = true;
    //END Renato
    int _speed_percent = 0, _movement = 0;

    // No routes, no move
    if (o_final_route.get_elite_genes_nr() == 0)
        _action = FREEZE;

    if( _action == FORWARD ){
        _speed_percent = 100;
        _movement = 9;
    } else if( _action == TURN_RIGHT ){
        _speed_percent = 100;
        _movement = 1;  
    } else if( _action == TURN_LEFT ){
        _speed_percent = 100;
        _movement = 2;  
    } else if( _action == REVERSE ){
        _speed_percent = 100;
        _movement = 3;  
    } else if( _action == FREEZE ){ 
        _speed_percent = 0;
        _movement = 9;  
    }  

    fprintf(stdout, "R%2d - %d/%d\t", route_nr, current_route_action+1, o_final_route.get_elite_genes_nr());
    if( _action == FORWARD ){
        CURRENT_BATTERY-=POWER_FORWARD;
        fprintf(stdout, "IRMA-III: LRN - FORWARD - \t\tBATTERY : %f\r", CURRENT_BATTERY);
        fprintf(fp_moves, "R%2d | 0 : FORWARD\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
    }else if( _action == TURN_RIGHT ) {
        CURRENT_BATTERY-=POWER_RIGHT;
        fprintf(stdout, "IRMA-III: LRN - TURN_RIGHT - \t\tBATTERY : %f\r", CURRENT_BATTERY);
        fprintf(fp_moves, "R%2d | 1 : TURN_RIGHT\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
    } else if( _action == TURN_LEFT ){
        CURRENT_BATTERY-=POWER_LEFT;
        fprintf(stdout, "IRMA-III: LRN - TURN_LEFT - \t\tBATTERY : %f\r", CURRENT_BATTERY);
        fprintf(fp_moves, "R%2d | 2 : TURN_LEFT\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
    }else if( _action == REVERSE ){
        CURRENT_BATTERY-=POWER_REVERSE;
        fprintf(stdout, "IRMA-III: LRN - REVERSE - \t\tBATTERY : %f\r", CURRENT_BATTERY);
        fprintf(fp_moves, "R%2d | 7 : REVERSE\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
    } else if( _action == FREEZE ){
        CURRENT_BATTERY-=POWER_FREEZE;
        fprintf(stdout, "IRMA-III: LRN - FREEZE\t\tBATTERY : %f\r", CURRENT_BATTERY);
        fprintf(fp_moves, "R%2d | 8 : FREEZE\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
    }

    // Deliver the magnitudes for the next step
    Deliver_Motor_Commands(_speed_percent, _movement);

    // TOO MUCH ERROR - THE THEORETICAL AND THE REAL COORDINATES ARE VERY DIFFERENT
    //···································································

    Update_Start_position();
    int _ideal_coord[3];
    o_final_route.get_current_ideal_coord(current_route_action , _ideal_coord);
    int _distance = o_routes.o_ffitness.o_virtualMotion.Calculate_Distance(start_coord, _ideal_coord);
    

    if( _distance > MAX_DISTANCE_ERROR ){
        Update_missions_extra_data(visited_feature_nr);
        if( mission_coord[visited_feature_nr][3] < TOLERANCE_GOAL_DISTANCE ) {
            fprintf(stdout, "Long Range Navigator: Feature: %2d found\n", visited_feature_nr);
            visited_feature_nr++;
            if( visited_feature_nr < missions_nr )
                Update_missions_extra_data(visited_feature_nr);
     }else{
            fprintf(stdout, "\n**********************************************************************\n");
            fprintf(stdout, "LONG RANGE NAVIGATOR: THE ERROR BETWEEN THE THEORETICAL AND REAL\n");
            fprintf(stdout, "LONG RANGE NAVIGATOR: POSITION IS TOO BIG - A NEW ROUTE WILL BE COMPUTED\n");
            fprintf(stdout, "**********************************************************************\n\n");
            is_route_obsolete = true;
            is_move_ready = false;
            cda.lockArea(LONG_NAV_AREA);
            pCDALongNav->lrn_route_obsolete_flag = is_route_obsolete;
            pCDALongNav->lrn_move_ready_flag = is_move_ready;
            cda.unlockArea(LONG_NAV_AREA);
            current_route_action = 0;
        }

    } else if( current_route_action+1 >= o_final_route.get_elite_genes_nr() ){ /* Route was successfully finished */
        fprintf(stdout, "\nLong Range Navigator: Route was successfully finished\n");
        fprintf(stdout, "*****************************************************\n\n");
        is_route_obsolete = true;
        is_move_ready = false;
        cda.lockArea(LONG_NAV_AREA);
        pCDALongNav->lrn_route_obsolete_flag = is_route_obsolete;
        pCDALongNav->lrn_move_ready_flag = is_move_ready;
        cda.unlockArea(LONG_NAV_AREA);
        current_route_action = 0;

        Update_missions_extra_data(visited_feature_nr);
        if( mission_coord[visited_feature_nr][3] < TOLERANCE_GOAL_DISTANCE ){
            fprintf(stdout, "Long Range Navigator: Feature: %2d found\n", visited_feature_nr);
            visited_feature_nr++;
            if( visited_feature_nr < missions_nr )
                Update_missions_extra_data(visited_feature_nr);
        }

    }

    if( (visited_feature_nr >= missions_nr) && (m_missions > 0) ){
        visited_feature_nr = 0;
        mission_accomplished_flag = true;
    }


    if( mission_accomplished_flag ) {
        fprintf(stdout, "Long Range Navigator: Mission was successfully Accomplished\n");
        fprintf(stdout, "***********************************************************\n\n");

        cda.lockArea(LONG_NAV_AREA);
        pCDALongNav->lrn_mission_accomplished = true;
        cda.unlockArea(LONG_NAV_AREA);
    }

    fprintf(stdout, "\n");
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
void LRNProcessor::Convert_Action_To_Motor_Cmd(char _action)
{
    //Renato
    bool go = true;
    //END Renato
    double _speed = 0.5, _steer = 0.5;
    int _speed_percent = 0, _movement = 0;

    if (o_final_route.get_elite_genes_nr() == 0)
        _action = FORWARD;
    /*
       if( _action == FORWARD )            {  _speed = 0.8;  _steer = 0.5;  }  // FORWARD
       else if( _action == TURN_RIGHT_1 )  {  _speed = 0.8;  _steer = 0.7;  }  // TURN_RIGHT_1
       else if( _action == TURN_LEFT_1 )   {  _speed = 0.5;  _steer = 0.2;  }  // TURN_LEFT_1
       else if( _action == TURN_RIGHT_3 )  {  _speed = 0.8;  _steer = 0.7;  }  // TURN_RIGHT_3
       else if( _action == TURN_LEFT_3 )   {  _speed = 0.5;  _steer = 0.3;  }  // TURN_LEFT_3
       else if( _action == TURN_RIGHT_2 )  {  _speed = 0.8;  _steer = 0.7;  }  // TURN_RIGHT_2
       else if( _action == TURN_LEFT_2 )   {  _speed = 0.5;  _steer = 0.3;  }  // TURN_LEFT_2
       else if( _action == REVERSE )       {  _speed = 0.2;  _steer = 0.5;  }  // REVERSE
       else if( _action == FREEZE )        {  _speed = 0.5;  _steer = 0.5;  }  // FREEZE
       */      
    if( _action == FORWARD )              {  _speed_percent = 100;  _movement = 9;  }  // FORWARD
    else if( _action == TURN_RIGHT )      {  _speed_percent = 100;  _movement = 1;  }  // TURN_RIGHT_1
    else if( _action == TURN_LEFT )      {  _speed_percent = 100;  _movement = 2;  }  // TURN_LEFT_1
    else if( _action == REVERSE )        {  _speed_percent = 100;  _movement = 3;  }  // REVERSE
    else if( _action == FREEZE )         {  _speed_percent = 0;   _movement = 9;  }  // FREEZE


    //	while( (monitor_current_nav == LRN) && (!is_route_obsolete) && (SUB_FORWARD < TOTAL_SUB_FORWARD)
    //			&& (SUB_TURN_RIGHT_1 < TOTAL_SUB_TURN_RIGHT_1) && (SUB_TURN_LEFT_1 < TOTAL_SUB_TURN_LEFT_1)
    //			&& (SUB_TURN_RIGHT_3 < TOTAL_SUB_TURN_RIGHT_3) && (SUB_TURN_LEFT_3 < TOTAL_SUB_TURN_LEFT_3)
    //			&& (SUB_TURN_RIGHT_2 < TOTAL_SUB_TURN_RIGHT_2) && (SUB_TURN_LEFT_2 < TOTAL_SUB_TURN_LEFT_2) )
    //	{        
    while( (monitor_current_nav == LRN) && (!is_route_obsolete) && (SUB_FORWARD < TOTAL_SUB_FORWARD)
            && (SUB_TURN_RIGHT < TOTAL_SUB_TURN_RIGHT) && (SUB_TURN_LEFT < TOTAL_SUB_TURN_LEFT) )
    {
        if( !is_move_ready )
        {
            fprintf(stdout, "R%2d - %d/%d\t", route_nr, current_route_action+1, o_final_route.get_elite_genes_nr());
            if( _action == FORWARD )
            {
                SUB_FORWARD++;
                N_FORWARD++;
                CURRENT_BATTERY-=POWER_FORWARD;
                fprintf(stdout, "IRMA-II: LRN - FORWARD - %d/%d\t\tBATTERY : %f\r", SUB_FORWARD, TOTAL_SUB_FORWARD, CURRENT_BATTERY);
                fprintf(fp_moves, "R%2d | 0 : FORWARD\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
            }else if( _action == TURN_RIGHT )
            {
                SUB_TURN_RIGHT++;
                N_TURN_RIGHT++;
                CURRENT_BATTERY-=POWER_RIGHT;
                fprintf(stdout, "IRMA-II: LRN - TURN_RIGHT - %d/%d\tBATTERY : %f\r", SUB_TURN_RIGHT_1, TOTAL_SUB_TURN_RIGHT_1, CURRENT_BATTERY);
                fprintf(fp_moves, "R%2d | 1 : TURN_RIGHT\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
            }
            else if( _action == TURN_LEFT )
            {
                SUB_TURN_LEFT++;
                N_TURN_LEFT++;
                CURRENT_BATTERY-=POWER_LEFT;
                fprintf(stdout, "IRMA-II: LRN - TURN_LEFT - %d/%d\tBATTERY : %f\r", SUB_TURN_LEFT_1, TOTAL_SUB_TURN_LEFT_1, CURRENT_BATTERY);
                fprintf(fp_moves, "R%2d | 2 : TURN_LEFT\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
            }
            //			else if( _action == TURN_RIGHT_1 )
            //			{
            //				SUB_TURN_RIGHT_1++;
            //				N_TURN_RIGHT_1++;
            //				CURRENT_BATTERY-=POWER_RIGHT_1;
            //				fprintf(stdout, "IRMA-II: LRN - TURN_RIGHT_1 - %d/%d\tBATTERY : %f\r", SUB_TURN_RIGHT_1, TOTAL_SUB_TURN_RIGHT_1, CURRENT_BATTERY);
            //				fprintf(fp_moves, "R%2d | 1 : TURN_RIGHT_1\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
            //			}
            //			else if( _action == TURN_LEFT_1 )
            //			{
            //				SUB_TURN_LEFT_1++;
            //				N_TURN_LEFT_1++;
            //				CURRENT_BATTERY-=POWER_LEFT_1;
            //				fprintf(stdout, "IRMA-II: LRN - TURN_LEFT_1 - %d/%d\tBATTERY : %f\r", SUB_TURN_LEFT_1, TOTAL_SUB_TURN_LEFT_1, CURRENT_BATTERY);
            //				fprintf(fp_moves, "R%2d | 2 : TURN_LEFT_1\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
            //			}
            //			else if( _action == TURN_RIGHT_3 )
            //			{
            //				SUB_TURN_RIGHT_3++;
            //				N_TURN_RIGHT_3++;
            //				CURRENT_BATTERY-=POWER_RIGHT_3;
            //				fprintf(stdout, "IRMA-II: LRN - TURN_RIGHT_3 - %d/%d\tBATTERY : %f\r", SUB_TURN_RIGHT_3, TOTAL_SUB_TURN_RIGHT_3, CURRENT_BATTERY);
            //				fprintf(fp_moves, "R%2d | 3 : TURN_RIGHT_3\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
            //			}
            //			else if( _action == TURN_LEFT_3 )
            //			{
            //				SUB_TURN_LEFT_3++;
            //				N_TURN_LEFT_3++;
            //				CURRENT_BATTERY-=POWER_LEFT_3;
            //				fprintf(stdout, "IRMA-II: LRN - TURN_LEFT_3 - %d/%d\tBATTERY : %f\r", SUB_TURN_LEFT_3, TOTAL_SUB_TURN_LEFT_3, CURRENT_BATTERY);
            //				fprintf(fp_moves, "R%2d | 4 : TURN_LEFT_3\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
            //			}
            //			else if( _action == TURN_RIGHT_2 )
            //			{
            //				SUB_TURN_RIGHT_2++;
            //				N_TURN_RIGHT_2++;
            //				CURRENT_BATTERY-=POWER_RIGHT_2;
            //				fprintf(stdout, "IRMA-II: LRN - TURN_RIGHT_2 - %d/%d\tBATTERY : %f\r", SUB_TURN_RIGHT_2, TOTAL_SUB_TURN_RIGHT_2, CURRENT_BATTERY);
            //				fprintf(fp_moves, "R%2d | 5 : TURN_RIGHT_2\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
            //			}
            //			else if( _action == TURN_LEFT_2 )
            //			{
            //				SUB_TURN_LEFT_2++;
            //				N_TURN_LEFT_2++;
            //				CURRENT_BATTERY-=POWER_LEFT_2;
            //				fprintf(stdout, "IRMA-II: LRN - TURN_LEFT_2 - %d/%d\tBATTERY : %f\r", SUB_TURN_LEFT_2, TOTAL_SUB_TURN_LEFT_2, CURRENT_BATTERY);
            //				fprintf(fp_moves, "R%2d | 6 : TURN_LEFT_2\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
            //			}
         /*   else if( _action == REVERSE )
            {	SUB_FORWARD++;
                N_REVERSE++;
                CURRENT_BATTERY-=POWER_REVERSE;
                fprintf(stdout, "IRMA-II: LRN - REVERSE - %d/%d\t\tBATTERY : %f\r", SUB_FORWARD, TOTAL_SUB_FORWARD, CURRENT_BATTERY);
                fprintf(fp_moves, "R%2d | 7 : REVERSE\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
            }*/
            else if( _action == FREEZE )
            {
                SUB_FORWARD = TOTAL_SUB_FORWARD;
                N_FREEZE++;
                CURRENT_BATTERY-=POWER_FREEZE;
                fprintf(stdout, "IRMA-II: LRN - FREEZE\t\tBATTERY : %f\r", CURRENT_BATTERY);
                fprintf(fp_moves, "R%2d | 8 : FREEZE\tBATTERY : %f\n", route_nr, CURRENT_BATTERY);
                if(freeze_times++ > 3){

                };

            }

            //         for(unsigned int i = 0; i < 100; i++)
            //            usleep(1000);

            // Deliver the magnitudes for the next step
            Deliver_Motor_Commands(_speed_percent, _movement);
            go = true;
            while (go == true && ctrl_current_loop != stop_cmd) //Patricio: if current loop is stop must end
            {
                usleep(5000);
                cda.lockArea(LONG_NAV_AREA);
                go = pCDALongNav->lrn_move_ready_flag;
                ctrl_current_loop = pCDALongNav->ctrl.loop;
                cda.unlockArea(LONG_NAV_AREA);
            }
        }  // end if
        Read_Flags();  // Read Flags from the Common Data Area

        if(ctrl_current_loop == stop_cmd) //Patricio: if current loop is stop must end
            cleanup();
    }  // End while


    // TOO MUCH ERROR - THE THEORETICAL AND THE REAL COORDINATES ARE VERY DIFFERENT
    //···································································

    Update_Start_position();
    int _ideal_coord[3];
    o_final_route.get_current_ideal_coord(current_route_action , _ideal_coord);
    int _distance = o_routes.o_ffitness.o_virtualMotion.Calculate_Distance(start_coord, _ideal_coord);
    if( _distance > MAX_DISTANCE_ERROR )
    {
        Update_missions_extra_data(visited_feature_nr);
        if( mission_coord[visited_feature_nr][3] < TOLERANCE_GOAL_DISTANCE )
        {
            fprintf(stdout, "Long Range Navigator: Feature: %2d found\n", visited_feature_nr);
            visited_feature_nr++;
            if( visited_feature_nr < missions_nr )
                Update_missions_extra_data(visited_feature_nr);
        }
        else{
            fprintf(stdout, "\n**********************************************************************\n");
            fprintf(stdout, "LONG RANGE NAVIGATOR: THE ERROR BETWEEN THE THEORETICAL AND REAL\n");
            fprintf(stdout, "LONG RANGE NAVIGATOR: POSITION IS TOO BIG - A NEW ROUTE WILL BE COMPUTED\n");
            fprintf(stdout, "**********************************************************************\n\n");
            is_route_obsolete = true;
            is_move_ready = false;
            cda.lockArea(LONG_NAV_AREA);
            pCDALongNav->lrn_route_obsolete_flag = is_route_obsolete;
            //pCDALongNav->lrn_move_ready_flag = is_move_ready;
            cda.unlockArea(LONG_NAV_AREA);
            current_route_action = 0;
        }

    }

    // ROUTE WAS SUCCESSFULLY FINISHED
    //···································································
    else  if( current_route_action+1 >= o_final_route.get_elite_genes_nr() )
    {
        fprintf(stdout, "\nLong Range Navigator: Route was successfully finished\n");
        fprintf(stdout, "*****************************************************\n\n");
        is_route_obsolete = true;
        is_move_ready = false;
        cda.lockArea(LONG_NAV_AREA);
        pCDALongNav->lrn_route_obsolete_flag = is_route_obsolete;
        //pCDALongNav->lrn_move_ready_flag = is_move_ready;
        cda.unlockArea(LONG_NAV_AREA);
        current_route_action = 0;

        Update_missions_extra_data(visited_feature_nr);
        if( mission_coord[visited_feature_nr][3] < TOLERANCE_GOAL_DISTANCE )
        {
            fprintf(stdout, "Long Range Navigator: Feature: %2d found\n", visited_feature_nr);
            visited_feature_nr++;
            if( visited_feature_nr < missions_nr )
                Update_missions_extra_data(visited_feature_nr);
        }

    }

    if( (visited_feature_nr >= missions_nr) && (m_missions > 0) )
    {
        visited_feature_nr = 0;
        mission_accomplished_flag = true;
    }


    if( mission_accomplished_flag )
    {
        fprintf(stdout, "Long Range Navigator: Mission was successfully Accomplished\n");
        fprintf(stdout, "***********************************************************\n\n");

        cda.lockArea(LONG_NAV_AREA);
        pCDALongNav->lrn_mission_accomplished = true;
        cda.unlockArea(LONG_NAV_AREA);
    }

    //···································································
    fprintf(stdout, "\n");
    SUB_FORWARD = 0;
    SUB_TURN_RIGHT_1 = 0;
    SUB_TURN_LEFT_1 = 0;
    SUB_TURN_RIGHT_3 = 0;
    SUB_TURN_LEFT_3 = 0;
    SUB_TURN_RIGHT_2 = 0;
    SUB_TURN_LEFT_2 = 0;
    SUB_TURN_RIGHT= 0;
    SUB_TURN_LEFT = 0;
    //···································································
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------
void LRNProcessor::Deliver_Motor_Commands(const double _speed, const double _steer)
{
    // Deliver the magnitudes for the next step
    cda.lockArea(LONG_NAV_AREA);
    pCDALongNav->lrn_speed = _speed;
    pCDALongNav->lrn_steering = _steer;
    // Inform that a new move is ready
    pCDALongNav->lrn_move_ready_flag = true;
    cda.unlockArea(LONG_NAV_AREA);
}

void LRNProcessor::Deliver_Motor_Commands(const int _speed_percent, const int _movement)
{
    // Deliver the magnitudes for the next step
    cda.lockArea(LONG_NAV_AREA);
    pCDALongNav->lrn_speed_percent = _speed_percent;
    pCDALongNav->lrn_movement = _movement;
    // Inform that a new move is ready
    pCDALongNav->lrn_move_ready_flag = true;
    cda.unlockArea(LONG_NAV_AREA);
}

//-------------------------------------------------------------------
void LRNProcessor::Update_missions_extra_data(const int _feature_index)
{
    int _distance ;
    Update_Start_position(); /* maybe this is unnecessary */
    _distance = o_routes.o_ffitness.o_virtualMotion.Calculate_Distance(start_coord, mission_coord[_feature_index]);
    mission_coord[_feature_index][3] = _distance;
    mission_coord[_feature_index][4] = actions_nr;    // LRN Global actions nr
    mission_coord[_feature_index][5] = start_coord[0];
    mission_coord[_feature_index][6] = start_coord[1];
    mission_coord[_feature_index][7] = start_coord[2];

}

//------------------------------------------------------------------
void LRNProcessor::Update_elite_data(const int _feature_index)
{
    int _goal_step_nr = 0;
    int _min_distance = 0;

    double temp_fulfillment[4];
    int _ideal_final_pos[3];
    _gen *GENOTYPE;

    GENOTYPE = new _gen[o_routes.getGenesNumber()];

    // Read Partial Result
    _goal_step_nr = o_routes.get_e_useful_steps_nr();
    _min_distance = o_routes.get_e_goal_distance();
    //   _fitness_e = o_routes.get_e_fitness();

    o_routes.get_e_final_position(_ideal_final_pos);

    o_routes.get_elite_genetype(GENOTYPE);
    o_routes.get_e_fulfillment_prod(temp_fulfillment);

    // FOR DEBUG USE - BEGIN
    if( _goal_step_nr > actions_nr ) fprintf(stdout, "Long Range Navigator: %d/%d\n", _goal_step_nr, actions_nr);
    // FOR DEBUG USE - END
    o_final_route.set_genes(0, _goal_step_nr, GENOTYPE);
    o_final_route.set_fitness(temp_fulfillment);
    o_final_route.set_final_coord(_ideal_final_pos);
    // Update Elite Organism
    if( m_missions > 0.0 )
    {
        o_final_route.set_missions_closer_dist(_feature_index, _min_distance);
        o_final_route.set_missions_steps_needed(_feature_index, _goal_step_nr);
        o_final_route.set_missions_closer_pos(_feature_index, _ideal_final_pos);
    }

    delete [] GENOTYPE;
}

//-------------------------------------------------------------------
void LRNProcessor::Update_elite_phenotype(void)
{
    // Complete Elite
    int _temp_coord[3];
    o_final_route.get_start_coord(_temp_coord);
    int _nr_gen = o_final_route.get_elite_genes_nr();  // Can be shorter than genes_nr

    for(int _igen = 0; _igen < _nr_gen; _igen++)
    {
        o_routes.o_ffitness.o_virtualMotion.ComputeNextPosition(_temp_coord, o_final_route.get_gen(_igen));
        o_final_route.set_path_step(_temp_coord);
    }
}


int LRNProcessor::Update_Slam_Map(void)
{
    int _width;
    int _heigh;
    int _aux = 0;
    int _ocount = 0;     // obstacle percentage
    int _opercent = 0.1; // obstacle percent
    int _onr;            // number of obstacles
    int auxY ;           // Y size of the new map cell
    int auxX ;           // X size of the new map cell
    int _point;          // cell value: obstacle, free or unknow
    int _xc,_yc;         // new x,y coord 
    char MAP_FILE[50];   // ascii map

    int _mapMeshSize = o_routes.o_ffitness.getMapMeshSize();
    _onr= _mapMeshSize*_mapMeshSize*0.1; // taking 20% of the cell area as obstacle 

    cda.lockArea(LASER_AREA);
     _width = pCDALaser->img_ancho;
     _heigh = pCDALaser->img_alto;
    cda.unlockArea(LASER_AREA);

    o_routes.o_ffitness.Set_Map_Dimensions(_width,_heigh);

    /* divide the slam map by _mapMeshSize to generate a lower resolution map */
    for (int _y = 0 ; _y <= _heigh-_mapMeshSize ; _y += _mapMeshSize){
        for(int _x = 0 ; _x <= _width-_mapMeshSize; _x += _mapMeshSize){
            auxY = _mapMeshSize+_y; auxX = _mapMeshSize+_x; 
            /* Searching obstacles in the _mapMeshSize cell */
            for (int _yc =_y ; _yc < auxY ;_yc++){
                for(int _xc =_x ; _xc < auxX ; _xc++){
                    cda.lockArea(LASER_AREA);
                    _point = pCDALaser->map[_xc][_yc];
                    cda.unlockArea(LASER_AREA);
                    /* count the obstacles nr in the cell */
                    if(_point == OBSTACLE) 
                        _ocount++;
                    /* set new map cell as obstacle */
                    if (_ocount >= _onr){
                        _ocount = 0;
                        o_routes.o_ffitness.o_virtualMotion.o_MAP.setcCellObstacle(_xc/_mapMeshSize,_yc/_mapMeshSize);
                        _yc = auxY;
                        _xc = auxX;
                    }
                }
            }
        }
    }

    snprintf(MAP_FILE, sizeof(MAP_FILE),  "./lrn_loaded_ascii_map%d", route_nr);
    o_routes.o_ffitness.o_virtualMotion.PrintMAPtoFile(MAP_FILE);

}

void LRNProcessor::Update_Internal_Map(void)
{
    int obstacles_nr = 0;
    int width, height;

    if( experimentation_room == FART_ROOM )
    {

        // take data from Mapper
        cda.lockArea(MAPPER_AREA);

        width = pCDAMapper->room_width;
        height = pCDAMapper->room_height;
        obstacles_nr = pCDAMapper->activeNodes;
        // Make dynamic array - and init internal map
        // make a new farmap with the new categories
        o_routes.o_ffitness.Init_Fart_MAP(obstacles_nr, width, height);
       // o_routes.o_ffitness.o_virtualMotion.set_selected_room(FART_ROOM);

        // Fill dynamic array
        for(int _cat = 0; _cat < obstacles_nr; _cat++)
        {
            o_routes.o_ffitness.FART_MAP[_cat][0] = pCDAMapper->categories[_cat][0];
            o_routes.o_ffitness.FART_MAP[_cat][1] = pCDAMapper->categories[_cat][1];
            o_routes.o_ffitness.FART_MAP[_cat][2] = pCDAMapper->categories[_cat][2];
            o_routes.o_ffitness.FART_MAP[_cat][3] = pCDAMapper->categories[_cat][3];
        }
        cda.unlockArea(MAPPER_AREA);



        // Update Internal Map Function Fitness with categories info
        o_routes.o_ffitness.Set_FART_MAP();
        // Update map in ELite
        double temp_cat[4];
        o_final_route.init_fart_map(obstacles_nr);
        for(int _i = 0; _i < obstacles_nr; _i++)
        {
            temp_cat[0] = o_routes.o_ffitness.FART_MAP[_i][0];
            temp_cat[1] = o_routes.o_ffitness.FART_MAP[_i][1];
            temp_cat[2] = o_routes.o_ffitness.FART_MAP[_i][2];
            temp_cat[3] = o_routes.o_ffitness.FART_MAP[_i][3];
            o_final_route.set_fart_map(_i, temp_cat);
        }
        char MAP_FILE[50];
        snprintf(MAP_FILE, sizeof(MAP_FILE),  "./lrn_loaded_ascii_map_%d", route_nr);
        o_routes.o_ffitness.o_virtualMotion.PrintMAPtoFile(MAP_FILE);
    }
    else
    {
        if( experimentation_room == H_ROOM )
        {
            double temp_cat[4];
            o_final_route.init_fart_map(CATEGORIES_H_ROOM);
            for(int _cat = 0; _cat < CATEGORIES_H_ROOM; _cat++)
            {
                temp_cat[0] = FART_H_ROOM[_cat][0];
                temp_cat[1] = FART_H_ROOM[_cat][1];
                temp_cat[2] = FART_H_ROOM[_cat][2];
                temp_cat[3] = FART_H_ROOM[_cat][3];
                o_final_route.set_fart_map(_cat, temp_cat);
            }
            o_routes.o_ffitness.o_virtualMotion.set_selected_room(H_ROOM);
        }
        else if( experimentation_room == L_ROOM )
        {
            double temp_cat[4];
            o_final_route.init_fart_map(CATEGORIES_L_ROOM);
            for(int _cat = 0; _cat < CATEGORIES_L_ROOM; _cat++)
            {
                temp_cat[0] = FART_L_ROOM[_cat][0];
                temp_cat[1] = FART_L_ROOM[_cat][1];
                temp_cat[2] = FART_L_ROOM[_cat][2];
                temp_cat[3] = FART_L_ROOM[_cat][3];
                o_final_route.set_fart_map(_cat, temp_cat);
            }
            o_routes.o_ffitness.o_virtualMotion.set_selected_room(L_ROOM);
        }
        else if( experimentation_room == O_ROOM )
        {
            double temp_cat[4];
            o_final_route.init_fart_map(CATEGORIES_O_ROOM);
            for(int _cat = 0; _cat < CATEGORIES_O_ROOM; _cat++)
            {
                temp_cat[0] = FART_O_ROOM[_cat][0];
                temp_cat[1] = FART_O_ROOM[_cat][1];
                temp_cat[2] = FART_O_ROOM[_cat][2];
                temp_cat[3] = FART_O_ROOM[_cat][3];
                o_final_route.set_fart_map(_cat, temp_cat);
            }
            o_routes.o_ffitness.o_virtualMotion.set_selected_room(O_ROOM);
        }
        else if( experimentation_room == S_ROOM )
        {
            double temp_cat[4];
            o_final_route.init_fart_map(CATEGORIES_S_ROOM);
            for(int _cat = 0; _cat < CATEGORIES_S_ROOM; _cat++)
            {
                temp_cat[0] = FART_S_ROOM[_cat][0];
                temp_cat[1] = FART_S_ROOM[_cat][1];
                temp_cat[2] = FART_S_ROOM[_cat][2];
                temp_cat[3] = FART_S_ROOM[_cat][3];
                o_final_route.set_fart_map(_cat, temp_cat);
            }
            o_routes.o_ffitness.o_virtualMotion.set_selected_room(S_ROOM);
        }
        else if( experimentation_room == T_ROOM )
        {
            double temp_cat[4];
            o_final_route.init_fart_map(CATEGORIES_T_ROOM);
            for(int _cat = 0; _cat < CATEGORIES_T_ROOM; _cat++)
            {
                temp_cat[0] = FART_T_ROOM[_cat][0];
                temp_cat[1] = FART_T_ROOM[_cat][1];
                temp_cat[2] = FART_T_ROOM[_cat][2];
                temp_cat[3] = FART_T_ROOM[_cat][3];
                o_final_route.set_fart_map(_cat, temp_cat);
            }
            o_routes.o_ffitness.o_virtualMotion.set_selected_room(T_ROOM);
        }

        // Update map in ELite
        char MAP_FILE[50];
        snprintf(MAP_FILE, sizeof(MAP_FILE),  "./lrn_loaded_ascii_map%d", route_nr);
        o_routes.o_ffitness.o_virtualMotion.PrintMAPtoFile(MAP_FILE);

    }
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
void LRNProcessor::Update_Missions_list(void){

    if( mission_op_mode == NORMAL ) {

        visited_feature_nr = 0;
        cda.lockArea(FEATURE_NAV_AREA);
        missions_nr = pCDAFeatureNav->fn_nFeatures;
        for(int _im = 0; _im < missions_nr; _im++)
        {
            mission_coord[_im][0] = 90;   // Orientation
            mission_coord[_im][1] = pCDAFeatureNav->fn_featureArray[_im][0];   // X Coord
            mission_coord[_im][2] = pCDAFeatureNav->fn_featureArray[_im][1];   // Y Coord
        }

        //Renato
        new_features_flag = false;
        pCDAFeatureNav->fn_feature_list_ready_flag  = false;
        //END Renato
        cda.unlockArea(FEATURE_NAV_AREA);

    }

    if( mission_op_mode == TEST ){
        missions_nr = 3;
        mission_coord[0][0] = 0;   /* degrees */
        mission_coord[0][1] = 250; /* xcord in cm */
        mission_coord[0][2] = 250; /* ycord in cm */
        mission_coord[1][0] = 270;
        mission_coord[1][1] = 200;
        mission_coord[1][2] = 125;
 //       mission_coord[2][0] = 90;
 //       mission_coord[2][1] = 500;
//        mission_coord[2][2] = 2500;
    }

    o_final_route.init_missions(missions_nr);

    int _distance;
    int temp_mission[3];

    for(int _im = 0; _im < missions_nr; _im++){
        temp_mission[0] = mission_coord[_im][0];
        temp_mission[1] = mission_coord[_im][1];
        temp_mission[2] = mission_coord[_im][2];
        o_final_route.set_mission(_im, temp_mission);

        _distance = o_routes.o_ffitness.o_virtualMotion.Calculate_Distance(start_coord, mission_coord[_im]);
        mission_coord[_im][3] = _distance; /* distance frome start_coord */
        mission_coord[_im][4] = -1; /* fitness value */
        mission_coord[_im][5] = start_coord[0]; 
        mission_coord[_im][6] = start_coord[1];
        mission_coord[_im][7] = start_coord[2];
    }
}

/* *
 * \brief Update step_lenght & angle_lenght values from VirtualExecutive.
 *  The function takes the executed steps  of the Executive class. It calculates the
 *  distance between and replaces this value with the step_lenght and the angle
 *  difference. 
 *  \param none
 *  \return none
 */
void LRNProcessor::Update_Step_Lenght(void){

    int _step;
    float _dx,_dy,_dangle,temp; // delta values

    cda.lockArea(LASER_AREA);
    _dx = pCDALaser->dx;
    _dy = pCDALaser->dy;
    _dangle = pCDALaser->ddir;
    cda.unlockArea(LASER_AREA);
    
    temp = lround((_dangle*180.0)/M_PI);

    o_routes.o_ffitness.o_virtualMotion.set_angle_lenght(temp);
    o_routes.o_ffitness.o_virtualMotion.set_step_lenght(_step);
}
//-------------------------------------------------------------------
void LRNProcessor::Set_Start_position(void){	

    o_routes.o_ffitness.o_virtualMotion.Set_StartCoordinates(start_coord);
    o_final_route.Set_StartCoordinates(start_coord);
}

//-------------------------------------------------------------------
void LRNProcessor::Update_Start_position(void){	
    int tmp;
    /* get coordinates & orientation from laser readings */
    cda.lockArea(LASER_AREA);
    tmp = init_coord[0]+ lround((pCDALaser->dir*180.0)/M_PI);   // Current Orientation
    start_coord[1] = init_coord[1]+ pCDALaser->x;   // Current X Coord*10cm
    start_coord[2] = init_coord[2]+ pCDALaser->y;   // Current Y Coord*10cm
    cda.unlockArea(LASER_AREA);

    /* to fix negatives and more than 360 degrees */
    while(tmp <= 0 || tmp >= 360){ 
        if (tmp < 0)   tmp = tmp + 360;
        if (tmp > 360) tmp = tmp - 360;
    }        

    /* set coordinates to virtualexecutive & elite */
    start_coord[0] = tmp;
    o_routes.o_ffitness.o_virtualMotion.Set_StartCoordinates(start_coord);
    o_final_route.Set_StartCoordinates(start_coord);
}

//-------------------------------------------------------------------
int LRNProcessor::Read_Configuration_File(void)
{
    string temp_var;

    // Object ro read Configuration File
    ConfigFile config( "./config/longRangeNavigator.conf" );

    // Update this variable with the value indicated in "",
    // if "" it is not present in the configuration file
    // then used the following default value
    config.readInto(population_size, "POPULATION_SIZE", 60);
    config.readInto(generations_nr, "GENERATIONS_NR", 10);
    config.readInto(actions_nr, "GENES_NR", 80);
    config.readInto(cross_points_nr, "CROSS_POINTS_NR", 16);

    config.readInto(temp_var, "SELECTION_METHOD");
    if( temp_var.compare(0,14, "ROULETTE_ELITE") == 0 )   {  selection_method = ROULETTE_ELITE;  }
    else if( temp_var.compare(0,8, "ROULETTE") == 0 )     {  selection_method = ROULETTE;        }
    else if( temp_var.compare(0,23, "TOURNAMENT_ELITE_RANDOM") == 0 )   {  selection_method = TOURNAMENT_ELITE_RANDOM;   }
    else if( temp_var.compare(0,16, "TOURNAMENT_ELITE") == 0 )  {  selection_method = TOURNAMENT_ELITE; }
    else if( temp_var.compare(0,10, "TOURNAMENT") == 0 )        {  selection_method = TOURNAMENT;      }

    else  {  selection_method = TOURNAMENT_ELITE_RANDOM;  }


    config.readInto<int>(init_coord[0], "START_ANGLE", 90);
    config.readInto<int>(init_coord[1], "START_X_COORD", 200);
    config.readInto<int>(init_coord[2], "START_Y_COORD", 200);

    config.readInto<int>(home_coord[1], "HOME_X_COORD", 200);
    config.readInto<int>(home_coord[2], "HOME_Y_COORD", 200);
    config.readInto<int>(MAX_DISTANCE_ERROR, "MAX_DISTANCE_ERROR", 300);

    config.readInto(temp_var, "EXPERIMENTATION_ROOM");

    if( temp_var.compare(0,6, "H_ROOM") == 0 )          {  experimentation_room = H_ROOM; }
    else if( temp_var.compare(0,6, "L_ROOM") == 0 )     {  experimentation_room = L_ROOM; }
    else if( temp_var.compare(0,6, "O_ROOM") == 0 )     {  experimentation_room = O_ROOM; }
    else if( temp_var.compare(0,6, "S_ROOM") == 0 )     {  experimentation_room = S_ROOM; }
    else if( temp_var.compare(0,6, "T_ROOM") == 0 )     {  experimentation_room = T_ROOM; }
    else if( temp_var.compare(0,9, "FART_ROOM") == 0 )  {  experimentation_room = FART_ROOM; }
    else  {  experimentation_room = SLAM_ROOM; }


    config.readInto<double>(m_curiosity, "MOTIVATION_CURIOSITY", 1.0);
    config.readInto<double>(m_energy, "MOTIVATION_ENERGY", 0.0);
    config.readInto<double>(m_homing, "MOTIVATION_HOMING", 0.0);
    config.readInto<double>(m_missions, "MOTIVATION_MISSIONS", 0.0);

    verbose_mode = OFF;
    config.readInto(temp_var, "VERBOSE_MODE");
    if( temp_var == "ON" )  {  verbose_mode = ON; }
    else  {  verbose_mode = OFF;   }

    config.readInto(temp_var, "FEATURES_MODE");
    if( temp_var == "TEST" )         {  mission_op_mode = TEST; }
    else if( temp_var == "NORMAL" )  {  mission_op_mode = NORMAL; }
    else {   mission_op_mode = TEST; }

    Show_Variables();

    return(0);
}

//-------------------------------------------------------------------
void LRNProcessor::Show_Variables(void)
{
    if( verbose_mode == ON )
    {
        fprintf(stdout, "········································\n");
        fprintf(stdout, "POPULATION_SIZE: %d\n", population_size);
        fprintf(stdout, "GENERATIONS_NR: %d\n", generations_nr);
        fprintf(stdout, "GENES_NR: %d\n", actions_nr);
        fprintf(stdout, "CROSS_POINTS_NR: %d\n", cross_points_nr);

        if( selection_method == ROULETTE)   {  fprintf(stdout, "SELECTION_METHOD: ROULETTE\n");   }
        else if( selection_method == ROULETTE_ELITE) {  fprintf(stdout, "SELECTION_METHOD: ROULETTE_ELITE\n");   }
        else if( selection_method == TOURNAMENT)     {  fprintf(stdout, "SELECTION_METHOD: TOURNAMENT\n"); }
        else if( selection_method == TOURNAMENT_ELITE)  {  fprintf(stdout, "SELECTION_METHOD: TOURNAMENT_ELITE\n"); }
        else if( selection_method == TOURNAMENT_ELITE_RANDOM) {  fprintf(stdout, "SELECTION_METHOD: TOURNAMENT_ELITE_RANDOM\n");   }

        fprintf(stdout, "START_POSITION: Angle:%3d\tX Coord:%4d\tY Coord:%4d\n", init_coord[0], init_coord[1], init_coord[2]);
        fprintf(stdout, "HOME_POSITION: X Coord:%4d\tY Coord:%4d\n", home_coord[1], home_coord[2]);
        fprintf(stdout, "MAX_DISTANCE_ERROR - between ideal and real position: %d\n", MAX_DISTANCE_ERROR);

        if( experimentation_room == H_ROOM )       {  fprintf(stdout, "EXPERIMENTATION_ROOM: H_ROOM\n");  }
        else if( experimentation_room == L_ROOM )  {  fprintf(stdout, "EXPERIMENTATION_ROOM: L_ROOM\n");  }
        else if( experimentation_room == O_ROOM )  {  fprintf(stdout, "EXPERIMENTATION_ROOM: O_ROOM\n");  }
        else if( experimentation_room == S_ROOM )  {  fprintf(stdout, "EXPERIMENTATION_ROOM: S_ROOM\n");  }
        else if( experimentation_room == T_ROOM )  {  fprintf(stdout, "EXPERIMENTATION_ROOM: T_ROOM\n");  }
        else if( experimentation_room == FART_ROOM ) {  fprintf(stdout, "EXPERIMENTATION_ROOM: FART_ROOM\n");   }

        fprintf(stdout, "MOTIVATION_CURIOSITY: %.3f\n", m_curiosity);
        fprintf(stdout, "MOTIVATION_ENERGY: %.3f\n", m_energy);
        fprintf(stdout, "MOTIVATION_HOMING: %.3f\n", m_homing);
        fprintf(stdout, "MOTIVATION_MISSIONS: %.3f\n", m_missions);

        if( mission_op_mode == TEST )          {  fprintf(stdout, "FEATURES OPERATION MODE: TEST\n");   }
        else if( mission_op_mode == NORMAL )   {  fprintf(stdout, "FEATURES OPERATION MODE: NORMAL\n"); }

        fprintf(stdout, "········································\n");
    }
}

//-------------------------------------------------------------------
void LRNProcessor::Print_output_data(int _verbose)
{
    if( _verbose >= VERBOSE_MEDIUM )
    {
        fprintf(stdout, "Theoretical useful steps used to achieved the goal: %d\n", o_routes.get_e_useful_steps_nr());
        if( m_missions > 0.0 )
        {
            // Print Results
            fprintf(stdout, "Theoretical Closer distance to the goal: %d\n", o_routes.get_e_goal_distance());
            fprintf(stdout, "Ideal Final Position:         Angle: %3d   X Coord: %5d   Y Coord: %5d\n",
                    mission_coord[visited_feature_nr][0], mission_coord[visited_feature_nr][1],
                    mission_coord[visited_feature_nr][2]);
        }

        int _e_final_coord[3];
        o_routes.get_e_final_position(_e_final_coord);
        fprintf(stdout, "Theoretical Final Position:   Angle: %3d   X Coord: %5d   Y Coord: %5d\n",
                _e_final_coord[0], _e_final_coord[1], _e_final_coord[2]);
        int _e_start_coord[3];
        o_routes.o_ffitness.o_virtualMotion.get_start_position(_e_start_coord);
        fprintf(stdout, "Theoretical Start Position:   Angle: %3d   X Coord: %5d   Y Coord: %5d\n",
                _e_start_coord[0], _e_start_coord[1], _e_start_coord[2]);
        fprintf(stdout, "Fitness obtained for this individual: %f\n", o_routes.get_e_fitness());
        double temp_fulfillment[4];
        o_routes.get_e_fulfillment_prod(temp_fulfillment);
        fprintf(stdout, "Curiosity: %.3f\t Energy: %.3f\t Homing: %.3f\t Missions: %.3f\n",
                temp_fulfillment[0], temp_fulfillment[1], temp_fulfillment[2], temp_fulfillment[3]);
        fprintf(fp_fitness, "R%2d | Curiosity: %.3f\t Energy: %.3f\t Homing: %.3f\t Missions: %.3f\n",
                N_FITNESS+1, temp_fulfillment[0], temp_fulfillment[1], temp_fulfillment[2], temp_fulfillment[3]);
        SUM_FITNESS_C+=temp_fulfillment[0];
        SUM_FITNESS_E+=temp_fulfillment[1];
        SUM_FITNESS_H+=temp_fulfillment[2];
        SUM_FITNESS_M+=temp_fulfillment[3];
        N_FITNESS++;
    }

    // Make Outputs Files
    char STATS_FILE[50];
    snprintf(STATS_FILE, sizeof(STATS_FILE),  "./lrn_stats%d", route_nr);
    o_routes.Print_Stats_File(STATS_FILE);

    // Make Outputs Files
    char MAP_FILE[50];
    snprintf(MAP_FILE, sizeof(MAP_FILE),  "./lrn_loaded_ascii_map%d", route_nr);
    o_routes.o_ffitness.o_virtualMotion.PrintMAPtoFile(MAP_FILE);
}

//-------------------------------------------------------------------
void LRNProcessor::Print_elite_output_data(void)
{
    char PATH_GENOTYPE_FILE[50];
    char PATH_PHENOTYPE_FILE[50];
    snprintf(PATH_GENOTYPE_FILE, sizeof(PATH_GENOTYPE_FILE),  "./lrn_elite_genotype%d", route_nr);
    snprintf(PATH_PHENOTYPE_FILE, sizeof(PATH_PHENOTYPE_FILE),  "./lrn_elite_phenotype%d", route_nr);

    o_final_route.Print_GENOTYPE(PATH_GENOTYPE_FILE);
    o_final_route.Print_PHENOTYPE(PATH_PHENOTYPE_FILE);
}

//-------------------------------------------------------------------
void LRNProcessor::Compute_time_difference(const long int _sec, const int _msec)
{
    // CALCULATE THE TIME USED TO COMPUTE THE PATH
    struct timeb tp;  ftime(&tp);
    fprintf(stdout, "Start time = %ld.%d\n", _sec, _msec);
    fprintf(stdout, "End time   = %ld.%d\n", tp.time, tp.millitm);

    int min = (tp.time - _sec)/60;
    int sec = (tp.time - _sec)%60;
    int msec = tp.millitm - _msec;
    if( msec < 0 )  msec = 1000 + msec;

    fprintf(stdout, "%2d Min %2d Sec %4d msec was needed to compute the route\n", min, sec, msec);
    fprintf(fp_moves, "========== GA ITERATION : ROUTE CREATED ==========\n");
    fprintf(stdout, "·······························································\n");

    char TIME_NEEDED[50];
    //   snprintf(TIME_NEEDED, sizeof(TIME_NEEDED),  "./lrn_time_used_%d", route_nr);
    snprintf(TIME_NEEDED, sizeof(TIME_NEEDED),  "./lrn_time_used");
    FILE *fp;
    //   if( (fp = fopen(TIME_NEEDED, "w")) )
    if( (fp = fopen(TIME_NEEDED, "a")) )
    {
        fprintf(fp, "Iteration nr: %3d \t%2d Min %2d Sec %4d msec was needed to compute the route\n", route_nr, min, sec, msec);
        fclose(fp);
    }
    else  {  printf("Error opening FILE TIME_NEEDED: LRN::Compute_time_difference\n");  }
}
//*******************************************************************
// LONG RANGE NAV PROCESSOR: LRNProcessor Class Implementation: END
//*******************************************************************


//*******************************************************************
// MAIN FUNCTION Class Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
//-------------------------------------------------------------------
int main()
{
    fprintf(stdout, "\nIRMA3: Longe Range Navigator Process\n");

    char fileMoves[14] = "lrn_moves.txt";
    if( (fp_moves = fopen(fileMoves, "w")) )
        printf("PRINTING MOVES IN FILE: %s\n", fileMoves);
    else
        printf("ERROR OPENING FILE: %s\n", fileMoves);

    char fileFitness[16] = "lrn_fitness.txt";
    if( (fp_fitness = fopen(fileFitness, "w")) )
        printf("PRINTING FITNESS IN FILE: %s\n", fileFitness);
    else
        printf("ERROR OPENING FILE: %s\n", fileFitness);

    LRNProcessor lrn;


    //	lrn.test();

    srand(time(NULL)); // set seed for random functions

    lrn.execute();

    //lrn.cleanup();

    return(0);
}
//*******************************************************************
// MAIN FUNCTION Class Implementation: END
//*******************************************************************
//

int LRNProcessor::test(){

    fprintf(stdout, "\nIRMA3: test\n");

    o_routes.CleanGenes();
    o_routes.Evaluate();
    return(0);
}
