#ifndef MEMREGISTER_H
#define MEMREGISTER_H

#include <cstdlib>
#include <errno.h>
#include "../../common/CogMem/parameters.h"


//Clase basica de registro de datos almacenados.
class MemRegister{
public:
    MemRegister(){}

    ~MemRegister(){}

    MemRegister(const MemRegister &o){
        for(int i=0;i<IMAGESIZE;i++){
            imagen[i]=o.imagen[i];
        }
    }

    //datos de imagen almacenada
    unsigned char imagen[IMAGESIZE];
};


#endif // MEMREGISTER_H
