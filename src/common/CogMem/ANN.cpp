#include "ANN.h"
//#include "bpnn/backprop.h"
#include "memsegment.h"
#include <math.h>

#define ABS(x)          (((x) > 0.0) ? (x) : (-(x)))


ANN::ANN(int size){

    list_errors = 0;
    input_n = size;
    hidden_n = NHIDDEN;
    output_n = size;
    input_units = alloc_1d_dbl(input_n);
    hidden_units = alloc_1d_dbl(hidden_n);
    output_units = alloc_1d_dbl(output_n);

    hidden_delta = alloc_1d_dbl(hidden_n);
    output_delta = alloc_1d_dbl(output_n);
    target = alloc_1d_dbl(output_n);

    input_weights = alloc_2d_dbl(input_n, hidden_n);
    hidden_weights = alloc_2d_dbl(hidden_n, output_n);

    input_prev_weights = alloc_2d_dbl(input_n, hidden_n);
    hidden_prev_weights = alloc_2d_dbl(hidden_n, output_n);

    randomizeWeights(input_weights, input_n, hidden_n);
    randomizeWeights(hidden_weights, hidden_n, output_n);
    zeroWeights(input_prev_weights, input_n, hidden_n);
    zeroWeights(hidden_prev_weights, hidden_n, output_n);
}

ANN::~ANN(){

}

void ANN::linkToSegment(MemRegister *Regs,int *nRegs){
    Registros = Regs;
    nRegistros = nRegs;
}

void ANN::randomizeWeights(double **w,int m,int n)
{
  int i, j;

  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {
      w[i][j] = dpn1();
    }
  }
}

void ANN::zeroWeights(double **w,int m,int n)
{
  int i, j;

  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {
      w[i][j] = 0.0;
    }
  }
}

void ANN::layerForward(double *l1, double *l2, double **conn, int n1, int n2)
{
  double sum;
  int j, k;

  /*** Set up thresholding unit ***/
  l1[0] = 1.0;

  /*** For each unit in second layer ***/
  for (j = 0; j < n2; j++) {

    /*** Compute weighted sum of its inputs ***/
    sum = 0.0;
    for (k = 0; k < n1; k++) {
      sum += conn[k][j] * l1[k];
    }
    l2[j] = squash(sum);
  }

}

void ANN::backpropFace(int epoch)
{
  int train_n, i;
  double sumerr;

  epochs = epoch;
  train_n = *this->nRegistros;

  if (epochs > 0) {
    printf("Training underway (going to %d epochs)\n", epochs);
    printf("Will save network every %d epochs\n", savedelta);
    fflush(stdout);
  }

  /*** Print out performance before any epochs have been completed. ***/
  printf("0 0.0 ");
  fflush(stdout);
  performanceOnImagelist();
  printf("\n");  fflush(stdout);
  if (list_errors) {
    printf("\nFailed to classify the following images from the training set:\n");
    performanceOnImagelist();
  }

  /************** Train it *****************************/
  for (epoch = 1; epoch <= epochs; epoch++) {

    printf("%d ", epoch);  fflush(stdout);

    sumerr = 0.0;
    for (i = 0; i < train_n; i++) {

      /** Set up input units on net with image i **/
      loadInputWithImage((MemRegister) Registros[i]);

      /** Set up target vector for image i **/
      for(int k=0;k<IMAGESIZE;k++){
          target[k] = ((double)Registros[i].imagen[k])/ 255.0;
      }

      /** Run backprop, learning rate 0.3, momentum 0.3 **/
      train(0.3, 0.3);

      sumerr += (out_err + hid_err);
    }
    printf("%g ", sumerr/train_n);

    printf("%g ", mse_err);

    /*** Evaluate performance on train, test, test2, and print perf ***/
    if (!(epoch % 10)) {
        performanceOnImagelist();
    }
    printf("\n");  fflush(stdout);

    /*** Save network every 'savedelta' epochs ***/
    if (!(epoch % savedelta)) {

    //  rutina de salvado //bpnn_save(net, netname);
    }

  }
  printf("\n"); fflush(stdout);

  /** Save the trained network **/
  if (epochs > 0) {
      for(int k=0;k<IMAGESIZE;k++){
          Resultado.imagen[k] = output_units[k]* 255.0;
      }
    //bpnn_save(this,netname);
  }
}

void ANN::train(double eta, double momentum)
{

  /*** Feed forward input activations. ***/
  layerForward(input_units, hidden_units,input_weights, input_n, hidden_n);
  layerForward(hidden_units, output_units,hidden_weights, hidden_n, output_n);

  /*** Compute error on output and hidden units. ***/
  outputError();
  hiddenError();
  MSE(output_units,target,output_n,0,&mse_err);


  /*** Adjust input and hidden weights. ***/
  adjustWeights(output_delta, output_n, hidden_units, hidden_n, hidden_weights, hidden_prev_weights, eta, momentum);
  adjustWeights(hidden_delta, hidden_n, input_units, input_n,input_weights, input_prev_weights, eta, momentum);

}


/*** Computes the performance of a net on the images in the imagelist. ***/
/*** Prints out the percentage correct on the image set, and the
     average error between the target and the output units for the set. ***/
void ANN::performanceOnImagelist()
{
  double err, val;
  int i, n, j, correct;

  err = 0.0;
  correct = 0;
  n = *nRegistros;
  if (n > 0) {
    for (i = 0; i < n; i++) {

      /*** Load the image into the input layer. **/
      loadInputWithImage((MemRegister) Registros[i]);

      /*** Run the net on this input. **/
      feedForward();

      /*** Set up the target vector for this image. **/
      for(int k=0;k<IMAGESIZE;k++){
          target[k] = ((double)Registros[i].imagen[k])/ 255.0;
          Resultado.imagen[i] = this->output_units[i] * 255;
      }

      /*** See if it got it right. ***/
      if (evaluatePerformance(&val)) {
        correct++;
      } else if (list_errors) {
        printf("outputs ");
        for (j = 1; j < this->output_n; j++) {
          printf("%.3f ", this->output_units[j]);
        }
        putchar('\n');
      }
      err += val;
    }
    err = err / (double) n;
    mean_err = err;

    if (!list_errors)
      printf("%g %g ", ((double) correct / (double) n) * 100.0, err);
  } else {
    if (!list_errors)
      printf("0.0 0.0 ");
  }
}

int ANN::evaluatePerformance(double *err)
{
  double delta;

  delta = this->target[1] - this->output_units[1];

  *err = (0.5 * delta * delta);

  /*** If the target unit is on... ***/
  if (this->target[1] > 0.5) {

    /*** If the output unit is on***/
    if (this->output_units[1] > 0.5) {
      return (1);

    /*** otherwise, we didn't think it was me... ***/
    } else {
      return (0);
    }

  /*** Else, the target unit is off... ***/
  } else {

    /*** If the output unit is on, then we mistakenly thought it was me ***/
    if (this->output_units[1] > 0.5) {
      return (0);

    /*** else, we correctly realized that it wasn't me ***/
    } else {
      return (1);
    }
  }

}

extern void exit();

#define TARGET_HIGH 0.9
#define TARGET_LOW 0.1

void ANN::loadInputWithImage(MemRegister img)
{
  double *units;
  int nr, nc, imgsize, i;

  nr = WIDTH;
  nc = HEIGHT;
  imgsize = IMAGESIZE;

  if (imgsize != this->input_n) {
    printf("LOAD_INPUT_WITH_IMAGE: This image has %d pixels,\n", imgsize);
    printf("   but your net has %d input units.  I give up.\n", this->input_n);
    exit (-1);
  }

  units = this->input_units;
  for (i = 0; i < imgsize; i++) {
      units[i] = ((double) img.imagen[i]) / 255.0;
  }
}

void ANN::outputError()
{
  int j;
  double o, t, errsum;

  errsum = 0.0;
  for (j = 1; j < output_n; j++) {
    o = output_units[j];
    t = target[j];
    output_delta[j] = o * (1.0 - o) * (t - o);
    errsum += ABS(output_delta[j]);
  }
  out_err = errsum;
}


void ANN::hiddenError()
{
  int j, k;
  double h, sum, errsum;

  errsum = 0.0;
  for (j = 1; j < hidden_n; j++) {
    h = hidden_units[j];
    sum = 0.0;
    for (k = 1; k < output_n; k++) {
      sum += output_delta[k] * hidden_weights[j][k];
    }
    hidden_delta[j] = h * (1.0 - h) * sum;
    errsum += ABS(hidden_delta[j]);
  }
  hid_err = errsum;
}

void ANN::MSE(){
    mse_err = MSE(output_units,input_units,output_n,0);
}
void ANN::MSE(double *a, double *b, int to, int from,double *mse){
    *mse = MSE(a,b,to,from);
}

double ANN::MSE(double *a, double *b, int to, int from){
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
        sumerr += temp*temp;
    }
    return sumerr/(to-from+1);
}


void ANN::adjustWeights(double *delta,int ndelta,double *ly,int nly, double **w, double **oldw, double eta, double momentum)
{
  double new_dw;
  int k, j;

  ly[0] = 1.0;
  for (j = 1; j < ndelta; j++) {
    for (k = 0; k < nly; k++) {
      new_dw = ((eta * delta[j] * ly[k]) + (momentum * oldw[k][j]));
      w[k][j] += new_dw;
      oldw[k][j] = new_dw;
    }
  }
}


void ANN::feedForward()
{
  /*** Feed forward input activations. ***/
  layerForward(input_units, hidden_units, input_weights, input_n, hidden_n);
  layerForward(hidden_units, output_units, hidden_weights, hidden_n, output_n);

}

void ANN::saveToFile(FILE * fd)
{
    int  i, j, memcnt;
    double dvalue;
    char *mem;

    fprintf(stdout,"Saving %dx%dx%d network to file\n", input_n, hidden_n, output_n);
    fflush(stdout);

    fwrite( (char *) &input_n, sizeof(int),1,fd);
    fwrite((char *) &hidden_n, sizeof(int),1,fd );
    fwrite( (char *) &output_n, sizeof(int),1,fd);

    memcnt = 0;
    mem = (char *) malloc ((unsigned) ((input_n) * (hidden_n) * sizeof(double)));
    for (i = 0; i < input_n; i++) {
      for (j = 0; j < hidden_n; j++) {
        dvalue = input_weights[i][j];
        fastcopy(&mem[memcnt], &dvalue, sizeof(double))
        memcnt += sizeof(double);
      }
    }
    fwrite(mem, (input_n) * (hidden_n) * sizeof(double),1,fd);
    free(mem);

    memcnt = 0;
    mem = (char *) malloc ((unsigned) ((hidden_n) * (output_n) * sizeof(double)));
    for (i = 0; i < hidden_n; i++) {
      for (j = 0; j < output_n; j++) {
        dvalue = hidden_weights[i][j];
        fastcopy(&mem[memcnt], &dvalue, sizeof(double))
        memcnt += sizeof(double);
      }
    }
    fwrite( mem, (hidden_n) * (output_n) * sizeof(double),1,fd);
    free(mem);

    return;
}

ANN* ANN::loadFromFile(FILE * fd)
{
    char *mem;
    ANN * _new;
    int  n1, n2, n3, i, j, memcnt;

    fread((char *) &n1, sizeof(int),1,fd);
    fread((char *) &n2, sizeof(int),1,fd);
    fread((char *) &n3, sizeof(int),1,fd);
    _new = new ANN(n1);

   fprintf(stderr,"contains a %dx%dx%d network\n",n1, n2, n3);
   fprintf(stderr,"Reading input weights...");  fflush(stdout);

    memcnt = 0;
    mem = (char *) malloc ((unsigned) ((n1) * (n2) * sizeof(double)));
    fread( mem, (n1) * (n2) * sizeof(double),1,fd);
    for (i = 0; i < n1; i++) {
      for (j = 0; j < n2; j++) {
        fastcopy(&(_new->input_weights[i][j]), &mem[memcnt], sizeof(double))
        memcnt += sizeof(double);
      }
    }
    free(mem);

   fprintf(stderr,"Done\nReading hidden weights...");  fflush(stdout);

    memcnt = 0;
    mem = (char *) malloc ((unsigned) ((n2) * (n3) * sizeof(double)));
    fread(mem, (n2) * (n3) * sizeof(double),1,fd);
    for (i = 0; i < n2; i++) {
      for (j = 0; j < n3; j++) {
        fastcopy(&(_new->hidden_weights[i][j]), &mem[memcnt], sizeof(double))
        memcnt += sizeof(double);
      }
    }
    free(mem);

   fprintf(stderr,"Done\n");  fflush(stdout);

    zeroWeights(_new->input_prev_weights, n1, n2);
    zeroWeights(_new->hidden_prev_weights, n2, n3);

    return _new;
}

/*** Return random number between 0.0 and 1.0 ***/
double ANN::drnd()
{
  return ((double) rand() / (double) RAND_MAX);
}

/*** Return random number between -1.0 and 1.0 ***/
double ANN::dpn1()
{
  return ((drnd() * 2.0) - 1.0);
}

/*** Allocate 1d array of doubles ***/

double *ANN::alloc_1d_dbl(int n)
{
  double *_new;

  _new = (double *) malloc ((unsigned) (n * sizeof (double)));
  if (_new == NULL) {
   fprintf(stderr,"ALLOC_1D_DBL: Couldn't allocate array of doubles\n");
    return (NULL);
  }
  return (_new);
}


/*** Allocate 2d array of doubles ***/

double **ANN::alloc_2d_dbl(int m,int n)
{
  int i;
  double **_new;

  _new = (double **) malloc ((unsigned) (m * sizeof (double *)));
  if (_new == NULL) {
   fprintf(stderr,"ALLOC_2D_DBL: Couldn't allocate array of dbl ptrs\n");
    return (NULL);
  }

  for (i = 0; i < m; i++) {
    _new[i] = alloc_1d_dbl(n);
  }

  return (_new);
}
