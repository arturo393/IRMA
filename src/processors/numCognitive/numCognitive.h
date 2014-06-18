#include "../../base/processor.h"
#include "cNN.h"

class NumCognitiveProcessor : public ProcessorInterface
{
private:

    cConceptualNN red;
    NumCogData   *pNumCognitive;

public:

    NumCognitiveProcessor() : ProcessorInterface()
    {
        ;
    }

    int init();
    int step();
    int cleanup();

};
