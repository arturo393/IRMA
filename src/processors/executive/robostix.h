#define LEFT    0
#define RIGHT   1
#include "driverSerial.h"

class Robostix : DriverSerial
{
private:     
        int i2cDev;
public:
        // Constructor
        Robostix();
        // Destructor
        virtual ~Robostix();
        // Funciones miembro o metodos
        virtual float sense(int channel);
        virtual int encoder(int channel);
        virtual float giro();
        virtual void move(float speed, float steering);
        virtual void move_v(float vd, float vi);
};
