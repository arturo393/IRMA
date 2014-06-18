#ifndef _DRIVER_SERIAL_H_
#define _DRIVER_SERIAL_H_

class DriverSerial
{
private:
        
public:
        // Constructor
        DriverSerial();
        // Destructor
        virtual ~DriverSerial();
        // Funciones miembro o metodos
        virtual float sense(int channel);
        virtual int encoder(int channel);
        virtual float giro();
        virtual void move(float speed, float steering);
        virtual void move_lrn(int seed_percent, int movement);
        virtual void move_v(float vd, float vi);
        virtual void move_test(int _speed_percent, int _movement);
};
#endif
