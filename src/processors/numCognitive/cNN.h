/***********************************************************************
 * MARBOT: COGNITIVE CONCEPTUAL NEURAL NETWORK
 ***********************************************************************/
//*******************************************************************
// Title: cNN.cpp
// Authors: Javiera Quiroz R
// Version: v.0.0.1
//
//*******************************************************************

#ifndef _MARBOT_CCONCEPT_NN_H_
#define _MARBOT_CCONCEPT_NN_H_

//*******************************************************************
// Librery Declaration
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

//OpenCV Declaration
#include <cv.h>
#include <highgui.h>

#define UMBRAL   3
#define MU       0.7
#define EPOCH    1000
#define ERROR    0.05
#define TRAIN    1
#define EVALUATE 0
//*******************************************************************
// Global Variables Definition
//don`t have any...

typedef struct nodeweight
{
    int    idl_1;             ///id de la capa actual
    int    idl_2;             ///id de la capa previa
    double wgth;
    double deltaW;
                              ///Puntero al siguiente peso para calcular
                              /// la entrada de la neurona a la que está asociada
                              /// la lista de pesos.
    struct nodeweight *nextwgth;
} nnWeight, *pnnWeight;

typedef struct node
{
    int    id;
    int    layer;
    float  umbral;
    double input;
    double output;
    double grad;            ///Gradiente de la neurona, para entrenamiento
                            ///Puntero a la siguiente neurona de la capa l.
    struct node *nn;
                            ///Puntero a la fila de pesos que multiplicados
                            /// con la capa n-1 entrega la entrada de la neurona
                            /// el tamaño de la lista de pesos debe ser igual al
                            /// tamaño de la lista de neuronas de la capa n-1.
    struct nodeweight *wgth;

} nnStruct, *pnnStruct;

typedef struct nodeLayer
{
    int    idLayer;
    int    numNeuronasLayer;
    struct nodeLayer *nextLayer;
    struct nodeLayer *prevLayer;
    struct node *nn;

} nnLayer, *pnnLayer;


//*******************************************************************
// cConceptual Class Declaration: BEGIN
//*******************************************************************
class cConceptualNN{

	private:

	pnnLayer layerList;
	pnnLayer inverseLayerList;
	pnnLayer somLayerList;
	char     tipo[10];
	double   outputTrain[11];
	char     status;


    int setNeuralNode(pnnStruct nn, int id, int layer, float umbral); //listo

    pnnStruct createNeuralNode();//listo
    int       deleteNeuralNode(         pnnStruct nn);//listo
    int       addNeuralNode2NeuralNode( pnnStruct* nn1,  pnnStruct* nn2 );//listo
    int       addWeight2NeuralNode(     pnnWeight wgth, pnnStruct* pnn );//listo

    int addNeuralNode2Layer(pnnLayer* layer, pnnStruct pnn);//listo

    pnnLayer createLayer(); //listo
    int      setLayerNode(pnnLayer layer, int idLayer, int numNeuronas); //listo
    int      deleteLayer(pnnLayer layer); //listo
    int      addLayer2Layer(pnnLayer* layer1, pnnLayer* layer2);//listo

    pnnLayer  createLayerOfNeurons( int neuronNum, int wgtNum, int layer );//listo
    pnnWeight createWeightNode();//listo
    int       deleteWeightNode(pnnWeight* wgth);//listo
    int       addWeightNode2WeightNode(pnnWeight* wgth1, pnnWeight* wgth2);//listo


    int setNodeId(pnnStruct nn, int id); //listo
    int getNodeId(pnnStruct nn);//listo

    int setNodeLayer(pnnStruct nn, int layer);//listo
    int getNodeLayer(pnnStruct nn);//listo

    int   setNodeUmbral(pnnStruct nn, float umbral); //listo
    float getNodeUmbral(pnnStruct nn);//listo


	double getWeight(    pnnWeight wgth ); //listo
	int    setWeight(    pnnWeight wgth, double val, int idActual, int idPrevio );//listo
	int    updateWeight( pnnWeight wgth, double val );//listo

	double getWeightPrevLayer( pnnStruct nn, int idPrevNN );
	double sigmoide( double input ); //listo

	int loadImg2NN(int i);
	int writeNN2Img();

	double randGenerator();//listo

	int printNN();

	int createNeuralMARBOT(); //listo
	int trainMARBOT(); //listo
	int evaluateMARBOT();//casi listo

	int createNeuralSOM();//listo
	int trainSOM();
	int evaluateSOM();

	public:

	cConceptualNN();
	~cConceptualNN();

	int createNeuralNet(const char* tipo2); //listo
	int train();//listo
	int evaluate();//listo

	int freeNN();

	char* getTipo();//listo pero revisar funcionamiento
	int   setTipo(const char* tipo2);//falta función para copiar cadena de carácteres.

    int setStatus(char stat);
	char getStatus();

};
//*******************************************************************
// cConceptualProcessor Class Declaration: END
//*******************************************************************

#endif

