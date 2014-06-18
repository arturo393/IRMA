#ifndef MEMSEGMENT_H
#define MEMSEGMENT_H

#include "ANN.h"
#include "../../common/CogMem/parameters.h"
#include <QThread>

#define FILEVER 1
// version 1: registro con imagenes de 960 bytes (32w*30h) usando QList en el segmento
// version 2: numero de registros estatico, se elimina el uso de QList

class MemSegment
{
public:
    MemSegment();
    MemSegment(ANN* ann);
    MemSegment(char * filename);

    int Add(MemRegister nuevo);
  // int Add(QImage nuevo);
    int Add(const unsigned char * nuevoBits);
    void Save(char * filename);
    void Load(char * filename);
    ANN * ann;

    int nRegistros;
    MemRegister Registros[NREG];
};

#endif // MEMSEGMENT_H
