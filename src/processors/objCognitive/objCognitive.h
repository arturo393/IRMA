#include "../base/processor.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "memorythread.h"
#include "hitreg.h"

class ObjCognitiveProcessor : public ProcessorInterface
{
private:

    CamBlobData * pCameraBlobber;
    ObjCognitiveData * pObjCogProc;


    QList<MemSegment*> segments;
    QList<MemoryThread*> memThreads;
    QList<bool> hit;
    QList<double> errors;
    QList<HitReg> *results;
    int alpha,beta,gamma,nThreads;
    QMutex mutex;
    bool creanuevo;
    MemRegister *actReg;
    bool evalAdding(double error);

public:

    ObjCognitiveProcessor();
    int init();
    int step();
    int cleanup();

};
