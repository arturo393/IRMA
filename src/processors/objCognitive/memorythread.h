#ifndef MEMORYTHREAD_H
#define MEMORYTHREAD_H

#include <QThread>
#include <QMutex>

#include "../../common/CogMem/memsegment.h"
#include "../../common/CogMem/memregister.h"
#include "../../common/CogMem/ANN.h"
#include "hitreg.h"

class MemoryThread : public QThread
{
public:
    MemoryThread();
    MemoryThread(int nThread,QMutex *mutex, int *beta,bool *hit,MemRegister * blob,MemSegment *segment,QList<HitReg>*results, double* error);
    MemSegment* segment;

private:
    void run();
    int id;
    bool *creanuevo,*hit;
    char * image;
    QMutex * mutex;
    QList<HitReg> *results;
    int *beta,nThread;
    double *error;
    MemRegister * blob;
    double MSE(unsigned char* a,unsigned char* b, int size,int from=0);


};

#endif // MEMORYTHREAD_H
