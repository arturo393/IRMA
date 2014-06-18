#ifndef MEMCOG_PARAMETERS_H
#define MEMCOG_PARAMETERS_H

//MemSegment
#define WIDTH 32        //register image width
#define HEIGHT 30       //register image height
#define NREG 100        //Number of registers stored in one memory segment

//MemRegister
#define IMAGESIZE 960   //WIDTH*HEIGHT

//objCognitive
#define ALPHA 70        //well known threshold.
#define BETA 90         //known theshold.
#define GAMMA 130       //not known but familiar theshold.
#define RESULTS 10      //max results to output
#define MSE_FACTOR 10000 //amplifying factor for MSE

//cameraBlobber
#define LAMBDA 70       //Image Binary Thresholding Threshold
#define BLOB_EDGE 10    //added border for clipping avoidance.
#define MAX_BLOBS 10    //maximum quantity of blobs to recognize per frame
#define FWIDTH 640      //frame width
#define FHEIGHT 480     //frame height

#endif
