#include "numCognitive.h"


int main(/*int argc, char* argv[]*/){
    NumCognitiveProcessor m;
    m.execute();
    return 0;
}



int NumCognitiveProcessor::init()
{
    char netName[] = "MARBOT";

    red.createNeuralNet(netName);
}

int NumCognitiveProcessor::step()
{

}

int NumCognitiveProcessor::cleanup()
{
    red.freeNN();
}

