/************************************************************************************
 * FILE:driverSerial.cpp, v.1.0.0, 07/30/2009
 * Author: Javiera Quiroz
 *
 * MARBOT: Driver Serial
 *
 * Implements the function calls to interact with the hardware
 *
 * DISCLAIMER: No liability is assumed by the author for any use made
 * of this program.
 * DISTRIBUTION: Any use may be made of this program, as long as the
 * clear acknowledgment is made to the author in code and runtime executables
************************************************************************************/

#include <time.h>
#include "driverSerial.h"

//using namespace std;

// Constructor
DriverSerial::DriverSerial() {}

// Destructor
DriverSerial::~DriverSerial() {}

// Funciones miembro o metodos
float DriverSerial::sense(int channel) {}

int DriverSerial::encoder(int channel) {}
float DriverSerial::giro() {}
void DriverSerial::move(float speed, float steering) {}
void DriverSerial::move_lrn(int seed_percent, int movement) {}
void DriverSerial::move_v(float vd,float vi){}
void DriverSerial::move_test(int _speed_percent,int _movement){}
