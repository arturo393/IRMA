//#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <termios.h>
#include <regex.h>
#include <sys/types.h>
#include <stdint.h>

#include "uart-io.h"
#include "msp.h"
#define DEBUG 1
#define UART_WAIT 100
//using namespace std;

// Constructor
MSP::MSP() {
    //Conexión Serial a MSP
    uartDev = start_msp("/dev/ttyUSB0");
    //Conexión Bluethooth serial
    //uartDev = start_msp("/dev/rfcomm0");
}
// Destructor
MSP::~MSP() {
	stop_msp(uartDev);
}
// Funciones miembro o metodos
float MSP::sense(int channel) {
	float tmp;
        fprintf(stdout,"Canal %d\n",channel);
	tmp = sense_adc(channel, uartDev);
        //fprintf(stdout, "Canal %d: %f \n",channel,tmp);
	return tmp;
}
float MSP::giro() {
	float tmp;
	tmp = sense_giro(uartDev);
	return tmp;
}
int MSP::encoder(int channel) {
        int tmp;
        //tmp = sense_c (channel, uartDev);
        return tmp;
}
void MSP::move_v(float vd,float vi)
{       
	set_M_left_v(vd,uartDev);
	set_M_right_v(vi,uartDev);
}
void MSP::move_test(float vd,float vi)
{       
    if( vd == vi){
        if (vd >= 0)
            set_f(vd,uartDev);
        else
            set_b(vd,uartDev);
    }
    else {
        set_ml(vd,uartDev);
        set_mr(vi,uartDev);
    }
}
void MSP::move_lrn(int _speed_percent, int _movement){          
           
    if (_movement == 9)
        set_f(_speed_percent,uartDev);
    else if(_movement == 1)
        set_r(_speed_percent,uartDev);
    else if(_movement == 2)
        set_l(_speed_percent,uartDev);
    else if(_movement == 3)
        set_b(_speed_percent,uartDev);
    
}

void MSP::move(float speed, float steering) {
	//printf("Speed MSP %f\n",speed);
	speed-=0.5;
        
	//speed/=2.0;
	//speed/=4.0;

	steering-=0.5;
	//steering/=-2.0;
	steering/=-1.0;
        
	if(steering>0) {
	    set_M_left  (0.5+(speed-steering), uartDev);
	    set_M_right (0.5+(speed), uartDev);
	} else {
	    set_M_left  (0.5+(speed), uartDev);
	    set_M_right (0.5+(speed+steering), uartDev);
	}
	//usleep(250);
/*	for(int i=0; i<25; i++)
 	  usleep(1000);
	set_M_left (0.5, uartDev);
	set_M_right(0.5, uartDev);
*/
}
