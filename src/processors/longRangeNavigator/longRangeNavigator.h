//*******************************************************************
// Title: longRangeNavigator.h
//
// Version: 0.1
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


#ifndef LONG_RANGE_NAV_H
#define LONG_RANGE_NAV_H


//*******************************************************************
// Librery Declaration
#include <stdio.h>
#include <sys/shm.h>
#include <cmath>
#include <unistd.h>
#include <sys/timeb.h>
#include "../base/processor.h"
#include "../../common/configFile/configFile.h"

// Route generator
#include "GALRN.h"


//*******************************************************************
// Global Variables Definition
const int TOTAL_SUB_FORWARD = 1; // Command number to complete a FORWARD action
const int TOTAL_SUB_TURN_RIGHT = 1;
const int TOTAL_SUB_TURN_LEFT = 1;
const int TOTAL_SUB_TURN_RIGHT_1 = 1; // Command number to complete a TURN RIGHT 1 action
const int TOTAL_SUB_TURN_LEFT_1 = 1; // Command number to complete a TURN LEFT 1 action
const int TOTAL_SUB_TURN_RIGHT_3 = TOTAL_SUB_TURN_RIGHT_1 * 3; // Command number to complete a TURN RIGHT 3 action
const int TOTAL_SUB_TURN_LEFT_3 = TOTAL_SUB_TURN_LEFT_1 * 3; // Command number to complete a TURN LEFT 3 action
const int TOTAL_SUB_TURN_RIGHT_2 = TOTAL_SUB_TURN_RIGHT_1 * 2; // Command number to complete a TURN RIGHT 2 action
const int TOTAL_SUB_TURN_LEFT_2 = TOTAL_SUB_TURN_LEFT_1 * 2; // Command number to complete a TURN LEFT 2 action

static int SUB_FORWARD = 0; // Number of motor commands doings to complete a FORWARD action
static int SUB_TURN_RIGHT_1 = 0; // Number of motor commands doings to complete a TURN RIGHT 1 action
static int SUB_TURN_LEFT_1 = 0; // Number of motor commands doings to complete a TURN LEFT 1 action
static int SUB_TURN_RIGHT_3 = 0; // Number of motor commands doings to complete a TURN RIGHT 3 action
static int SUB_TURN_LEFT_3 = 0; // Number of motor commands doings to complete a TURN LEFT 3 action
static int SUB_TURN_RIGHT_2 = 0; // Number of motor commands doings to complete a TURN RIGHT 2 action
static int SUB_TURN_LEFT_2 = 0; // Number of motor commands doings to complete a TURN LEFT 2 action
static int SUB_TURN_RIGHT = 0; // Number of motor commands doings to complete a TURN RIGHT 1 action
static int SUB_TURN_LEFT = 0; // Number of motor commands doings to complete a TURN LEFT 1 action
const int MAX_MISSIONS_NUMBER = 10;

const int TOLERANCE_GOAL_DISTANCE = 5; // in cm

const bool ON = true;
const bool OFF = false;

FILE *fp_moves; // File to save the movements performed by LRN
FILE *fp_fitness; // File to save the fitness of each individual used by LRN
static int N_FORWARD = 0; // Total movements counter
static int N_TURN_RIGHT_1 = 0; // Total movements counter
static int N_TURN_LEFT_1 = 0; // Total movements counter
static int N_TURN_RIGHT_3 = 0; // Total movements counter
static int N_TURN_LEFT_3 = 0; // Total movements counter
static int N_TURN_RIGHT_2 = 0; // Total movements counter
static int N_TURN_LEFT_2 = 0; // Total movements counter
static int N_REVERSE = 0; // Total movements counter
static int N_FREEZE = 0; // Total movements counter
static int N_TURN_RIGHT = 0; // Total movements counter
static int N_TURN_LEFT = 0; // Total movements counter
static int N_FITNESS = 0; // Counter of generations of GA used
static double SUM_FITNESS_C = 0; // Addition of fitness to calcute the average fitness of Curiosity
static double SUM_FITNESS_E = 0; // Addition of fitness to calcute the average fitness of Energy
static double SUM_FITNESS_H = 0; // Addition of fitness to calcute the average fitness of Homing
static double SUM_FITNESS_M = 0; // Addition of fitness to calcute the average fitness of Missions

// OPERATION MODE
const char TEST = 1;
const char NORMAL = 2;
const char FEATURE_LIST = 3;
//*******************************************************************
// DEFINE TO DEBUG
//*******************************************************************

//#define IRMA2
#define OBSTACLE 0


//*******************************************************************
// LRNProcessor Class Declaration: BEGIN
//*******************************************************************

class LRNProcessor : public ProcessorInterface {
private:
    // Declaration of a Pointer to the Share Memory of each process
    // The pointer have the format defined in common/CDA/(process_name)DataArea.h
    LongNavData *pCDALongNav;
    ExecutiveData *pCDAExecutive;
    MapperData *pCDAMap;
    MonitorData *pCDAMonitor;
    MapperData *pCDAMapper;
    LaserData *pCDALaser;
    FeatureNavData *pCDAFeatureNav;

    bool is_move_ready;
    bool is_route_obsolete;
    bool mission_accomplished_flag;
    bool new_features_flag;
    int ctrl_current_loop; // Patricio: check if current loop is stop
    int monitor_current_nav;
    int visited_feature_nr;
    int missions_nr; // Experiment
    int mission_coord[MAX_MISSIONS_NUMBER][8]; /* mission cord in cm & degrees */
    int route_nr; // Experiment
    int current_route_action;
    bool verbose_mode;
    int start_coord[3];
    int home_coord[3];
    int init_coord[3];
    int freeze_times;

    int MAX_DISTANCE_ERROR;
    int population_size;
    int generations_nr;
    int actions_nr; // Internal Use  Genetic algortihm
    int cross_points_nr;
    char selection_method;
    double m_curiosity; /* motivation curiosity */
    double m_energy;    /* motivation energy    */
    double m_homing;    /* motivation goming    */
    double m_missions;  /* motivation missions  */
    char experimentation_room;
    char mission_op_mode;
    double cmd[ACTIONS_NUMBER][2];

    // Create the path planner
    c_pop o_routes;
    // Path and information related to the route to be executed
    c_Best_route o_final_route;

    // Read flags from Common data area
    void Read_Flags(void);
    // Configure Path Planner
    int Configure_path_planner(void);
    // Configure Final Path or elite individual
    int Configure_final_path(void);
    // Compute a route
    void Compute_new_path(void);
    // Convert action to motor command
    void Convert_Action_To_Motor_Cmd(char _action);
    // Convert action to motor and check for mission complete
    void Action_To_Motor(char _action);
    // Update Motor Commands into LONG RANGE NAV Data Area
    void Deliver_Motor_Commands(const double _speed, const double _steer);
    void Deliver_Motor_Commands(const int _speed_percent, const int _movement);
    // Update mission data
    void Update_missions_extra_data(const int _feature_index);
    // Update Elite Organism Output Data
    void Update_elite_data(const int _feature_index);
    // Update Best Path Phenotype
    void Update_elite_phenotype(void);
    // Set start position to VirtualExecutive and Elite_Individual
    void Set_Start_position(void);	
    // Update SLAM map from MAPPER Data Area
    int Update_Slam_Map(void);
    // Update mission list from FEATURE NAVIGATOR Data Area
    void Update_Internal_Map(void);
    // Update mission list from FEATURE NAVIGATOR Data Area
    void Update_Missions_list(void);
    // Update the current position and make it the new start position
    void Update_Start_position(void);

    // Read start File for first configurations
    int Read_Configuration_File(void);
    // Show parameters
    void Show_Variables(void);
    // Print best solution
    void Print_output_data(int _verbose);
    // Print Genotype and Phenotype to file
    void Print_elite_output_data(void);
    // Print Time Difference into a file - Time used to compute a route
    void Compute_time_difference(const long int _sec, const int _msec);
    
    void CtrlLoop(bool _go);
    int UpdateStep(void);
    void SetActionCmd(void);
    void AdjustStep(void);

public:

    LRNProcessor() : ProcessorInterface() {
        ;
    }

    int init(void);
    int step(void);
    int cleanup(void);
    int test(void);
};
//*******************************************************************
// LRNProcessor Class Declaration: END
//*******************************************************************

#endif   // LONG_RANGE_NAV_H
