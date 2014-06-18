#ifndef HITREG_H
#define HITREG_H

#include "../../common/CogMem/memregister.h"

class HitReg
{
public:
    HitReg();
    HitReg(MemRegister *reg,int idSeg,double error);
    MemRegister *reg;
    int idSeg;
    double error;
};

#endif // HITREG_H
