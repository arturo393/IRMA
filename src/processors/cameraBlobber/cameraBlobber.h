#include "../base/processor.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "cvblobslib/Blob.h"
#include "cvblobslib/BlobResult.h"

using namespace cv;

class CameraBlobber : public ProcessorInterface
{
private:

    VideoCapture *L_cap;
    Mat *L_Img;
    Mat *L_HSLImg;
    Mat *L_GrayImg;
    Mat *L_scaled;
    Mat *L_grayScaled;
    IplImage *L_outputI;


    VideoCapture *R_cap;
    Mat *R_Img;
    Mat *R_HSLImg;
    Mat *R_GrayImg;
    Mat *R_scaled;
    Mat *R_grayScaled;
    IplImage *R_outputI;

    CamBlobData *pCameraBlobber;

    Mat *kernel;
    int binaryThreshold;
    int edge;

public:

    CameraBlobber();

    int init();
    int step();
    int cleanup(){;}

};
