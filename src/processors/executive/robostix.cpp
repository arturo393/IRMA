/************************************************************************************
 * FILE: robostix.cpp, v.1.0.0, 07/30/2009
 * Author: Linus Cassasa.
 *
 * IRMA-2: Robostix Layer
 *
 * Implements the function calls to interact with the hardware
 *
 * DISCLAIMER: No liability is assumed by the author for any use made
 * of this program.
 * DISTRIBUTION: Any use may be made of this program, as long as the
 * clear acknowledgment is made to the author in code and runtime executables
************************************************************************************/

#include <time.h>
#include "i2c-io_linus.h"
#include "robostix.h"


//using namespace std;

// Constructor
Robostix::Robostix() {
	i2cDev = start_robostix("/dev/i2c-0");
}
// Destructor
Robostix::~Robostix() {
	stop_robostix(i2cDev);
}
// Funciones miembro o metodos
float Robostix::sense(int channel) {
	float tmp;
	tmp = sense_adc (channel, i2cDev);

	return tmp;
}

int Robostix::encoder(int channel) {
        int tmp;
        tmp = sense_c (channel, i2cDev);

        return tmp;
}
void Robostix::move_v(float vd, float vi)
{
    //TODO:No implementado
}
void Robostix::move(float speed, float steering) {
	speed-=0.5;
	//speed/=2.0;
	speed/=4.0;

	steering-=0.5;
	steering/=-2.0;
	if(steering>0) {
	    set_M_left  (0.5+(speed-steering), i2cDev);
	    set_M_right (0.5+(speed), i2cDev);
	} else {
	    set_M_left  (0.5+(speed), i2cDev);
	    set_M_right (0.5+(speed+steering), i2cDev);
	}
	//usleep(250);
/*	for(int i=0; i<25; i++)
 	  usleep(1000);
	set_M_left (0.5, i2cDev);
	set_M_right(0.5, i2cDev);
*/
}

float Robostix::giro() {
}
