//*******************************************************************
// Title: GALRN.cpp
//
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

// Libraries declaration
#include "GALRN.h"


//===================================================================



//*******************************************************************
// Population Class Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
// Default Constructor

Organism::Organism() {
    // It must be consistent with the value in Population
    genesNumber = GENES_NUMBER;

    fitness = -1.0;
    ranking = -1;
    slice = 0.0;
    useful_steps_nr = genesNumber;

    for (int i = 0; i < genesNumber; i++) {
        a_genes[i] = (_gen) RandomRange(ACTIONS_NUMBER - 1);
    }
}


//-------------------------------------------------------------------
// Destructor

Organism::~Organism() {
    ;
}

//-------------------------------------------------------------------

void Organism::PrintGenome(const char a_fileName[]) const {
    printf("GENOME of INDIVIDUAL (GENOTYPE):\n");

    if (strlen(a_fileName) != 0) {
        printf("PRINTING IN FILE: %s\n", a_fileName);
        FILE *fp;
        if ((fp = fopen(a_fileName, "w"))) {
            fprintf(fp, "\nGENOME\n");
            for (int _igen = 0; _igen < genesNumber; _igen++)
                fprintf(fp, "%x  ", a_genes[_igen]);
            fclose(fp);
        } else {
            printf("Error opening FILE in function: Organism::PrintGenome\n");
        }
    } else {
        for (int _igen = 0; _igen < genesNumber; _igen++)
            printf("%x  ", a_genes[_igen]);
    }
}

//-------------------------------------------------------------------

_gen Organism::getGene(int _igen) const {
    return (this->a_genes[_igen]);
}
//-------------------------------------------------------------------

double Organism::getFitness(void) const {
    return (this->fitness);
}
//-------------------------------------------------------------------

int Organism::getGenesNumber(void) const {
    return (this->genesNumber);
}
//-------------------------------------------------------------------

int Organism::getRanking(void) const {
    return (this->ranking);
}
//-------------------------------------------------------------------

double Organism::getSlice(void) const {
    return (this->slice);
}
//-------------------------------------------------------------------

int Organism::get_useful_steps_nr(void) const {
    return (this->useful_steps_nr);
}
//-------------------------------------------------------------------

int Organism::get_min_goal_distance(void) const {
    return (this->min_goal_distance);
}
//-------------------------------------------------------------------

void Organism::get_final_position(int _coordinates[]) const {
    _coordinates[0] = final_position[0];
    _coordinates[1] = final_position[1];
    _coordinates[2] = final_position[2];
}
//-------------------------------------------------------------------

void Organism::get_fulfillment_products(double _fulfillment[]) const {
    _fulfillment[0] = fulfillment_products[0];
    _fulfillment[1] = fulfillment_products[1];
    _fulfillment[2] = fulfillment_products[2];
    _fulfillment[3] = fulfillment_products[3];
}

//-------------------------------------------------------------------

void Organism::setGene(int _igen, _gen _value) {
    this->a_genes[_igen] = _value;
}
//-------------------------------------------------------------------

void Organism::setFitness(double _value) {
    this->fitness = _value;
}
//-------------------------------------------------------------------

void Organism::setRanking(int _value) {
    this->ranking = _value;
}
//-------------------------------------------------------------------

void Organism::setSlice(double _value) {
    this->slice = _value;
}
//-------------------------------------------------------------------

void Organism::setGenesNumber(int _value) {
    this->genesNumber = _value;
}
//-------------------------------------------------------------------

void Organism::set_useful_steps_nr(int _value) {
    this->useful_steps_nr = _value;
}
//-------------------------------------------------------------------

void Organism::set_min_goal_distance(int _value) {
    this->min_goal_distance = _value;
}
//-------------------------------------------------------------------

void Organism::set_final_position(const int _coordinates[]) {
    final_position[0] = _coordinates[0];
    final_position[1] = _coordinates[1];
    final_position[2] = _coordinates[2];
}
//-------------------------------------------------------------------

void Organism::set_fulfillment_products(const double _fulfillment[]) {
    fulfillment_products[0] = _fulfillment[0];
    fulfillment_products[1] = _fulfillment[1];
    fulfillment_products[2] = _fulfillment[2];
    fulfillment_products[3] = _fulfillment[3];
}
//*******************************************************************
// Population Class Implementation: END
//*******************************************************************



//*******************************************************************
// Population Class Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------

Population::Population() {
    populationSize = POPULATION;
    generationsNumber = GENERATIONS;
    genesNumber = GENES_NUMBER;
    selectionMethod = TOURNAMENT_ELITE;
    eliteNumber = ELITE;
    elite_fixed = FIXED_ELITE;
    randomImmigrants = IMMIGRANTS;
    tournamentParam = TOURNAMENT_PARAM;
    crossPointsNumber = CROSS_POINTS;
    crossoverProbability = CROSSOVER_RATE;
    mutationsRate = MUTATION_RATE;

    selected = 0;
    sumFitness = 0.0;
    avgFitness = 0.0;
}


//-------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------

Population::~Population() {
    ;
    //   delete [] ao_pool;
    //   delete [] ao_matingPool;
    //   delete [] ao_stats;
    //
    //   delete [] a_popList;
    //   delete [] a_points;
}


//-------------------------------------------------------------------
// Needed only to make successive experiments

void Population::Reinit(void) {
    // Crossover points
    a_points[0] = genesNumber / (crossPointsNumber);

    for (int _ipts = 1; _ipts < crossPointsNumber - 1; _ipts++)
        a_points[_ipts] = a_points[_ipts - 1] + genesNumber / (crossPointsNumber);

    a_points[crossPointsNumber - 1] = genesNumber;

    // Ranking list
    for (int _iorg = 0; _iorg < this->populationSize; _iorg++) {

        this->a_popList[_iorg] = _iorg;

        for (int _igen = 0; _igen < genesNumber; _igen++)
            this->ao_pool[_iorg].setGene(_igen, (_gen) RandomRange(ACTIONS_NUMBER - 1));

        this->ao_pool[_iorg].setFitness(-1.0);
        this->ao_pool[_iorg].setRanking(-1);
    }
}

//-------------------------------------------------------------------
// void Create()
// This function create the population after the pool variables are seted
//-------------------------------------------------------------------

void Population::Create(void) {
    // Ranking list
    for (int _iorg = 0; _iorg < this->populationSize; _iorg++)
        this->a_popList[_iorg] = _iorg;

    // Crossover points
    a_points[0] = genesNumber / (crossPointsNumber);
    for (int _ipts = 1; _ipts < crossPointsNumber - 1; _ipts++)
        a_points[_ipts] = a_points[_ipts - 1] + genesNumber / (crossPointsNumber);
    a_points[crossPointsNumber - 1] = genesNumber;

    // Initialize population
    // If dynamic arrays are used, e.g. with vectors
    // ao_pool
    // ao_matingPool
    // ao_stats

#ifdef T_CREATE
    printf("\nCREATE POPULATION:");
    this->DisplayPool(VERBOSE_LOW);
#endif   // end T_CREATE
    //···································································
}


//-------------------------------------------------------------------

void Population::Evolve(const char _verbose) {
    //···································································
#ifndef T_EVOLVE


    //   std::cout << "Evaluate BEFORE starting GA" << std::endl;
    // Fix the movements (left followed right) of the top 1 ranking organism	
    this->CleanGenes();
    // Evaluate de fitness of the first generation
    this->Evaluate();
    this->DisplayPool(_verbose);
    //   std::cout << "Displaying pool AFTER evaluate:"<<std::endl;


    // Only when Elite is used the ranking is calculated and used
    for (int _ign = 0; _ign < generationsNumber; _ign++) {
        //   printf("generationsNumber = %d\n", generationsNumber);
        if (_verbose >= VERBOSE_MIN)
            printf("(%3d) - COMPUTING GENERATION: %5d FROM %5d\n", DEBUG_ITERATIONS, _ign, generationsNumber);

        this->Select(selectionMethod);
        this->Crossover();
        this->Mutate();
        //        this->CleanGenes();
        this->Evaluate();
        this->DisplayPool(_verbose);

        this->RankingUpdate(); // Sort the population and Update sumFitness and avgFitness
        if (ao_pool[a_popList[0]].getFitness() <= 0.0)
            this->Sort_by_useful_steps(0, populationSize - 1); // Sort the population and Update sumFitness and avgFitness

        this->SetGenerationStats(_ign); // Set Statistics


        if (_verbose >= VERBOSE_LOW) {
            std::cout << "AVERAGE FITNESS OF THIS GENERATION: " << avgFitness << std::endl;
            std::cout << "FITNESS FITTEST INDIVIDUAL OF THIS GENERATION: " << ao_pool[a_popList[0]].getFitness()<< std::endl;
            printf("FITNESS FITTEST INDIVIDUAL OF THIS GENERATION: %1.10f\n", ao_pool[a_popList[0]].getFitness());
        } // end if VERBOSE_HIGH
    } // end for GENERATIONS
    std::cout << "DISPLAY BEST INDIVIDUAL :"<<std::endl;

    ao_pool[a_popList[0]].PrintGenome("");

    if (_verbose >= VERBOSE_HIGH) {
        Print_Outputs_Files();
    }

    //···································································
#else
    printf("\nTEST EVOLVE:");
    printf("\nCleanGenes:");
    this->CleanGenes();
    printf("\nCreate:");
    this->Create();
    printf("\nEvaluate:");
    this->Evaluate();
    printf("\nSelect: %d", selectionMethod);
    this->Select(selectionMethod);
    printf("\nCrossover:");
    this->Crossover();
    printf("\nMutate:");
    this->Mutate();
    printf("\nEvaluate:");
    this->Evaluate();
    this->DisplayPool(VERBOSE_MIN);

    printf("\nTEST FITNESS FUNCTION:");
    this->RankingUpdate(); // Sort the population and Update sumFitness and avgFitness
    this->o_ffitness.CalcFitness(this->ao_pool[a_popList[0]]);
#endif   // end T_EVOLVE
    //···································································
    printf("*********************************************************************\n");
    DEBUG_ITERATIONS++;
}
/**
 * \brief Fix the left followed by right movement of the top 1 organism  
 * Whenever there are a TURN_RIGHT followed by TURN_LEFT, or viceversa, it avoid this step and decreases 
 * the useful steps of the. Only takes the top 1 organism.
 * \param none
 * \return none
 */
void Population::CleanGenes() {
    //···································································
#ifdef T_CLEAN_GENES
    printf("\nTEST CLEAN GENES:");
#endif   // end T_CLEAN_GENES
    //···································································

    int _iorg = a_popList[0]; //just for the number 1 ranking
    _gen _temp_a_genes[GENES_NUMBER];
    int _igen = 0;
    _gen _action = 0;
    int _genesNumber = 0;
    int _index_temp = 0;

    _genesNumber = ao_pool[_iorg].get_useful_steps_nr();
    _index_temp = 0;

    for (_igen = 0; _igen < _genesNumber; _igen++) {
        _action = ao_pool[_iorg].getGene(_igen);


        if ((_index_temp > 0) &&
                ((_action == TURN_RIGHT && _temp_a_genes[_index_temp-1] == TURN_LEFT) || \
                 (_action == TURN_LEFT  && _temp_a_genes[_index_temp-1] == TURN_RIGHT)))
            _index_temp--;
        //          if ((_index_temp > 0) &&
        //              ((_action == TURN_RIGHT_1 && _temp_a_genes[_index_temp-1] == TURN_LEFT_1)  ||
        //               (_action == TURN_LEFT_1  && _temp_a_genes[_index_temp-1] == TURN_RIGHT_1) ||
        //               (_action == TURN_RIGHT_2 && _temp_a_genes[_index_temp-1] == TURN_LEFT_2)  ||
        //               (_action == TURN_LEFT_2  && _temp_a_genes[_index_temp-1] == TURN_RIGHT_2) ||
        //               (_action == TURN_RIGHT_3 && _temp_a_genes[_index_temp-1] == TURN_LEFT_3)  ||
        //               (_action == TURN_LEFT_3  && _temp_a_genes[_index_temp-1] == TURN_RIGHT_3)))
        //                    _index_temp--;
        else {
            _temp_a_genes[_index_temp] = _action;
            _index_temp++;
        }
    }

    for (_igen = 0; _igen < _index_temp; _igen++)
        ao_pool[_iorg].setGene(_igen, _temp_a_genes[_igen]);

    ao_pool[_iorg].set_useful_steps_nr(_index_temp); //NO SE CAMBIA EL NUMERO TOTAL DE GENES!!!!

}


/**
 * \brief Set  fitness, useful steps, minimun goal distance to mission and final position 
 * on each organism of the population.
 * \return 0
 */
int Population::Evaluate() {

    int _iorg;
    int _goal_step;
    int _goal_distance;
    int _final_coord[3];
    double _fitness = -1.0;


    for (_iorg = 0; _iorg < populationSize; _iorg++) {
        //      if( this->getFitness(_iorg) == -1.0)
        //      {
        _fitness = this->o_ffitness.CalcFitness(ao_pool[_iorg]);
        _goal_step = genesNumber;
        _goal_distance = -1;
        this->o_ffitness.o_virtualMotion.get_current_position(_final_coord);
        _goal_step = this->o_ffitness.o_virtualMotion.get_useful_steps_nr();

        if (this->o_ffitness.a_motivations[3] > 0.0) {
            _goal_step = this->o_ffitness.o_virtualMotion.get_min_missions_steps();
            _goal_distance = this->o_ffitness.o_virtualMotion.get_min_missions_distance();
            this->o_ffitness.o_virtualMotion.get_mission_closer_position(_final_coord);
        }

        this->ao_pool[_iorg].set_fulfillment_products(this->o_ffitness.a_fitness);
        this->ao_pool[_iorg].setFitness(_fitness);
        this->ao_pool[_iorg].set_useful_steps_nr(_goal_step);
        this->ao_pool[_iorg].set_min_goal_distance(_goal_distance);
        this->ao_pool[_iorg].set_final_position(_final_coord);
        //      }  // end if(this->getFitness(_iorg) == -1)
    } // end for(_iorg = 0; _iorg < populationSize; _iorg++)

    return (populationSize - _iorg);
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------

void Population::Select(char _method) {
    this->selected = 0;

    if (_method == ROULETTE) {
        this->Roulette(); // ### Roulette Wheel ###
    }// end if( _method == ROULETTE )

    else if (_method == ROULETTE_ELITE) {
        this->Elite(); // ### Elite ###
        this->Roulette(); // ### Roulette Wheel ###
    }// end if( _method == ROULETTE_ELITE )

    else if (_method == TOURNAMENT) {
        this->Tournament(); // ### Tournament ###
    }// end else if( _method == TOURNAMENT )

    else if (_method == TOURNAMENT_ELITE) {
        this->Elite(); // ### Elite ###
        this->Tournament(); // ### Tournament ###
    }// end else if( _method == TOURNAMENT_ELITE )

    else if (_method == TOURNAMENT_ELITE_RANDOM) {
        this->Elite(); // ### Elite ###
        this->RandomImmigrants(); // ### Random Immigrants ###
        this->Tournament(); // ### Tournament ###
    } // end else if( _method == TOURNAMENT_ELITE_RANDOM )

    //···································································
#ifdef T_SELECT
    this->DisplayMating(1);
#endif   // end T_SELECT
    //···································································
} // End Function



//-------------------------------------------------------------------
// void Crossover(void)
// Elite and/or  random Immigrants are placed in the new pool if there are
// The new pool is complited with the previouly selected mating pool
//-------------------------------------------------------------------

void Population::Crossover(void) {
    //···································································
#ifdef T_CROSSOVER   // Show info for DEBUG PURPOSE only
    printf("\nTEST CROSSOVER");
    for (int _ipts = 0; _ipts < crossPointsNumber; _ipts++) {
        printf("\n\tCross Points Total Number(%3d):   GEN:%5d", _ipts, a_points[_ipts]);
    }
#endif   // end T_CROSSOVER
    //···································································

    int _parents[2], _children[2], _firstOrg;

    if ((selectionMethod == ROULETTE_ELITE) || (selectionMethod == TOURNAMENT_ELITE)) {
        _firstOrg = eliteNumber + elite_fixed;
    }
    else if ((selectionMethod == TOURNAMENT_ELITE_RANDOM)) {
        _firstOrg = eliteNumber + elite_fixed + randomImmigrants;
    }
    else if ((selectionMethod == ROULETTE) || (selectionMethod == TOURNAMENT)) {
        _firstOrg = 0;
    } // end ELSE IF

    if ((_firstOrg % 2) == 1) {
        _firstOrg++;
    }

    // Make clone of Elite Individual (put them into new population)
    for (int _ie = 0; _ie < _firstOrg; _ie++)
        Duplicate_Individual(_ie, _ie);

    for (int _iorg = _firstOrg; _iorg < populationSize; _iorg += 2) {
        _parents[0] = _iorg;
        _parents[1] = _iorg + 1;
        _children[0] = _iorg;
        _children[1] = _iorg + 1;
        this->CrossParents(_parents, _children, a_points);
    }
    //printf("\nCROOSOVER LOGIC FOR ELITE PLUS ROULETTE OR TOURNAMENT: %5d", _firstOrg);
}


//-------------------------------------------------------------------
// void Mutate(void)
// Every single acction can be mutated with a mutationsRate probability
//-------------------------------------------------------------------

void Population::Mutate(void) {
    //···································································
#ifdef T_MUTATION    // Show info for DEBUG PURPOSE only
    printf("\nTEST MUTATION WITH PROBABILITY (%f):", this->getMutationsRate());
#endif   // end T_MUTATION
    //···································································
    _gen _newGen;
    int _start_individual = 0;

    if ((selectionMethod == ROULETTE_ELITE) || (selectionMethod == TOURNAMENT_ELITE)) {
        _start_individual = eliteNumber + elite_fixed;
    }
    else if ((selectionMethod == TOURNAMENT_ELITE_RANDOM)) {
        _start_individual = eliteNumber + elite_fixed + randomImmigrants;
    }
    else if ((selectionMethod == ROULETTE) || (selectionMethod == TOURNAMENT)) {
        _start_individual = 0;
    } // end ELSE IF

    if ((_start_individual % 2) == 1) {
        _start_individual++;
    }

    for (int _iorg = _start_individual; _iorg < populationSize; _iorg++) {
        // If the mutation rate is mached then a randomly selected gene is mutated.
        if (doubleRandom01(6) < this->mutationsRate) {
            //···································································
#ifdef T_MUTATION    // Show info for DEBUG PURPOSE only
            printf("\nORGANISM BEFORE %5d of %5d: MUTATED GEN: %3d\n", _iorg, populationSize - 1, _newGen);
            for (int _igen = 0; _igen < genesNumber; _igen++)
                printf("%x  ", this->ao_pool[_iorg].getGene(_igen));
#endif   // end T_MUTATION
            //···································································
            for (int _igen = 0; _igen < genesNumber; _igen++) {
                if (doubleRandom01(6) < this->mutationsRate) {
                    _newGen = (_gen) RandomRange(ACTIONS_NUMBER - 1);
                    this->setGene(_iorg, _igen, _newGen);
                }
            }
            // If the individual is mutated a new fitness value must be computed
            this->setFitness(_iorg, -1);
            //···································································
#ifdef T_MUTATION    // Show info for DEBUG PURPOSE only
            printf("\nORGANISM AFTER %5d of %5d\n", _iorg, populationSize - 1);
            for (int _igen = 0; _igen < genesNumber; _igen++)
                printf("%x  ", this->a_pool[_iorg].getGene(_igen));
#endif   // end T_MUTATION
            //···································································
        } // end if( doubleRandom01(6) < _rate )
    } // end for(int _iorg = 0; _iorg < populationSize; _iorg++)
}


//-------------------------------------------------------------------
// void Elite(void)
// The best N Individuals are placed into the mating pool and then into the next generation
// The population must be sorted.
//-------------------------------------------------------------------

void Population::Elite(void) {
    this->RankingUpdate(); // Sort the population and Update sumFitness and avgFitness
    //···································································
#ifdef T_ELITE_SELECTION   // Show info for DEBUG PURPOSE only
    printf("\nTEST SELECT USING ELITE (%d):", this->eliteNumber);
#endif   // end T_ELITE_SELECTION
    //···································································
    // The Elite members are mapped into the Mating pool
    for (int _iorg = 0; _iorg < eliteNumber; _iorg++) {
        //···································································
#ifdef T_ELITE_SELECTION   // Show info for DEBUG PURPOSE only
        printf("\nELITE SELECTED: %3d   ORG: %2d   Ranking: %d",
                selected, a_popList[_iorg], this->ao_pool[a_popList[_iorg]].getRanking());
#endif   // end T_ELITE_SELECTION
        //···································································
        this->ToMating(a_popList[_iorg]); // ToMating() increases selected
    } // end for(_iorg = 0; _iorg < eliteNumber; _iorg++)


#ifdef USE_ELITE_FIX_FUNCTION
    if (elite_fixed != 0) {
        // The Elite member are mapped into the Mating pool
        for (int _iorg = 0; _iorg < elite_fixed; _iorg++)
            this->ToMating(a_popList[0]); // ToMating() increases selected

        Elite_Fix_Function();
    }
#endif
}


//-------------------------------------------------------------------
// void Elite_Fix_Function(void);
// This Function generate new random genes for the elite clons,
// when a "bad" genes combination is found
//-------------------------------------------------------------------

void Population::Elite_Fix_Function(void) {
    //···································································
#ifdef T_ELITE_FIX_FUNCTION
    printf("\nTEST ELITE FIX FUNCTION");
#endif   // end T_ELITE_FIX_FUNCTION
    //···································································
    _gen _action;
    int _igen = 0;
    bool _change = false;
    double _sum_angle_left = 0.0;
    double _sum_angle_right = 0.0;

    while (_igen < genesNumber) {
        while (((_action = ao_matingPool[0].getGene(_igen++)) == FORWARD)) {
            _sum_angle_left = 0.0;
            _sum_angle_right = 0.0;
        }
        if (_action == TURN_LEFT) _sum_angle_left += o_ffitness.o_virtualMotion.get_angle_length();
        if (_action == TURN_RIGHT) _sum_angle_right += o_ffitness.o_virtualMotion.get_angle_length();
        //      if( _action == TURN_LEFT_1 )  _sum_angle_left += o_ffitness.o_virtualMotion.get_angle_length();
        //      if( _action == TURN_LEFT_2 )  _sum_angle_left += o_ffitness.o_virtualMotion.get_angle_length()*2;
        //      if( _action == TURN_LEFT_3 )  _sum_angle_left += o_ffitness.o_virtualMotion.get_angle_length()*3;
        //      if( _action == TURN_RIGHT_1 )  _sum_angle_right += o_ffitness.o_virtualMotion.get_angle_length();
        //      if( _action == TURN_RIGHT_2 )  _sum_angle_right += o_ffitness.o_virtualMotion.get_angle_length()*2;
        //      if( _action == TURN_RIGHT_3 )  _sum_angle_right += o_ffitness.o_virtualMotion.get_angle_length()*3;
        if ((_sum_angle_right >= 180) || (_sum_angle_left >= 180)) {
            _change = true;
            _sum_angle_left = 0.0;
            _sum_angle_right = 0.0;
        }
        if ((_action == TURN_LEFT)) {
            if ((ao_matingPool[0].getGene(_igen) == TURN_RIGHT)) {
                _change = true;
            }
        }
        if (_action == TURN_RIGHT) {
            if (ao_matingPool[0].getGene(_igen) == TURN_LEFT)
            {
                _change = true;
            }
        }


        //      if( (_action == TURN_LEFT_1) || (_action == TURN_LEFT_2) || (_action == TURN_LEFT_3) )
        //      {
        //         if( (ao_matingPool[0].getGene(_igen) == TURN_RIGHT_1) ||
        //             (ao_matingPool[0].getGene(_igen) == TURN_RIGHT_2) ||
        //             (ao_matingPool[0].getGene(_igen) == TURN_RIGHT_3) )
        //         {
        //            _change = true;
        //         }
        //      }
        //      if( (_action == TURN_RIGHT_1) || (_action == TURN_RIGHT_2) || (_action == TURN_RIGHT_3) )
        //      {
        //         if( (ao_matingPool[0].getGene(_igen) == TURN_LEFT_1) ||
        //             (ao_matingPool[0].getGene(_igen) == TURN_LEFT_2) ||
        //             (ao_matingPool[0].getGene(_igen) == TURN_LEFT_3) )
        //
        //         {
        //            _change = true;
        //         }
        //      }

        if (_change == true) {
            _change = false;
            for (int _iorg = 1; _iorg <= elite_fixed; _iorg++) {
                ao_matingPool[_iorg].setGene(_igen - 1, (_gen) RandomRange(ACTIONS_NUMBER - 1)); // _action
                ao_matingPool[_iorg].setGene(_igen - 0, (_gen) RandomRange(ACTIONS_NUMBER - 1)); // _igen++
                ao_matingPool[_iorg].setFitness(-1.0); // The fitness of the clone must be recalculated
                ao_matingPool[_iorg].setRanking(-1); // The ranking of the clone must be recalculated
            }
        } // end if _change
    } // end While

    //···································································
#ifdef T_ELITE_FIX_FUNCTION
    for (int _iorg = 0; _iorg < (elite_fixed + eliteNumber); _iorg++) {
        ao_matingPool[_iorg].PrintGenome("./TEST_ELITE_FIX_FUNCTION");
    }
#endif   // end T_ELITE_FIX_FUNCTION
    //···································································
}


//-------------------------------------------------------------------
// void RandomImmigrants(void)
// The Number of Random Immigrants are created and placed into the Mating pool
//-------------------------------------------------------------------

void Population::RandomImmigrants(void) {
    //···································································
#ifdef T_IMMIGRANTS_SELECTION    // Show info for DEBUG PURPOSE only
    printf("\nTEST SELECT USING RANDOM IMMIGRANTS (%d):", this->randomImmigrants);
#endif   // end T_IMMIGRANTS_SELECTION
    //···································································
    // ### Random Immigrants: Mating Pool it is random fulled when is create ###
    for (int _iorg = 0; _iorg < randomImmigrants; _iorg++)
        ToMating(-1); // ToMating() increases selected
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------

void Population::Roulette(void) {
    double _slice = 0.0;
    this->SumTotalFitness();

    // The slice is calculated proportionaly to the individuals fitness
    // Then the individuals slices are mapped to contiguous segments of a line [0, 1]
    for (int _iorg = 0; _iorg < populationSize; _iorg++) {
        // The Slice is calculated and "concatenated"
        if (sumFitness > 0.0) {
            _slice += this->getFitness(this->a_popList[_iorg]) / sumFitness;
        } else {
            _slice += 0.0;
        }
        // Then the respective upper limit for each individual is set
        this->setSlice(this->a_popList[_iorg], _slice);
    } // end for(_iorg = 0; _iorg < populationSize; _iorg++)
    //···································································
#ifdef T_ROULETTE_SELECTION
    printf("\nTEST SELECT USING ROULETTE WHEEL:"); // Show info for DEBUG PURPOSE only
#endif   // end T_ROULETTE_SELECTION
    //···································································
#ifdef T_ROULETTE_SLICE    // Show info for DEBUG PURPOSE only
    int _orgN = 0;
    printf("\nORG: %3d   SLICE: %1.6f   ACCUM: %1.6f",
            a_popList[_orgN], getSlice(a_popList[_orgN]), getSlice(a_popList[_orgN]));
    for (_orgN = 1; _orgN < populationSize; _orgN++)
        printf("\nORG: %3d   SLICE: %1.6f   ACCUM: %1.6f",
                a_popList[_orgN], getSlice(a_popList[_orgN]) - getSlice(a_popList[_orgN - 1]), getSlice(a_popList[_orgN]));
#endif   // end T_ROULETTE_SLICE
    //···································································

    double tempRand1 = 0.0;
    while (selected != populationSize) // Until complete the mating pool
    {
        // A random number between 0.0 and 1.0 is generated
        tempRand1 = doubleRandom01(6);

        // Search into the pool for the first individual which
        // Slice is greater than tempRand1, This individual is
        // mapped into the mating pool
        int aux_selected = selected;
        for (int _iorg = 0; _iorg < populationSize; _iorg++) {
            //···································································
#ifdef T_ROULETTE_SELECTION   // Show info for DEBUG PURPOSE only
            printf("\nROULETTE SELECTED: %3d   ORG: %d   RAND: %f   SLICE: %f",
                    selected, ao_popList[_iorg], tempRand1, this->getSlice(a_popList[_iorg]));
#endif   // end T_ROULETTE_SELECTION
            //···································································
            if (tempRand1 <= this->getSlice(a_popList[_iorg])) {
                //···································································
#ifdef T_ROULETTE_SELECTION
                printf("  *"); // Show info for DEBUG PURPOSE only
#endif   // end T_ROULETTE_SELECTION
                //···································································
                this->ToMating(this->a_popList[_iorg]); // ToMating() increases selected
                break;
            } // end if( tempRand1 <= getSlice(_iorg) )
        } // end for(_iorg = 0; _iorg < populationSize; _iorg++)
        if (aux_selected == selected) {
            this->ToMating(this->a_popList[selected]);
        }
    } // end while( selected != populationSize )
}


//-------------------------------------------------------------------
// void Tournament(void)
// Pick individs randomly to create the new generation
//-------------------------------------------------------------------

void Population::Tournament(void) {
    int tempRand1, tempRand2;
    //···································································
#ifdef T_TOURNAMENT_SELECTION
    printf("\nTEST SELECT USING TOURNAMENT (%f):", this->tournamentParam);
#endif   // end T_TOURNAMENT_SELECTION
    //···································································
    while (selected != populationSize) {
        // Two individuals are chosen randomly
        tempRand1 = RandomRange(populationSize - 1);
        tempRand2 = RandomRange(populationSize - 1);

        // The two individuals are sorted (greater to leaser)
        if (getFitness(tempRand1) < getFitness(tempRand2)) {
            SWAP(int, tempRand1, tempRand2);
            //double aux = tempRand1;  tempRand1 = tempRand2;  tempRand2 = aux;
        } // end if( getFitness(tempRand1) < getFitness(tempRand2) )

        // A random number between 0 and 1 is chosen. If this number is leaser that Tournament Parameter,
        // the fitter of the two individuals is selected to be a parent and placed into the mating pool
        // otherwise the less fit individual is selected and placed into the mating pool
        if (doubleRandom01(6) < tournamentParam) {
            //···································································
#ifdef T_TOURNAMENT_SELECTION    // Show info for DEBUG PURPOSE only
            printf("\nTOURNAMENT SELECTED: %3d   ORG: %d   RANKING: %d",
                    selected, tempRand1, this->getRanking(tempRand1));
#endif   // end T_TOURNAMENT_SELECTION
            //···································································
            ToMating(tempRand1); // ToMating() increases selected
        }// end if( doubleRandom01(6) < tournamentParam )
        else {
            //···································································
#ifdef T_TOURNAMENT_SELECTION    // Show info for DEBUG PURPOSE only
            printf("\nTOURNAMENT SELECTED: %3d   ORG: %d   RANKING: %d",
                    selected, tempRand2, this->getRanking(tempRand2));
#endif   // end T_TOURNAMENT_SELECTION
            //···································································
            ToMating(tempRand2); // ToMating() increases selected
        } // end else of if( doubleRandom01(6) < tournamentParam )
    } // end while( selected != populationSize )
}


//-------------------------------------------------------------------
// _pointsNumber: it is the number of cross points
// the segments between the cross points are interspersed
// not all individuals mate every generation. If the random value is
// greater than crossoverProbability are placed unalterated into the
// next generation; Otherwise the crossover take place.
//-------------------------------------------------------------------

void Population::CrossParents(const int _parents[], const int _children[], const int _a_points[]) {
    //···································································
#ifdef T_CROSSOVER_PARENTS    // Show info for DEBUG PURPOSE only
    printf("\nTEST CROSSOVER PARENTS: %3d   %3d", _parents[0], _parents[1]);
    printf("\nTEST CROSSOVER WITH PROBABILITY (%f):", this->crossoverProbability);

    printf("\nParent Member %5d of %5d:  FITNESS: %2.3f   RANKING: %3d\n",
            _parents[0], populationSize - 1, this->ao_matingPool[_parents[0]].getFitness(), this->ao_matingPool[_parents[0]].getRanking());
    for (int _igen = 0; _igen < genesNumber; _igen++)
        printf("%x  ", this->ao_matingPool[_parents[0]].getGene(_igen));

    printf("\nParent Member %5d of %5d:  FITNESS: %2.3f   RANKING: %3d\n",
            _parents[1], populationSize - 1, this->ao_matingPool[_parents[1]].getFitness(), this->ao_matingPool[_parents[1]].getRanking());
    for (int _igen = 0; _igen < genesNumber; _igen++)
        printf("%x  ", this->ao_matingPool[_parents[1]].getGene(_igen));
#endif   // end T_CROSSOVER_PARENTS
    //···································································

    bool _wasItCross = false; // This couple was cross??
    int _igen = 0, _ipts = 0;
    while (_ipts < crossPointsNumber) {
        if (doubleRandom01(6) < crossoverProbability) {
            _wasItCross = true;
            while (_igen < _a_points[_ipts]) {
                ao_pool[_children[0]].setGene(_igen, ao_matingPool[_parents[1]].getGene(_igen));
                ao_pool[_children[1]].setGene(_igen, ao_matingPool[_parents[0]].getGene(_igen));
                _igen++;
            }
        }// end if
        else {
            while (_igen < _a_points[_ipts]) {
                ao_pool[_children[0]].setGene(_igen, ao_matingPool[_parents[0]].getGene(_igen));
                ao_pool[_children[1]].setGene(_igen, ao_matingPool[_parents[1]].getGene(_igen));
                _igen++;
            }
        } // end else
        _ipts++;
    } // end while( _igen < genesNumber )

    // After each cross the fitness is set to -1,
    // it means that the fitness value must be computed again
    if (_wasItCross) {
        ao_pool[_children[0]].setFitness(-1.0); // The fitness of the child must be calculated
        ao_pool[_children[1]].setFitness(-1.0); // The fitness of the child must be calculated
    }        // If no cross operation is perfermed the children
    // fitness is equal to the parents fitness
    else {
        ao_pool[_children[0]].setFitness(ao_matingPool[_parents[0]].getFitness());
        ao_pool[_children[1]].setFitness(ao_matingPool[_parents[1]].getFitness());
    }

    //···································································
#ifdef T_CROSSOVER_PARENTS    // Show info for DEBUG PURPOSE only
    printf("\nChild Member %5d of %5d:  FITNESS: %2.3f   RANKING: %3d\n",
            _children[0], populationSize - 1, this->ao_pool[_children[0]].getFitness(), this->ao_pool[_children[0]].getRanking());
    for (int _igen = 0; _igen < genesNumber; _igen++)
        printf("%x  ", this->ao_pool[_children[0]].getGene(_igen));

    printf("\nChild Member %5d of %5d:  FITNESS: %2.3f   RANKING: %3d\n",
            _children[1], populationSize - 1, this->ao_pool[_children[1]].getFitness(), this->ao_pool[_children[1]].getRanking());
    for (int _igen = 0; _igen < genesNumber; _igen++)
        printf("%x  ", this->ao_pool[_children[1]].getGene(_igen));
#endif   // end T_CROSSOVER_PARENTS
    //···································································
}


//-------------------------------------------------------------------
// void MakeClon(const int _parent, const int _child)
// Used to place the Elite and Random Immigrants into the new Population
//-------------------------------------------------------------------

void Population::Duplicate_Individual(const int _parent, const int _child) {
    for (int _igen = 0; _igen < genesNumber; _igen++)
        ao_pool[_child].setGene(_igen, ao_matingPool[_parent].getGene(_igen));

    // The children have the same fitness as the parent
    ao_pool[_child].setFitness(ao_matingPool[_parent].getFitness());
    ao_pool[_child].setRanking(-1); // The ranking of the child must be calculated
}


//-------------------------------------------------------------------

int Population::getPopulationSize(void) const {
    return (this->populationSize);
}
//-------------------------------------------------------------------

int Population::getGenerationsNumber(void) const {
    return (this->generationsNumber);
}
//-------------------------------------------------------------------

int Population::getGenesNumber(void) const {
    return (this->genesNumber);
}
//-------------------------------------------------------------------

char Population::getSelectionMethod(void) const {
    return (this->selectionMethod);
}
//-------------------------------------------------------------------

int Population::getEliteNumber(void) const {
    return (this->eliteNumber);
}
//-------------------------------------------------------------------

int Population::get_elite_fixed(void) const {
    return (this->elite_fixed);
}
//-------------------------------------------------------------------

int Population::getRandomImmigrants(void) const {
    return (this->randomImmigrants);
}
//-------------------------------------------------------------------

int Population::getCrossPointsNumber(void) const {
    return (this->crossPointsNumber);
}
//-------------------------------------------------------------------

double Population::getMutationsRate(void) const {
    return (this->mutationsRate);
}
//-------------------------------------------------------------------

double Population::getTournamentParam(void) const {
    return (this->tournamentParam);
}
//-------------------------------------------------------------------

double Population::getCrossoverProbability(void) const {
    return (this->crossoverProbability);
}
//-------------------------------------------------------------------

_gen Population::get_elite_step(int _step) const {
    return (this->ao_pool[a_popList[0]].getGene(_step));
}
//-------------------------------------------------------------------

void Population::get_e_final_position(int _coordinates[]) const {
    this->ao_pool[a_popList[0]].get_final_position(_coordinates);
}

//-------------------------------------------------------------------

int Population::get_e_useful_steps_nr(void) const {
    return (this->ao_pool[a_popList[0]].get_useful_steps_nr());
}
//-------------------------------------------------------------------

int Population::get_e_goal_distance(void) const {
    return (this->ao_pool[a_popList[0]].get_min_goal_distance());
}
//-------------------------------------------------------------------

double Population::get_e_fitness(void) const {
    return (this->ao_pool[a_popList[0]].getFitness());
}
//-------------------------------------------------------------------

void Population::get_e_fulfillment_prod(double _fulfillment[]) const {
    this->ao_pool[a_popList[0]].get_fulfillment_products(_fulfillment);
}
//-------------------------------------------------------------------

void Population::get_elite_genetype(_gen _genotype[]) const {
    for (int _igen = 0; _igen < genesNumber; _igen++) {
        _genotype[_igen] = this->ao_pool[a_popList[0]].getGene(_igen);
    }
}

//-------------------------------------------------------------------

void Population::setPopulationSize(int _value) {
    this->populationSize = _value;
    for (int _iorg = 0; _iorg < this->populationSize; _iorg++)
        this->a_popList[_iorg] = _iorg;

    // Re-Initialize population
}

//-------------------------------------------------------------------

void Population::setGenerationsNumber(int _value) {
    this->generationsNumber = _value;
}
//-------------------------------------------------------------------

void Population::setGenesNumber(int _value) {
    this->genesNumber = _value;
    for (int _iorg = 0; _iorg < this->populationSize; _iorg++)
        ao_pool[_iorg].setGenesNumber(genesNumber);
}
//-------------------------------------------------------------------

void Population::setSelectionMethod(char _value) {
    this->selectionMethod = _value;
}
//-------------------------------------------------------------------

void Population::setEliteNumber(int _value) {
    this->eliteNumber = _value;
}
//-------------------------------------------------------------------

void Population::set_elite_fixed(int _value) {
    this->elite_fixed = _value;
}
//-------------------------------------------------------------------

void Population::setRandomImmigrants(int _value) {
    this->randomImmigrants = _value;
}
//-------------------------------------------------------------------

void Population::setCrossPointsNumber(int _value) {
    this->crossPointsNumber = _value;
}
//-------------------------------------------------------------------

void Population::setMutationsRate(double _value) {
    this->mutationsRate = _value;
}
//-------------------------------------------------------------------

void Population::setTournamentParam(double _value) {
    this->tournamentParam = _value;
}
//-------------------------------------------------------------------

void Population::setCrossoverProbability(double _value) {
    this->crossoverProbability = _value;
}


//*******************************************************************
// Utilities Implementation for GA: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
// toMating(int _iorg):
// Place the _iorg individuals of the current pool into the Mating pool
// If _iorg is equal to -1 then a random Immigrant is placed into the mating pool
//
// If a copy is made, The Copy have the some fitness, ranking and genes
//-------------------------------------------------------------------

void Population::ToMating(int _iorg) {
    int _igen;
    // The _iorg individual is mapped to the Mating Pool
    if (_iorg >= 0) {
        // The Fitness and Ranking are mapped
        this->ao_matingPool[selected].setFitness(ao_pool[_iorg].getFitness());
        this->ao_matingPool[selected].setRanking(ao_pool[_iorg].getRanking());

        // The genes are mapped
        for (_igen = 0; _igen < genesNumber; _igen++)
            this->ao_matingPool[selected].setGene(_igen, ao_pool[_iorg].getGene(_igen));
    }// end if( _iorg >= 0 )
    // if _iorg is equal to -1 a RANDOM IMMIGRANT is placed in the mating pool
    else {
        // The genes are randomly created
        for (_igen = 0; _igen < genesNumber; _igen++)
            this->ao_matingPool[selected].setGene(_igen, (_gen) RandomRange(ACTIONS_NUMBER - 1));

        // In this moment the random immigrant doesnt have a Fitness and Ranking
        this->ao_matingPool[selected].setFitness(-1.0);
        this->ao_matingPool[selected].setRanking(-1);
    } // end else if( _iorg >= 0 )
    this->selected++;
}

//-------------------------------------------------------------------
// Quicksort: Sort population by useful steps nr: in greater to lesser order
//-------------------------------------------------------------------

void Population::Sort_by_useful_steps(int first, int last) {
    int left = first;
    int right = last;
    double pivot = this->ao_pool[a_popList[(first + last) / 2]].get_useful_steps_nr();
    int aux;

    do {
        while (this->ao_pool[a_popList[left]].get_useful_steps_nr() > pivot) left++;
        while (this->ao_pool[a_popList[right]].get_useful_steps_nr() < pivot) right--;
        if (left <= right) {
            aux = this->a_popList[right];
            this->a_popList[right] = this->a_popList[left];
            this->a_popList[left] = aux;
            left++;
            right--;
        } // end if( left <= right )
    } while (left <= right);

    if (first < right) this->Sort_by_useful_steps(first, right);
    if (last > left) this->Sort_by_useful_steps(left, last);
}


//-------------------------------------------------------------------
// Quicksort: Sort population by fitness: in greater to lesser order
// The Organism arent sorted, only an array with info of the position of the Organism,
// to see the sorted population you must take the info of the array
//-------------------------------------------------------------------

void Population::SortPopGL(int first, int last) {
    int left = first;
    int right = last;
    double pivot = this->getFitness(this->a_popList[(first + last) / 2]);
    int aux;

    do {
        while (this->getFitness(this->a_popList[left]) > pivot) left++;
        while (this->getFitness(this->a_popList[right]) < pivot) right--;
        if (left <= right) {
            aux = this->a_popList[right];
            this->a_popList[right] = this->a_popList[left];
            this->a_popList[left] = aux;
            left++;
            right--;
        } // end if( left <= right )
    } while (left <= right);

    if (first < right) this->SortPopGL(first, right);
    if (last > left) this->SortPopGL(left, last);
}


//-------------------------------------------------------------------
// void RankingUpdate(void):
// Update the ranking position to each Organism. This Method update
// the variables sumFitness and avgFitness and sort the population.
//-------------------------------------------------------------------

void Population::RankingUpdate(void) {
    this->SortPopGL(0, populationSize - 1);

    this->sumFitness = 0.0;
    // After sort the population (greater to lesser) the variable raking properly
    for (int _iorg = 0; _iorg < populationSize; _iorg++) {
        this->ao_pool[a_popList[_iorg]].setRanking(_iorg);
        this->sumFitness += ao_pool[_iorg].getFitness(); // Sum Fitness of every Organism
    } // end for(int _iorg = 0; _iorg < populationSize; _iorg++)
    this->avgFitness = this->sumFitness / this->populationSize;

    //···································································
#ifdef T_RANKING_UPDATE    // Show info for DEBUG PURPOSE only
    printf("\nTEST RANKING UPDATE:");
    for (int _iorg = 0; _iorg < populationSize; _iorg++) {
        printf("\nFitness for member %5d of %5d:  %2.3f \t Ranking: %d",
                _iorg, populationSize - 1, this->ao_pool[_iorg].getFitness(), this->ao_pool[_iorg].getRanking());
    }
#endif   // end T_RANKING_UPDATE
    //···································································
}


//-------------------------------------------------------------------
// sumTotalFitness(void)
// This method calcule the total fitness of the current generation
//-------------------------------------------------------------------

inline double Population::SumTotalFitness(void) {
    this->sumFitness = 0.0;

    // Sum Fitness of every Organism
    for (int _iorg = 0; _iorg < populationSize; _iorg++)
        this->sumFitness += this->ao_pool[_iorg].getFitness();

    return (sumFitness);
}


//-------------------------------------------------------------------
// avgTotalFitness(void)
// This method calcule the average fitness of the current generation
//-------------------------------------------------------------------

inline double Population::AvgTotalFitness(void) {
    this->avgFitness = this->SumTotalFitness() / populationSize;
    return (avgFitness);
}


//-------------------------------------------------------------------
// void SetGenerationStats(int _ign)
// Must be called after use the RankingUpdate function
//-------------------------------------------------------------------

void Population::SetGenerationStats(int _ign) {
    ao_stats[_ign].avgFitness = avgFitness;
    ao_stats[_ign].maxFitness = ao_pool[a_popList[0]].getFitness();
    ao_stats[_ign].minFitness = ao_pool[a_popList[populationSize - 1]].getFitness();
    ao_pool[a_popList[0]].get_fulfillment_products(ao_stats[_ign].fulfillment_products);

    int _auxValidIndividuals = 0;
    double _auxSumFitness = 0.0;
    double _minUtilFitness = 1.0;
    double _auxFitness = 0.0;

    for (int _iorg = 0; _iorg < populationSize; _iorg++) {
        _auxFitness = ao_pool[a_popList[_iorg]].getFitness();
        if (_auxFitness != 0.0) {
            _auxValidIndividuals++;
            _auxSumFitness += _auxFitness;
            if (_auxFitness < _minUtilFitness) _minUtilFitness = _auxFitness;
        }
    } // end FOR
    ao_stats[_ign].utilMembers = _auxValidIndividuals;
    ao_stats[_ign].avgRelFitness = _auxSumFitness / _auxValidIndividuals;
    ao_stats[_ign].minRelFitness = _minUtilFitness;
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------

void Population::DisplayPool(char _verbose) const {
    double _sumFitness = 0.0;


    std::cout <<"Pool - Actual Generation"<< std::endl;
    for (int _iorg = 0; _iorg < populationSize; _iorg++) {
        _sumFitness += ao_pool[_iorg].getFitness();

        // Show Individual Identificator
        if (_verbose >= VERBOSE_MIN) {
            std::cout << "Pool Member "<< _iorg << " of " << populationSize-1;

        } // end if( _verbose >= 0 )

        // Show Fitness and Ranking
        if (_verbose >= VERBOSE_LOW) {
            printf("\tFITNESS: %2.3f   RANKING: %5d   LENGTH: %5d\n",
                    this->ao_pool[_iorg].getFitness(), this->ao_pool[_iorg].getRanking(), this->ao_pool[_iorg].getGenesNumber());
        } // end if( _verbose >= 1 )

        // Show Genes
        if (_verbose >= VERBOSE_HIGH) {
            for (int _igen = 0; _igen < genesNumber; _igen++)
                printf("%x  ", this->ao_pool[_iorg].getGene(_igen));
        } // end if( _verbose >= 2 )
    } // end for (int _iorg = 0; _iorg < populationSize; _iorg++)

    // Show Population Statistics
    if (_verbose >= VERBOSE_MEDIUM) {
        printf("\nTotal Fitness of the current population: %f \n", _sumFitness);
        printf("Average Fitness of the current population: %f \n", _sumFitness / populationSize);
    } // end if( verbose >= 3 )
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------

void Population::DisplayMating(char _verbose) const {
    double _sumFitness = 0.0;

    printf("\n\t::MATING POOL::");
    for (int _iorg = 0; _iorg < populationSize; _iorg++) {
        _sumFitness += ao_matingPool[_iorg].getFitness();

        // Show Individual Identificator
        if (_verbose >= VERBOSE_MIN) {
            printf("\nMating Pool Member %5d of %5d: ", _iorg, populationSize - 1);
        } // if( _verbose >= 0 )

        // Show Fitness and Ranking
        if (_verbose >= VERBOSE_LOW) {
            printf("FITNESS: %2.3f   RANKING: %5d \n",
                    this->ao_matingPool[_iorg].getFitness(), this->ao_matingPool[_iorg].getRanking());
        } // if( _verbose >= 1 )

        // Show Genes
        if (_verbose >= VERBOSE_MEDIUM) {
            for (int _igen = 0; _igen < genesNumber; _igen++)
                printf("%x  ", this->ao_matingPool[_iorg].getGene(_igen));
        } // if( _verbose >= 2 )
    } // end for (int _iorg = 0; _iorg < populationSize; _iorg++)

    // Show Population Statistics
    if (_verbose >= VERBOSE_HIGH) {
        printf("\nTotal Fitness of the Mating population: %f \n", _sumFitness);
        printf("Average Fitness of the Mating population: %f \n", _sumFitness / populationSize);
    } // if( _verbose >= 3 )
    printf("\n");
}

//-------------------------------------------------------------------

//-------------------------------------------------------------------

void Population::Print_Outputs_Files(void) {
    char GENOTYPE_FILE[100];
    char PHENOTYPE_FILE[100];
    char STATS_FILE[100];
    char MAP_FILE[100];

    snprintf(GENOTYPE_FILE, sizeof (GENOTYPE_FILE), "./lrn_genotype_%d", DEBUG_ITERATIONS);
    snprintf(PHENOTYPE_FILE, sizeof (PHENOTYPE_FILE), "./lrn_phenotype_%d", DEBUG_ITERATIONS);
    snprintf(STATS_FILE, sizeof (STATS_FILE), "./lrn_stats_%d", DEBUG_ITERATIONS);
    snprintf(MAP_FILE, sizeof (MAP_FILE), "./lrn_MAP_%d", DEBUG_ITERATIONS);

    this->Print_Stats_File(STATS_FILE);
    this->Print_e_genotype_File(GENOTYPE_FILE);
    this->Print_e_phenotype_File(PHENOTYPE_FILE);
    this->Print_e_ascii_path_File(MAP_FILE);
}


//-------------------------------------------------------------------
// Apend the population stats in a file
//-------------------------------------------------------------------

void Population::Print_Stats_File(const char a_fileName[]) const {
    printf("GENERATION STATS:\n");

    if (strlen(a_fileName) != 0) {
        FILE *fp;
        if ((fp = fopen(a_fileName, "w"))) {
            printf("PRINTING IN FILE: %s\n", a_fileName);
            fclose(fp);
        } else {
            printf("Error opening FILE stats - FUNCTION: Population::PrintGenerationStats\n");
        }

        // AVGFITNESS  MAXFITNESS  MINFITNESS  UTILMEMBERS  AVGrelFITNESS  MINrelFITNESS
        for (int _ign = 0; _ign < generationsNumber; _ign++)
            ao_stats[_ign].PrintStats(a_fileName);
    } else {
        // AVGFITNESS  MAXFITNESS  MINFITNESS  UTILMEMBERS  AVGrelFITNESS  MINrelFITNESS
        for (int _ign = 0; _ign < generationsNumber; _ign++)
            ao_stats[_ign].PrintStats("");
    }
}
//-------------------------------------------------------------------

void Population::Print_e_genotype_File(const char a_fileName[]) const {
    this->ao_pool[a_popList[0]].PrintGenome(a_fileName);
}
//-------------------------------------------------------------------

void Population::Print_e_phenotype_File(const char a_fileName[]) {
    this->o_ffitness.PrintAgentPHENOTYPE(this->ao_pool[a_popList[0]], a_fileName);
}
//-------------------------------------------------------------------

void Population::Print_e_ascii_path_File(const char a_fileName[]) {
    this->o_ffitness.CalcFitness(this->ao_pool[a_popList[0]]);
    this->o_ffitness.PrintMAP(a_fileName);
}

//-------------------------------------------------------------------
// void UnsortSelected(int _value)
// unsort: This function is needed to mix the selected individuls in
// the mating pool (separated the elites and random immigrants members
//-------------------------------------------------------------------

void Population::UnsortSelected(int _value) {
    _gen auxGene;
    double auxFitness = 0.0;
    int auxRanking = 0;
    int newPosition = 0;

    for (int _iorg = 0; _iorg < _value; _iorg++) {
        newPosition = RandomRange(populationSize - 1);

        // swap Fitness
        auxFitness = this->ao_matingPool[newPosition].getFitness();
        this->ao_matingPool[newPosition].setFitness(this->ao_matingPool[_iorg].getFitness());
        this->ao_matingPool[_iorg].setFitness(auxFitness);

        // swap Ranking
        auxRanking = this->ao_matingPool[newPosition].getRanking();
        this->ao_matingPool[newPosition].setRanking(this->ao_matingPool[_iorg].getRanking());
        this->ao_matingPool[_iorg].setRanking(auxRanking);

        // swap genes
        for (int _igen = 0; _igen < genesNumber; _igen++) {
            auxGene = this->ao_matingPool[newPosition].getGene(_igen);
            this->ao_matingPool[newPosition].setGene(_igen, this->ao_matingPool[_iorg].getGene(_igen));
            this->ao_matingPool[_iorg].setGene(_igen, auxGene);
        } // end for(int _igen = 0; _igen < genesNumber; _igen++)
    } // end for(int _iorg = 0; _iorg < _value; _iorg++)
}
//-------------------------------------------------------------------
// Utilities Implementation for GA: END
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Organism related function: BEGIN
//-------------------------------------------------------------------

_gen Population::getGene(int _iorg, int _igen) const {
    return (this->ao_pool[_iorg].getGene(_igen));
}
//-------------------------------------------------------------------

double Population::getFitness(int _iorg) const {
    return (this->ao_pool[_iorg].getFitness());
}
//-------------------------------------------------------------------

int Population::getRanking(int _iorg) const {
    return (this->ao_pool[_iorg].getRanking());
}
//-------------------------------------------------------------------

double Population::getSlice(int _iorg) const {
    return (this->ao_pool[_iorg].getSlice());
}

//-------------------------------------------------------------------

void Population::setGene(int _iorg, int _igen, _gen _value) {
    this->ao_pool[_iorg].setGene(_igen, _value);
}
//-------------------------------------------------------------------

void Population::setFitness(int _iorg, double _value) {
    this->ao_pool[_iorg].setFitness(_value);
}
//-------------------------------------------------------------------

void Population::setRanking(int _iorg, int _value) {
    this->ao_pool[_iorg].setRanking(_value);
}
//-------------------------------------------------------------------

void Population::setSlice(int _iorg, double _value) {
    this->ao_pool[_iorg].setSlice(_value);
}
//-------------------------------------------------------------------
// Organism related function: END
//-------------------------------------------------------------------
//*******************************************************************
// Population Class Implementation: END
//*******************************************************************


//*******************************************************************
// POPULATION STATS Class Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------

Pop_Stats::Pop_Stats() {
    avgFitness = 0.0;
    maxFitness = 0.0;
    minFitness = 0.0;
    avgRelFitness = 0.0;
    minRelFitness = 0.0;
    fulfillment_products[0] = 0.0;
    fulfillment_products[1] = 0.0;
    fulfillment_products[2] = 0.0;
    fulfillment_products[3] = 0.0;
}

//-------------------------------------------------------------------

Pop_Stats::~Pop_Stats() {
    ;
}


//-------------------------------------------------------------------

void Pop_Stats::PrintStats(const char a_fileName[]) const {
    if (strlen(a_fileName) != 0) {
        // MAXFITNESS   AVGFITNESS  UTILMEMBERS   AVGrelFITNESS   MINrelFITNESS      Curiosity   Energy   Homing   Missions
        //printf("PRINTING IN FILE: ../Scilab.Scripts/stats\n");
        FILE *fp;
        if ((fp = fopen(a_fileName, "a"))) {
            fprintf(fp, "%1.10f   %1.10f   %3d   %1.10f   %1.10f      %1.10f   %1.10f   %1.10f   %1.10f\n",
                    maxFitness, avgFitness, utilMembers, avgRelFitness, minRelFitness,
                    fulfillment_products[0], fulfillment_products[1], fulfillment_products[2], fulfillment_products[3]);
            fclose(fp);
        } else {
            printf("Error opening FILE stats - FUNCTION: Pop_Stats::PrintStats\n");
        }
    } else {
        printf("MAX_FIT    AVG_FIT    UTIL   AVGrel     MINrelFit      Curiosity   Energy   Homing   Missions\n");
        printf("%1.10f   %1.10f   %3d   %1.10f   %1.10f      %1.10f   %1.10f   %1.10f   %1.10f\n",
                maxFitness, avgFitness, utilMembers, avgRelFitness, minRelFitness,
                fulfillment_products[0], fulfillment_products[1], fulfillment_products[2], fulfillment_products[3]);
    }
}
//*******************************************************************
// POPULATION STATS Class Implementation: END
//*******************************************************************



//*******************************************************************
// FitnessFunction Class Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------

FitnessFunction::FitnessFunction() {
    a_fitness[0] = 0.0;
    a_fitness[1] = 0.0;
    a_fitness[2] = 0.0;
    a_fitness[3] = 0.0;
    a_motivations[0] = MOTIVATION_CURIOSITY;
    a_motivations[1] = MOTIVATION_ENERGY;
    a_motivations[2] = MOTIVATION_HOMING;
    a_motivations[3] = MOTIVATION_MISSIONS;
    NormalizeTo_1(a_motivations, 4, a_motivations, 4);

    //fart_categories = 0;
}

//-------------------------------------------------------------------
// Destructor

FitnessFunction::~FitnessFunction() {
    for (int _y = 0; _y < fart_categories; _y++)
        delete [] FART_MAP[_y];
    delete [] FART_MAP;
}

/**
 * \brief Calculates fitness of one organism
 * First execute de movementes of organism, then calculates its fitness according to the 
 * motivations. For the fitness it uses mandami fuzzy logic.
 * \param c_organism organism with the path.
 * \return _fitness one fitness value.
 */

double FitnessFunction::CalcFitness(const c_organism& agent) {


    this->o_virtualMotion.Executor(agent, a_fitness);
    //···································································
#ifdef T_MOTIVATION_FITNESS
    printf("\na_fitness: Curiosity: %f | Energy: %f | Homing: %f | Missions: %f +", a_fitness[0], a_fitness[1], a_fitness[2], a_fitness[3]);
    printf("\na_motivations: CURIOSITY: %f   ENERGY: %f   HOMING: %f   MISSIONS: %f +", a_motivations[0], a_motivations[1], a_motivations[2], a_motivations[3]);
#endif   // end T_MOTIVATION_FITNESS
    //···································································

    double temp_fitness[4];
    // MOTIVATION FULFILLMENT PRODUCT
    temp_fitness[0] = a_fitness[0] * a_motivations[0];
    temp_fitness[1] = a_fitness[1] * a_motivations[1];
    temp_fitness[2] = a_fitness[2] * a_motivations[2];
    temp_fitness[3] = a_fitness[3] * a_motivations[3];

    //···································································
#ifdef T_MOTIVATION_FITNESS
    printf("\nMFP temp_fitness: CURIOSITY: %f   ENERGY: %f   HOMING: %f   MISSIONS: %f *", temp_fitness[0], temp_fitness[1], temp_fitness[2], temp_fitness[3]);
#endif   // end T_MOTIVATION_FITNESS
    //···································································

    o_mandami.CalcOutputs(temp_fitness); // MOTIVATION FULFILLMENT ARE INPUTS TO MANDAMI

    double _fitness = o_mandami.getOutput(0);
    int valid_input = 0;
    for (int _i = 0; _i < 4; _i++)
        if (a_motivations[_i] > 0) valid_input++;
    _fitness *= 4 / valid_input;
    //   NormalizeTo_1(a_motivations, 4, &_fitness, 1);

    //···································································
#ifdef T_MANDAMI_FITNESS
    printf("\nTEST MANDAMI FITNESS\t%f", _fitness);
#endif   // end T_MANDAMI_FITNESS
    //···································································
    return (_fitness);
}


//-------------------------------------------------------------------
// void NormalizeMotivations(double _motivations[])
// This function is needed to make sure that the sum of the Motivations
// don't surpass 1.0
//-------------------------------------------------------------------

void FitnessFunction::NormalizeTo_1(double _inputsParam[], int _inSize, double _outputs[], int _outSize) {
    double _sum = 0.0;

    for (int _index = 0; _index < _inSize; _index++) {
        _sum += _inputsParam[_index];
    }

    for (int _index = 0; _index < _outSize; _index++) {
        //      _outputs[_index] = _outputs[_index] * (_inSize/_validInput);
        _outputs[_index] = _outputs[_index] / _sum;
    }
}


//-------------------------------------------------------------------
// void PrintAgentGENOTYPE(const c_organism& agent, const char a_fileName[])
// For Debug Purpose, print into a File the actions of the given individual
// If the file name array is empty the actions are printed in the standard output
//-------------------------------------------------------------------

void FitnessFunction::PrintAgentGENOTYPE(const c_organism& agent, const char a_fileName[]) const {
    printf("GENOTYPE:\n");
    int _genesNumber = agent.getGenesNumber();

    if (strlen(a_fileName) != 0) {
        printf("PRINTING IN FILE: %s\n", a_fileName);
        FILE *fp;
        if ((fp = fopen(a_fileName, "w"))) {
            for (int _igen = 0; _igen < _genesNumber; _igen++)
                fprintf(fp, "%x  ", agent.getGene(_igen));
            fclose(fp);
        } else {
            printf("Error opening FILE in function: PrintFittestPHENOTYPE\n");
        }
    } else {
        for (int _igen = 0; _igen < _genesNumber; _igen++)
            printf("%x  ", agent.getGene(_igen));
    }
}


//-------------------------------------------------------------------
// void PrintFittestPHENOTYPE(const c_organism& agent, const char a_fileName[])
// For Debug Purpose, Compute the route of an individuals and save the route and
// the maps into a file a_fileName
//-------------------------------------------------------------------

void FitnessFunction::PrintAgentPHENOTYPE(const c_organism& agent, const char a_fileName[]) {
    printf("PHENOTYPE:\n");
    int _genesNumber = agent.getGenesNumber();

    printf("PRINTING IN FILE: %s\n", a_fileName);
    FILE *fp;
    if ((fp = fopen(a_fileName, "w"))) {
        // ROBOT DIAMETER
        fprintf(fp, "%d\n", o_virtualMotion.get_robot_diameter());
        // ROBOT DIAMETER + SENSOR RANGE
        fprintf(fp, "%d\n", o_virtualMotion.get_robot_diameter_sensors());
        // ACTIONS OR STPES NUMBER
        fprintf(fp, "%d\n", _genesNumber);
        // START COORDINATES
        // Angle, START_X_COORD, START_Y_COORD
        fprintf(fp, "%d,%d,%d,%d\n", 0, o_virtualMotion.startPosition[0], o_virtualMotion.startPosition[1], o_virtualMotion.startPosition[2]);
        int _position[3];
        o_virtualMotion.get_start_position(_position);
        // // CURRENT COORDINATES OR TRAJECTORY
        for (int _igen = 0; _igen < _genesNumber; _igen++) {
            o_virtualMotion.ComputeNextPosition(_position, agent.getGene(_igen));
            // STEP, ANGLE, XCOORD, YCOORD
            fprintf(fp, "%d,%d,%d,%d\n", _igen, _position[0], _position[1], _position[2]); // CURRENT COORDINATES OR TRAJECTORY
        }
        // GOALS AREA
        fprintf(fp, "%ld\n", lround(o_virtualMotion.get_robot_diameter() * GOAL_SIZE));
        // HOMING COORDINATES
        fprintf(fp, "%d,%d,%d\n", o_virtualMotion.goalPosition[0], o_virtualMotion.goalPosition[1], o_virtualMotion.goalPosition[2]);
        // MISSIONS NUMBER
        fprintf(fp, "%d\n", o_virtualMotion.totalMissions);
        // MISSIONS COORDINATES
        this->CalcFitness(agent);
        for (int _i = 0; _i < o_virtualMotion.totalMissions; _i++) {
            // Mission Angle, X coord, Y coord,
            // Min distance achieved, Steps needed,
            // Best Position achieved Angle, X coord, Y coord
            fprintf(fp, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                    o_virtualMotion.missionPosition[_i][0], o_virtualMotion.missionPosition[_i][1], o_virtualMotion.missionPosition[_i][2],
                    o_virtualMotion.missionPosition[_i][3], o_virtualMotion.missionPosition[_i][4],
                    o_virtualMotion.missionPosition[_i][5], o_virtualMotion.missionPosition[_i][6], o_virtualMotion.missionPosition[_i][7]);
        }

        // PARA LOS ESCENARIOS DE PRUEBA LA CODIFICACION DE FART ES (X0, Y0; 1-X1, 1-Y1)
        fprintf(fp, "0.000,0.000,0.000,0.000\n");
        if (fart_categories == 0) {
            if (o_virtualMotion.get_selected_room() == H_ROOM) {
                for (int _cat = 0; _cat < CATEGORIES_H_ROOM; _cat++)
                    fprintf(fp, "%f,%f,%f,%f\n", FART_H_ROOM[_cat][0], FART_H_ROOM[_cat][1], FART_H_ROOM[_cat][2], FART_H_ROOM[_cat][3]);
            } else if (o_virtualMotion.get_selected_room() == L_ROOM) {
                for (int _cat = 0; _cat < CATEGORIES_L_ROOM; _cat++)
                    fprintf(fp, "%f,%f,%f,%f\n", FART_L_ROOM[_cat][0], FART_L_ROOM[_cat][1], FART_L_ROOM[_cat][2], FART_L_ROOM[_cat][3]);
            } else if (o_virtualMotion.get_selected_room() == O_ROOM) {
                for (int _cat = 0; _cat < CATEGORIES_O_ROOM; _cat++)
                    fprintf(fp, "%f,%f,%f,%f\n", FART_O_ROOM[_cat][0], FART_O_ROOM[_cat][1], FART_O_ROOM[_cat][2], FART_O_ROOM[_cat][3]);
            } else if (o_virtualMotion.get_selected_room() == S_ROOM) {
                for (int _cat = 0; _cat < CATEGORIES_S_ROOM; _cat++)
                    fprintf(fp, "%f,%f,%f,%f\n", FART_S_ROOM[_cat][0], FART_S_ROOM[_cat][1], FART_S_ROOM[_cat][2], FART_S_ROOM[_cat][3]);
            } else if (o_virtualMotion.get_selected_room() == T_ROOM) {
                for (int _cat = 0; _cat < CATEGORIES_T_ROOM; _cat++)
                    fprintf(fp, "%f,%f,%f,%f\n", FART_S_ROOM[_cat][0], FART_S_ROOM[_cat][1], FART_S_ROOM[_cat][2], FART_S_ROOM[_cat][3]);
            }
        }            // AQUI VA FART DEL MAPA INTERNO - GUARDAR COPIA INTERNA DEL MAPA DE GABRIEL
        else {
            // FART_ROOM
            for (int _cat = 0; _cat < fart_categories; _cat++)
                fprintf(fp, "%f,%f,%f,%f\n", FART_MAP[_cat][0], FART_MAP[_cat][1], FART_MAP[_cat][2], FART_MAP[_cat][3]);
        }
        fclose(fp);
    } else {
        printf("Error opening FILE in function: PrintFittestPHENOTYPE\n");
    }
}


//-------------------------------------------------------------------
// void PrintMAP(const char a_fileName[])
// DEBUG PURPOSE ONLY
// PrintMap into a File
//-------------------------------------------------------------------

void FitnessFunction::PrintMAP(const char a_fileName[]) {
    printf("TESTING ROBOT BEHAVIOR:\n");
    o_virtualMotion.PrintMAPtoFile(a_fileName);
}
/*! \brief Creates a new InternalMap
 *  Creates a new free map in the InternalMap class of widthxheight.
 * */

void FitnessFunction::Set_Map_Dimensions(const int _width, const int _height) {
    map_width = _width;
    map_height = _height;
    this->o_virtualMotion.o_MAP.Set_MAP_Dimension(map_width, map_height);
}

/*! \brief Set obstacles values to the InternalMap
 *   Giving a value of the mapper, xcoord and ycoord; the function puts the _point value
 *   either it is obstacle or unknow, in the InternalMap
 */
void FitnessFunction::Set_SLAM_MAP(const int _xs, const int _ys , const unsigned char _map[][4096]) {

    int _point;
    int _xc,_yc;
    int _ocount = 0;
    int _opercent = 0.5;
    int _mapMeshSize; 
    int _onr; 
    _mapMeshSize= this->o_virtualMotion.o_MAP.getMapMeshSize();
    _onr= _mapMeshSize*_mapMeshSize/2;

    for (int _y=_ys ; _y < _mapMeshSize + _ys; _y++){
        for(int _x=_xs; _x < _mapMeshSize + _xs; _x++){
            _point = _map[_x][_y];

            if(_point == 0) 
                _ocount++;

            if (_ocount >= _onr){
                _ocount = 0;
                _xc =_x/_mapMeshSize;
                _yc =_y/_mapMeshSize;
                this->o_virtualMotion.o_MAP.setcCellObstacle(_xc,_yc);
                _x = _mapMeshSize;
                _y = _mapMeshSize;


            }
        }
        // o_routes.o_ffitness.Set_SLAM_MAP(pCDALaser->map[_x][_y],_x,_y);
    }
}

void FitnessFunction::Set_Object_Obstacle(const int xleft, const int ydown, const int xrigth, const int yup) {
    for (int _y = ydown; _y < yup; _y++) {
        for (int _x = xleft; _x < xrigth; _x++) {
            this->o_virtualMotion.o_MAP.setcCellObstacle(_x, _y);
        }
    }
}
//-------------------------------------------------------------------

void FitnessFunction::Init_Fart_MAP(const int _size, const int _width, const int _height) {
    for (int _y = 0; _y < fart_categories; _y++)
        delete [] FART_MAP[_y];
    delete [] FART_MAP;

    fart_categories = _size;
    map_width = _width;
    map_height = _height;
    this->o_virtualMotion.o_MAP.Set_MAP(map_width, map_height);

    FART_MAP = new double*[fart_categories];
    for (int _y = 0; _y < fart_categories; _y++)
        FART_MAP[_y] = new double[4];
}


//-------------------------------------------------------------------
// Use once these variables are seted ::: fart_categories; **FART_MAP;

void FitnessFunction::Set_FART_MAP(void) {
    this->o_virtualMotion.o_MAP.CleanMap();

    int auxXS, auxYS, auxXE, auxYE;
    int _mesh_size = this->o_virtualMotion.o_MAP.getMapMeshSize();
    for (int _cat = 0; _cat < fart_categories; _cat++) {
        auxXS = lround((FART_MAP[_cat][0]) * map_width / _mesh_size);
        auxYS = lround((FART_MAP[_cat][1]) * map_height / _mesh_size);
        auxXE = lround((1 - FART_MAP[_cat][2]) * map_width / _mesh_size);
        auxYE = lround((1 - FART_MAP[_cat][3]) * map_height / _mesh_size);
        // For Debug Use: BEGIN
        if (auxXS < 0) fprintf(stdout, "FitnessFunction FART Map O: X0 = %d\n", auxXS);
        if (auxYS < 0) fprintf(stdout, "FitnessFunction FART Map O: Y0 = %d\n", auxYS);
        if (auxXE > this->o_virtualMotion.o_MAP.getMapXDim()) fprintf(stdout, "FitnessFunction FART Map O: X1 = %d\n", auxXE);
        if (auxYE > this->o_virtualMotion.o_MAP.getMapYDim()) fprintf(stdout, "FitnessFunction FART Map O: Y1 = %d\n", auxYE);
        // For Debug Use: END
        for (int _y = auxYS; _y < auxYE; _y++) {
            for (int _x = auxXS; _x < auxXE; _x++) {
                this->o_virtualMotion.o_MAP.setcCellObstacle(_x, _y);
            }
        }
    }
}


//-------------------------------------------------------------------

void FitnessFunction::Update_Motivations(const double _motivations[]) {
    a_motivations[0] = _motivations[0];
    a_motivations[1] = _motivations[1];
    a_motivations[2] = _motivations[2];
    a_motivations[3] = _motivations[3];
    NormalizeTo_1(a_motivations, 4, a_motivations, 4);
}
//*******************************************************************
// FitnessFunction Class Implementation: END
//*******************************************************************


//*******************************************************************
// VirtualExecutive Class Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
//-------------------------------------------------------------------

VirtualExecutive::VirtualExecutive() {
    // To speed up the calculations - the functions are call once
    // _velv: virtual executive local variables
    _velv_mapMeshSize = this->o_MAP.getMapMeshSize();
    _velv_mapWidth = this->o_MAP.getMapWidth();
    _velv_mapHeight = this->o_MAP.getMapHeight();
    _velv_mapXDim = this->o_MAP.getMapXDim();
    _velv_mapYDim = this->o_MAP.getMapYDim();

    crashFlag = false;
    sensorsRange = SENSOR_RANGE;
    robotRadius = lround(ROBOT_DIAMETER / 2.0);
    robotDiameter = ROBOT_DIAMETER;
    robotRadiusPlusSensors = robotRadius + SENSOR_RANGE;
    robotCellRadius = lround((robotRadius * 1.0) / _velv_mapMeshSize);
    robotCellDiameter = robotCellRadius * 2;
    robotCellRadiusPlusSensors = lround((robotRadiusPlusSensors * 1.0) / _velv_mapMeshSize);
    robotCellDiameterPlusSensors = robotCellRadiusPlusSensors * 2;

    // Energy Related Variables
    initialBattery = STARTING_BATTERY;
    currentBattery = initialBattery;
    // Homing Related Variables
    startPosition[0] = START_ANGLE;
    startPosition[1] = START_X_COORD;
    startPosition[2] = START_Y_COORD;
    goalPosition[0] = GOAL_ANGLE;
    goalPosition[1] = GOAL_X_COORD;
    goalPosition[2] = GOAL_Y_COORD;
    // Missions Related Variables
    currentPosition[0] = startPosition[0];
    currentPosition[1] = startPosition[1];
    currentPosition[2] = startPosition[2];
    totalMissions = 0;
    _genesNumber = GENES_NUMBER;

    step_lenght = STEP_SIZE;
    angle_length = ANGLE_SIZE;

    //o_MAP.SetTestScenario(SELECTED_ROOM);

    // Decreasing Priority of Mission; X Coordinate of the missions;
    // Y Coordinate of mission; Accomplishment (1)
    //char ***a_listMissions;
}

//-------------------------------------------------------------------

VirtualExecutive::~VirtualExecutive() {
    ;
}

//-------------------------------------------------------------------

void VirtualExecutive::CleanAll(void) {
    this->crashFlag = false; // Crashes
    this->o_MAP.CleanMap(); // Map

    this->currentBattery = initialBattery; // Battery

    currentPosition[0] = startPosition[0]; // Current Robot Position: Angle
    currentPosition[1] = startPosition[1]; // Current Robot Position: X Coord
    currentPosition[2] = startPosition[2]; // Current Robot Position: Y Coord


    goalPosition[0] = GOAL_ANGLE;     // Final Robot Position: Angle
    goalPosition[1] = GOAL_X_COORD;   // Final Robot Position: X Coord
    goalPosition[2] = GOAL_Y_COORD;   // Final Robot Position: Y Coord
    if (totalMissions != 0) {
        missionPosition[0][3] = -1; //fitness value
        this->Distance_Missions(currentPosition, 0);
    }
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

void VirtualExecutive::Executor(const c_organism& agent, double a_fitness[]) {
    //···································································
#ifdef T_EXECUTOR
    printf("\nTEST VIRTUAL EXECUTIVE");
#endif   // end T_EXECUTOR
    //···································································
    /* set all the map as free
     * set battery level to initials
     * set current posito as start position
     * i think this is not needed. Correction, is needed for the battery
     */
    this->CleanAll();


    this->crashFlag = false; // Crashes
    int _prevCoord[3]; // Angle, X Coord, Y Coord
    int _igen = 0;
    _gen _action = FREEZE;
    _genesNumber = agent.getGenesNumber();
    bool _emptyBatteryFlag = false;

    // compute each action of every gen
    while (!crashFlag && (_igen < _genesNumber) && !_emptyBatteryFlag)
    {
        _action = agent.getGene(_igen);
        // Compute next position
        _prevCoord[0] = currentPosition[0];
        _prevCoord[1] = currentPosition[1];
        _prevCoord[2] = currentPosition[2];

        if (_action != FREEZE) {

            this->ComputeNextPosition(currentPosition, _action);

            if (totalMissions != 0)
                this->Distance_Missions(currentPosition, _igen);

            // Verify any crash and set Visited and Revisited cells
            this->crashFlag = this->RobotSweptArea(_prevCoord, _action);
        }

        // Set Current Battery Level
        this->UpdateBatteryLevel(_action);
        if( currentBattery <= 0.0 )  _emptyBatteryFlag = true;

        //···································································
#ifdef T_EXECUTOR
        printf("\nGEN NUMBER: %i  ACTION %c   BATT:%d", _igen, _action, this->currentBattery);
        printf("\nSTART: ANGLE: %d   X COORD: %d   Y COORD: %d", _prevCoord[0], _prevCoord[1], _prevCoord[2]);
        printf("\nEND  : ANGLE: %d   X COORD: %d   Y COORD: %d", currentPosition[0], currentPosition[1], currentPosition[2]);
#endif   // end T_EXECUTOR
        //···································································
        _igen++;
    } // end for(int _igen = 0; _igen < agent.getGenesNumber(); _igen++)

    useful_steps_nr = _igen;
    if (totalMissions != 0) useful_steps_nr = missionPosition[0][4];

    if (this->crashFlag) {
        int _arraySize = 4;
        for (int _i = 0; _i < _arraySize; _i++)
            a_fitness[_i] = 0.0;
    } else {
        a_fitness[0] = this->Calc_Fitness_Curiosity();
        a_fitness[1] = this->Calc_Fitness_Energy();
        a_fitness[2] = this->Calc_Fitness_Homing();
        a_fitness[3] = this->Calc_Fitness_Mission();
        //···································································
#ifdef T_MOTIVATION_FITNESS
        printf("\nCURIOSITY: %f   ENERGY: %f   HOMING: %f   MISSIONS: %f", a_fitness[0], a_fitness[1], a_fitness[2], a_fitness[3]);
#endif   // end T_MOTIVATION_FITNESS
        //···································································
    }
}


//-------------------------------------------------------------------
// Inputs values are in degrees an millimeters, which must be converted to cms
//-------------------------------------------------------------------

void VirtualExecutive::Set_StartCoordinates(const int _coordinates[]) {
    startPosition[0] = _coordinates[0]; // Angle in degrees
    startPosition[1] = _coordinates[1]; // X Coordinate cms
    startPosition[2] = _coordinates[2]; // Y Coordinate cms
    currentPosition[0] = startPosition[0];
    currentPosition[1] = startPosition[1];
    currentPosition[2] = startPosition[2];
}

//-------------------------------------------------------------------
// Inputs values are in degrees an millimeters, which must be converted to cms
//-------------------------------------------------------------------

void VirtualExecutive::Set_GoalCoordinates(const int _coordinates[]) {
    goalPosition[0] = _coordinates[0]; // Angle in degrees
    goalPosition[1] = _coordinates[1]; // X Coordinate cms
    goalPosition[2] = _coordinates[2]; // Y Coordinate cms
}

//-------------------------------------------------------------------
// Inputs values are in degrees an millimeters, which must be converted to cms
//-------------------------------------------------------------------

void VirtualExecutive::Set_Initial_Battery(const double _value) {
    initialBattery = _value;
}
//-------------------------------------------------------------------

void VirtualExecutive::set_step_lenght(const int _value) {
    step_lenght = _value;
}
//-------------------------------------------------------------------

void VirtualExecutive::set_angle_lenght(const int _value) {
    angle_length = _value;
}
//-------------------------------------------------------------------

void VirtualExecutive::set_selected_room(const char _value) {
    this->o_MAP.set_selected_room(_value);
}

//-------------------------------------------------------------------
// Inputs values are in degrees an millimeters, which must be converted to cms
//-------------------------------------------------------------------

void VirtualExecutive::Set_Mission(const int _coordinates[]) {
    totalMissions = 1;
    missionPosition[0][0] = _coordinates[0]; // Angle in degrees
    missionPosition[0][1] = _coordinates[1]; // X Coordinate Millimeters
    missionPosition[0][2] = _coordinates[2]; // Y Coordinate MIllimeters

    missionPosition[0][3] = -1; // Fitness Value [0.0, 1.0]
    this->Distance_Missions(currentPosition, 0);
    o_MAP.setcCellMission(missionPosition[0][1] / _velv_mapMeshSize, missionPosition[0][2] / _velv_mapMeshSize);
}


//-------------------------------------------------------------------
// Internal values are in cms and degrees, they must by converted to millimeter
//-------------------------------------------------------------------

void VirtualExecutive::Show_Missions(void) const {
    printf("Missions Number: %d\n", totalMissions);
    for (int _im = 0; _im < totalMissions; _im++) {
        printf("Angle in degrees: %3d\tX Coord (in mm): %5d\tY Coord (in mm): %5d\n",
                missionPosition[_im][0], missionPosition[_im][1], missionPosition[_im][2]);
    }
}


//-------------------------------------------------------------------
// double CalcFCuriosity(void)
// Compute the curiosity fitness - related to the visited and sensed
// In this case the area is handled as cells
//-------------------------------------------------------------------

double VirtualExecutive::Calc_Fitness_Curiosity(void) {
    double _fitness = 0.0;
    double _visited = 0.0;
    double _revisited = 0.0;
    double _total = 0.0;

    //   _total = this->o_MAP.getFreeCellsNumber();
    _total = _genesNumber * (step_lenght / _velv_mapMeshSize) * robotCellDiameterPlusSensors;
    _visited = this->o_MAP.getVisitedCellsNumber();
    _revisited = this->o_MAP.getRevisitedCellsNumber();
    _visited -= (_revisited * REVISITED_PENALTY);

    if (_total != 0.0)
        _fitness = _visited / _total;

    return (_fitness);
}


//-------------------------------------------------------------------
// double CalcFEnergy(void)
// Compute the Energy fitness - related to the energy use
// An efficient energy usage is reward
//-------------------------------------------------------------------

double VirtualExecutive::Calc_Fitness_Energy(void) {
    double _fitness = 0.0;

    if (this->initialBattery != 0.0)
        _fitness = POWER_MULT * (this->currentBattery) / (this->initialBattery);

    if (this->currentBattery <= 0.0)
        _fitness = 0.0;

    return (_fitness);
}


//-------------------------------------------------------------------
// double CalcFHoming(void)
// Compute the homing fitness - related to the distance between
// the robot and the start point, we consider home a circular area,
// twice the robot area with center in [startXCoord, startYCoord]
//-------------------------------------------------------------------

double VirtualExecutive::Calc_Fitness_Homing(void) {
    double _fitness = 0.0;
    double _distance;
    double _homePerimeter = robotDiameter*GOAL_SIZE;
    double _maxAllowedDistance = _homePerimeter*GOAL_SIGNAL_RANGE; // 4 time _homePerimeter

    _distance = this->Calculate_Distance(currentPosition, goalPosition);

    // If final robot distance is smaller than homes perimeter
    // then its individual fitness is maximal = 1.0
    if (_distance < _homePerimeter) {
        _fitness = 1.0;
    }
    // The Individual Homing Fitness is inversely proportional to the
    // home distance between _homePerimeter and twice _homePerimeter
    // if the home distance is bigger then th Individual Fitness become minimal = 0.0
    else if ((_distance > _homePerimeter) && (_distance < (_maxAllowedDistance))) {
        _fitness = (GOAL_SIGNAL_RANGE / (GOAL_SIGNAL_RANGE - 1)) - (_distance / (_homePerimeter * (GOAL_SIGNAL_RANGE - 1)));
    } else if (_distance > _maxAllowedDistance) {
        _fitness = 0.0;
    }

    return (_fitness);
}


//-------------------------------------------------------------------
// Compute Missions related fitness
// Modified to work only with one mission
//-------------------------------------------------------------------

double VirtualExecutive::Calc_Fitness_Mission(void) {
    double _fitness = 0.0;
    double _fitness_dist = 0.0;
    double _fitness_step = 0.0;
    double distance_weight = 0.50;
    int _distance;
    int _goalPerimeter = lround(robotDiameter * MISSION_AREA);
    int _maxAllowedDistance = lround(robotDiameter * MISSION_AREA * MISSION_MAX_RANGE); // 4 time _homePerimeter

    _distance = missionPosition[0][3]; // Fitness Value [0.0, 1.0]
    // If final robot distance is smaller than goal perimeter
    // then its individual fitness is maximal = 1.0
    if (_distance < _goalPerimeter) {
        _fitness_dist = 1.0;
    }        // The Individual Homing Fitness is inversely proportional to the
    // home distance between _homePerimeter and twice _homePerimeter
    // if the home distance is bigger then th Individual Fitness become minimal = 0.0
    else if ((_distance > _goalPerimeter) && (_distance < (_maxAllowedDistance))) {
        _fitness_dist = (MISSION_MAX_RANGE / (MISSION_MAX_RANGE - 1)) - (_distance / (_goalPerimeter * (MISSION_MAX_RANGE - 1)));
    } else if (_distance > _maxAllowedDistance) {
        _fitness = 0.0;
    }

    double _min_step_to_goal = (missionPosition[0][4])*1.0;
    _fitness_step = ((_genesNumber - _min_step_to_goal) / _genesNumber);

    if (_fitness_dist != 0.0) {
        _fitness = _fitness_dist * distance_weight + (1 - distance_weight) * _fitness_step;
    } else {
        _fitness = 0.0;
    }

    return (_fitness);
}


//-------------------------------------------------------------------
// Compute the distance of the robot's between two given position
// Consider only lineal distance (euclidian distance)
// A better value can be obtain by compute the Obstacle free Distance
//-------------------------------------------------------------------

int VirtualExecutive::Calculate_Distance(const int _1st_position[], const int _2nd_position[]) {
    int _distance = 0;
    double temp = 0.0;
    double _delta_x = (_2nd_position[1] - _1st_position[1])*1.0;
    double _delta_y = (_2nd_position[2] - _1st_position[2])*1.0;

    temp = pow(_delta_x, 2.0) + pow(_delta_y, 2.0);
    _distance = lround(sqrt(temp));

    return (_distance);
}

//-------------------------------------------------------------------
// This Function must be modify depending in the real robot movement
// logic and limitaciones
//-------------------------------------------------------------------

void VirtualExecutive::ComputeNextPosition(int _currentPosition[], const _gen _action) {
    //···································································
#ifdef T_C_NEXT_POSITION
    printf("\nTEST COMPUTE NEXT POSITION:");
    printf("\nSTART COORINATES:\n ANGLE: %3d\tX COORD: %5d\tY COORD: %5d" \
            , _currentPosition[0], _currentPosition[1], _currentPosition[2]);
#endif   // end T_C_NEXT_POSITION
    //···································································
    if (_action == FORWARD) {
        double _angle = (_currentPosition[0] / 180.0) * M_PI;
        _currentPosition[1] += lround(cos(_angle) * step_lenght);
        _currentPosition[2] += lround(sin(_angle) * step_lenght);
    }
    else if (_action == REVERSE) {
        double _angle = (_currentPosition[0] / 180.0) * M_PI;
        _currentPosition[1] -= lround(cos(_angle) * step_lenght);
        _currentPosition[2] -= lround(sin(_angle) * step_lenght);
    } 
    else if (_action == TURN_RIGHT) {
        _currentPosition[0] -= angle_length;
        if (_currentPosition[0] < 0)
            _currentPosition[0] = 360 + _currentPosition[0];
        if (_currentPosition[0] > 360)
            _currentPosition[0] = _currentPosition[0] - 360;
    } 
    else if (_action == TURN_LEFT) {
        _currentPosition[0] += angle_length;
        if (_currentPosition[0] < 0)
            _currentPosition[0] = 360 + _currentPosition[0];
        if (_currentPosition[0] > 360)
            _currentPosition[0] = _currentPosition[0] - 360;
    }        //   else if( _action == TURN_RIGHT_1 )
    //   {
    //      _currentPosition[0] -= angle_length;
    //      if( _currentPosition[0] < 0 )
    //         _currentPosition[0] = 360 + _currentPosition[0];
    //      if( _currentPosition[0] > 360 )
    //         _currentPosition[0] = _currentPosition[0] - 360;
    //   }
    //   else if( _action == TURN_LEFT_1 )
    //   {
    //      _currentPosition[0] += angle_length;
    //      if( _currentPosition[0] < 0 )
    //         _currentPosition[0] = 360 + _currentPosition[0];
    //      if( _currentPosition[0] > 360 )
    //         _currentPosition[0] = _currentPosition[0] - 360;
    //   }
    //   else if( _action == TURN_RIGHT_2 )
    //   {
    //      _currentPosition[0] -= angle_length*2;
    //      if( _currentPosition[0] < 0 )
    //         _currentPosition[0] = 360 + _currentPosition[0];
    //      if( _currentPosition[0] > 360 )
    //         _currentPosition[0] = _currentPosition[0] - 360;
    //   }
    //   else if( _action == TURN_LEFT_2 )
    //   {
    //      _currentPosition[0] += angle_length*2;
    //      if( _currentPosition[0] < 0 )
    //         _currentPosition[0] = 360 + _currentPosition[0];
    //      if( _currentPosition[0] > 360 )
    //         _currentPosition[0] = _currentPosition[0] - 360;
    //   }
    //   else if( _action == TURN_RIGHT_3 )
    //   {
    //      _currentPosition[0] -= angle_length*3;
    //      if( _currentPosition[0] < 0 )
    //         _currentPosition[0] = 360 + _currentPosition[0];
    //      if( _currentPosition[0] > 360 )
    //         _currentPosition[0] = _currentPosition[0] - 360;
    //   }
    //   else if( _action == TURN_LEFT_3 )
    //   {
    //      _currentPosition[0] += angle_length*3;
    //      if( _currentPosition[0] < 0 )
    //         _currentPosition[0] = 360 + _currentPosition[0];
    //      if( _currentPosition[0] > 360 )
    //         _currentPosition[0] = _currentPosition[0] - 360;
    //   }
    else if (_action == FREEZE) {
        ;
    }
    //···································································
#ifdef T_C_NEXT_POSITION
    printf("\nEND COORINATES:\n ANGLE: %3d\tX COORD: %5d\tY COORD: %5d", _currentPosition[0], _currentPosition[1], _currentPosition[2]);
#endif   // end T_C_NEXT_POSITION
    //···································································
}

//-------------------------------------------------------------------
// Calculate the distance to each mission every step
//-------------------------------------------------------------------

void VirtualExecutive::Distance_Missions(const int _currentPosition[], const int _step) {
    int _distance;

    _distance = Calculate_Distance(_currentPosition, missionPosition[0]);
    if (missionPosition[0][3] == -1) {
        missionPosition[0][3] = _distance;
        missionPosition[0][4] = _step;
        missionPosition[0][5] = _currentPosition[0];
        missionPosition[0][6] = _currentPosition[1];
        missionPosition[0][7] = _currentPosition[2];
    } else if (missionPosition[0][3] > _distance) {
        missionPosition[0][3] = _distance;
        missionPosition[0][4] = _step;
        missionPosition[0][5] = _currentPosition[0];
        missionPosition[0][6] = _currentPosition[1];
        missionPosition[0][7] = _currentPosition[2];
    }

}

//-------------------------------------------------------------------
// bool RobotSweptArea(const double _prevCoord[], const _gen _action)
// Approximation of the Robot Swept Area
// Verify Obstacles and Set Visited Zones into this Area
// The function works with cells on the map
// Return true if there is an obstacle into the swept area, otherwise return false
//-------------------------------------------------------------------

bool VirtualExecutive::RobotSweptArea(const int _prevCoord[], const _gen _action) 
{
    //···································································
#ifdef T_CRASH_CHECKER
    printf("\nTEST CRASH CHECKER:");
#endif   // end T_CRASH_CHECKER
    //···································································
    int _leftWall = this->currentPosition[1] - robotRadius;
    int _lowerWall = this->currentPosition[2] - robotRadius;
    int _rightWall = this->currentPosition[1] + robotRadius;
    int _upperWall = this->currentPosition[2] + robotRadius;

    // Verify Crash with Map's boundaries
    // Main position + Robot Diameter, perhaps even part of the sensores range
    // Crash with Maps Left wall
    if( _leftWall <= 0 )
    {
        //···································································
#ifdef T_CRASH_CHECKER
        printf("\nCrash with Maps Left wall:");
#endif   // end T_CRASH_CHECKER
        //···································································
        return(true);
    }
    // Crash with Maps Lower wall
    else if( _lowerWall <= 0 )
    {
        //···································································
#ifdef T_CRASH_CHECKER
        printf("\nCrash with Maps Lower wall:");
#endif   // end T_CRASH_CHECKER
        //···································································
        return(true);
    }
    // Crash with Maps Right wall
    else if( _rightWall >= _velv_mapWidth )
    {
        //···································································
#ifdef T_CRASH_CHECKER
        printf("\nCrash with Maps Right wall:");
#endif   // end T_CRASH_CHECKER
        //···································································
        return(true);
    }
    // Crash with Maps Upper wall
    else if( _upperWall >= _velv_mapHeight )
    {
        //···································································
#ifdef T_CRASH_CHECKER
        printf("\nCrash with Maps Upper wall:");
#endif   // end T_CRASH_CHECKER
        //···································································
        return(true);
    }
    // Compute the main cell of the robot representation
    // int lroundf(double x);
    // round their argument to the nearest integer value,
    // rounding always away from zero.
    int _startXCell = lround(_prevCoord[1]/_velv_mapMeshSize);
    int _startYCell = lround(_prevCoord[2]/_velv_mapMeshSize);
    int _stopXCell = lround(currentPosition[1]/_velv_mapMeshSize);
    int _stopYCell = lround(currentPosition[2]/_velv_mapMeshSize);

    // Calculate the displacement, this is part of the stop condition
    double temp;
    double _delta_x = (_startXCell - _stopXCell)*1.0;
    double _delta_y = (_startYCell - _stopYCell)*1.0;
    temp = pow(_delta_x, 2.0) + pow(_delta_y, 2.0);

    double _totalDisplacement = sqrt(temp);
    double _displacement = 0.0; // Accumulate Displacement

    // Convert angle form degree to radians to use trigonometric functions
    double _angle = (_prevCoord[0]/180.0)*M_PI;

    int _crrntXCell = _startXCell, _crrntYCell = _startYCell;
    int auxXS, auxYS, auxXE, auxYE;
    bool _stop = false;
    // BEGIN : Verify crashes in the (local) start position
    do {
        auxXS = _crrntXCell - robotCellRadius;
        auxYS = _crrntYCell - robotCellRadius;
        auxXE = _crrntXCell + robotCellRadius;
        auxYE = _crrntYCell + robotCellRadius;

        // Could the robot's border been crashing with the MAP boundaries??
        if( auxXS < 0 )   return(true);
        if( auxYS < 0 )   return(true);
        if( auxXE >= _velv_mapXDim ) return(true);
        if( auxYE >= _velv_mapYDim ) return(true);

        auxXS = _crrntXCell - robotCellRadiusPlusSensors;
        auxYS = _crrntYCell - robotCellRadiusPlusSensors;
        auxXE = _crrntXCell + robotCellRadiusPlusSensors;
        auxYE = _crrntYCell + robotCellRadiusPlusSensors;

        // The robot's sensor are sensing outside the MAP boundaries??
        if( auxXS < 0 )   auxXS = 0;
        if( auxYS < 0 )   auxYS = 0;
        if( auxXE > _velv_mapXDim ) auxXE = _velv_mapXDim;
        if( auxYE > _velv_mapYDim ) auxYE = _velv_mapYDim;

        // For Debug Use: BEGIN
        if( auxXS < 0 )   fprintf(stdout, "RobotSweptArea Map O: X0 = %d\n", auxXS);
        if( auxYS < 0 )   fprintf(stdout, "RobotSweptArea Map O: Y0 = %d\n", auxYS);
        if( auxXE > this->o_MAP.getMapXDim() )  fprintf(stdout, "RobotSweptArea Map O: X1 = %d\n", auxXE);
        if( auxYE > this->o_MAP.getMapYDim() )  fprintf(stdout, "RobotSweptArea Map O: Y1 = %d\n", auxYE);
        // For Debug Use: END

        // Could the robot been crashing with something else??
        for(int _y = auxYS; _y < auxYE; _y++)
        {
            for(int _x = auxXS; _x < auxXE; _x++)
            {
                // Are there Obstacles here????
                if( this->o_MAP.cCellObstacle(_x, _y) )
                {
                    //···································································
#ifdef T_CRASH_CHECKER
                    printf("\nSTART: Are there Obstacles here????: (%4d, %4d)", _x, _y);
#endif   // end T_CRASH_CHECKER
                    //···································································
                    return(true);
                }

                // Set this cell as visited or revisited
                if( !this->o_MAP.cCellVisited(_x, _y) )
                    this->o_MAP.setcCellVisited(_x, _y);
                else
                    this->o_MAP.setcCellRevisited(_x, _y);
            }  // end for auxYS
        }  // end for auxXS

        // update current cell postition 
        if( _action == FORWARD )
        {
            _crrntXCell += lround(cos(_angle)*robotCellDiameter);
            _crrntYCell += lround(sin(_angle)*robotCellDiameter);
        }
        if( _action == REVERSE )
        {
            _crrntXCell -= lround(cos(_angle)*robotCellDiameter);
            _crrntYCell -= lround(sin(_angle)*robotCellDiameter);
        }
        // Calculate the displacement, this is part of the stop condition      

        _delta_x = (_startXCell - _stopXCell)*1.0;
        _delta_y = (_startYCell - _stopYCell)*1.0;

        temp = pow(_delta_x, 2.0) + pow(_delta_y, 2.0);

        _displacement = sqrt(temp);
        if( _displacement >= _totalDisplacement )
        {
            _stop = true;
            _crrntXCell = _stopXCell;
            _crrntYCell = _stopYCell;
        }
    }while( !_stop );
    // END : Verify crashes in the (local) start position

    return(false);
}


bool VirtualExecutive::RobotCrash_MapBorderCoord() {
    //···································································
#ifdef T_CRASH_CHECKER
    //    printf("\nTEST MAP BORDER CRASH CHECKER:");
#endif   // end T_CRASH_CHECKER
    //···································································
    int _leftWall = this->currentPosition[1] - robotRadius;
    int _lowerWall = this->currentPosition[2] - robotRadius;
    int _rightWall = this->currentPosition[1] + robotRadius;
    int _upperWall = this->currentPosition[2] + robotRadius;

    // Verify Crash with Map's boundaries
    // Main position + Robot Diameter, perhaps even part of the sensores range
    // Crash with Maps Left wall
    if (_leftWall <= 0) {
        //···································································
#ifdef T_CRASH_CHECKER
        std::cout << "\tCrash with Maps Left wall:" << std::endl;
#endif   // end T_CRASH_CHECKER
        //···································································
        return (true);
    }// Crash with Maps Lower wall
    else if (_lowerWall <= 0) {
        //···································································
#ifdef T_CRASH_CHECKER
        std::cout << "\tCrash with Maps Lower wall:" << std::endl;
#endif   // end T_CRASH_CHECKER
        //···································································
        return (true);
    }// Crash with Maps Right wall
    else if (_rightWall >= _velv_mapWidth) {
        //···································································
#ifdef T_CRASH_CHECKER
        std::cout << "\tCrash with Maps Right wall:" << std::endl;
#endif   // end T_CRASH_CHECKER
        //···································································
        return (true);
    }// Crash with Maps Upper wall
    else if (_upperWall >= _velv_mapHeight) {
        //···································································
#ifdef T_CRASH_CHECKER
        std::cout << "\tCrash with Maps Upper wall:" << std::endl;
#endif   // end T_CRASH_CHECKER
        //···································································
        return (true);
    }
    return (false);
}

bool VirtualExecutive::RobotCrash_MapBorderCells(const int _prevCoord[], const _gen _action) {

    int robot_left_XCell;
    int robot_down_YCell;
    int robot_right_XCell;
    int robot_up_YCell;

    int _startXCell = lround(_prevCoord[1] / _velv_mapMeshSize);
    int _startYCell = lround(_prevCoord[2] / _velv_mapMeshSize);
    int _stopXCell  = lround(currentPosition[1] / _velv_mapMeshSize);
    int _stopYCell  = lround(currentPosition[2] / _velv_mapMeshSize);

    // Calculate the displacement, this is part of the stop condition
    double _delta_x = (_startXCell - _stopXCell)*1.0;
    double _delta_y = (_startYCell - _stopYCell)*1.0;
    double _totalDisplacement = sqrt(pow(_delta_x, 2.0) + pow(_delta_y, 2.0));
    double _displacement = 0.0; // Accumulate Displacement

    // Convert angle form degree to radians to use trigonometric functions
    double _angle = (_prevCoord[0] / 180.0) * M_PI;
    bool stop = false;

    int _currentXCell = _startXCell;
    int _currentYCell = _startYCell;
    // BEGIN : Verify crashes in the (local) start position
    do {

        robot_left_XCell  = _currentXCell - robotCellRadius;
        robot_down_YCell  = _currentYCell - robotCellRadius;
        robot_right_XCell = _currentXCell + robotCellRadius;
        robot_up_YCell    = _currentYCell + robotCellRadius;

        // Could the robot's border been crashing with the MAP boundaries??
        if (robot_left_XCell <= 0) return (true);
        if (robot_down_YCell <= 0) return (true);
        if (robot_right_XCell >= _velv_mapXDim) return (true);
        if (robot_up_YCell >= _velv_mapYDim) return (true);

        // Should consider the action, because the angle it is not enough
        if (_action == FORWARD) {
            _currentXCell += lround(cos(_angle) * robotCellDiameter);
            _currentYCell += lround(sin(_angle) * robotCellDiameter);
        }
        if (_action == REVERSE) {
            _currentXCell -= lround(cos(_angle) * robotCellDiameter);
            _currentYCell -= lround(sin(_angle) * robotCellDiameter);
        }

        _delta_x = (_currentXCell - _stopXCell)*1.0;
        _delta_y = (_currentYCell - _stopYCell)*1.0;


        _displacement = sqrt(pow(_delta_x, 2.0) + pow(_delta_y, 2.0));
        if (_displacement >= _totalDisplacement) {
            stop = true;
            _currentXCell = _stopXCell;
            _currentYCell = _stopYCell;
        }
    } while (!stop);


}

bool VirtualExecutive::SensorCrash_MapBorderCells(const int _prevCoord[], const _gen _action) {


    int sensor_left_XCell;
    int sensor_down_YCell;
    int sensor_right_XCell;
    int sensor_up_YCell;

    int startXCell = lround(_prevCoord[1] / _velv_mapMeshSize);
    int startYCell = lround(_prevCoord[2] / _velv_mapMeshSize);
    int stopXCell = lround(currentPosition[1] / _velv_mapMeshSize);
    int stopYCell = lround(currentPosition[2] / _velv_mapMeshSize);

    // Calculate the displacement, this is part of the stop condition
    double _delta_x = (startXCell - stopXCell)*1.0;
    double _delta_y = (startYCell - stopYCell)*1.0;
    double _totalDisplacement = sqrt(pow(_delta_x, 2.0) + pow(_delta_y, 2.0));
    double _displacement = 0.0; // Accumulate Displacement

    // Convert angle form degree to radians to use trigonometric functions
    double _angle = (_prevCoord[0] / 180.0) * M_PI;
    bool stop = false;

    int currentXCell = startXCell;
    int currentYCell = startYCell;
    // BEGIN : Verify crashes in the (local) start position
    do {
        sensor_left_XCell = currentXCell - robotCellRadiusPlusSensors;
        sensor_down_YCell = currentXCell - robotCellRadiusPlusSensors;
        sensor_right_XCell = currentXCell + robotCellRadiusPlusSensors;
        sensor_up_YCell = currentYCell + robotCellRadiusPlusSensors;

        // The robot's sensor are sensing outside the MAP boundaries??
        if (sensor_left_XCell < 0) sensor_left_XCell = 0;
        if (sensor_down_YCell < 0) sensor_down_YCell = 0;
        if (sensor_right_XCell > _velv_mapXDim) sensor_right_XCell = _velv_mapXDim;
        if (sensor_up_YCell > _velv_mapYDim) sensor_up_YCell = _velv_mapYDim;

        // Could the robot been crashing with something else??
        for (int _y = sensor_down_YCell; _y < sensor_up_YCell; _y++) {
            for (int _x = sensor_left_XCell; _x < sensor_down_YCell; _x++) {
                if (this->o_MAP.cCellObstacle(_x, _y)) return (true); 
                if (!this->o_MAP.cCellVisited(_x, _y)) this->o_MAP.setcCellVisited(_x, _y);
                else this->o_MAP.setcCellRevisited(_x, _y);
            }
        }
        // Should consider the action, because the angle it is not enough
        if (_action == FORWARD) {
            currentXCell += lround(cos(_angle) * robotCellDiameter);
            currentYCell += lround(sin(_angle) * robotCellDiameter);
        }
        if (_action == REVERSE) {
            currentXCell -= lround(cos(_angle) * robotCellDiameter);
            currentYCell -= lround(sin(_angle) * robotCellDiameter);
        }

        // How can I be sure that I reach the current position?
        // if next cell is equal or close to the end of the displacement
        // the next cell is seted to the current robot position and after
        // verify any crashes the function will return.
        // Calculate the displacement, this is part of the stop condition
        _delta_x = (startXCell - stopXCell)*1.0;
        _delta_y = (startYCell - stopYCell)*1.0;
        _displacement = sqrt(pow(_delta_x, 2.0) + pow(_delta_y, 2.0));
        if (_displacement >= _totalDisplacement) {
            stop = true;
            currentXCell = stopXCell;
            currentYCell = stopYCell;
        }
    } while (!stop);
    // END : Verify crashes in the (local) start position

    return (false);
}


//-------------------------------------------------------------------
// void UpdateBatteryLevel(const _gen _action)
// Decrease the battery level according to the action consumption
//-------------------------------------------------------------------
void VirtualExecutive::UpdateBatteryLevel(const _gen _action) {
    if (_action == FORWARD) {
        this->currentBattery -= POWER_FORWARD;
    } else if (_action == REVERSE) {
        this->currentBattery -= POWER_REVERSE;
    } else if (_action == TURN_RIGHT) {
        this->currentBattery -= POWER_RIGHT_1;
    } else if (_action == TURN_LEFT) {
        this->currentBattery -= POWER_LEFT_1;
    }        //   else if( _action == TURN_RIGHT_1 )
    //   {
    //      this->currentBattery -= POWER_RIGHT_1;
    //   }
    //   else if( _action == TURN_LEFT_1 )
    //   {
    //      this->currentBattery -= POWER_LEFT_1;
    //   }
    //   else if( _action == TURN_RIGHT_2 )
    //   {
    //      this->currentBattery -= POWER_RIGHT_2;
    //   }
    //   else if( _action == TURN_LEFT_2 )
    //   {
    //      this->currentBattery -= POWER_LEFT_2;
    //   }
    //   else if( _action == TURN_RIGHT_3 )
    //   {
    //      this->currentBattery -= POWER_RIGHT_3;
    //   }
    //   else if( _action == TURN_LEFT_3 )
    //   {
    //      this->currentBattery -= POWER_LEFT_3;
    //   }
    else if (_action == FREEZE) {
        this->currentBattery -= POWER_FREEZE;
    }
}

//-------------------------------------------------------------------

void VirtualExecutive::get_start_position(int _coordinates[]) const {
    _coordinates[0] = startPosition[0];
    _coordinates[1] = startPosition[1];
    _coordinates[2] = startPosition[2];
}
//-------------------------------------------------------------------

void VirtualExecutive::get_goal_position(int _coordinates[]) const {
    _coordinates[0] = goalPosition[0];
    _coordinates[1] = goalPosition[1];
    _coordinates[2] = goalPosition[2];
}
//-------------------------------------------------------------------

void VirtualExecutive::get_current_position(int _coordinates[]) const {
    _coordinates[0] = currentPosition[0];
    _coordinates[1] = currentPosition[1];
    _coordinates[2] = currentPosition[2];
}
//-------------------------------------------------------------------

void VirtualExecutive::get_mission_position(int _coordinates[]) const {
    _coordinates[0] = missionPosition[0][0];
    _coordinates[1] = missionPosition[0][1];
    _coordinates[2] = missionPosition[0][2];
}
//-------------------------------------------------------------------

void VirtualExecutive::get_mission_closer_position(int _coordinates[]) const {
    _coordinates[0] = missionPosition[0][5];
    _coordinates[1] = missionPosition[0][6];
    _coordinates[2] = missionPosition[0][7];
}
//-------------------------------------------------------------------

int VirtualExecutive::getMapWidth(void) const {
    return (o_MAP.getMapWidth());
}
//-------------------------------------------------------------------

int VirtualExecutive::getMapHeight(void) const {
    return (o_MAP.getMapHeight());
}
//-------------------------------------------------------------------

int VirtualExecutive::get_MapMeshSize(void) const {
    return (o_MAP.getMapMeshSize());
}
//-------------------------------------------------------------------

int VirtualExecutive::get_robot_diameter(void) const {
    return (robotDiameter);
}
//-------------------------------------------------------------------

int VirtualExecutive::get_robot_diameter_sensors(void) const {
    return (robotDiameter + sensorsRange);
}
//-------------------------------------------------------------------

char VirtualExecutive::getXYcCellData(int _XCoord, int _YCoord) const {
    char _info = o_MAP.getXYcCellData(_XCoord, _YCoord);
    return (_info);
}

//-------------------------------------------------------------------

char VirtualExecutive::get_selected_room(void) const {
    return (o_MAP.get_selected_room());
}
//-------------------------------------------------------------------

int VirtualExecutive::get_step_lenght(void) const {
    return (step_lenght);
}
//-------------------------------------------------------------------

int VirtualExecutive::get_angle_length(void) const {
    return (angle_length);
}
//-------------------------------------------------------------------

int VirtualExecutive::get_useful_steps_nr(void) const {
    return (useful_steps_nr);
}
//-------------------------------------------------------------------

int VirtualExecutive::get_min_missions_distance(void) const {
    return (missionPosition[0][3]);
}
//-------------------------------------------------------------------

int VirtualExecutive::get_min_missions_steps(void) const {
    return (missionPosition[0][4]);
}

//-------------------------------------------------------------------
// void PrintMAPtoFile(const char a_fileName[])
// This Function Calls the Print Function of the class internal Map
//-------------------------------------------------------------------

void VirtualExecutive::PrintMAPtoFile(const char a_fileName[]) {
   this->o_MAP.PrintMAPtoFile(a_fileName);
}
//*******************************************************************
// VirtualExecutive Class Implementation: END
//*******************************************************************


//*******************************************************************
// Elite_Invidivual Class Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
//-------------------------------------------------------------------

Elite_Invidivual::Elite_Invidivual() {
    robot_diameter = ROBOT_DIAMETER; // Millimeters
    robot_diameter_sensors = ROBOT_DIAMETER + SENSOR_RANGE; // Millimeters
    //genes_nr
    genes_nr = GENES_NUMBER;
    a_genes = new _gen[genes_nr];

    home_coord[0] = HOME_ANGLE; // Angle Degrees
    home_coord[1] = HOME_X_COORD; // X Coord Millimeters
    home_coord[2] = HOME_Y_COORD; // Y Coord Millimeters
    start_coord[0] = START_ANGLE; // Angle Degrees
    start_coord[1] = START_X_COORD; // X Coord Millimeters
    start_coord[2] = START_Y_COORD; // Y Coord Millimeters

    missions_nr = 1;
    // Allocate dynamic memory, Access as matrix
    // Remember: that C create first the rows and then columns
    a_missions_coord = new int*[missions_nr];
    for (int _y = 0; _y < missions_nr; _y++)
        a_missions_coord[_y] = new int[8];
    a_missions_coord[0][0] = 90; // Angle Degrees
    a_missions_coord[0][1] = 2000; // X Coord Millimeters
    a_missions_coord[0][2] = 800; // Y Coord Millimeters
    a_missions_coord[0][3] = calculate_Distance(start_coord, a_missions_coord[0]); // Y Coord Millimeters
    a_missions_coord[0][4] = 0; // Y Coord Millimeters
    a_missions_coord[0][5] = start_coord[0]; // Y Coord Millimeters
    a_missions_coord[0][6] = start_coord[1]; // Y Coord Millimeters
    a_missions_coord[0][7] = start_coord[2]; // Y Coord Millimeters

    a_fitness[0] = 0.0; // Fitness fulfillment product for Curiostity
    a_fitness[1] = 0.0; // Fitness fulfillment product for Energy
    a_fitness[2] = 0.0; // Fitness fulfillment product for Homing
    a_fitness[3] = 0.0; // Fitness fulfillment product for Missions
    /* 
       fart_categories = 1;
       a_FART_map = new double*[fart_categories];
       for (int _y = 0; _y < fart_categories; _y++)
       a_FART_map[_y] = new double[4];
       */
    a_path = new int*[genes_nr];
    for (int _y = 0; _y < genes_nr; _y++)
        a_path[_y] = new int[3];

    ready_path = 0;
    ready_gen = 0;
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

Elite_Invidivual::~Elite_Invidivual() {
    // Free Memory
    for (int _y = 0; _y < missions_nr; _y++)
        delete [] a_missions_coord[_y];
    delete [] a_missions_coord;

    /* IRMA 3 does not use features
       for (int _y = 0; _y < fart_categories; _y++)
       delete [] a_FART_map[_y];
       delete [] a_FART_map;
       */
    for (int _y = 0; _y < genes_nr; _y++)
        delete [] a_path[_y];
    delete [] a_path;

    delete [] a_genes;
}

//-------------------------------------------------------------------

void Elite_Invidivual::Clean_All(void) {
    a_fitness[0] = 0.0; // Fitness fulfillment product for Curiostity
    a_fitness[1] = 0.0; // Fitness fulfillment product for Energy
    a_fitness[2] = 0.0; // Fitness fulfillment product for Homing
    a_fitness[3] = 0.0; // Fitness fulfillment product for Missions
    ready_path = 0;
    ready_gen = 0;
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------

void Elite_Invidivual::Print_GENOTYPE(const char a_fileName[]) {
    printf("GENOME OF THE BEST INDIVIDUAL (GENOTYPE):\n");

    if (strlen(a_fileName) != 0) {
        printf("PRINTING IN FILE: %s\n", a_fileName);
        FILE *fp;
        if ((fp = fopen(a_fileName, "w"))) {
            fprintf(fp, "\nGENOME\n");
            for (int _igen = 0; _igen < genes_nr; _igen++)
                fprintf(fp, "%x  ", a_genes[_igen]);
            fclose(fp);
        } else {
            printf("Error opening FILE in function: Organism::PrintGenome\n");
        }
    } else {
        for (int _igen = 0; _igen < genes_nr; _igen++)
            printf("%x  ", a_genes[_igen]);
    }
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

void Elite_Invidivual::Print_PHENOTYPE(const char a_fileName[]) {
    printf("PRINTING IN FILE: %s\n", a_fileName);
    FILE *fp;
    if ((fp = fopen(a_fileName, "w"))) {
        // ROBOT DIAMETER
        fprintf(fp, "%d\n", robot_diameter);
        // ROBOT DIAMETER + SENSOR RANGE
        fprintf(fp, "%d\n", robot_diameter_sensors);
        // ACTIONS OR STPES NUMBER
        if (ready_gen < genes_nr) {
            fprintf(fp, "%d\n", ready_gen);
        } else {
            fprintf(fp, "%d\n", genes_nr);
        }
        // START COORDINATES
        // Angle, START_X_COORD, START_Y_COORD
        fprintf(fp, "%d,%d,%d,%d\n", 0, start_coord[0], start_coord[1], start_coord[2]);

        //CURRENT COORDINATES OR TRAJECTORY
        int _igen = 0;
        while ((_igen < ready_gen) && (_igen < genes_nr)) {
            fprintf(fp, "%d,%d,%d,%d\n", _igen, a_path[_igen][0], a_path[_igen][1], a_path[_igen][2]); // CURRENT COORDINATES OR TRAJECTORY
            _igen++;
        }

        // GOALS AREA
        fprintf(fp, "%d\n", robot_diameter / 2);
        // HOMING COORDINATES
        fprintf(fp, "%d,%d,%d\n", home_coord[0], home_coord[1], home_coord[2]);
        // MISSIONS NUMBER
        fprintf(fp, "%d\n", missions_nr);
        // MISSIONS COORDINATES
        for (int _im = 0; _im < missions_nr; _im++) {
            // Angle, X coord, Y coord
            // Closer Distance, Steps needed
            // Closer Position
            fprintf(fp, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                    a_missions_coord[_im][0], a_missions_coord[_im][1], a_missions_coord[_im][2],
                    a_missions_coord[_im][3], a_missions_coord[_im][4],
                    a_missions_coord[_im][5], a_missions_coord[_im][6], a_missions_coord[_im][7]);
        }

        // PARA LOS ESCENARIOS DE PRUEBA LA CODIFICACION DE FART ES (X0, Y0; 1-X1, 1-Y1)
        // AQUI VA FART DEL MAPA INTERNO - GUARDAR COPIA INTERNA DEL MAPA USADO
        fprintf(fp, "0.000,0.000,0.000,0.000\n");
        for (int _cat = 0; _cat < fart_categories; _cat++) {
            fprintf(fp, "%f,%f,%f,%f\n", a_FART_map[_cat][0], a_FART_map[_cat][1], a_FART_map[_cat][2], a_FART_map[_cat][3]);
        }
        fclose(fp);
    } else {
        printf("Error opening FILE in function: Elite_Invidivual::Print_PHENOTYPE\n");
    }
}

//-------------------------------------------------------------------

_gen Elite_Invidivual::get_gen(const int _nr) const {
    return (this->a_genes[_nr]);
}
//-------------------------------------------------------------------

int Elite_Invidivual::get_genes_nr(void) const {
    return (genes_nr);
}
//-------------------------------------------------------------------

int Elite_Invidivual::get_elite_genes_nr(void) const {
    return (ready_gen);
}
//-------------------------------------------------------------------

void Elite_Invidivual::get_start_coord(int _coordinates[]) {
    _coordinates[0] = start_coord[0];
    _coordinates[1] = start_coord[1];
    _coordinates[2] = start_coord[2];
}
//-------------------------------------------------------------------

void Elite_Invidivual::get_home_coord(int _coordinates[]) {
    _coordinates[0] = home_coord[0];
    _coordinates[1] = home_coord[1];
    _coordinates[2] = home_coord[2];
}
//-------------------------------------------------------------------

void Elite_Invidivual::get_final_coord(int _coordinates[]) {
    _coordinates[0] = final_coord[0];
    _coordinates[1] = final_coord[1];
    _coordinates[2] = final_coord[2];
}
//-------------------------------------------------------------------

void Elite_Invidivual::get_current_ideal_coord(const int _index, int _coord[]) const {
    _coord[0] = a_path[_index][0];
    _coord[1] = a_path[_index][1];
    _coord[2] = a_path[_index][2];
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_genes_nr(const int _value) {
    // Free Memory
    delete [] a_genes;
    // Free Memory
    for (int _y = 0; _y < genes_nr; _y++)
        delete [] a_path[_y];
    delete [] a_path;

    genes_nr = _value;
    a_genes = new _gen[genes_nr];

    a_path = new int*[genes_nr];
    for (int _y = 0; _y < genes_nr; _y++)
        a_path[_y] = new int[3];
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_genes(const int _from, const int _until, const _gen _genes[]) {
    int _igen = _from;

    while ((ready_gen < genes_nr) && (_igen < _until)) {
        a_genes[ready_gen++] = _genes[_igen++];
    }
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_genes(const _gen _genes[]) {
    ready_gen = 0;
    int _igen = 0;
    while ((ready_gen < genes_nr)) {
        a_genes[ready_gen++] = _genes[_igen++];
    }
}
//-------------------------------------------------------------------

void Elite_Invidivual::init_missions(const int _size) {
    // Free Memory
    for (int _y = 0; _y < missions_nr; _y++)
        delete [] a_missions_coord[_y];
    delete [] a_missions_coord;

    missions_nr = _size;
    a_missions_coord = new int*[missions_nr];
    for (int _y = 0; _y < missions_nr; _y++)
        a_missions_coord[_y] = new int[8];
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_mission(const int _index, const int _coord[]) {
    a_missions_coord[_index][0] = _coord[0];
    a_missions_coord[_index][1] = _coord[1];
    a_missions_coord[_index][2] = _coord[2];
}
////-------------------------------------------------------------------
//void Elite_Invidivual::set_missions(const int _size, const int _coord[][3])
//{
//   // Free Memory
//   for(int _y = 0; _y < missions_nr; _y++)
//      delete [] a_missions_coord[_y];
//   delete [] a_missions_coord;
//
//   missions_nr = _size;
//   a_missions_coord = new int*[missions_nr];
//   for(int _y = 0; _y < missions_nr; _y++)
//      a_missions_coord[_y] = new int[8];
//
//   for(int _im = 0; _im < missions_nr; _im++)
//   {
//      a_missions_coord[_im][0] = _coord[_im][0];
//      a_missions_coord[_im][1] = _coord[_im][1];
//      a_missions_coord[_im][2] = _coord[_im][2];
//   }
//}
//-------------------------------------------------------------------

void Elite_Invidivual::set_missions_closer_pos(const int _mission_nr, const int _coord[]) {
    a_missions_coord[_mission_nr][5] = _coord[0];
    a_missions_coord[_mission_nr][6] = _coord[1];
    a_missions_coord[_mission_nr][7] = _coord[2];
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_missions_closer_dist(const int _mission_nr, const int _value) {
    a_missions_coord[_mission_nr][3] = _value;
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_missions_steps_needed(const int _mission_nr, const int _value) {
    a_missions_coord[_mission_nr][4] = _value;
}
//-------------------------------------------------------------------

void Elite_Invidivual::init_fart_map(const int _size) {
    // Free Memory
    for (int _y = 0; _y < fart_categories; _y++)
        delete [] a_FART_map[_y];
    delete [] a_FART_map;

    fart_categories = _size;
    a_FART_map = new double*[fart_categories];
    for (int _y = 0; _y < fart_categories; _y++)
        a_FART_map[_y] = new double[4];
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_fart_map(const int _index, const double _categories[]) {
    a_FART_map[_index][0] = _categories[0];
    a_FART_map[_index][1] = _categories[1];
    a_FART_map[_index][2] = _categories[2];
    a_FART_map[_index][3] = _categories[3];
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_fart_map(const int _size, const double _categories[][4]) {
    // Free Memory
    for (int _y = 0; _y < fart_categories; _y++)
        delete [] a_FART_map[_y];
    delete [] a_FART_map;

    fart_categories = _size;
    a_FART_map = new double*[fart_categories];
    for (int _y = 0; _y < fart_categories; _y++)
        a_FART_map[_y] = new double[4];

    for (int _icat = 0; _icat < fart_categories; _icat++) {
        a_FART_map[_icat][0] = _categories[_icat][0];
        a_FART_map[_icat][1] = _categories[_icat][1];
        a_FART_map[_icat][2] = _categories[_icat][2];
        a_FART_map[_icat][3] = _categories[_icat][3];
    }
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_fart_map(const int _size, const float _categories[][4]) {
    // Free Memory
    for (int _y = 0; _y < fart_categories; _y++)
        delete [] a_FART_map[_y];
    delete [] a_FART_map;

    fart_categories = _size;
    a_FART_map = new double*[fart_categories];
    for (int _y = 0; _y < fart_categories; _y++)
        a_FART_map[_y] = new double[4];

    for (int _icat = 0; _icat < fart_categories; _icat++) {
        a_FART_map[_icat][0] = _categories[_icat][0];
        a_FART_map[_icat][1] = _categories[_icat][1];
        a_FART_map[_icat][2] = _categories[_icat][2];
        a_FART_map[_icat][3] = _categories[_icat][3];
    }
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_path_step(const int _coord[]) {
    if (ready_path < genes_nr) {
        a_path[ready_path][0] = _coord[0];
        a_path[ready_path][1] = _coord[1];
        a_path[ready_path][2] = _coord[2];
        ready_path++;
    }
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_robot_diameter(const int _value) {
    robot_diameter = _value;
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_robot_diameter_sensors(const int _value) {
    robot_diameter_sensors = _value;
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_fitness(const double _motivations[]) {
    a_fitness[0] = _motivations[0];
    a_fitness[1] = _motivations[1];
    a_fitness[2] = _motivations[2];
    a_fitness[3] = _motivations[3];
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_home_coord(const int _coord[]) {
    home_coord[0] = _coord[0];
    home_coord[1] = _coord[1];
    home_coord[2] = _coord[2];
}
//-------------------------------------------------------------------

void Elite_Invidivual::Set_StartCoordinates(const int _coord[]) {
    start_coord[0] = _coord[0];
    start_coord[1] = _coord[1];
    start_coord[2] = _coord[2];
}
//-------------------------------------------------------------------

void Elite_Invidivual::set_final_coord(const int _coord[]) {
    final_coord[0] = _coord[0];
    final_coord[1] = _coord[1];
    final_coord[2] = _coord[2];
}
//*******************************************************************
// Elite_Invidivual Class Implementation: END
//
//*******************************************************************
