/* 
 * File:   clientdummy.h
 * Author: root
 *
 * Created on 13 de mayo de 2013, 03:07 PM
 */

#ifndef CLIENTDUMMY_H
#define	CLIENTDUMMY_H

class clientdummy {
    
public:
    clientdummy();
    clientdummy(const clientdummy& orig);
    virtual ~clientdummy();
    void init();
    virtual float sense(int channel);
    virtual int encoder(int channel);
    virtual float giro();
    virtual void move(float speed, float steering);
private:
    int sockfd;

};

#endif	/* CLIENTDUMMY_H */

