/************************************************************************************
 * FILE: closeRangeNavigator.cpp, v.1.0.0, 07/30/2009
 * Authors: Tomas Arredondo, Gabriel Cisternas, Linus Casassa.
 *
 * IRMA-2: Close Range Navigation Process
 *
 * Close Range Navigation logic used on IRMA-2 Proyect
 *
 * DISCLAIMER: No liability is assumed by the author for any use made
 * of this program.
 * DISTRIBUTION: Any use may be made of this program, as long as the
 * clear acknowledgment is made to the author in code and runtime executables
************************************************************************************/
/******************************************************************************
*
*   Copyright (c) 2009 Tomas Arredondo, Gabriel Cisternas, Linus Casassa.
*   	<tomas.arredondo@usm.cl>
*
*   This program is open-source software; you can redistribute it and/or modify
*   it under the terms of the GNU Lesser General Public License version 3 as
*   published by the Free Software Foundation.
*
*   See README and LICENSE for more details.
*
******************************************************************************/
#include <stdio.h>
#include "closeRangeNavigator.h"
#include <sys/shm.h>
#include <unistd.h>

int CRNProcessor::init()
{
    pCloseNav = (CloseNavData *)cda.getMemPtr(CLOSE_NAV_AREA);
    pExecutive = (ExecutiveData *)cda.getMemPtr(EXECUTIVE_AREA);

	pCtrl = &(pCloseNav->ctrl); // Todos los procesadores tienen que definir pCtrl
	sem_area = CLOSE_NAV_AREA; // Todos los procesadores tienen que definir sem_area

    initInputs(rl);
    initOutputs(rl);

	pCtrl->loop = run_cmd;

	cda.lockArea(CLOSE_NAV_AREA);
		pCloseNav->crn_move_ready_flag = false;
	cda.unlockArea(CLOSE_NAV_AREA);
    return 0;
}


int CRNProcessor::cleanup() {
	fprintf(stdout, "\nIRMA2: Close Range Navigator Finished \n");
    return 0;
}


int CRNProcessor::step()
{
    // capture input values
    x[0]=pExecutive->exec_adc[0];
    x[1]=pExecutive->exec_adc[1];
    x[2]=pExecutive->exec_adc[2];
    x[3]=pExecutive->exec_adc[3];
    x[4]=pExecutive->exec_adc[4];


    
    fprintf(stdout,"%f %f %f %f %f\n",
            pExecutive->exec_adc[0],pExecutive->exec_adc[1],pExecutive->exec_adc[2],pExecutive->exec_adc[3],pExecutive->exec_adc[4]);
    
    /*
     * Adelante:     
     * Atras:
     * Derecha:
     * Izquierda:
     * 
     */

    
    // eval degree membership for inputs with input member functions
    in_fns.EvalInputs(x);

    // calc mamdani output
    mamdani.calc(rl, in_fns, drive);
    

    // To avoid brusque movements, the mamdani outputs are
    // limit to a min of 0.2 and a max of 0.8
    if(drive.speed<0.2)
        drive.speed=0.2;
    if(drive.steer<0.2)
        drive.steer=0.2;
    if(drive.speed>0.8)
        drive.speed=0.8;
    if(drive.steer>0.8)
        drive.steer=0.8;

	cda.lockArea(CLOSE_NAV_AREA);
		// Inform the magnitudes for the next step
                
		pCloseNav->crn_speed=drive.speed;
		pCloseNav->crn_steering=drive.steer;
               
	cda.unlockArea(CLOSE_NAV_AREA);

	printf("Inputs %f %f %f %f %f\n", x[0], x[1], x[2], x[3], x[4]);
	printf("Outputs speed:%f - steer:%f\n\n", drive.speed, drive.steer);

	cda.lockArea(CLOSE_NAV_AREA);
		// Inform executive that a new move is ready
		pCloseNav->crn_move_ready_flag=true;
	cda.unlockArea(CLOSE_NAV_AREA);
	// Go to sleep so robot can execute move
	usleep(500000);

	return 0;
}

int main()
{
	fprintf(stdout, "\nIRMA2: Close Range Navigator Process \n");
	CRNProcessor crn;

	crn.execute();

	return 0;
}

void CRNProcessor::initInputs(fuzzy_rules_t& rl)
{
    /*
    cout << "***** Sample Fuzzy Code using simfuzlib *****" << endl << endl;
    cout << "Based on IRMA2 robot which has 5 input sensors (values 0-1)." << endl << endl;
    cout << "Each input is applied to fuzzy membership functions : Low, Med, High." << endl;
    cout << "After defuzzification two ouputs (drive & steer) are produced (values 0-1)." << endl;
    cout << "Based on the fuzzy rules, the robot has a predilection to go straight if able" << endl;
    cout << "otherwise it will seek an empty forward area to move to but will turn to the right when totally blocked." << endl << endl;
    */

    // init fuzzy rules
    int h=0; // 3^5=243 rules!

    for (int g=0; g < 3; g++)
        for (int i=0; i < 3; i++)
            for (int j=0; j < 3; j++)
                for (int k=0; k < 3; k++)
                    for (int l=0; l < 3; l++)
                    {
                        rl.in[h][0]=g;
                        rl.in[h][1]=i;
                        rl.in[h][2]=j;
                        rl.in[h][3]=k;
                        rl.in[h][4]=l;
                        h++;
                    }
}

void CRNProcessor::initOutputs(fuzzy_rules_t& rl)
{
        //L=0, M=1, H=2

        // rl.out[i][0] => L=back M=stop H=forward
        // rl.out[i][1] => L=left M=straight H=right

        int i=0;
        rl.out[i][0]=L;
        rl.out[i][1]=H;
        i++; // L L L L L num:0
        rl.out[i][0]=L;
        rl.out[i][1]=H;
        i++; // L L L L M
        rl.out[i][0]=L;
        rl.out[i][1]=H;
        i++; // L L L L H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L L M L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L L M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L L M H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L L H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L L H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L L H H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L M L L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L M L M num:10
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L M L H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L M M L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L M M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L M M H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L M H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L M H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L M H H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L H L L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L H L M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L H L H num:20
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L H M L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L H M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L H M H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L H H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L H H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L L H H H

        //Van 27 reglas

        rl.out[i][0]=L;
        rl.out[i][1]=H;
        i++; // L M L L L num:27
        rl.out[i][0]=L;
        rl.out[i][1]=H;
        i++; // L M L L M
        rl.out[i][0]=L;
        rl.out[i][1]=H;
        i++; // L M L L H
        rl.out[i][0]=L;
        rl.out[i][1]=H;
        i++; // L M L M L num:30
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L M L M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L M L M H
        rl.out[i][0]=L;
        rl.out[i][1]=H;
        i++; // L M L H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L M L H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L M L H H
        rl.out[i][0]=L;
        rl.out[i][1]=H;
        i++; // L M M L L
        rl.out[i][0]=L;
        rl.out[i][1]=H;
        i++; // L M M L M
        rl.out[i][0]=L;
        rl.out[i][1]=H;
        i++; // L M M L H
        rl.out[i][0]=L;
        rl.out[i][1]=H;
        i++; // L M M M L
        rl.out[i][0]=H;
        rl.out[i][1]=H;
        i++; // L M M M M num:40
        rl.out[i][0]=H;
        rl.out[i][1]=H;
        i++; // L M M M H
        rl.out[i][0]=H;
        rl.out[i][1]=H;
        i++; // L M M H L
        rl.out[i][0]=H;
        rl.out[i][1]=H;
        i++; // L M M H M
        rl.out[i][0]=H;
        rl.out[i][1]=H;
        i++; // L M M H H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // L M H L L
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // L M H L M
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // L M H L H
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // L M H M L
        rl.out[i][0]=H;
        rl.out[i][1]=H;
        i++; // L M H M M
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // L M H M H num:50
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // L M H H L
        rl.out[i][0]=H;
        rl.out[i][1]=H;
        i++; // L M H H M
        rl.out[i][0]=H;
        rl.out[i][1]=H;
        i++; // L M H H H

        //Van 54 reglas

        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L H L L L num:54
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L H L L M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L H L L H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L H L M L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L H L M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L H L M H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L H L H L num:60
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L H L H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L H L H H
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // L H M L L
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // L H M L M
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // L H M L H
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // L H M M L
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // L H M M M
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // L H M M H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // L H M H L
        rl.out[i][0]=H;
        rl.out[i][1]=H;
        i++; // L H M H M num:70
        rl.out[i][0]=H;
        rl.out[i][1]=H;
        i++; // L H M H H
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // L H H L L
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // L H H L M
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // L H H L H
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // L H H M L
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // L H H M M
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // L H H M H
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // L H H H L
        rl.out[i][0]=H;
        rl.out[i][1]=H;
        i++; // L H H H M
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // L H H H H num:80

        //Van 81 reglas

        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M L L L L num:81
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L L L M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L L L H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M L L M L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L L M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L L M H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M L L H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L L H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L L H H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M L M L L num:90
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L M L M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L M L H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M L M M L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L M M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L M M H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L M H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L M H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L M H H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M L H L L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L H L M num:100
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L H L H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M L H M L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L H M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L H M H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L H H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L H H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M L H H H

        //Van 108 reglas!

        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M M L L L num:108
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M M L L M
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M M L L H num:110
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M M L M L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M M L M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M M L M H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M M L H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M M L H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M M L H H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M M M L L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M M M L M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M M M L H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M M M M L num:120
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M M M M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M M M M H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M M M H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M M M H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M M M H H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M M H L L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M M H L M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M M H L H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M M H M L
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // M M H M M num:130
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // M M H M H
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // M M H H L
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // M M H H M
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // M M H H H

        //Van 135 Reglas

        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M H L L L num:135
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M H L L M
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M H L L H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M H L M L
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M H L M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M H L M H num:140
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M H L H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M H L H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // M H L H H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M H M L L
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M H M L M
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M H M L H
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // M H M M L
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // M H M M M
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // M H M M H
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // M H M H L num:150
        rl.out[i][0]=H;
        rl.out[i][1]=H;
        i++; // M H M H M
        rl.out[i][0]=H;
        rl.out[i][1]=H;
        i++; // M H M H H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M H H L L
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M H H L M
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // M H H L H
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // M H H M L
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // M H H M M
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // M H H M H
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // M H H H L
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // M H H H M num:160
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // M H H H H

        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H L L L L num:162
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H L L L M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L L L H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H L L M L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L L M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L L M H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H L L H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L L H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L L H H num:170
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H L M L L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L M L M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L M L H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H L M M L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L M M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L M M H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L M H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L M H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L M H H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H L H L L num:180
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L H L M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L H L H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L H M L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L H M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L H M H
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L H H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L H H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H L H H H

        //Van 189 reglas

        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M L L L num:189
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M L L M num:190
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M L L H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M L M L
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M L M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H M L M H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M L H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H M L H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H M L H H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M M L L
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M M L M
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M M L H num:200
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M M M L
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M M M M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H M M M H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M M H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H M M H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H M M H H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M H L L
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M H L M
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H M H L H
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // H M H M L num:210
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // H M H M M
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // H M H M H
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // H M H H L
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // H M H H M
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // H M H H H

        //Quedan 216 reglas!!

        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H H L L L num:216
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H H L L M
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H H L L H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H H L M L
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H H L M M num:220
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H H L M H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H H L H L
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H H L H M
        rl.out[i][0]=M;
        rl.out[i][1]=H;
        i++; // H H L H H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H H M L L
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H H M L M
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H H M L H
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // H H M M L
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // H H M M M
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // H H M M H num:230
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // H H M H L
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // H H M H M
        rl.out[i][0]=H;
        rl.out[i][1]=H;
        i++; // H H M H H
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H H H L L
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H H H L M
        rl.out[i][0]=M;
        rl.out[i][1]=L;
        i++; // H H H L H
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // H H H M L
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // H H H M M
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // H H H M H
        rl.out[i][0]=H;
        rl.out[i][1]=L;
        i++; // H H H H L num:240
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // H H H H M
        rl.out[i][0]=H;
        rl.out[i][1]=M;
        i++; // H H H H H
}
