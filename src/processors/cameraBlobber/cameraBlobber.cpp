#include "cameraBlobber.h"
#include "../../common/CogMem/parameters.h"

using namespace cv;
int main(/*int argc, char* argv[]*/){
    CameraBlobber m;
    m.execute();
    /*m.init();
    for(int i=0;i<1000;i++)
    {
        m.step();
    }*/
    return 0;
}

CameraBlobber::CameraBlobber() : ProcessorInterface()
{
    ;
}

/** executes the principal initalization steps before starting the capture process
  */
int CameraBlobber::init(){
    printf("CameraBlobber: init...  \n");
    //pointer to the camera blobber CDA
    pCameraBlobber = (CamBlobData *)cda.getMemPtr(CAM_BLOB_AREA);

    pCtrl = &(pCameraBlobber->ctrl);	// All processors must define pCtrl
    pCtrl->loop=stop_cmd;
    sem_area = CAM_BLOB_AREA;	// All processors mus define sem_area

    //private attributes initialization
    L_cap = new VideoCapture(0);  //Video grabber
    L_Img = new Mat();  //Matrix containing the grabbed image
    L_HSLImg = new Mat();  //Matrix containing the grabbed image in HSL representation
    L_GrayImg = new Mat();  //Matrix containing the grabbed image Grayscaled

    /*************** \v/ DESCOMENTAR PARA USO CON DOS CAMARAS!!!!!!!!!!***************************/
    R_cap = L_cap;//new VideoCapture(0);  //Video grabber
    R_Img = new Mat();  //Matrix containing the grabbed image
    R_HSLImg = new Mat();  //Matrix containing the grabbed image in HSL representation
    R_GrayImg = new Mat();  //Matrix containing the grabbed image Grayscaled


    kernel = new Mat(3,3,CV_8UC1); //Kernel for morphologic operators.
    kernel->data[1]=kernel->data[3]=kernel->data[4]=kernel->data[5]=kernel->data[7]=255;
    kernel->data[0]=kernel->data[2]=kernel->data[6]=kernel->data[8]=0;

    //threshold for the HSL binary thresholding on L chanel
    binaryThreshold = LAMBDA;
    //blob extracting edge
    edge = BLOB_EDGE;

    //Secure-write the maxblob parameter.
    cda.lockArea(CAM_BLOB_AREA);
        pCameraBlobber->maxblobs=10;
    cda.unlockArea(CAM_BLOB_AREA);

    //stabilizes camera image before starting the capture process
    L_cap->operator >>(*L_Img);
    R_cap->operator >>(*R_Img);
    L_cap->operator >>(*L_Img);
    R_cap->operator >>(*R_Img);
    L_cap->operator >>(*L_Img);
    R_cap->operator >>(*R_Img);
    L_cap->operator >>(*L_Img);
    R_cap->operator >>(*R_Img);
    L_cap->operator >>(*L_Img);
    R_cap->operator >>(*R_Img);

    //getting sure that the matrixes aren't empty.
    L_grayScaled = new Mat();
    L_scaled = new Mat(WIDTH,HEIGHT,L_Img->type());

    L_outputI = cvCreateImage(cvSize(L_Img->cols,L_Img->rows),IPL_DEPTH_8U,3);

    R_grayScaled = new Mat();
    R_scaled = new Mat(WIDTH,HEIGHT,R_Img->type());

    R_outputI = cvCreateImage(cvSize(R_Img->cols,R_Img->rows),IPL_DEPTH_8U,3);
    }


/** Executes one step of caturing and segmentation, writes the resulting data into the CDA
  */
int CameraBlobber::step(){


    printf("CameraBlobber: step...  \n");
    //---------------  VARIABLE INITIALIZATION  -----------------------------//
    static int nImg=0; //incremental counter for naming images.
    int maxblobs = pCameraBlobber->maxblobs; //defines the maximum number of blobs that will be considered to write to CDA
    int writeOutput = 0;
    int videoPixels,videoWidth,videoHeight;


    //-------------- FRAME CAPTURE -----------------------------------------//

    //captures a new frame from the buffer.
    L_cap->operator >>(*L_Img);
    R_cap->operator >>(*R_Img);

    //writes what it sees to a file
    if(writeOutput){
        imwrite("L_out.jpg",*L_Img);
        imwrite("R_out.jpg",*R_Img);
    }

    //fast access atributes.
    videoPixels = L_Img->rows * L_Img->cols;
    videoWidth = L_Img->cols;
    videoHeight = L_Img->rows;


    //-------------- COLOR SPACE CONVERSION AND THRESHOLDING ---------------//
    int from_to[] = {2,0,2,1,2,2};

    //Left
    cv::cvtColor(*L_Img,*L_HSLImg,CV_RGB2HLS); //RGB -> HSL for better thresholding

    mixChannels(L_HSLImg,1,L_HSLImg,1,from_to,3);
    cv::cvtColor(*L_Img,*L_GrayImg,CV_RGB2GRAY);
    threshold(*L_GrayImg,*L_GrayImg,binaryThreshold,255,THRESH_BINARY);

    //Right
    cv::cvtColor(*R_Img,*R_HSLImg,CV_RGB2HLS); //RGB -> HSL for better thresholding

    mixChannels(R_HSLImg,1,R_HSLImg,1,from_to,3);
    cv::cvtColor(*R_Img,*R_GrayImg,CV_RGB2GRAY);
    threshold(*R_GrayImg,*R_GrayImg,binaryThreshold,255,THRESH_BINARY);

    //-------------- CLEANING SPUREOUS PIXELS -------------------------------//

    static Point p;
    p.x=-1;
    p.y=-1;

    //Left
    erode(*L_GrayImg,*L_GrayImg,*kernel,p,4);
    dilate(*L_GrayImg,*L_GrayImg,*kernel,p,3);

    //Right
    erode(*R_GrayImg,*R_GrayImg,*kernel,p,4);
    dilate(*R_GrayImg,*R_GrayImg,*kernel,p,3);

    //writes what it sees to a file
    if(writeOutput){
        imwrite("L_outGray.jpg",*L_GrayImg);
        imwrite("R_outGray.jpg",*R_GrayImg);
    }

    //-------------- BLOBBING PROCESS -------------------------------------//

    // object that will contain blobs of inputImage
    CBlobResult L_blobs,R_blobs;

    //temporary images used for compatibility whith cvblobslib
    IplImage L_inputI,*L_inputImage;
    L_inputI = *L_GrayImg;
    L_inputImage = &L_inputI;

    IplImage R_inputI,*R_inputImage;
    R_inputI = *R_GrayImg;
    R_inputImage = &R_inputI;
    // Extract the blobs using a determined threshold in the image
    L_blobs = CBlobResult(L_inputImage , NULL, 128, true );
    R_blobs = CBlobResult(R_inputImage , NULL, 128, true );

    // discard the blobs with less area than 5000 pixels
    // ( the criteria to filter can be any class derived from COperadorBlob )
    L_blobs.Filter( L_blobs, B_INCLUDE, CBlobGetArea(), B_GREATER, 5000);
    R_blobs.Filter( R_blobs, B_INCLUDE, CBlobGetArea(), B_GREATER, 5000);


    //Fills the image with a uniform color
    for(int i=L_outputI->width*L_outputI->height*3;i>=0;i--){
        L_outputI->imageData[i]=0;
        R_outputI->imageData[i]=0;
    }

    //temporal variables for blob operating.
    int L_nBlobs = L_blobs.GetNumBlobs();
    int R_nBlobs = R_blobs.GetNumBlobs();
    int xmin,xmax,ymin,ymax,rangox,rangoy;
    CBlob *tempBlob;
    Mat * img2;
    img2=NULL;


    //Locks the CamBlob area to write on it
    cda.lockArea(CAM_BLOB_AREA);

    //for every blob detected up to maxblobs
    for(int i=0;i<(L_nBlobs-1) && i<maxblobs;i++){

        //gets the 'i' blob
        tempBlob = L_blobs.GetBlob(i+1);
        //fills the blob area in the output image
        tempBlob->FillBlob(L_outputI,CV_RGB(255,20*i,255-20*i));


        /* sets the Region of Interest */

        xmin = ((tempBlob->minx>edge)?tempBlob->minx-edge:0);
        ymin = ((tempBlob->miny>edge)?tempBlob->miny-edge:0);
        xmax = ((tempBlob->maxx<(videoWidth-edge))?tempBlob->maxx+edge:videoWidth);
        ymax = ((tempBlob->maxy<(videoHeight-edge))?tempBlob->maxy+edge:videoHeight);

        rangox = xmax - xmin;
        rangoy = ymax - ymin;
        Rect ROI(xmin,ymin ,rangox ,rangoy );
        img2 = new Mat(*L_Img,ROI);

        Size s(WIDTH,HEIGHT);
        resize(*img2,*L_scaled,s);
        cvtColor(*L_scaled,*L_grayScaled,CV_RGB2GRAY);


        //cvtColor(*img2,*img2,CV_RGB2HSV);

        //Writes the blob images to files.
        if(writeOutput){
            char name[100];
            sprintf(name,"L_out%d.jpg",++nImg);
            cv::imwrite(name,*img2);

            sprintf(name,"L_outsc%d.jpg",nImg);
            cv::imwrite(name,*L_scaled);
        }

        //blob info
        pCameraBlobber->L_blobInfo[4*i]=tempBlob->minx;
        pCameraBlobber->L_blobInfo[4*i+1]=tempBlob->miny;
        pCameraBlobber->L_blobInfo[4*i+2]=tempBlob->maxx;
        pCameraBlobber->L_blobInfo[4*i+3]=tempBlob->maxy;

        //copies the scaled ROI into the blob list
        for(int j=0;j<(960*3);j++){
            pCameraBlobber->L_scaledBlobs[(i*960*3)+j]=L_scaled->data[j+2-2*(j%3)];
        }

        //sets the number of blobs to i
        pCameraBlobber->L_nBlobs=i+1;
    }

    //Now the right eye
    for(int i=0;i<(R_nBlobs-1) && i<maxblobs;i++){

        //gets the 'i' blob
        tempBlob = R_blobs.GetBlob(i+1);
        //fills the blob area in the output image
        tempBlob->FillBlob(R_outputI,CV_RGB(255,20*i,255-20*i));


        /* sets the Region of Interest */

        xmin = ((tempBlob->minx>edge)?tempBlob->minx-edge:0);
        ymin = ((tempBlob->miny>edge)?tempBlob->miny-edge:0);
        xmax = ((tempBlob->maxx<(videoWidth-edge))?tempBlob->maxx+edge:videoWidth);
        ymax = ((tempBlob->maxy<(videoHeight-edge))?tempBlob->maxy+edge:videoHeight);

        rangox = xmax - xmin;
        rangoy = ymax - ymin;
        Rect ROI(xmin,ymin ,rangox ,rangoy );
        img2 = new Mat(*R_Img,ROI);

        Size s(WIDTH,HEIGHT);
        resize(*img2,*R_scaled,s);
        cvtColor(*R_scaled,*R_grayScaled,CV_RGB2GRAY);


        //cvtColor(*img2,*img2,CV_RGB2HSV);

        //Writes the blob images to files.
        if(writeOutput){
            char name[100];
            sprintf(name,"R_out%d.jpg",++nImg);
            cv::imwrite(name,*img2);

            sprintf(name,"R_outsc%d.jpg",nImg);
            cv::imwrite(name,*R_scaled);
        }

        //blob info
        pCameraBlobber->R_blobInfo[4*i]=tempBlob->minx;
        pCameraBlobber->R_blobInfo[4*i+1]=tempBlob->miny;
        pCameraBlobber->R_blobInfo[4*i+2]=tempBlob->maxx;
        pCameraBlobber->R_blobInfo[4*i+3]=tempBlob->maxy;

        //copies the scaled ROI into the blob list
        for(int j=0;j<(960*3);j++){
            pCameraBlobber->R_scaledBlobs[(i*960*3)+j]=R_scaled->data[j+2-2*(j%3)];
        }

        //sets the number of blobs to i
        pCameraBlobber->R_nBlobs=i+1;
    }


    for(int i=0;i<videoPixels*3;i++){
        pCameraBlobber->L_originalImg[i]=L_Img->data[i+2-2*(i%3)];
        pCameraBlobber->R_originalImg[i]=R_Img->data[i+2-2*(i%3)];
    }

    for(int i=0;i<videoPixels*3;i++){
        pCameraBlobber->L_blobbedImg[i]=L_outputI->imageData[i+2-2*(i%3)];
        pCameraBlobber->R_blobbedImg[i]=R_outputI->imageData[i+2-2*(i%3)];
    }

    pCameraBlobber->newInfo=1;

    //release the camerablobber CDA
    cda.unlockArea(CAM_BLOB_AREA);

    if(writeOutput){
        imwrite("L_blobs.jpg",L_outputI);
        imwrite("R_blobs.jpg",R_outputI);
    }

    if(img2!=NULL){
        free(img2);
    }

}
