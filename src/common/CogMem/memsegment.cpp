
//#include "memregister.h"
#include "memsegment.h"

MemSegment::MemSegment(char * filename)
{
    nRegistros = 0;
    Load(filename);
}

MemSegment::MemSegment()
{
    nRegistros = 0;
    ann = new ANN(IMAGESIZE);
    ann->linkToSegment(Registros,&nRegistros);
}

MemSegment::MemSegment(ANN* ann_)
{
    nRegistros = 0;
    ann = ann_;
    ann->linkToSegment(Registros,&nRegistros);
}

int MemSegment::Add(MemRegister nuevo){
    if( nRegistros++ < NREG){
        Registros[nRegistros-1]=nuevo;
    }else{
        nRegistros --;
        return -1;
    }
    printf("my registers: %d\n",nRegistros);
    return 0;
}

int MemSegment::Add(const unsigned char* nuevoBits){
    MemRegister *registro = new MemRegister();

    for(int i=0;i<IMAGESIZE;i++){
        registro->imagen[i] = nuevoBits[i];
    }
    Add(*registro);
    return 0;
}

/** not compatible with non-GUI application due the use of QImage.
int MemSegment::Add(const QImage nuevo){
    MemRegister *registro = new MemRegister();
    uchar* entrada = (uchar*) nuevo.bits();

    for(int i=0;i<IMAGESIZE;i++){
        registro->imagen[i] = entrada[i];
    }
    Add(*registro);
    return 0;
}*/


void MemSegment::Save(char *filename){
    FILE * fd = fopen(filename,"wb+");
    if(fd > 0){
        printf("Guardar Segmento: %d bytes\n",sizeof(*this));
        fflush(stdout);
        fwrite(this,sizeof(*this),1,fd);
        printf("Guardar ANN\n");
        fflush(stdout);
        ann->saveToFile(fd);
        fclose(fd);
    }else{
        fprintf(stderr,"MEMSEGMENT: Failed opening file descriptor '%s', the data will be lost.\n",filename);
    }
}

void MemSegment::Load(char *filename){
    FILE * fd = fopen(filename,"rb+");

    printf("leyendo segmento: %d bytes\n",sizeof(*(this)));
    fflush(stdout);
    fread(this,sizeof(*this),1,fd);

    for(int i=0;i<960;i++){
        printf("%d ",this->Registros[0].imagen[i]);
    }

    printf("leyendo ANN\n");
    fflush(stdout);

    ann = ANN::loadFromFile(fd);
    ann->linkToSegment(Registros,&nRegistros);
    fclose(fd);
}
