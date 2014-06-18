//void start_pwm(int i2cDev);
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
using namespace std;


void set_M_left (float gVal, int uartDev); // gVal de 0 a 1
void set_M_right (float gVal, int uartDev); // gVal de 0 a 1
void set_M_left_v (float gVal, int uartDev); // gVal de -100 a 100
void set_M_right_v (float gVal, int uartDev); // gVal de -100 a 100
void set_ml(float gVal, int uartDev);
void set_mr(float gVal, int uartDev);
void set_l(float gVal, int uartDev);
void set_r(float gVal, int uartDev);
void set_f(float gVal, int uartDev);
void set_b(float gVal, int uartDev);

float sense_adc (int channel, int uartDev);
float sense_giro (int uartDev);
int sense_c (int channel, int uartDev);
int start_msp(const char *uartDevName);
void stop_msp(int uartDev);
