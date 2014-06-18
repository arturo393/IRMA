void start_pwm(int i2cDev);

void set_M_left (float gVal, int i2cDev); // gVal de 0 a 1
void set_M_right (float gVal, int i2cDev); // gVal de 0 a 1

float sense_adc (int channel, int i2cDev);
int sense_c (int channel, int i2cDev);
int start_robostix(const char *i2cDevName);
void stop_robostix(int i2cDev);
