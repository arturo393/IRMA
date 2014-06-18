/***********************************************************************
* NEURAL NETWORK CLASS
************************************************************************/
//*******************************************************************
// Title: cNN.cpp
// Authors: Javiera Quiroz R
// Version: v.0.0.1
//
//*******************************************************************


//*******************************************************************
// Librery Declaration
#include "cNN.h"
#include "imgTrain.h"



//*******************************************************************
// Global Variables Definition
//don`t have any...
/**
 * Setea una de la neurona con sus respectivos parámetros.
 * \param nn Puntero a la Neurona representada por el tipo de dato nnStruct.
 * \param id Identificador de la neurona
 * \param layer Capa a la que pertenece la neurona
 * \param umbral Valor en el cual la neurona dispara
 * \return Un entero, 1 en caso de correcta ejecución.
 */
int cConceptualNN::setNeuralNode(pnnStruct nn, int id, int layer, float umbral)
{
    if ( nn == NULL )
    {
        printf("Error: En clase cConceptualNN, setNode(pnnStruct nn, int id, int layer, float umbral).\n");
        printf("La variable nn se encuentra vacía.\nSe sale");
        exit(1);
    }
    nn->id     = id;
    nn->layer  = layer;
    nn->umbral = umbral;
    return (1);
}

/**
 * Setea la capa con sus respectivos parámetros.
 * \param layer pnnLayer puntero a la capa a inicializar.
 * \param idLayer Identificador de la neurona
 * \param numNeuronas Entero que indica el número de neuronas de la capa
 * \return Un entero, 1 en caso éxito
 */
int cConceptualNN::setLayerNode( pnnLayer layer, int idLayer, int numNeuronas )
{
    if ( layer == NULL )
    {
        printf("Error: En clase cConceptualNN, setLayerNode(pnnStruct nn, int id, int layer, float umbral).\n");
        printf("La variable nn se encuentra vacía.\nSe sale");
        exit(1);
    }
    layer->idLayer          = idLayer;
    layer->numNeuronasLayer = numNeuronas;

    return (1);
}

/**
 * Crea una neurona reservando el espacio dinámicamente.
 * \return Puntero a la Neurona creada en caso de éxito, y NULL en el caso contrario.
 */
pnnStruct cConceptualNN::createNeuralNode()
{
      pnnStruct p;

      if ((p = (pnnStruct)malloc(sizeof(nnStruct))) != NULL)          //si p no es nulo crea el espacio para los nodos
         {
             p->id     = 0;
             p->layer  = 0;
             p->umbral = 0.0;
             p->input  = 0.0;
             p->output = 0.0;                                   //asigna a cadena el valor del string
             p->grad   = 0.0;
             p->nn     = NULL;
             p->wgth   = NULL;                               //asigna a prox la direccion a la que apunta lista

             return (p);                                      //retorna p
         }
      else  return(NULL);
}

/**
 * Crea una capa de neuronas vacía.
 * \return Puntero a la capa de neuronas creada en caso de éxito, y NULL en el caso contrario.
 */
pnnLayer cConceptualNN::createLayer()
{
    pnnLayer p;

    if ( ( p = (pnnLayer)malloc( sizeof(nnLayer) ) ) != NULL )          //si p no es nulo crea el espacio para los nodos
         {
             p->idLayer          = 0;
             p->numNeuronasLayer = 0;
             p->nextLayer        = NULL;
             p->prevLayer        = NULL;
             p->nn               = NULL;

             return (p);                                      //retorna p
         }
      else  return(NULL);
}

/**
 * Crea una capa de \a neuronNum neuronas. Cada neurona de la capa tendrá
 * \a wgtNum pesos. El identificador de la capa deberá también indicarse.
 * \param neuronNum Número entero de neuronas que contiene la capa.
 * \param wgtNum Número entero de pesos que posee cada neurona.
 * \param layer Número entero para indicar el identificador de la capa de neuronas.
 * \return la capa de neuronas creada (puntero a pnnLayer).
 */
pnnLayer cConceptualNN::createLayerOfNeurons( int neuronNum, int wgtNum, int layer )
{
    pnnLayer  tempLayer = NULL;

    pnnStruct tempNeuron = NULL;  //puntero a la lista de nodos neuronales
    pnnStruct tempNeuron2 = NULL; //puntero al nodo neuronal creado

    pnnWeight tempWgth = NULL;    //puntero a la lista de nodos pesos
    pnnWeight tempWgth2 = NULL;   //puntero al nodo peso creado

    if ( neuronNum <= 0 || wgtNum < 0 )
    {
        printf( "Error: El número de neuronas indicados o el número de" );
        printf( " pesos indicados es menor o igual a cero.\nSe sale "   );
        exit(1);
    }

    tempLayer = this->createLayer();                          //se crea capa de neuranas vacía
    this->setLayerNode(tempLayer, layer, neuronNum);          //se inicializa capa de neuronas

    // procedimiento para crear las neuronas de la capa
    for( int i = neuronNum -1; i >= 0 ; i-- )
    {
        tempNeuron2 = this->createNeuralNode();               //se crea nodo vacío

        //NOTA, MEJOR SERÍA HACER UNA SIGMOIDE...
        this->setNeuralNode( tempNeuron2, i, layer, UMBRAL);  //se inicializa el nodo

        //procedimiento para crear los pesos de cada neurona dependiendo del wgtNum
        //para red marbot wgtNum es equivalente al número de neuronas de la capa previa
        //para red som wgtNum es equivalente al número de neuronas con vecindad radio 1
        if(wgtNum > 0)
        {
            for( int j = wgtNum-1; j >= 0 ; j-- )
            {
                tempWgth2 = this->createWeightNode();         //se crea nodo peso vacío
                this->setWeight( tempWgth2, randGenerator(), i, j );  //se inicializa nodo peso
         //       printf("peso %f ",tempWgth2->wgth);

                //procedimiento para primer nodo peso creado
//                if(j <= wgtNum -1) tempWgth = tempWgth2;
//
//                else
                     this->addWeightNode2WeightNode(&tempWgth,  //se agrega a la lista de
                                                    &tempWgth2);//nodos pesos el nuevo nodo
                                                               //creado
                // printf("peso %f\n",tempWgth->wgth);
            }
            this->addWeight2NeuralNode(tempWgth,               //se agrega la lista de nodos
                                       &tempNeuron2);           //pesos a la neurona creada.
        }

        //procedimiento para el primer nodo neuronal creado
//        if (i <= neuronNum -1) tempNeuron = tempNeuron2;
//
//        else
             this->addNeuralNode2NeuralNode(&tempNeuron,        //se agrega a la lista de nodos
                                            &tempNeuron2);      //neuronales el nuevo nodo neuronal
                                                               //creado.
    }
    this->addNeuralNode2Layer( &tempLayer, tempNeuron ); //se agrega neuronas a la capa
    return (tempLayer);
}

/**
  * Generador de números aleatorios.
  * \return double número aleatorio
  */
double cConceptualNN::randGenerator()
{

   return (drand48());
}

/**
  * Función sigmoide.
  * \param input Número tipo double perteneciente al dominio de la función
  * sigmoide.
  * \return Retorna la salida de la función para el valor dado por \a input.
  */
double cConceptualNN::sigmoide(double input)
{
    return ( 1.0/(1.0 + exp( UMBRAL - input) ) );
}

/**
 * Crea un nodo peso para la neurona.
 * \return Puntero al nodo peso creado en caso de éxito, y NULL en el caso contrario.
 */
pnnWeight cConceptualNN::createWeightNode()
{
    pnnWeight p;

    if ((p = (pnnWeight)malloc(sizeof(nnWeight))) != NULL)          //si p no es nulo crea el espacio para los nodos
         {
             p->idl_1    = 0;
             p->idl_2    = 0;
             p->wgth     = 0.0;
             p->deltaW   = 0.0;
             p->nextwgth = NULL;

             return (p);                                      //retorna p
         }
      else  return(NULL);
}

/**
 * Libera el espacio ocupado por una neurona.
 * \param nnList Puntero a la Neurona representada por el tipo de dato nnStruct.
 * \return retorna 1 cuando termina
 */
int cConceptualNN::deleteNeuralNode(pnnStruct pnn)
{
    if( pnn == NULL)
        printf("Warning: la variable se encuentra null.\nNo se hace nada");
    else if ( pnn->nn != NULL )
    {
        printf("Warning: Se eliminará un puntero que no apunta a NULL");
        free(pnn);
    }
    else
        free(pnn);

    return(1);
}

/**
 * Libera el espacio ocupado por una capa de neuronas. Este método libera el espacio
 * reservado para la estructura nnLayer, no libera a las neuronas que la capa contenga.
 * \param layer Puntero a la estructura capa de neuronas.
 * \return retorna 1 cuando termina
 */
int cConceptualNN::deleteLayer(pnnLayer layer)
{
    if( layer == NULL)
        printf("Warning: la variable se encuentra null.\nNo se hace nada");
    else if ( layer->nn != NULL ||
              layer->nextLayer != NULL ||
              layer->prevLayer == NULL )
    {
        printf("Warning: Se eliminará un puntero que no apunta a NULL");
        free(layer);
    }
    else
        free(layer);

    return(1);
}


/**
 * Libera el espacio ocupado por un nodo peso. Este método libera el espacio
 * reservado para la estructura nnWeight, no libera los demás nodos al que este apunte.
 * \param wgth Puntero a la estructura de pesos.
 * \return retorna 1 cuando termina
 */
int cConceptualNN::deleteWeightNode(pnnWeight* wgth)
{
    if( *wgth == NULL)
        printf("Warning: la variable se encuentra null.\nNo se hace nada");
    else if ( (*wgth)->nextwgth != NULL)
    {
        printf("Warning: Se eliminará un puntero que no apunta a NULL");
        free(*wgth);
    }
    else
        free(wgth);

    return(1);
}

/**
 * Asocia al puntero a la siguiente Neurona de nn1 a la neurona apuntada por nn2.
 * \param nn1 Puntero a la neurona a la cual se le quiere asociar otra neurona.
 * \param nn2 Puntero a la neurona que será asociada a nn1.
 * \return retorna 1 cuando termina
 */
int cConceptualNN::addNeuralNode2NeuralNode(pnnStruct* nn1, pnnStruct* nn2)
{
    if( *nn2 == NULL) printf("Warning: cConceptualNN::addNeuralNode2NeuralNode.\nVariable nn2 nula");
    (*nn2)->nn = *nn1;        //se agrega el elemento al inicio de la lista
    *nn1     = *nn2;        //se actualiza puntero al inicio de la lista.
    return(1);
}

/**
 * Asocia a la capa de neuronas apuntada por layer2 al puntero de la siguiente capa de la capa layer1.
 * \param layer1 Puntero a la capa a la cual se le quiere asociar otra capa.
 * \param layer2 Puntero a la capa que será asociada a layer1.
 * \return retorna 1 cuando termina
 */
int cConceptualNN::addLayer2Layer(pnnLayer* layer1, pnnLayer* layer2)
{
    if(*layer2 == NULL) printf("Warning: cConceptualNN::addLayer2Layer.\nVariable layer2 nula");

    //revisar
    if( *layer1 != NULL )
    {
        (*layer1)->prevLayer = (*layer2);
    }
    (*layer2)->nextLayer = *layer1;   //se agrega el elemento al inicio de la lista
    *layer1            = *layer2;   //se actualiza puntero al inicio de la lista.

    return(1);
}

/**
 * Asocia el nodo peso apuntado por layer2 al puntero del siguiente nodo peso de wgth1.
 * \param wgth1 Puntero al nodo peso al cual se le quiere asociar otra nodo.
 * \param wgth2 Puntero al nodo que será asociado a wgth1.
 * \return retorna 1 cuando termina
 */
int cConceptualNN::addWeightNode2WeightNode(pnnWeight* wgth1, pnnWeight* wgth2)
{
    if(*wgth2 == NULL)
    {
        printf("Warning: cConceptualNN::addWeightNode2WeightNode(pnnWeight wgth1, pnnWeight wgth2)");
        printf("\nVariable wgth2 nula");
    }
    //printf("en agregar peso: %f\t",(*wgth2)->wgth);
    ///revisar acá
    if(*wgth1 != NULL)
    (*wgth2)->nextwgth = (*wgth1);   //se agrega el elemento al inicio de la lista.
    (*wgth1)           = (*wgth2);   //se actualiza puntero al incio de la lista.
    //printf("valor %f\n",(*wgth1)->wgth);
    return(1);
}

/**
 * Asocia la lista de pesos a la neurona.
 * \param wgth Puntero a la lista con los pesos a asociar.
 * \param pnn Puntero a la neurona a la que se asociarán los pesos.
 * \return retorna 1 cuando termina
 */
int cConceptualNN::addWeight2NeuralNode(pnnWeight wgth, pnnStruct* pnn)
{
    //verificación de pnn
    if ( *pnn == NULL )
    {
        printf("Error: en cConceptualNN::addWeight2NeuralNode(pnnWeight wgth, pnnStruct pnn)");
        printf("\npnn es un puntero a nulo. Se sale");
        exit(1);
    }

    //verificación de wgth
    if( wgth == NULL )
    {
     printf("Warning: en cConceptualNN::addWeight2NeuralNode(pnnWeight wgth, pnnStruct pnn)");
     printf("\nwgth es un puntero a null");
    }

    //asignación de wgth a pnn
    (*pnn)->wgth = wgth;
    return(1);
}

/**
 * Asocia la lista de neuronas a la capa.
 * \param layer Puntero al que se le asociará la lista de neuronas
 * \param pnn Puntero a la lista de neuronas.
 * \return retorna 1 cuando termina
 */
 int cConceptualNN::addNeuralNode2Layer(pnnLayer* layer, pnnStruct pnn)
{
    //verificación de layer
    if ( *layer == NULL )
    {
        printf("Error: en cConceptualNN::addNeuralNode2Layer(pnnLayer layer, pnnStruct pnn)");
        printf("\npnnLayer es un puntero a nulo. Se sale");
        exit(1);
    }

    //verificación de pnn
    if(pnn == NULL)
    {
     printf("Warning: en cConceptualNN::addNeuralNode2Layer(pnnLayer layer, pnnStruct pnn)");
     printf("\npnn es un puntero a null");
    }

    //asignación de pnn a layer
    (*layer)->nn = pnn;
    return(1);
}

/**
 * Setea el identificador de la neurona.
 * \param nn Puntero a la Neurona representada por el tipo de dato nnStruct.
 * \param id Identificador de la neurona
 * \return Un entero, 1 en caso de correcta ejecución.
 */
int cConceptualNN::setNodeId(pnnStruct nn, int id)
{
    //verificación de nn
    if ( nn == NULL )
    {
        printf("Error: En clase cConceptualNN, setNodeId(pnnStruct nn, int id).\n");
        printf("La variable nn apunta a nulo.\nSe sale");
        exit(1);
    }

    //seteo del id de nn
    nn->id = id;
    return 1;

}

/**
 * Obtiene el identificador del nodo neuronal indicado por \a nn
 * \param nn Puntero al nodo neuronal representado por el tipo de dato nnStruct.
 * \return Un entero, el identificador del nodo.
 */
int cConceptualNN::getNodeId(pnnStruct nn)
{
    //verificación de nn
    if ( nn == NULL )
    {
        printf("Error: En clase cConceptualNN, getNode(pnnStruct nn).\n");
        printf("La variable nn se encuentra vacía.\nSe sale");
        exit(1);
    }

    //se retorna identificador de nn
    return nn->id;
}

/**
 * Setea la capa a la que pertenece la Neurona.
 * \param nn Puntero a la Neurona representada por el tipo de dato nnStruct.
 * \param layer Capa a la que pertenece la neurona
 * \return Un entero, 1 en caso de correcta ejecución.
 */
int cConceptualNN::setNodeLayer(pnnStruct nn, int layer)
{
    //verificación de nn
    if ( nn == NULL)
    {
        printf("Error: En clase cConceptualNN, setNodeLayer(pnnStruct nn, int layer).\n");
        printf("La variable nn apunta a NULL.\nSe sale");
        exit(1);
    }

    //se inicializa identificador de capa de nn
    nn->layer = layer;
    return 1;
}

/**
 * Obtiene la capa a la que pertenece la neurona \a nn.
 * \param nn Puntero al nodo neuronal representado por el tipo de dato nnStruct.
 * \return Un entero, el valor de la capa a la que pertenece el nodo.
 */
int cConceptualNN::getNodeLayer(pnnStruct nn)
{
    //verificación de nn
    if ( nn == NULL )
    {
        printf("Error: En clase cConceptualNN, getNodeLayer(pnnStruct nn).\n");
        printf("La variable nn apunta a NULL.\nSe sale");
        exit(1);
    }

    //retorno del valor de la capa a la que pertenece la neurona
    return nn->layer;
}

/**
 * Setea el umbral de disparo de la neurona
 * \param nn Puntero a la Neurona representada por el tipo de dato nnStruct.
 * \param umbral Valor en el cual la neurona dispara
 * \return Un entero, 1 en caso de correcta ejecución.
 */
int cConceptualNN::setNodeUmbral(pnnStruct nn, float umbral)
{
    //verificación de nn
    if ( nn == NULL )
    {
        printf("Error: En clase cConceptualNN, setNodeUmbral(pnnStruct nn, float umbral).\n");
        printf("La variable nn se encuentra vacía.\nSe sale");
        exit(1);
    }

    //inicialización del umbral de la neurona
    nn->umbral = umbral;
    return 1;
}

/**
 * Obtiene el umbral de disparo de la neurona
 * \param nn Puntero a la Neurona representada por el tipo de dato nnStruct.
 * \return Un entero, 1 en caso de correcta ejecución.
 */
float cConceptualNN::getNodeUmbral(pnnStruct nn)
{
    //verificación de nn
    if ( nn == NULL )
    {
        printf("Error: En clase cConceptualNN, getNodeUmbral(pnnStruct nn).\n");
        printf("La variable nn se encuentra vacía.\nSe sale");
        exit(1);
    }

    //retorno del valor del umbral de la neurona
    return nn->umbral;
}

/**
 * Obtiene el peso del nodo peso indicado por \a wgth
 * \param wgth Puntero al nodo peso representado por el tipo de dato nnWeight.
 * \return Un double, el valor del peso del nodo.
 */
double cConceptualNN::getWeight(pnnWeight wgth)
{
    //verificación de wgth
    if ( wgth == NULL )
    {
        printf("Error: En clase cConceptualNN, getWeight(pnnWeight wgth).\n");
        printf("La variable wgth se encuentra vacía.\nSe sale");
        exit(1);
    }
    //retorno del valor de wgth
    return wgth->wgth;
}

/**
 * Setea el peso del nodo peso indicado por \a wgth con el valor \a val.
 * \param wgth Puntero al nodo peso representado por el tipo de dato nnWeight.
 * \param idActual En el caso de red MARBOT corresponde al identificador
 * de la neurona de la capa actual a la que pertenece el peso.\n
 * Para el caso de la red SOM corresponde al identificador de la neurona a la
 * que pertenece el peso.
 * \param idPrevio En el caso de red MARBOT corresponde al identificador de la
 * neurona de la capa previa a la que pertenece el peso.\n
 * Para el caso de la red SOM, corresponde al identificador de la neurona vecina
 * dentro de la vecindad de radio 1.
 * \param val  Valor con el que se seteará el peso del nodo (double).
 * \return Entero 1 en caso de éxito.
 */
int cConceptualNN::setWeight(pnnWeight wgth, double val, int idActual, int idPrevio)
{
    if ( wgth == NULL )
    {
        printf("Error: En clase cConceptualNN, setWeight(pnnWeight wgth, double val).\n");
        printf("La variable wgth se encuentra vacía.\nSe sale");
        exit(1);
    }
    wgth->wgth  = val;
    wgth->idl_1 = idActual;
    wgth->idl_2 = idPrevio;
    return 1;
}

/**
 * Actualiza el peso del nodo peso indicado por \a wgth con el valor \a val.
 * \param wgth Puntero al nodo peso representado por el tipo de dato nnWeight.
 * \param val  Valor con el que se actualizará el peso del nodo (double).
 * \return Entero 1 en caso de éxito.
 */
int cConceptualNN::updateWeight(pnnWeight wgth, double val)
{
    if ( wgth == NULL )
    {
        printf("Error: En clase cConceptualNN, updateWeight(pnnWeight wgth, double val).\n");
        printf("La variable wgth se encuentra vacía.\nSe sale");
        exit(1);
    }
    wgth->wgth = val;
    return 1;
}


/**
  * Crea una red neuronal dependiendo de lo indicado en \a tipo.
  * \param tipo Puntero a char que indica el tipo de red a crear:
  * estos pueden ser: SOM o MARBOT.
  * \return un entero.
  */
int cConceptualNN::createNeuralNet(const char* tipo2)
{

    this->setTipo(tipo2);

    int c;

    if( strcmp( this->tipo, "SOM" ) == 0 ) c = 0;
    else if (strcmp( this->tipo, "MARBOT" ) == 0 ) c = 1;
    else c = 2;

    switch(c)
    {
        case 0:
                    printf("Creando red SOM...");
                    this->createNeuralSOM();
                    break;
        case 1:
                    printf("Creando red MARBOT...");
                    this->createNeuralMARBOT();
                    break;
        default:
                    printf("El tipo de red indicado no existe.\n");
                    printf("Los tipos de redes disponibles son SOM y MARBOT. Se sale\n");
                    exit(1);
                    break;
    }
  return(1);   //código para crear red de neuronas
}

/**
  * Entrena la red neuronal creada.
  * \return un entero en caso de éxito.
  */
int cConceptualNN::train()
{
    int c;

    if( strcmp( this->tipo, "SOM" ) == 0 ) c = 0;
    else if (strcmp( this->tipo, "MARBOT" ) == 0 ) c = 1;
    else c = 2;

    switch(c)
    {
        case 0:
                    printf("Entrenando red SOM...");
                    this->trainSOM();
                    break;
        case 1:
                    printf("Entrenando red MARBOT...");
                    this->trainMARBOT();
                    break;
        default:
                    printf("El tipo de red seteado es incorrecto.\n");
                    printf("Los tipos de redes disponibles son SOM y MARBOT. Se sale\n");
                    exit(1);
                    break;
    }
  return(1);
}

/**
  * Evalúa la red neuronal creada.
  * \return un entero en caso de éxito.
  */
int cConceptualNN::evaluate()
{
    int c;

    if( strcmp( this->tipo, "SOM" ) == 0 ) c = 0;
    else if (strcmp( this->tipo, "MARBOT" ) == 0 ) c = 1;
    else c = 2;


    switch(c)
    {
        case 0:
                    printf("Evaluando red SOM...");
                    this->evaluateSOM();
                    break;
        case 1:
                    printf("Evaluando red MARBOT...");
                    this->evaluateMARBOT();
                    break;
        default:
                    printf("El tipo de red seteado es incorrecto.\n");
                    printf("Los tipos de redes disponibles son SOM y MARBOT. Se sale\n");
                    exit(1);
                    break;
    }
  return(1);
}

/**
  * Setea el atributo tipo de la clase.
  * \param tipo cadena de carácteres con el nombre del tipo. Los tipos permitidos de nuerona
  * son MARBOT y SOM.
  * \return un entero en caso de éxito.
  */
int cConceptualNN::setTipo(const char* tipo2)
{
    if( tipo2 == NULL )
    {
        printf("Error: No indicó el tipo de neurona, en cConceptualNN::setTipo(char* tipo).\n Se sale");
        exit(1);
    }
   //  if ((tipo = (char*)malloc(sizeof(char)*10)) != NULL)
    //código para copiar texto en otra variable
    strcpy( tipo, tipo2 );
    return(1);
}

/**
  * Entrega el valor del atributo tipo de la clase.
  * \return cadena de carácteres
  * nota... revisar esta función...
  */
char* cConceptualNN::getTipo()
{
    return (this->tipo);
}

/**
  * Crea una red neuronal marbot, 13 capas de neuronas,
  * de las cuales la capa interna número 7 es la salida
  * de la red neuronal.
  * \return un número entero al momento de crear la red.
  */
int cConceptualNN::createNeuralMARBOT()
{
    int numNeuron = 0;
    int numWeigth = 0;

    pnnLayer tempLayer;

    this->layerList        = NULL;
    this->inverseLayerList = NULL;


    for( int i = 6/*12*/; i >= 0; i-- )
    {
        switch(i)
        {
            case 0:
                printf("Creando primera capa...\n");
                numWeigth = 1;                      //actualización número de pesos=número
                                                    //de neuronas capa previa
                numNeuron = 30*32;                  //actualización próximo número de neuronas
                break;

            case 1:
                printf("Creando segunda capa...\n");
                numNeuron = numWeigth;                   //actualización próximo número de neuronas
                numWeigth = 30*32;              //actualización número de pesos=número
                                                    //de neuronas capa previa

                break;

            case 2:
                printf("Creando tercera capa...\n");
                numNeuron = numWeigth;                   //actualización próximo número de neuronas
                numWeigth = 400;              //actualización número de pesos=número
                                                    //de neuronas capa previa

                break;

            case 3:
                printf("Creando cuarta capa...\n");
                numNeuron = numWeigth;                   //actualización próximo número de neuronas
                numWeigth = 200;              //actualización número de pesos=número
                                                    //de neuronas capa previa

                break;

            case 4:
                printf("Creando quinta capa...\n");
                numNeuron = numWeigth;                   //actualización próximo número de neuronas
                numWeigth = 100;              //actualización número de pesos=número
                                                    //de neuronas capa previa

                break;

            case 5:
                printf("Creando sexta capa...\n");

                numNeuron = numWeigth;                   //actualización próximo número de neuronas
                numWeigth = 50;              //actualización número de pesos=número
                                                    //de neuronas capa previa

                break;

            case 6:
                printf("Creando septima capa...\n");
                numNeuron = 10/*numWeigth*/;                     //actualización próximo número de neuronas
                numWeigth = 20;               //actualización número de pesos=número
                                                     //de neuronas capa previa

                break;
//
//            case 7:
//                printf("Creando octava capa...\n");
//                numNeuron = numWeigth;                   //actualización próximo número de neuronas
//                numWeigth = 10;              //actualización número de pesos=número
//                                                    //de neuronas capa previa
//
//                break;
//
//            case 8:
//                printf("Creando novena capa...\n");
//                numNeuron = numWeigth;                   //actualización próximo número de neuronas
//                numWeigth = 20;              //actualización número de pesos=número
//                                                    //de neuronas capa previa
//
//                break;
//
//            case 9:
//                printf("Creando décima capa...\n");
//                numNeuron = numWeigth;                   //actualización próximo número de neuronas
//                numWeigth = 50;              //actualización número de pesos=número
//                                                    //de neuronas capa previa
//
//                break;
//
//            case 10:
//                printf("Creando onceava capa...\n");
//                numNeuron = numWeigth;                   //actualización próximo número de neuronas
//                numWeigth = 100;              //actualización número de pesos=número
//                                                    //de neuronas capa previa
//
//                break;
//
//            case 11:
//                printf("Creando duodécima capa...\n");
//                numNeuron = numWeigth;                     //actualización próximo número de neuronas
//                numWeigth = 200;               //actualización número de pesos=número
//                                                     //de neuronas capa previa
//
//                break;
//
//            case 12:
//                printf("Creando treceava capa...\n");
//                numWeigth = 400;               //actualización número de pesos=número
//                                                     //de neuronas capa previa
//                numNeuron = 30*32;                   //actualización próximo número de neuronas
//                break;

            default:
                printf("No se hizo nada :( ...");
                break;
        }
        tempLayer = this->createLayerOfNeurons( numNeuron, numWeigth, i );

        this->addLayer2Layer( &(this->layerList) , &tempLayer );

        if( i == 6/*12*/ )
            this->inverseLayerList = this->layerList;
    }
    printf("Creación de red MARBOT exitosa...\n");
   // printf(" %d %d \n",this->inverseLayerList, this->layerList);
    return 1;
}


/**
  * Crea una red neuronal SOM, de 1 capa.
  * esta red se entrena utilizando el algoritmo
  * LVQ
  * \return entero 1 en caso de éxito
  */
int cConceptualNN::createNeuralSOM()
{

// revisar la creación de esta red....
    layerList      = this->createLayerOfNeurons( 30*32, 0    , 0 );
    //creo vector de entrada
    somLayerList = this->createLayerOfNeurons( 30*32, 30*32, 0 );
    printf("Creación de red SOM exitosa...");
    return (1);


}

/**
  * Método que entrena a la red neuronal tipo MARBOT
  * utilizando backpropagation y el método del descenso
  * del gradiente.
  * \return un entero cuando termina.
  */
int cConceptualNN::trainMARBOT()
{
    pnnLayer  tempLayer=NULL;

    pnnStruct tempNN=NULL;           //puntero a una neurona de la lista de neuronas de la capa actual
    pnnStruct tempNextNN=NULL;       //puntero a una neurona de la lista de neuronas de la siguiente capa
    //pnnStruct *tempFirstNNLayer; //punter a una neurona de la lista de neuronas de la primera capa
    pnnStruct tempPrevNN=NULL;       //puntero a una neurona de la lista de neuronas de la capa revia

    pnnWeight tempWgt=NULL;

    double TminusO;
    double error = 500.0;
    double sum;

    int epoch = 0;
    int i;

    this->setStatus(TRAIN);

    for( i=0;i<6;i++)
    while( ( epoch < EPOCH) && (error > ERROR ) )
    {
        epoch++;
        this->evaluateMARBOT(i);
        //procedimiento de entrenamiento
 //printf("hola");getchar();
        ///procedimiento para calcular el gradiente
        error = 0.0;
        for( tempLayer  = inverseLayerList;
             tempLayer != NULL;
             tempLayer  = tempLayer->prevLayer )
        {
       //     printf("hola2");getchar();
            for( tempNN   = tempLayer->nn,i=0;
                 tempNN != NULL;
                 tempNN   = tempNN->nn )
            {
                sum = 0.0;
                if( tempLayer->idLayer != 6/*12*/ )
                {
                    for( tempNextNN = tempLayer->nextLayer->nn, //tempPrevNN = la primera neurona de la capa siguiente (l+1)
                         tempWgt    =  tempNextNN->wgth;       // se inicializa además el puntero al peso de la neurona
                                                           //correspondiente a la capa (l+1)
                         tempNextNN != NULL;                   //mientras haya neuronas en la capa siguiente (l+1)

                         tempNextNN = tempNextNN->nn )         //se avanza una neurona
                    {
                        sum += ( this->getWeightPrevLayer(tempNextNN, tempNN->id) )*( tempNextNN->grad);
                    }
                    tempNN->grad  = sum*( tempNN->output)*(1 - tempNN->output);
                }
                else
                {
                    //procedimiento para layer 13

//                    for( tempFirstNNLayer  = layerList->nn;
//                         tempFirstNNLayer != NULL;
//                         tempFirstNNLayer  = tempFirstNNLayer->nn )
//                    for(i=0;i<10;i++)
//                    {
                        //TminusO      = tempFirstNNLayer->input - tempNN->output;
                        TminusO         = outputTrain[i] - tempNN->output;
                        tempNN->grad = (TminusO)*( tempNN->output)*(1-tempNN->output);
                        error          += 0.5*pow(TminusO,2.0);
                        printf("error %f\t dif %f\t grad %f\t outputNN %f\t outputTrain %f\n",error,TminusO,tempNN->grad, tempNN->output,outputTrain[i] );
                        i++;
                   // }
                }
            }
        }
    //    printf("chao");getchar();
        ///procedimiento para actualizar los pesos
        for( tempLayer  = layerList;             //inicialización
             tempLayer != NULL;                 //mientras existan capas
             tempLayer  = tempLayer->nextLayer ) //avanzo una capa
        {
     //       printf("hola3");getchar();
            //printf("hola");getchar();
            for( tempNN  = tempLayer->nn; //inicialización
                 tempNN != NULL;         //mientras la capa tenga neuronas
                 tempNN  = tempNN->nn )   //avanza una neurona
            {
                ///el bias de la primera capa no se actualiza
                if( tempLayer->idLayer != 0 )
                {
                         //       printf("hola");getchar();
                    for( tempPrevNN = tempLayer->prevLayer->nn, //tempPrevNN = la primera neurona de la capa previa (l-1)
                         tempWgt    =  tempNN->wgth;              // se inicializa además el puntero al peso de la neurona
                                                                //correspondiente a la capa (l)
                         tempPrevNN != NULL;                       //mientras haya neuronas en la capa previa

                         tempWgt    = tempWgt->nextwgth,         //se avanza un peso
                         tempPrevNN = tempPrevNN->nn )            //y se avanza una neurona
                    {
                               //printf("hola");getchar();
                        //if(tempLayer->idLayer == 2) printf("grad %f \t",tempNN->grad);
                        tempWgt->deltaW = MU*( tempNN->grad)*( tempPrevNN->output);
                        tempWgt->wgth  += tempWgt->deltaW;
                    }
                }
                ///aunque se podrian actualizar...
                else
                {
                    tempNN->wgth->deltaW = MU*( tempNN->grad)*( tempNN->input);
                    tempNN->wgth->wgth  += tempNN->wgth->deltaW;
                }
            }
        }
    }

    return 1;

}

/**
  * Método que evalúa una imagen entrante en la red.
  * Genera un archivo mostrando la salida de la capa número 7
  * de la red.
  * \return un entero.
  */
int cConceptualNN::evaluateMARBOT()
{
    pnnLayer  tempLayer;

    pnnStruct tempNN;
    pnnStruct tempPrevNN;

    pnnWeight tempWgt;

    double sum;
    double firstInput;

    //cuidado con este for... revisar
//    printf("peso1 %f", layerList->nn->wgth->wgth);
//    getchar();
    for( tempLayer = layerList;             //inicialización
         tempLayer != NULL;                 //mientras existan capas
         tempLayer = tempLayer->nextLayer ) //avanzo una capa
    {
//        printf("peso1 %f", tempLayer->nn->wgth->wgth);
//    getchar();
        for( tempNN  = tempLayer->nn; //inicialización
             tempNN != NULL;         //mientras la capa tenga neuronas
             tempNN  = tempNN->nn )   //avanza una neurona
        {
            sum = 0.0;

            if( tempLayer->idLayer == 0 )
            {
                //poner procedimiento caso primera capa neurona
                //this->loadImg2NN(i);   //se carga el input en la primera capa de neuronas

                firstInput = (tempNN->input)*(tempNN->wgth->wgth);
                tempNN->output = sigmoide(firstInput);      //se calcula el output en base al input

            }
            else
            {
                for( tempPrevNN = tempLayer->prevLayer->nn, //tempPrevNN = la primera neurona de la capa previa (l-1)
                     tempWgt    =  tempNN->wgth;              // se inicializa además el puntero al peso de la neurona
                                                              //correspondiente a la capa (l)
                     tempPrevNN != NULL;                       //mientras haya neuronas en la capa previa

                     tempWgt    = tempWgt->nextwgth,         //se avanza un peso
                     tempPrevNN = tempPrevNN->nn )            //y se avanza una neurona
                {
                    sum += (tempWgt->wgth)*(tempPrevNN->output);
                }
                tempNN->input  = sum;
                tempNN->output = sigmoide(tempNN->input);      //se calcula el output en base al input
                //poner procedimiento resto de las neuronas
                //falta poner el resultado que entrega la capa de neuronas Nº 7

                if( tempLayer->idLayer == 6 ) printf("%f ", tempNN->output);
            }

        }

    }
    printf("\n");
   // this->writeNN2Img();
    return 1;
}

/**
  * Método que obtiene el valor del peso que relaciona a la neurona de la
  * capa previa cuyo id está determinado por \a idPrevNN, con la neurona
  * de la capa actual apuntado por \a nn.
  * \param nn Puntero a la neurona de la capa actual.
  * \param idPrevNN Identificador de la neurona de la capa previa,
  * cuyo tipo de dato es entero.
  * \return el valor del peso que relaciona a ambas neuronas (double).
  */
double cConceptualNN::getWeightPrevLayer( pnnStruct nn, int idPrevNN )
{
    pnnWeight tempWgt;

    for( tempWgt = nn->wgth; tempWgt != NULL; tempWgt= tempWgt->nextwgth )
    {
        if( tempWgt->idl_2 == idPrevNN ) return (tempWgt->wgth);
        else continue;
    }
    printf("Warning: No se encontró el peso asociado a la neurona de la capa previa.");
    return (0.0);
}
/**
  * Método que carga la imagen cuyo nombre es el indicado en \a filename,
  * en la capa de entrada de las neuronas.
  * El método busca la imagen indicada en \a filename en el directorio ./imgTemp.
  * \param filename cadena de carácteres con el nombre del archivo de imagen a cargar.
  * \return un entero en caso de éxito.
  */
int cConceptualNN::loadImg2NN(int i)
{
    IplImage* img = 0;
    CvScalar  s;

   // char fileName[] = "input.png";

    pnnStruct tempNN;

    int i, j;

    img = cvLoadImage(fileName);

    if(!img) printf("Could not load image file: %s\n",fileName);

    //procedimiento para comprobar que el tamaño de la imagen corresponde con el
    //número de la primera capa de neuronas

    if( layerList->numNeuronasLayer != (img->width)*(img->height) )
    {
        printf("Error: El número de neuronas de la primera capa no coincide con el tamaño de la imagen.\nSe sale.");
        exit(1);
    }

    tempNN = layerList->nn;

//corregir acá para agregar más cosas
    outputTrain[0]=1.0;
    for(i=1;i<10;i++)
    outputTrain[i]=0.0;

    for( i = 0; i < (img->height); i++ )
    {
        for( j = 0; j < (img->width); j++)
        {   //se agrega valor imagen a input primera capa de neuronas
            s             = cvGet2D(img,i,j); // get the (i,j) pixel value
            tempNN->input = (s.val[0] +
                             s.val[1] +
                             s.val[2])/3.0;

            //se cambia de neurona para siguiente ciclo.
            tempNN        = tempNN->nn;
        }
    }
    cvReleaseImage(&img);

    return 1;
}

int cConceptualNN::writeNN2Img()
{
    IplImage* img=cvCreateImage(cvSize(30,32),IPL_DEPTH_8U,1);
    CvScalar s;
    pnnStruct tempNN;
    int i,j;

    char outFileName[] = "salida.png";

    tempNN = inverseLayerList->nn;

    for( i = 0; i < (img->height); i++ )
    {
        for( j = 0; j < (img->width); j++)
        {   //se agrega valor imagen a input primera capa de neuronas

            s.val[0] = tempNN->output;
            cvSet2D(img,i,j,s); // set the (i,j) pixel value

            //se cambia de neurona para siguiente ciclo.
            tempNN        = tempNN->nn;
        }
    }

     if(!cvSaveImage(outFileName,img)) printf("Could not save: %s\n",outFileName);

     cvReleaseImage(&img);


     return 1;

}

/**
  * Constructor de la clase.
  */
cConceptualNN::cConceptualNN()
{
    this->layerList        = NULL;
    this->inverseLayerList = NULL;
   // this->tipo             = NULL;

    srand48(time(NULL)); //se inicializa semilla función random

}

int cConceptualNN::freeNN()
{
    pnnLayer  tempLayer, tempLayer2;
    pnnStruct tempNN, tempNN2;
    pnnWeight tempWgt, tempWgt2;

    tempLayer = layerList;

    layerList        = NULL;
    inverseLayerList = NULL;

    while( layerList != NULL )
    {
        tempNN = tempLayer->nn; //se inicializa el temporal de neuronas
        tempLayer->nn = NULL;   //se limpia el puntero a las neuronas de la capa
        while( tempLayer->nn != NULL )
        {
            tempWgt      = tempNN->wgth;   //se inicializa el temporal de pesos
            tempNN->wgth = NULL;           //se limpia
            while( tempWgt != NULL )
            {

                 tempWgt2 = tempWgt;           //se rescata el nodo al inicio de la lista pesos
                 tempWgt  = tempWgt->nextwgth; //se cambia temp1 al siguiente en la lista de pesos

                 tempWgt2->nextwgth = NULL;    //se limpia el puntero al siguente nodo del primer nodo
                this->deleteWeightNode(&tempWgt2); //se libera el nodo
            }
            tempNN2 = tempNN;      //se rescata nodo al inicio de la lista de neuronas
            tempNN = tempNN->nn;   //se cambia temp1 al siguiente de la lista de neuronas

            tempNN2->nn = NULL;   //se limpia el puntero al siguiente nodo del primer nodo
            this->deleteNeuralNode(tempNN2);//se libera el nodo
        }
        tempLayer2 = tempLayer; //se rescata nodo al inicio de la lista de capas de nn
        tempLayer  = tempLayer->nextLayer; //se cambia el inicio de lalista a la siguiente capa

        tempLayer->prevLayer  = NULL;  //se setea el puntero de temp1 a la capa previa a nulo
        tempLayer2->nextLayer = NULL; //se setea el puntero de temp2 a la siguiente capa en nulo
        this->deleteLayer(tempLayer2);//se libera el nodo
    }
    return 1;
}
/**
  * Destructor de la clase.
  */
cConceptualNN::~cConceptualNN()
{

//    pnnLayer  tempLayer, tempLayer2;
//    pnnStruct tempNN, tempNN2;
//    pnnWeight tempWgt, tempWgt2;
//
//    tempLayer = layerList;
//
//    layerList        = NULL;
//    inverseLayerList = NULL;
//
//    while( layerList != NULL )
//    {
//        tempNN = tempLayer->nn; //se inicializa el temporal de neuronas
//        tempLayer->nn = NULL;   //se limpia el puntero a las neuronas de la capa
//        while( tempLayer->nn != NULL )
//        {
//            tempWgt      = tempNN->wgth;   //se inicializa el temporal de pesos
//            tempNN->wgth = NULL;           //se limpia
//            while( tempWgt != NULL )
//            {
//
//                 tempWgt2 = tempWgt;           //se rescata el nodo al inicio de la lista pesos
//                 tempWgt  = tempWgt->nextwgth; //se cambia temp1 al siguiente en la lista de pesos
//
//                 tempWgt2->nextwgth = NULL;    //se limpia el puntero al siguente nodo del primer nodo
//                this->deleteWeightNode(&tempWgt2); //se libera el nodo
//            }
//            tempNN2 = tempNN;      //se rescata nodo al inicio de la lista de neuronas
//            tempNN = tempNN->nn;   //se cambia temp1 al siguiente de la lista de neuronas
//
//            tempNN2->nn = NULL;   //se limpia el puntero al siguiente nodo del primer nodo
//            this->deleteNeuralNode(tempNN2);//se libera el nodo
//        }
//        tempLayer2 = tempLayer; //se rescata nodo al inicio de la lista de capas de nn
//        tempLayer  = tempLayer->nextLayer; //se cambia el inicio de lalista a la siguiente capa
//
//        tempLayer->prevLayer  = NULL;  //se setea el puntero de temp1 a la capa previa a nulo
//        tempLayer2->nextLayer = NULL; //se setea el puntero de temp2 a la siguiente capa en nulo
//        this->deleteLayer(tempLayer2);//se libera el nodo
//    }
    //hay que hacer prueva con valgrind, de lo contrario hay que cambiar el destructor.
    //delete this->layerList;
    //delete this->inverseLayerList;
    //free(this->tipo);
}

int cConceptualNN::trainSOM()
{
    this->evaluateSOM();
    return 1;
}

int cConceptualNN::evaluateSOM()
{
    pnnLayer  tempLayer;

    pnnStruct tempNN;
    pnnStruct tempPrevNN;

    pnnWeight tempWgt;

    double sum;
    double firstInput;

    //cuidado con este for... revisar
//    printf("peso1 %f", layerList->nn->wgth->wgth);
//    getchar();
    tempLayer = layerList;             //inicialización

//        printf("peso1 %f", tempLayer->nn->wgth->wgth);
//    getchar();
    for( tempNN  = tempLayer->nn; //inicialización
         tempNN != NULL;         //mientras la capa tenga neuronas
         tempNN  = tempNN->nn )   //avanza una neurona
    {
         sum = 0.0;

        if( tempLayer->idLayer == 0 )
        {
            //poner procedimiento caso primera capa neurona
            //this->loadImg2NN(i);   //se carga el input en la primera capa de neuronas

            firstInput = (tempNN->input)*(tempNN->wgth->wgth);
            tempNN->output = sigmoide(firstInput);      //se calcula el output en base al input

        }
        else
        {
            for( tempPrevNN = tempLayer->prevLayer->nn, //tempPrevNN = la primera neurona de la capa previa (l-1)
                 tempWgt    =  tempNN->wgth;              // se inicializa además el puntero al peso de la neurona
                                                              //correspondiente a la capa (l)
                 tempPrevNN != NULL;                       //mientras haya neuronas en la capa previa

                 tempWgt    = tempWgt->nextwgth,         //se avanza un peso
                 tempPrevNN = tempPrevNN->nn )            //y se avanza una neurona
            {
                sum += (tempWgt->wgth)*(tempPrevNN->output);
            }
            tempNN->input  = sum;
            tempNN->output = sigmoide(tempNN->input);      //se calcula el output en base al input
            //poner procedimiento resto de las neuronas
            //falta poner el resultado que entrega la capa de neuronas Nº 7

            if( tempLayer->idLayer == 6 ) printf("%f ", tempNN->output);
        }

    }

    return 1;
}

int cConceptualNN::setStatus(char stat)
{
    this->status = stat;
    return 1;
}

char cConceptualNN::getStatus()
{
    return this->status;
}
