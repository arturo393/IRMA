#include "memorythread.h"

MemoryThread::MemoryThread()
{
}


MemoryThread::MemoryThread(int nThread,QMutex *mutex,int *beta,bool *hit,MemRegister * blob,MemSegment *segment,QList<HitReg>*results,double *error)
{
    this->nThread = nThread;
    this->beta = beta;
    this->image = image;
    this->mutex = mutex;
    this->hit = hit;
    this->blob = blob;
    this->segment = segment;
    this->results = results;
    this->error = error;
}

void MemoryThread::run(){
    double mse;
    segment->ann->loadInputWithImage(*blob);
    segment->ann->feedForward();
    segment->ann->MSE();

    *error = MSE_FACTOR * segment->ann->mse_err;
    printf("Seg%d error:%f\n",nThread,*error);
    fflush(stdout);

    if(*error<*beta){
        for(int i=0;i<segment->nRegistros;i++){
            mse = MSE_FACTOR * MSE(blob->imagen,segment->Registros[i].imagen,IMAGESIZE);
            if(mse < *beta){
                //add image as a hit to the list
                mutex->lock();
                    *results << HitReg(&segment->Registros[i],nThread,mse);
                mutex->unlock();
            }
        }
    }
}

double MemoryThread::MSE(unsigned char *a, unsigned char *b, int to, int from){
    if(a==NULL || b==NULL){
        fprintf(stderr,"MSE: the arrays must be not null");
    }
    if(to<from){
        fprintf(stderr,"MSE: from must be lest or equal than to");
    }
    double sumerr=0;
    double temp;
    for(int i=from;i<=to;i++){
        temp = b[i]-a[i];
        temp/=256;
        sumerr += temp*temp;
    }
    return sumerr/(to-from+1);
}

