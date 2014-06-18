/* 
 * File:   arduino.cpp
 * Author: root
 * 
 * Created on 12 de mayo de 2013, 08:50 PM
 */

#include "arduino.h"

Arduino::Arduino() {
    uartDev = start_msp("/dev/rfcomm1");
}

Arduino::Arduino(const Arduino& orig) {
}

Arduino::~Arduino() {
    stop_msp(uartDev);
}
float Arduino::sense(int channel) {
	float tmp;
	tmp = sense_adc (channel, uartDev);
	return tmp;
}
float Arduino::giro() {
	float tmp;
	tmp = sense_giro(uartDev);
	return tmp;
}
int Arduino::encoder(int channel) {
        int tmp;
        //tmp = sense_c (channel, uartDev);
        return tmp;
}

void Arduino::move(float speed, float steering) {
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

