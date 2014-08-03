//*******************************************************************
// Title: monitor.cpp
// Authors: Tomas Arredondo, Renato Agurto, Nicolas Navarro
// Version: v.0.0.1
// Date: 09/15/2009
//
// First aproach of the monitor used on IRMA-2 Project
//
// Version: v.0.0.2
// Copyright (C) 2009 by Nicolas Navarro
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
 *   Copyright (c) 2009 Tomas Arredondo, Renato Agurto, Nicolas Navarro
 *		<tomas.arredondo@usm.cl>
 *
 *   This program is open-source software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License version 3 as
 *   published by the Free Software Foundation.
 *
 *   See README and LICENSE for more details.
 *
 ******************************************************************************/

//*******************************************************************
// Library Declaration
#include "monitor.h"


//*******************************************************************
// MONITOR PROCESSOR: MonitorProcessor Class Implementation: BEGIN
//*******************************************************************
//-------------------------------------------------------------------
//-------------------------------------------------------------------

int MonitorProcessor::init() {
    pCDACloseNav = (CloseNavData *) cda.getMemPtr(CLOSE_NAV_AREA);
    pCDALongNav = (LongNavData *) cda.getMemPtr(LONG_NAV_AREA);
    pCDAFeatureNav = (FeatureNavData *) cda.getMemPtr(FEATURE_NAV_AREA);
    pCDAExecutive = (ExecutiveData *) cda.getMemPtr(EXECUTIVE_AREA);
    pCDAMonitor = (MonitorData *) cda.getMemPtr(MONITOR_AREA);

    pCtrl = &(pCDALongNav->ctrl); // Todos los procesadores tienen que definir pCtrl
    sem_area = LONG_NAV_AREA; // Todos los procesadores tienen que definir sem_area
    //initInputs(rl);
    //initOutputs(rl);
    pCtrl->loop = run_cmd;

    cda.lockArea(CLOSE_NAV_AREA);
    pCDACloseNav->crn_move_ready_flag = false;
    cda.unlockArea(CLOSE_NAV_AREA);

    mission_accomplished_flag = false;
    cda.lockArea(LONG_NAV_AREA);
    pCDALongNav->lrn_move_ready_flag = false;
    pCDALongNav->lrn_mission_accomplished = mission_accomplished_flag;
    cda.unlockArea(LONG_NAV_AREA);

    cda.lockArea(FEATURE_NAV_AREA);
    pCDAFeatureNav->fn_move_ready_flag = false;
    cda.unlockArea(FEATURE_NAV_AREA);

    cda.lockArea(EXECUTIVE_AREA);
    pCDAExecutive->exec_move_ready_flag = true;
    cda.unlockArea(EXECUTIVE_AREA);


    /************************ START: Config File reading.**************************/

    ConfigFile config("./config/monitor.conf");

    config.readInto(MAX_CLOSE_RANGE_HYSTERESIS, "MAX_CLOSE_RANGE_HYSTERESIS", 10);
    config.readInto(sleep_time, "MICRO_SEC_SLEEP", 5000);
    config.readInto(max_steps_nr, "MAX_STEPS_NUMBER", 400);

    config.readInto<bool>(status_CRN, "CRN", ON);
    config.readInto<bool>(status_LRN, "LRN", ON);
    config.readInto<bool>(status_FN, "FN", OFF);

    if ((status_CRN == OFF) && (status_LRN == OFF)) status_CRN = ON;

    fprintf(stdout, "\nConfig file readed:");
    fprintf(stdout, "\tMAX_CLOSE_RANGE_HYSTERESIS: %d", MAX_CLOSE_RANGE_HYSTERESIS);
    fprintf(stdout, "\tMAX_STEPS_NUMBER: %d", max_steps_nr);
    fprintf(stdout, "\t CRN=%d ,LRN=%d, FN=%d\n", status_CRN, status_LRN, status_FN);

    /************************END: Config File reading.**************************/


    //Initial Navigation mode
    //current_nav = LRN;
    current_nav = CRN;
    cda.lockArea(MONITOR_AREA);
    pCDAMonitor->monitor_current_nav = current_nav;
    cda.unlockArea(MONITOR_AREA);

    printf("Initial Navigator: ");
    if (current_nav == LRN) printf("LRN\n");
    else if (current_nav == CRN) printf("CRN\n");

    exec_or_nav = NAV;
    printf("listening for navigator...\n");

    num_steps = 0;
    execute_ready_flag = false;
    flag_fn = false;

    return (0);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

int MonitorProcessor::cleanup() {
    return (0);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

int MonitorProcessor::step() {
    cda.lockArea(EXECUTIVE_AREA);
    execute_ready_flag = pCDAExecutive->exec_ready_flag;
    cda.unlockArea(EXECUTIVE_AREA);

    if ((num_steps < max_steps_nr) && (!mission_accomplished_flag) && (execute_ready_flag)) {
        updateVariables();
        setNavigationMode();
        updateExecutiveData();

        cda.lockArea(LONG_NAV_AREA);
        mission_accomplished_flag = pCDALongNav->lrn_mission_accomplished;
        cda.unlockArea(LONG_NAV_AREA);

        if (mission_accomplished_flag) {
            fprintf(stdout, "\n********************************************************\n");
            fprintf(stdout, "MONITOR: EXPERIMENT WAS COMPLETED - LONG RANGE NAVIGATOR\n");
            fprintf(stdout, "MONITOR: PLEASE RESTART PROCESSES\n");
            fprintf(stdout, "********************************************************\n\n");
        }
    } else if (num_steps >= max_steps_nr) {
        cda.lockArea(CLOSE_NAV_AREA);
        pCDACloseNav->ctrl.loop = stop_cmd;
        cda.unlockArea(CLOSE_NAV_AREA);

        cda.lockArea(LONG_NAV_AREA);
        pCDALongNav->ctrl.loop = stop_cmd;
        cda.unlockArea(LONG_NAV_AREA);

        cda.lockArea(FEATURE_NAV_AREA);
        pCDAFeatureNav->ctrl.loop = stop_cmd;
        cda.unlockArea(FEATURE_NAV_AREA);

        fprintf(stdout, "\n**********************************************************************\n");
        fprintf(stdout, "MONITOR: EXPERIMENT WAS COMPLETED - MAX NUMBER OF STEPS REACHED\n");
        fprintf(stdout, "MONITOR: PLEASE RESTART PROCESSES\n");
        fprintf(stdout, "**********************************************************************\n\n");
    }
    // Go to sleep so we dont eat all the CPU
    usleep(sleep_time);

    // TODO
    //   // Whenever the experiment is over, the map must be sent
    //   // to the main computer
    //   if( num_executed_steps >= MAX_STEPS && MAX_STEPS != -1)
    //   {
    //      free_steps = false;
    //      fprintf(stdout, "Maximum number of steps (%d) reached\n", num_executed_steps);
    //
    //      fprintf(stdout, "Requesting to send map to main computer...\n");
    //
    //      cda.lockArea(CLOSE_NAV_AREA);
    //         pCloseNav->ctrl.loop = stop_cmd;
    //      cda.unlockArea(CLOSE_NAV_AREA);
    //
    //      cda.lockArea(NETWORK_AREA);
    //         pNetwork->send_map_flag = true;
    //      cda.unlockArea(NETWORK_AREA);
    //
    //      // Modificaciones para el LRN
    //      cda.lockArea(LONG_NAV_AREA);
    //         pLongNav->lg_move_ready_flag = false;
    //         pLongNav->lg_route_obsolete_flag = true;
    //      cda.unlockArea(LONG_NAV_AREA);
    //      num_executed_steps = 0;
    //      // Modificaciones para el LRN
    //   }


    return (0);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

void MonitorProcessor::updateVariables() {
    hysteresis_flag = (hysteresis_CRN_steps > MAX_CLOSE_RANGE_HYSTERESIS);
    cda.lockArea(MONITOR_AREA);
    pCDAMonitor->monitor_hysteresis = hysteresis_flag;
    cda.unlockArea(MONITOR_AREA);

    collision_flag = checkSensors();
    flag_fn = checkFlagFN();

    //   printf("Variable:\nhysteresis=%d\ncollision=%d\nflag_fn =%d,hysteresis_CRN_steps=%d\n",
    //         hysteresis_flag, collision_flag, flag_fn, hysteresis_CRN_steps);
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

void MonitorProcessor::setNavigationMode() {
    int old_nav = current_nav;

    if (old_nav == CRN) {
        if (status_LRN == ON && hysteresis_flag && !collision_flag && !flag_fn) {
            current_nav = LRN;
            fprintf(stdout, "Actual Navigation Mode: Long Range Navigator\n");
        } else if (status_FN == ON && hysteresis_flag && !collision_flag && flag_fn) {
            current_nav = FN;
            fprintf(stdout, "Actual Navigation Mode: Feature Navigator\n");
        }
        if (collision_flag)
            hysteresis_CRN_steps = 0;
    } else if (old_nav == LRN) {
        if ((status_CRN == ON && collision_flag) || status_LRN == OFF) {
            current_nav = CRN;
            hysteresis_CRN_steps = 0;
            fprintf(stdout, "Actual Navigation Mode: Close Range Navigator\n");
            cda.lockArea(LONG_NAV_AREA);
            pCDALongNav->lrn_route_obsolete_flag = true;
            cda.unlockArea(LONG_NAV_AREA);
        }
    }

    cda.lockArea(MONITOR_AREA);
    pCDAMonitor->monitor_current_nav = current_nav;
    cda.unlockArea(MONITOR_AREA);

    ///////////////////////////////////////////////////////////////////////////
    // FEARTURE NAVIGATOR
    //            else if( status_FN == ON && flag_fn ){
    //                printf("flag_fn = %d \n",flag_fn);
    //                current_nav = FN;
    //                fprintf(stdout, "Actual Navigation Mode: Feature Navigator\n");
    //                cda.lockArea(LONG_NAV_AREA);
    //                pCDALongNav->lrn_route_obsolete_flag = true;
    //                cda.unlockArea(LONG_NAV_AREA);
    //            }
    //    else if (old_nav == FN) {
    //        if ((status_CRN == ON && collision_flag) || (status_LRN == OFF && !flag_fn)) {
    //            current_nav = CRN;
    //            hysteresis_CRN_steps = 0;
    //            fprintf(stdout, "Actual Navigation Mode: Close Range Navigator\n");
    //
    //            if (status_CRN == ON && collision_flag) {
    //                cda.lockArea(FEATURE_NAV_AREA);
    //                pCDAFeatureNav->fn_route_obsolete_flag = true;
    //                cda.unlockArea(FEATURE_NAV_AREA);
    //            }
    //
    //        } else if (status_LRN == ON && !flag_fn) {
    //            current_nav = LRN;
    //            fprintf(stdout, "Actual Navigation Mode: Long Range Navigator\n");
    //        }
    //
    //    }
    /////////////////////////////////////////////////////////////////////
    // if (current_nav != old_nav) // maybe???

}


//-------------------------------------------------------------------
//-------------------------------------------------------------------

void MonitorProcessor::updateExecutiveData() {
    float _speed; // = 0.5;
    float _steer; // = 0.5;
    int _speed_percent, _movement;
    bool _move_flag;
    int temp;

    if (exec_or_nav == NAV) {

        if (current_nav == CRN) {
           // cout << "Current Navigator: CRN";
            cda.lockArea(CLOSE_NAV_AREA);
            _speed = pCDACloseNav->crn_speed;
            _steer = pCDACloseNav->crn_steering;
            _move_flag = pCDACloseNav->crn_move_ready_flag;
            cda.unlockArea(CLOSE_NAV_AREA);
        }

        // Listening to LRN
        if (current_nav == LRN) {
            // cout << "Current Navigator: LRN";
            cda.lockArea(LONG_NAV_AREA);
            _speed = pCDALongNav->lrn_speed;
            _steer = pCDALongNav->lrn_steering;
            _speed_percent = pCDALongNav->lrn_speed_percent;
            _movement = pCDALongNav->lrn_movement;
            _move_flag = pCDALongNav->lrn_move_ready_flag;
            cda.unlockArea(LONG_NAV_AREA);
        }

        // Listening to FN
//        if (current_nav == FN) {
//            // printf("Escuchando al FN\n");
//            cda.lockArea(FEATURE_NAV_AREA);
//            _speed = pCDAFeatureNav->fn_speed;
//            _steer = pCDAFeatureNav->fn_steering;
//            _move_flag = pCDAFeatureNav->fn_move_ready_flag;
//            cda.unlockArea(FEATURE_NAV_AREA);
//        }

        if (_move_flag) {

            /* *******************************************************************************************************
               FIXED NAVIGATOR

               _speed = 0.8;
               _steer = 0.5;

               if ((num_steps >=30 && num_steps < 37) || (num_steps >=58 && num_steps < 65)  || (num_steps >=125 && num_steps < 137))
               {
               _speed = 0.8;
               _steer = 0.7;
               }

               else if ((num_steps >=215 && num_steps < 225) || (num_steps >=267 && num_steps < 280) || (num_steps >=352 && num_steps < 370) || (num_steps >=395 && num_steps < 413))
               {
               _speed = 0.5;
               _steer = 0.3;
               }


             ********************************************************************************************************/

            // Update Executive Data
            cda.lockArea(EXECUTIVE_AREA);
            pCDAExecutive->exec_speed = _speed;
            pCDAExecutive->exec_steering = _steer;
            pCDAExecutive->exec_speed_percent = _speed_percent;
            pCDAExecutive->exec_movement = _movement;
            pCDAExecutive->exec_move_ready_flag = _move_flag;
            cda.unlockArea(EXECUTIVE_AREA);
            cout << "Executing Navigator";
            exec_or_nav = EXEC;
        }
    }// end if( exec_or_nav == NAV )

        // Listening to EXECUTIVE
    else {
        // printf("Escuchando al executive\n");
        // Update Executive Data
        cda.lockArea(EXECUTIVE_AREA);
        _move_flag = pCDAExecutive->exec_move_ready_flag;
        cda.unlockArea(EXECUTIVE_AREA);

        if (_move_flag == false) {
            if (current_nav == CRN) {
                hysteresis_CRN_steps++;
                //            printf("Variable:hysteresis_CRN_steps++\n");

                cda.lockArea(CLOSE_NAV_AREA);
                pCDACloseNav->crn_move_ready_flag = _move_flag;
                cda.unlockArea(CLOSE_NAV_AREA);
            }

            if (current_nav == LRN) {
                cda.lockArea(LONG_NAV_AREA);
                pCDALongNav->lrn_move_ready_flag = _move_flag;
                cda.unlockArea(LONG_NAV_AREA);
            }

            if (current_nav == FN) {
                cda.lockArea(FEATURE_NAV_AREA);
                pCDAFeatureNav->fn_move_ready_flag = _move_flag;
                cda.unlockArea(FEATURE_NAV_AREA);
            }
            exec_or_nav = NAV;
            num_steps++;
            fprintf(stdout, "Paso %d ejecutado.\n", num_steps);
            //if (num_steps%25 == 0)
            //getchar();
        }
    }
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------

bool MonitorProcessor::checkSensors() {
    float x[5];

    cda.lockArea(EXECUTIVE_AREA);
    x[0] = pCDAExecutive->exec_adc[0];
    x[1] = pCDAExecutive->exec_adc[1];
    x[2] = pCDAExecutive->exec_adc[2];
    x[3] = pCDAExecutive->exec_adc[3];
    x[4] = pCDAExecutive->exec_adc[4];
    cda.unlockArea(EXECUTIVE_AREA);

    bool _collision = ((x[0] < min_sensor_dist && x[0] > 0) || (x[1] < min_sensor_dist && x[1] > 0) ||
            (x[2] < min_sensor_dist && x[2] > 0) || (x[3] < min_sensor_dist && x[3] > 0) || (x[4] < min_sensor_dist && x[4] > 0));

    return _collision;
}


//-------------------------------------------------------------------
//-------------------------------------------------------------------

bool MonitorProcessor::checkFlagFN() {
    bool _flag_fn;
    if (status_FN == ON) {
        cda.lockArea(FEATURE_NAV_AREA);
        _flag_fn = pCDAFeatureNav->fn_move_active_flag;
        cda.unlockArea(FEATURE_NAV_AREA);
    } else {
        _flag_fn = false;
    }

    return _flag_fn;
}

//*******************************************************************
// MONITOR PROCESSOR: MonitorProcessor Class Implementation: END
//*******************************************************************

//-------------------------------------------------------------------
//-------------------------------------------------------------------

int main() {
    fprintf(stdout, "\nIRMA2: Monitor Process \n");
    MonitorProcessor mon;

    mon.execute();

    return (0);
}
