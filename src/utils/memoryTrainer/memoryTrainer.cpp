#include "../../common/CogMem/memsegment.h"
#include "../../common/CogMem/memregister.h"
#include "../../common/CogMem/ANN.h"
#include "memoryTrainer.h"
#include <QtGui/QApplication>

int main(int argc, char * argv[])
{
    QCoreApplication a(argc, argv);

    QList<MemoryTrainer*> memTrainers;
    bool exists;
    int i=1;

    char filename[500];
    FILE * fd;

    do{
        sprintf(filename,"../../common/CogMem/Memory/segmento%d.mem",i);
        fd = fopen(filename,"rb+");
        if(fd > 0){
            fclose(fd);
            MemoryTrainer * temp = new MemoryTrainer(i);
            memTrainers << temp;
            memTrainers[i-1]->start();
            i++;
            exists=true;

        }else{
            exists=false;
        }
    }while(exists);

    for(int j=0;j<i-1;j++){
        memTrainers[j]->wait();
    }
    return 0;
}
MemoryTrainer::MemoryTrainer(){
}


MemoryTrainer::MemoryTrainer(int id_)
{
    id = id_;
}
void MemoryTrainer::run()
{
    MemSegment *segment;
    char filename[500];

    sprintf(filename,"../../common/CogMem/Memory/segmento%d.mem",id);

    segment = new MemSegment(filename);
    segment->ann->backpropFace(30);
    segment->Save(filename);
}
