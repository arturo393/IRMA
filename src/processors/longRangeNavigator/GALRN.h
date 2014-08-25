//*******************************************************************
// Title: GALRN.h
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


#ifndef GALRN_H
#define GALRN_H


//*******************************************************************
// Librery Declaration
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "Utils.h"


// Used by Virtual Executive
#include <math.h>
#include "InternalMap.h"

// Used by FitnessFunction
#include "MandamiFuzzyModel.h"
//#include "SugenoFuzzyModel.h"


//*******************************************************************
// Global Variables Definition
const int ACTIONS_NUMBER = 5;

const int GENES_NUMBER = 400;     // 400
const int POPULATION = 100;      // 100
const int GENERATIONS = 100;     // 100
const int ELITE = 1;             // 1
const int FIXED_ELITE = 3;       // 1
const int IMMIGRANTS = 6;        // 6%
const int CROSS_POINTS = 40;     // 40
const double MUTATION_RATE = 0.20;   // 15%
const double TOURNAMENT_PARAM = 0.85;// 75%
const double CROSSOVER_RATE = 0.80;  // 75%

//===================================================================
// Motivations
const double MOTIVATION_CURIOSITY = 1.00;  // RANGE [0, 1]
const double MOTIVATION_ENERGY    = 0.00;  // RANGE [0, 1]
const double MOTIVATION_HOMING    = 0.00;  // RANGE [0, 1]
const double MOTIVATION_MISSIONS  = 1.00;  // RANGE [0, 1]

const double REVISITED_PENALTY = 0.30;  // Used when a map cell is revisited (Patricio: was 0.20)
const char SELECTED_ROOM = H_ROOM;     // Ideal Room

const int START_ANGLE      = 90;  // Starting Angle - DEGREES
const int START_X_COORD    = 1000;  // Starting X Coordinate - Millimeters
const int START_Y_COORD    = 1000;  // Starting Y Coordinate - Millimeters
const double STARTING_BATTERY = 1.0;   // Starting Battery Level - RANGE [0, 1] Same value for each GA iteration
static double CURRENT_BATTERY = 25.0;   // Updated when the robot actually moves. (Patricio: POWER_FORWARD*MAX_STEPS_NUMBER = 0.005*1000)
// CURRENT_BATTERY: Each 30 steps can consume up to 1.0 of Battery. For 1000 steps can consume up to 33.3 of Battery. <-- Calculated manually (should be in configFile)

const int GOAL_ANGLE    = START_ANGLE;   // Home Angle - DEGREES
const int GOAL_X_COORD  = START_X_COORD;   // Home X Coordinate - Millimeters
const int GOAL_Y_COORD  = START_Y_COORD;   // Home Y Coordinate - Millimeters
const int HOME_ANGLE    = START_ANGLE;   // Home Angle - DEGREES
const int HOME_X_COORD  = START_X_COORD;   // Home X Coordinate - Millimeters
const int HOME_Y_COORD  = START_Y_COORD;   // Home Y Coordinate - Millimeters
//===================================================================
// Actions
const char FORWARD      = 0;  // 0
const char REVERSE      = 1;  // 7
const char FREEZE       = 2;  // 8
const char TURN_RIGHT   = 3;
const char TURN_LEFT    = 4;
//const char TURN_RIGHT_1 = 1;  // 1
//const char TURN_LEFT_1  = 2;  // 2
//const char TURN_RIGHT_3 = 3;  // 3
//const char TURN_LEFT_3  = 4;  // 4
//const char TURN_RIGHT_2 = 5;  // 5
//const char TURN_LEFT_2  = 6;  // 6


const int STEP_SIZE   = 50;    // Distance to cover in Forward movements - Millimeters
const int ANGLE_SIZE  = 30;   // Angle to shift in Turns movements - DEGREES

const double GOAL_SIZE = 1.0;  // Related to the robot diameter
const double GOAL_SIGNAL_RANGE = 3.0;  // Range is GOAL_SIGNAL_RANGE times GOAL SIZE
const double MISSION_AREA = 1.0;  // Related to the robot diameter
const double MISSION_MAX_RANGE = (8.0);  // Range is GOAL_SIGNAL_RANGE times GOAL SIZE
// Robot Characteristics
const int ROBOT_DIAMETER = 38.5;  // Variables in Millimeters
const int SENSOR_RANGE   = 10;  // Variables in Millimeters

// original values:
// Calc_Fitness_Energy divides current by initial battery and current
// is calculated by multiplying each step by this power loss. The number 
// of steps executed is in config/longRangeNavigator.conf in the gumstix and
// currently is set to 40 to work with the numbers below.
// POWER_MULT and other POWER_X values should cause Calc_Fitness_Energy
// values to go from [0...1]
 const double POWER_MULT = 2.0;
 // 	Value of BATTERY for each GA iteration: 1.0
// 	Current population size (max number of steps): 40 <-- config/longRangeNavigator.conf in gumstix
//	The most consumer step: FORWARD == REVERSE: 1.0/40=0.0250 (Patricio)
//	While turning has half of the power loss: RIGHT == LEFT: 0.0250/2=0.0125
 const double POWER_FORWARD = 0.0250;  // Power needed to move the robot FORWARD - RANGE [0, 1]
 const double POWER_RIGHT =   0.0125;  // Power needed to turn the robot RIGHT - RANGE [0, 1]
 const double POWER_LEFT  =   0.0125;  // Power needed to turn the robot LEFT - RANGE [0, 1]
 const double POWER_RIGHT_1 = 0.0125;  // Power needed to turn the robot RIGHT - RANGE [0, 1]
 const double POWER_LEFT_1  = 0.0125;  // Power needed to turn the robot LEFT - RANGE [0, 1]
 //	The following movements are not implemented
 const double POWER_RIGHT_2 = 0.0150;  // Power needed to turn the robot RIGHT - RANGE [0, 1]
 const double POWER_LEFT_2  = 0.0150;  // Power needed to turn the robot LEFT - RANGE [0, 1]
 const double POWER_RIGHT_3 = 0.0200;  // Power needed to turn the robot RIGHT - RANGE [0, 1]
 const double POWER_LEFT_3  = 0.0200;  // Power needed to turn the robot LEFT - RANGE [0, 1]
 const double POWER_REVERSE = 0.0250;  // Power needed to move the robot REVERSE - RANGE [0, 1]
 const double POWER_FREEZE  = 0.0001;  // Power needed to keep the robot in STANDBY - RANGE [0, 1]
// current values:
// 	Value of BATTERY for each GA iteration: 1.0
//	Minimum fitness value forced : 0.85
// 	Current population size (max number of steps): 30 <-- updated manually (should be from configFile)
//	(Patricio) The most consumer step: FORWARD == REVERSE: (1.0-0.1)/30=0.0050
//const double POWER_FORWARD = 0.0050;  // Power needed to move the robot FORWARD - RANGE [0, 1]
//const double POWER_REVERSE = 0.0050;  // Power needed to move the robot REVERSE - RANGE [0, 1]
// 	(Patricio) Experimental proportions:
//		RIGHT/LEFT1: x0.1	    | RIGHT/LEFT2: x0.3	 | RIGHT/LEFT2: x0.5      | FREEZE: x0.02
// 		RIGHT/LEFT1: 0.0005 | RIGHT/LEFT2: 0.0015 | RIGHT/LEFT2: 0.0025 | FREEZE: 0.0001
//const double POWER_RIGHT_1 = 0.0005;  // Power needed to turn the robot RIGHT - RANGE [0, 1]
//const double POWER_LEFT_1  = 0.0005;  // Power needed to turn the robot LEFT - RANGE [0, 1]
//const double POWER_RIGHT_2 = 0.0015;  // Power needed to turn the robot RIGHT - RANGE [0, 1]
//const double POWER_LEFT_2  = 0.0015;  // Power needed to turn the robot LEFT - RANGE [0, 1]
//const double POWER_RIGHT_3 = 0.0025;  // Power needed to turn the robot RIGHT - RANGE [0, 1]
//const double POWER_LEFT_3  = 0.0025;  // Power needed to turn the robot LEFT - RANGE [0, 1]
//const double POWER_FREEZE  = 0.0001;  // Power needed to keep the robot in STANDBY - RANGE [0, 1]

//===================================================================
//===================================================================
const char ROULETTE = 0;                  // 0
const char ROULETTE_ELITE = 1;            // 1
const char TOURNAMENT = 2;                // 2
const char TOURNAMENT_ELITE = 3;          // 3
const char TOURNAMENT_ELITE_RANDOM = 4;   // 4

const char VERBOSE_OFF        = 0x00;  // 0
const char VERBOSE_MIN        = 0x01;  // 1
const char VERBOSE_LOW        = 0x02;  // 2
const char VERBOSE_MEDIUM     = 0x03;  // 3
const char VERBOSE_HIGH       = 0x04;  // 4
const char VERBOSE_VERY_HIGH  = 0x05;  // 5

static int DEBUG_ITERATIONS = 0;
//===================================================================

//*******************************************************************
//*******************************************************************
// DEFINE TO DEBUG
//#define T_C_NEXT_POSITION
//#define T_CRASH_CHECKER
//#define T_EXECUTOR
//#define T_MANDAMI_FITNESS
//#define T_MOTIVATION_FITNESS
//===================================================================
// DEFINE TO DEBUG
//#define TEST_GA_FUNCTIONS
//#define T_CREATE
//#define T_EVOLVE
//#define T_EVALUATE
//#define T_SELECT
//#define T_CROSSOVER
//#define T_CROSSOVER_PARENTS
//#define T_MUTATION
//#define T_ROULETTE_SLICE
//#define T_ROULETTE_SELECTION
//#define T_ELITE_SELECTION
//#define T_TOURNAMENT_SELECTION
//#define T_IMMIGRANTS_SELECTION
//#define T_RANKING_UPDATE
//#define T_ELITE_FIX_FUNCTION
#define USE_ELITE_FIX_FUNCTION



//===================================================================
typedef char _gen;   // Codification: Actions - Five Actions
class Organism;
class VirtualExecutive;
class FitnessFunction;
class Pop_Stats;
class Population;
//===================================================================


//*******************************************************************
// Organism Class Declaration: BEGIN
//*******************************************************************
class Organism
{
   private:
      _gen a_genes[GENES_NUMBER];

      int genesNumber;
      double fitness;
      int ranking;   // Smaller number means better position [0, ...]
      double slice;  // Slice size used for roulette wheel selection
      int min_goal_distance;  // When the goal was achieved
      int useful_steps_nr;   // When the goal was achieved
      int final_position[3];  // Final position after usefull_steps_nr
      double fulfillment_products[4];

   public:
      Organism();    // Default Constructor
      Organism(int _number);    // Constructor
      ~Organism();   // Destructor

      void PrintGenome(const char a_fileName[]) const;
      _gen getGene(int _igen) const;
      double getFitness(void) const;
      int getGenesNumber(void) const;
      int getRanking(void) const;
      double getSlice(void) const;
      int get_min_goal_distance(void) const;
      int get_useful_steps_nr(void) const;
      void get_final_position(int _coordinates[]) const;
      void get_fulfillment_products(double _fulfillment[]) const;

      void setGene(int _igen, _gen _value);
      void setFitness(double _value);
      void setGenesNumber(int _value);    // Change Varible value, but no resize the organism yet!
      void setRanking(int _value);
      void setSlice(double _value);
      void set_useful_steps_nr(int _value);
      void set_min_goal_distance(int _value);
      void set_final_position(const int _coordinates[]);
      void set_fulfillment_products(const double _fulfillment[]);
}; // end Class Organism
typedef Organism c_organism;
//*******************************************************************
// Organism Class Declaration: END
//*******************************************************************


//*******************************************************************
// VirtualExecutive Class Declaration: BEGIN
// This class is implemented in FitnessFunction.cpp
//*******************************************************************
class VirtualExecutive
{
   // All the Private Variables and Functions are designed
   // for each particular problem
   private:
      bool crashFlag;
      int _genesNumber;
      int useful_steps_nr;
      int sensorsRange;
      int robotCellRadius;
      int robotCellDiameter;
      int robotCellRadiusPlusSensors;
      int robotCellDiameterPlusSensors;
      int robotRadius;
      int robotDiameter;
      int robotRadiusPlusSensors;
      int step_lenght;
      int angle_length;

      
      // Energy Related Variables
      double initialBattery;     // Initial Battery or Energy level
      double currentBattery;     // Current or Final Battery level
      // Homing Related Variables
      int startPosition[3];   // Start Orientation and Position, degrees and cms
      int goalPosition[3];    // Goal Orientation and Position, degrees and cms
      int homePosition[3];    // Home  Orientation and Position, degrees and cms
      // Missions Related Variables
      int currentPosition[3]; // Current Orientation and Position, degrees and cms
      int totalMissions;         // Total number of Missions
//      double sub_goal_position[1][5];  // Feature's coordinates
      int missionPosition[1][8]; // Mission's coordinates and sub fitness

      // To speed up the calculations - the functions are call once
      // _velv: virtual executive local variables
      int _velv_mapMeshSize;
      int _velv_mapWidth;
      int _velv_mapHeight;
      int _velv_mapXDim;
      int _velv_mapYDim;

      // Functions
      void CleanAll(void);
      bool RobotSweptArea(const int _prevCoord[], const _gen _action);

      void UpdateBatteryLevel(const _gen _action);

      double Calc_Fitness_Curiosity(void);
      double Calc_Fitness_Energy(void);
      double Calc_Fitness_Homing(void);
      double Calc_Fitness_Mission(void);
    
   public:
      c_Map o_MAP;      // Must be public to be updated with FART map values
      

      VirtualExecutive();     // Default Constructor
      ~VirtualExecutive();    // Destructor
      friend class FitnessFunction;

      void Executor(const c_organism& agent, double a_fitness[]);
      void PrintMAPtoFile(const char a_fileName[]);
      void ComputeNextPosition(int _currentPosition[], const _gen _action);
      void Distance_Missions(const int _currentPosition[], const int _step);
      int Calculate_Distance(const int _1st_position[], const int _2nd_position[]);
           
      bool RobotCrash_MapBorderCoord();
      bool SensorCrash_MapBorderCells(const int _prevCoord[], const _gen _action);
      bool RobotCrash_MapBorderCells(const int _prevCoord[], const _gen _action);

      void Set_StartCoordinates(const int _coordinates[]);
      void Set_GoalCoordinates(const int _coordinates[]);
      void Set_Initial_Battery(const double _value);
      void Set_Mission(const int _coordinates[]);
//      void Set_Missions(const int _quantity, const int _coordinates[MAX_MISSIONS][3]);
      void Show_Missions(void) const;
      void set_step_lenght(const int _value);
      void set_angle_lenght(const int _value);
      void set_selected_room(const char _value);

      void get_start_position(int _coordinates[]) const;
      void get_goal_position(int _coordinates[]) const;
      void get_current_position(int _coordinates[]) const;
      void get_mission_position(int _coordinates[]) const;
      int getMapWidth(void) const;
      int getMapHeight(void) const;
      int get_MapMeshSize(void) const;
      int get_robot_diameter(void) const;
      int get_robot_diameter_sensors(void) const;
      char getXYcCellData(int _XCoord, int _YCoord) const;
      char get_selected_room(void) const;
      int get_step_lenght(void) const;
      int get_angle_length(void) const;
      int get_useful_steps_nr(void) const;
      int get_min_missions_distance(void) const;
      int get_min_missions_steps(void) const;
      void get_mission_closer_position(int _coordinates[]) const;

}; // end Class VirtualExecutive
typedef VirtualExecutive c_vExecutive;
//*******************************************************************
// VirtualExecutive Class Declaration: BEGIN
// This Function is implemented in FitnessFunction.cpp
//*******************************************************************


//*******************************************************************
// FitnessFunction Class Declaration: BEGIN
// This Function is implemented in FitnessFunction.cpp
//*******************************************************************
class FitnessFunction
{
   // All the Private Variables and Functions are designed
   // for each particular problem
   private:
      c_FMandami o_mandami;

      void NormalizeTo_1(double _inputsParam[], int _inSize, double _outputs[], int _outSize);
      void PrintMAP(const char a_fileName[]);   // DEBUG PURPOSE ONLY

   public:
      int fart_categories;
      int map_width;    // in mm
      int map_height;   // in mm
      double **FART_MAP;
      double a_fitness[4];       // Fitness for each Motivation
      double a_motivations[4];   // Must be public to be updated with IRMA's Main Process
      c_vExecutive o_virtualMotion; // Must be public to update Mission with IRMA's Main Process

      FitnessFunction();   // Default Constructor
      ~FitnessFunction();  // Destructor
      friend class Population;
      friend class Elite_Invidivual;

      double CalcFitness(const c_organism& agent);
      void PrintAgentGENOTYPE(const c_organism& agent, const char a_fileName[]) const;
      void PrintAgentPHENOTYPE(const c_organism& agent, const char a_fileName[]);
      void Init_Fart_MAP(const int _size, const int _width, const int _height);
      void Set_FART_MAP(void);   // Use once these variables are seted ::: fart_categories; **FART_MAP;

      /* SLAM IRMA3 MAP */
      void Set_Map_Dimensions(const int _width, const int _height);
      int Set_SLAM_MAP(const int _x, const int _y, const unsigned char _map[][600]);   
      void Set_Object_Obstacle(const int xs, const int ys, const int xe, const int ye);

      int getMapMeshSize(void) const;

      void Update_Motivations(const double _motivations[]);
}; // end Class FitnessFunction
typedef FitnessFunction c_fitness;
//*******************************************************************
// FitnessFunction Class Declaration: END
// This Function is implemented in FitnessFunction.cpp
//*******************************************************************


//*******************************************************************
// Population Class Declaration: BEGIN
//*******************************************************************
class Pop_Stats
{
   public:
      int utilMembers;     // Number of individuals with non-Zero fitness
      double avgFitness;   // Population average Fitness
      double maxFitness;   // Population Max Fitness
      double minFitness;   // Population Min Fitness
      double avgRelFitness;// Population Average Fitness with non-Zero fitness
      double minRelFitness;// Population Average Fitness with non-Zero fitness
      double fulfillment_products[4];

      Pop_Stats();   // Constructor
      ~Pop_Stats();  // Destructor

      void PrintStats(const char a_fileName[]) const;
};
typedef Pop_Stats c_popStats;
//*******************************************************************
// Population Class Declaration: END
//*******************************************************************


//*******************************************************************
// Population Class Declaration
//*******************************************************************
class Population
{
   private:
      int populationSize;        // Population Size
      int generationsNumber;     // Number of generations
      int genesNumber;           // Equal to the Genes number for each Organism
      char selectionMethod;      // Selection Method
      int eliteNumber;           // Elite Group Number
      int elite_fixed;           // Elite Group Number
      int randomImmigrants;      // Random new Population Number
      int crossPointsNumber;     // Define gene exchange
      double mutationsRate;       // Mutation Probability
      double tournamentParam;     // Parameter used in Tournament
      double crossoverProbability;// Not all individuals mate every generation

      double sumFitness;         // Used to calculate slice into Roulette Wheel Selection
      double avgFitness;
      int selected;              // Number of Organism in the Mating Pool

      int a_popList[POPULATION]; // To make the population ranking
      int a_points[CROSS_POINTS];// _pointsNumber: it is the number of cross points

      c_organism ao_pool[POPULATION];  // To used the class Organism
      c_organism ao_matingPool[POPULATION];  // Selected Organism to breed the new pool
      c_popStats ao_stats[GENERATIONS];

      void Roulette(void);
      void Tournament(void);
      void Elite(void);
      void RandomImmigrants(void);
      void CrossParents(const int _parents[], const int _children[], const int _a_points[]);
      void Duplicate_Individual(const int _parent, const int _child);
      void Elite_Fix_Function(void);

      void ToMating(int _iorg);
      // Quicksort: Sort population by fitness: in greater to lesser order
      void SortPopGL(int first, int last);
      void UnsortSelected(int _value);    // Usort Mating pool
      void RankingUpdate(void);           // Call SortPopGL
      void Sort_by_useful_steps(int first, int last);
      double SumTotalFitness(void);       // from ao_pool
      double AvgTotalFitness(void);       // from ao_pool

      void SetGenerationStats(int _ign);

      // Organism related function referenced only to the pool
      _gen getGene(int _iorg, int _igen) const;
      double getFitness(int _iorg) const;
      int getRanking(int _iorg) const;
      double getSlice(int _iorg) const;
      void setGene(int _iorg, int _igen, _gen _value);
      void setFitness(int _iorg, double _value);
      void setRanking(int _iorg, int _value);
      void setSlice(int _iorg, double _value);

    public:
      FitnessFunction o_ffitness;   // To compute the Fitness

      Population();        // Default Constructor
      ~Population();       // Destructor

      // After call create make sure, that you seted all the variables
      // Export Number of Gene to Organism, before create the Population
      void Reinit(void);   // Need only to make successive experiments
      void Create(void);
      void Evolve(const char _verbose);
      void CleanGenes(void);
      int Evaluate(void);
      void Select(char _method);
      void Crossover(void);
      void Mutate(void);

      void DisplayPool(char _verbose) const;
      void DisplayMating(char _verbose) const;
      void Print_Outputs_Files(void);
      void Print_Stats_File(const char a_fileName[]) const;
      void Print_e_genotype_File(const char a_fileName[]) const;
      void Print_e_phenotype_File(const char a_fileName[]);
      void Print_e_ascii_path_File(const char a_fileName[]);

      int getPopulationSize(void) const;
      int getGenerationsNumber(void) const;
      int getGenesNumber(void) const;
      char getSelectionMethod(void) const;
      int getEliteNumber(void) const;
      int get_elite_fixed(void) const;
      int getRandomImmigrants(void) const;
      int getCrossPointsNumber(void) const;
      double getMutationsRate(void) const;
      double getTournamentParam(void) const;
      double getCrossoverProbability(void) const;
      _gen get_elite_step(int _step) const;
      void get_e_final_position(int _coordinates[]) const;
      int get_e_useful_steps_nr(void) const;
      int get_e_goal_distance(void) const;
      double get_e_fitness(void) const;
      void get_elite_genetype(_gen _genotype[]) const;
      void get_e_fulfillment_prod(double _fulfillment[]) const;

      void setPopulationSize(int _value);    // just change the varible value, does not resize any array yet
      void setGenerationsNumber(int _value);
      void setGenesNumber(int _value);       // just change the varible value, does not resize any array yet
      void setSelectionMethod(char _value);
      void setEliteNumber(int _value);
      void set_elite_fixed(int _value);
      void setRandomImmigrants(int _value);
      void setCrossPointsNumber(int _value);
      void setMutationsRate(double _value);
      void setTournamentParam(double _value);
      void setCrossoverProbability(double _value);
        
      int getMapMeshSize(void) const;
}; // end Class Population
typedef Population c_pop;
//*******************************************************************
// Population Class Declaration: END
//*******************************************************************

//*******************************************************************
// Elite_Invidivual Class Declaration: BEGIN
//*******************************************************************
class Elite_Invidivual
{
   private:
      int robot_diameter;         // Millimeters
      int robot_diameter_sensors; // Millimeters
      int genes_nr;
      int ready_gen;
      _gen *a_genes;
      int missions_nr;
      int **a_missions_coord;  // Angle, X Coord, Y Coord (Degrees and Millimeters)
      int fart_categories;
      double **a_FART_map;
      int ready_path;
      int **a_path;

   public:
      double a_fitness[4];    // Fitness fulfillment product for each motivations
      int home_coord[3];      // Angle, X Coord, Y Coord (Degrees and Millimeters)
      int start_coord[3];     // Angle, X Coord, Y Coord (Degrees and Millimeters)
      int final_coord[3];     // Angle, X Coord, Y Coord (Degrees and Millimeters)

      Elite_Invidivual();     // Default Constructor
      ~Elite_Invidivual();    // Destructor

      void Clean_All(void);
      void Print_GENOTYPE(const char a_fileName[]);
      void Print_PHENOTYPE(const char a_fileName[]);

      void set_robot_diameter(const int _value);
      void set_robot_diameter_sensors(const int _value);
      void set_fitness(const double _fitness[]);
      void set_home_coord(const int _coord[]);
      void Set_StartCoordinates(const int _coord[]);
      void set_final_coord(const int _coord[]);
      void set_genes_nr(const int _value);   // Set path lenght
      void set_genes(const int _from, const int _until, const _gen _genes[]);
      void set_genes(const _gen _genes[]);
      void init_missions(const int _size);
      void set_mission(const int _index, const int _coord[]);
//      void set_missions(const int _size, const int _coord[][3]);
      void set_missions_closer_pos(const int _mission_nr, const int _coord[]);
      void set_missions_closer_dist(const int _mission_nr, const int _value);
      void set_missions_steps_needed(const int _mission_nr, const int _value);
      void init_fart_map(const int _size);
      void set_fart_map(const int _index, const double _categories[]);
      void set_fart_map(const int _size, const double _categories[][4]);
      void set_fart_map(const int _size, const float _categories[][4]);
//      void set_path_lenght(const int _size);
      void set_path_step(const int _coord[]);

      _gen get_gen(const int _nr) const;
      int get_genes_nr(void) const; // Get path lenght
      int get_elite_genes_nr(void) const;
      void get_start_coord(int _coordinates[]);
      void get_home_coord(int _coordinates[]);
      void get_final_coord(int _coordinates[]);
      void get_current_ideal_coord(const int _index, int _coord[]) const;
};
typedef Elite_Invidivual c_Best_route;
//*******************************************************************
// Elite_Invidivual Class Declaration: END
//*******************************************************************


#endif   // GALRN_H
