/* 
 * File:   arduino.h
 * Author: root
 *
 * Created on 12 de mayo de 2013, 08:50 PM
 */

#ifndef ARDUINO_H
#define	ARDUINO_H

#include "driverSerial.h"

class Arduino : DriverSerial {
public:
    Arduino();
    Arduino(const Arduino& orig);
    virtual ~Arduino();
    virtual float sense(int channel);
    virtual int encoder(int channel);
    virtual float giro();
    virtual void move(float speed, float steering);
private:
    int uartDev;

};

#endif	/* ARDUINO_H */

