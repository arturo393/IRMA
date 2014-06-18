#define LEFT    0
#define RIGHT   1
#include "driverSerial.h"

class MSP : DriverSerial {
private:
    int uartDev;
public:
    // Constructor
    MSP();
    // Destructor
    virtual ~MSP();
    // Funciones miembro o metodos
    virtual float sense(int channel);
    virtual int encoder(int channel);
    virtual float giro();
    virtual void move(float speed, float steering);
    virtual void move_lrn(int _speed_percent, int _movement);
    virtual void move_v(float vd, float vi);
    virtual void move_test(float vd,float vi);
};
