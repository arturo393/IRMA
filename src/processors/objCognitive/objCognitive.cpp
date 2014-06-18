#include "objCognitive.h"
#include <QtGui/QApplication>
#include "memorythread.h"
#include "hitreg.h"

using namespace cv;
int main(int argc, char* argv[]){
    QCoreApplication a(argc, argv);

    ObjCognitiveProcessor m;
    //m.init();
    m.execute();
    //m.step();
    return 0;
}


ObjCognitiveProcessor::ObjCognitiveProcessor() : ProcessorInterface()
{
    ;
}

int ObjCognitiveProcessor::init(){
    printf("ObjCognitiveProc: Init start\n");


    pCameraBlobber  = (CamBlobData *)cda.getMemPtr(CAM_BLOB_AREA);
    pObjCogProc  = (ObjCognitiveData *)cda.getMemPtr(OBJ_COGNITIVE_AREA);

    pCtrl = &(pObjCogProc->ctrl);	// Todos los procesadores tienen que definir pCtrl
    sem_area = OBJ_COGNITIVE_AREA;	// Todos los procesadores tienen que definir sem_area

    results = new QList<HitReg>();
    bool exists;
    nThreads=1;
    alpha=ALPHA;
    beta=BETA;
    gamma=GAMMA;

    char filename[500];
    FILE * fd;
    actReg = new MemRegister();

    do{
        sprintf(filename,"../../common/CogMem/Memory/segmento%d.mem",nThreads);
        fd = fopen(filename,"rb+");
        if(fd > 0){
            fclose(fd);
            hit << false;
            errors << 0;
            MemSegment * tempSeg = new MemSegment(filename);
            segments << tempSeg;
            MemoryThread * temp = new MemoryThread(nThreads,&mutex,&beta,&hit[nThreads-1],actReg,segments[nThreads-1],results, &errors[nThreads-1]);
            memThreads << temp;
            nThreads++;
            exists=true;
        }else{
            exists=false;
        }
    }while(exists);

    //if there are no files with data, we create a blank segment.
    if(nThreads==1){
        hit << false;
        errors << 0;
        MemSegment * tempSeg = new MemSegment();
        segments << tempSeg;
        MemoryThread * temp = new MemoryThread(nThreads,&mutex,&beta,&hit[nThreads-1],actReg,segments[nThreads-1],results, &errors[nThreads-1]);
        memThreads << temp;
        nThreads++;
    }

    printf("Alpha = %d | Beta = %d | Gamma = %d \n",alpha,beta,gamma);
    printf("ObjCognitiveProc: Init done\n");
    return 0;
}

int ObjCognitiveProcessor::cleanup(){


    char filename[500];
    //execute saving procedure
    for(int i=0;i<segments.size();i++){

        sprintf(filename,"../../common/CogMem/Memory/segmento%d.mem",i+1);
        segments.at(i)->Save(filename);
    }

    return 0;
}

int ObjCognitiveProcessor::step(){

    int nBlobsL,nBlobsR;
    MemRegister blobs[10];
    //int fWIDTH = 640;
    //int fHEIGT = 480;
    //int originalImg[fWIDTH*fHEIGT*3];

    //Critic Zone
    cda.lockArea(CAM_BLOB_AREA);

    nBlobsL = pCameraBlobber->L_nBlobs;
    nBlobsR = pCameraBlobber->L_nBlobs;

    if(pCameraBlobber->newInfo==1 && nBlobsL>0){

        printf("%d Blobs\n",nBlobsL);
        fflush(stdout);
        pCameraBlobber->newInfo=0;
        for(int i=0;i<nBlobsL;i++){
            for(int j=0;j<IMAGESIZE;j++){

                blobs[i].imagen[j]=(pCameraBlobber->L_scaledBlobs[((i*IMAGESIZE)+j)*3] +
                                                                  pCameraBlobber->L_scaledBlobs[1+((i*IMAGESIZE)+j)*3] +
                                                                  pCameraBlobber->L_scaledBlobs[2+((i*IMAGESIZE)+j)*3])/3;;
            }
        }
    }else{
        printf("no Blobs\n");
        fflush(stdout);
        cda.unlockArea(CAM_BLOB_AREA);
        return 0;
    }

    cda.unlockArea(CAM_BLOB_AREA);
    //Out of critic zone

    //Critic Zone
    cda.lockArea(OBJ_COGNITIVE_AREA);
    for(int cl=0;cl<(FWIDTH*FHEIGHT*3);cl++){
        pObjCogProc->L_cognitiveImg[cl] = pCameraBlobber->L_originalImg[cl];
    }
    cda.unlockArea(OBJ_COGNITIVE_AREA);
    //Out of critic zone

    //for each blob present in the image.

    for(int b=0;b<nBlobsL;b++){
        results->clear();
        printf("blob %d\t",b);
        fflush(stdout);
        //set the actual blob for the threads
        for(int j=0;j<IMAGESIZE;j++){
            actReg->imagen[j] = blobs[b].imagen[j];
        }

        //for each thread in the list.
        for(int j=0;j<nThreads-1;j++){
            //execute start method.
            memThreads[j]->start();
        }

        //wait until all threads are done.
        for(int j=0;j<nThreads-1;j++){
            memThreads[j]->wait();
        }
        //if there are coincidences.
        if(results->size()>0){
            int size=results->size();

            //sort them by error.
            for(int r=1;r<size;r++){
                for(int q=0;q<(size-r);q++){
                    if(results->at(q).error>results->at(q+1).error){
                        results->swap(q,q+1);
                    }
                }
            }

            //Critic Zone
            cda.lockArea(OBJ_COGNITIVE_AREA);

            //copy the 'n' better results to the CDA
            int xBlob = pCameraBlobber->L_blobInfo[4*b];
            int yBlob = pCameraBlobber->L_blobInfo[4*b+1];


            //for(int r=0;r<RESULTS && r<results->size();r++){
            int r=0;
            int sFact = 2;
            for(int y=0; y<HEIGHT*sFact;y++){
                    for(int x=0;x<WIDTH*sFact;x++){
                        pObjCogProc->L_cognitiveImg[3*((yBlob+y)*FWIDTH + (xBlob+x) +r*WIDTH)] = results->at(r).reg->imagen[((y/sFact)*WIDTH)+(x/sFact)];
                        pObjCogProc->L_cognitiveImg[3*((yBlob+y)*FWIDTH + (xBlob+x) +r*WIDTH)+1] = results->at(r).reg->imagen[((y/sFact)*WIDTH)+(x/sFact)];
                        pObjCogProc->L_cognitiveImg[3*((yBlob+y)*FWIDTH + (xBlob+x) +r*WIDTH)+2] = results->at(r).reg->imagen[((y/sFact)*WIDTH)+(x/sFact)];
                    }
                }
            //}

            cda.unlockArea(OBJ_COGNITIVE_AREA);
            //Out of critic zone
        }

        //find min error
        double minError=errors[0];
        double minErrorNotFull=IMAGESIZE*2; //Maximum error
        double bestThread=0;
        double bestThreadNotFull=nThreads;
        for(int r=0;r<nThreads-1;r++){

            if(minErrorNotFull>errors[r] && segments[r]->nRegistros<NREG){
                minErrorNotFull=errors[r];
                bestThreadNotFull=r;
            }

            if(minError>errors[r]){
                minError=errors[r];
                bestThread=r;
            }
        }

        printf("min error: %f\t",minError);

        //if the best error (minError) qualifies to be added to a segment, we do so.
        if(evalAdding(minError)){
            printf("ADDING REGISTER\n");
            fflush(stdout);
            if(bestThreadNotFull!= nThreads){ //if the segment is not full
                segments[bestThreadNotFull]->Add(blobs[b]) ;
            }else{ //create a new segment
                printf("CREATING A NEW SEGMENT!\n");
                fflush(stdout);
                hit << false;
                errors << 0;
                MemSegment * tempSeg = new MemSegment();
                segments << tempSeg;
                MemoryThread * temp = new MemoryThread(nThreads,&mutex,&beta,&hit[nThreads-1],actReg,segments[nThreads-1],results, &errors[nThreads-1]);
                memThreads << temp;
                segments[bestThread]->Add(blobs[b]);
                nThreads++;
                printf("segments: %d\n",nThreads-1);
            }
        }else{
            printf("NOT ADDING REGISTER\n");
            fflush(stdout);
        }


    }
    return 0;
}

bool ObjCognitiveProcessor::evalAdding(double error){
    if(error<alpha){
        return false;
    }else if(error<beta){
        return false;
    }else if(error<gamma){
        return true;
    }
    return false;
}
