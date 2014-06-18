#include "hitreg.h"

HitReg::HitReg()
{
}

HitReg::HitReg(MemRegister *reg, int idSeg, double error)
{
    this->reg=reg;
    this->idSeg=idSeg;
    this->error=error;
}
