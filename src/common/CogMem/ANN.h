#ifndef ANN_H
#define ANN_H
#include "memregister.h"
#include "math.h"
#include <cstdio>
#include <QtGui/QMainWindow>
#include <QString>
//#include <QImage>

#define NHIDDEN 400

#define ABS(x)          (((x) > 0.0) ? (x) : (-(x)))

#define fastcopy(to,from,len)\
{\
  register char *_to,*_from;\
  register int _i,_l;\
  _to = (char *)(to);\
  _from = (char *)(from);\
  _l = (len);\
  for (_i = 0; _i < _l; _i++) *_to++ = *_from++;\
}



class ANN
{
public:

    ANN(int size);
    ANN(QString *trainname,QString *test1name,QString *test2name);
    ~ANN();

    //BPNN *net;

    int input_n;                  /* number of input units */
    int hidden_n;                 /* number of hidden units */
    int output_n;                 /* number of output units */

    double *input_units;          /* the input units */
    double *hidden_units;         /* the hidden units */
    double *output_units;         /* the output units */

    double *hidden_delta;         /* storage for hidden unit error */
    double *output_delta;         /* storage for output unit error */

    double *target;               /* storage for target vector */

    double **input_weights;       /* weights from input to hidden layer */
    double **hidden_weights;      /* weights from hidden to output layer */

                            /*** The next two are for momentum ***/
    double **input_prev_weights;  /* previous change on input to hidden wgt */
    double **hidden_prev_weights; /* previous change on hidden to output wgt */

    double hid_err;
    double out_err;
    double mse_err;

    double mean_err;
    double sum_err;

    MemRegister *Registros;
    MemRegister Resultado;
    int *nRegistros;
    int epochs;
    int savedelta;
    int list_errors;
    char *netname;

    void linkToSegment(MemRegister *Regs,int *nRegs);
    void backpropFace(int epoch);
    void performanceOnImagelist();
    int  evaluatePerformance(double *err);
    void loadInputWithImage(MemRegister);
    void loadTarget(MemRegister);
    void train(double eta, double momentum);
    void layerForward(double *l1, double *l2, double **conn, int n1, int n2);
    void feedForward();
    void adjustWeights(double *delta,int ndelta,double *ly,int nly, double **w, double **oldw, double eta, double momentum);
    void hiddenError();
    void outputError();
    void saveToFile(FILE * fd);
    double *alloc_1d_dbl(int n);
    double **alloc_2d_dbl(int m,int n);
    void MSE(double *a, double *b, int to, int from,double *mse);
    double MSE(double *a, double *b, int to, int from);
    void MSE();

    static void randomizeWeights(double** w,int m,int n);
    static void zeroWeights(double** w,int m,int n);
    static ANN* loadFromFile(FILE * fd);
    static double drnd();
    static double dpn1();
            /*** The squashing function.  Currently, it's a sigmoid. ***/

    double squash(double x)
    {
      return (1.0 / (1.0 + exp(-x/100)));
    }
};

#endif // ANN_H
