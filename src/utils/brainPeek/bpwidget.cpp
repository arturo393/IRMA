#include "bpwidget.h"
#include "ui_bpwidget.h"
#include "../../common/CDA/commonDataArea.h"
#include "../../processors/base/processor.h"


bpWIdget::bpWIdget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::bpWIdget)
{

    CDA cda;
    CamBlobData * pCamBlob;
    ui->setupUi(this);

    pCamBlob = (CamBlobData*) cda.getMemPtr(CAM_BLOB_AREA);
    QImage * image;

    //Left
    image = new QImage(QSize(640,480),QImage::Format_RGB888);
    for(int i=0;i<640*480*3;i++){
        image->bits()[i]=pCamBlob->L_originalImg[i];
    }
    ui->L_Image->setPixmap(QPixmap::fromImage(*image).scaled(ui->L_Image->geometry().width(),ui->L_Image->geometry().height()));

    for(int i=0;i<640*480*3;i++){
        image->bits()[i]=pCamBlob->L_blobbedImg[i];
    }
    ui->L_blobbed->setPixmap(QPixmap::fromImage(*image).scaled(ui->L_blobbed->geometry().width(),ui->L_blobbed->geometry().height()));

    free(image);

    //Right
    image = new QImage(QSize(640,480),QImage::Format_RGB888);
    for(int i=0;i<640*480*3;i++){
        image->bits()[i]=pCamBlob->R_originalImg[i];
    }
    ui->R_Image->setPixmap(QPixmap::fromImage(*image).scaled(ui->R_Image->geometry().width(),ui->R_Image->geometry().height()));

    for(int i=0;i<640*480*3;i++){
        image->bits()[i]=pCamBlob->R_blobbedImg[i];
    }
    ui->R_blobbed->setPixmap(QPixmap::fromImage(*image).scaled(ui->R_blobbed->geometry().width(),ui->R_blobbed->geometry().height()));

    free(image);
    image = new QImage(QSize(32,30),QImage::Format_RGB888);

    for(int j=0;j<pCamBlob->L_nBlobs;j++){

        for(int i=0;i<(960*3);i++){
            image->bits()[i]=pCamBlob->L_scaledBlobs[(960*3*j)+i];
        }

        switch(j){
            case 0: ui->L_blob_01->setPixmap(QPixmap::fromImage(*image)); break;
            case 1: ui->L_blob_02->setPixmap(QPixmap::fromImage(*image)); break;
            case 2: ui->L_blob_03->setPixmap(QPixmap::fromImage(*image)); break;
            case 3: ui->L_blob_04->setPixmap(QPixmap::fromImage(*image)); break;
            case 4: ui->L_blob_05->setPixmap(QPixmap::fromImage(*image)); break;
            case 5: ui->L_blob_06->setPixmap(QPixmap::fromImage(*image)); break;
            case 6: ui->L_blob_07->setPixmap(QPixmap::fromImage(*image)); break;
            case 7: ui->L_blob_08->setPixmap(QPixmap::fromImage(*image)); break;
            case 8: ui->L_blob_09->setPixmap(QPixmap::fromImage(*image)); break;
            case 9: ui->L_blob_10->setPixmap(QPixmap::fromImage(*image)); break;
            default: break;
        }
    }

    for(int j=0;j<pCamBlob->R_nBlobs;j++){

        for(int i=0;i<(960*3);i++){
            image->bits()[i]=pCamBlob->R_scaledBlobs[(960*3*j)+i];
        }

        switch(j){
            case 0: ui->R_blob_01->setPixmap(QPixmap::fromImage(*image)); break;
            case 1: ui->R_blob_02->setPixmap(QPixmap::fromImage(*image)); break;
            case 2: ui->R_blob_03->setPixmap(QPixmap::fromImage(*image)); break;
            case 3: ui->R_blob_04->setPixmap(QPixmap::fromImage(*image)); break;
            case 4: ui->R_blob_05->setPixmap(QPixmap::fromImage(*image)); break;
            case 5: ui->R_blob_06->setPixmap(QPixmap::fromImage(*image)); break;
            case 6: ui->R_blob_07->setPixmap(QPixmap::fromImage(*image)); break;
            case 7: ui->R_blob_08->setPixmap(QPixmap::fromImage(*image)); break;
            case 8: ui->R_blob_09->setPixmap(QPixmap::fromImage(*image)); break;
            case 9: ui->R_blob_10->setPixmap(QPixmap::fromImage(*image)); break;
            default: break;
        }
    }
    free(image);
    destroy(pCamBlob);

}

bpWIdget::~bpWIdget()
{
    delete ui;
}

void bpWIdget::correCaptura(){
    CDA cda;
    CamBlobData * pCamBlob;
    ObjCognitiveData * pCogMem;

    pCamBlob = (CamBlobData*) cda.getMemPtr(CAM_BLOB_AREA);
    pCogMem = (ObjCognitiveData*) cda.getMemPtr(OBJ_COGNITIVE_AREA);

    pCamBlob->ctrl.loop = run_cmd; //step;
    pCogMem->ctrl.loop = run_cmd; //step;

    cmd = 0;
    timerid = startTimer(200);
}

void bpWIdget::timerEvent(QTimerEvent* event){

    if(cmd==1)killTimer(timerid);
    refrescar();

}

void bpWIdget::pasoCaptura(){
    killTimer(timerid);

    CDA cda;
    CamBlobData * pCamBlob;
    ObjCognitiveData * pCogMem;

    pCamBlob = (CamBlobData*) cda.getMemPtr(CAM_BLOB_AREA);
    pCogMem = (ObjCognitiveData*) cda.getMemPtr(OBJ_COGNITIVE_AREA);

    pCamBlob->ctrl.loop = step_cmd; //step;
    pCogMem->ctrl.loop = step_cmd; //step;

    cmd = 1;
    timerid = startTimer(200);

}

void bpWIdget::refrescar(){

    CDA cda;
    CamBlobData * pCamBlob;
    ObjCognitiveData * pCogMem;

    pCamBlob = (CamBlobData*) cda.getMemPtr(CAM_BLOB_AREA);
    pCogMem = (ObjCognitiveData*) cda.getMemPtr(OBJ_COGNITIVE_AREA);

    QImage image(QSize(640,480),QImage::Format_RGB888);
    QPixmap original;
    QPixmap blobs;

    //LEFT IMAGE
    for(int i=0;i<640*480*3;i++){
        image.bits()[i]=(pCamBlob->L_originalImg[i] + pCamBlob->L_blobbedImg[i])/2;
    }

    original = QPixmap::fromImage(image.scaled(ui->L_Image->geometry().width(),ui->L_Image->geometry().height()));
    ui->L_Image->setPixmap(original);

    //RIGHT IMAGE
    for(int i=0;i<640*480*3;i++){
        image.bits()[i]=(pCamBlob->R_originalImg[i] + pCamBlob->R_blobbedImg[i])/2;
    }

    original = QPixmap::fromImage(image.scaled(ui->R_Image->geometry().width(),ui->R_Image->geometry().height()));
    ui->R_Image->setPixmap(original);

    //LEFT BLOBBED IMAGE
    for(int i=0;i<640*480*3;i++){
        //image.bits()[i]=pCamBlob->L_blobbedImg[i];
        image.bits()[i]=pCogMem->L_cognitiveImg[i];
    }

    blobs = QPixmap::fromImage(image);
    ui->L_blobbed->setPixmap(blobs.scaled(ui->L_blobbed->geometry().width(),ui->L_blobbed->geometry().height()));

    //RIGHT BLOBBED IMAGE
    for(int i=0;i<640*480*3;i++){

       // image.bits()[i]=pCamBlob->R_blobbedImg[i];
        image.bits()[i]=pCogMem->R_cognitiveImg[i];
    }

    blobs = QPixmap::fromImage(image);
    ui->R_blobbed->setPixmap(blobs.scaled(ui->R_blobbed->geometry().width(),ui->R_blobbed->geometry().height()));


    QImage blob(QSize(32,30),QImage::Format_RGB888);

    int bx,by,bw,bh;
    //Left Blobs
    for(int j=0;j<pCamBlob->L_nBlobs;j++){

        for(int i=0;i<(960*3);i++){
            blob.bits()[i]=pCamBlob->L_scaledBlobs[(960*3*j)+i];
        }
        bx = pCamBlob->L_blobInfo[4*j];
        by = pCamBlob->L_blobInfo[4*j+1];
        bw = pCamBlob->L_blobInfo[4*j+2]-bx;
        bh = pCamBlob->L_blobInfo[4*j+3]-by;

        QPixmap tmp;
        tmp = QPixmap::fromImage(blob);
        switch(j){
            case 0: ui->L_blob_01->setAll(tmp,bx,by,bw,bh); break;
            case 1: ui->L_blob_02->setAll(tmp,bx,by,bw,bh); break;
            case 2: ui->L_blob_03->setAll(tmp,bx,by,bw,bh); break;
            case 3: ui->L_blob_04->setAll(tmp,bx,by,bw,bh); break;
            case 4: ui->L_blob_05->setAll(tmp,bx,by,bw,bh); break;
            case 5: ui->L_blob_06->setAll(tmp,bx,by,bw,bh); break;
            case 6: ui->L_blob_07->setAll(tmp,bx,by,bw,bh); break;
            case 7: ui->L_blob_08->setAll(tmp,bx,by,bw,bh); break;
            case 8: ui->L_blob_09->setAll(tmp,bx,by,bw,bh); break;
            case 9: ui->L_blob_10->setAll(tmp,bx,by,bw,bh); break;
            default: break;
        }
    }

    //Right Blobs
    for(int j=0;j<pCamBlob->R_nBlobs;j++){

        for(int i=0;i<(960*3);i++){
            blob.bits()[i]=pCamBlob->R_scaledBlobs[(960*3*j)+i];
        }

        bx = pCamBlob->L_blobInfo[4*j];
        by = pCamBlob->L_blobInfo[4*j+1];
        bw = pCamBlob->L_blobInfo[4*j+2]-bx;
        bh = pCamBlob->L_blobInfo[4*j+3]-by;

        QPixmap tmp;
        tmp = QPixmap::fromImage(blob);
        switch(j){
            case 0: ui->R_blob_01->setAll(tmp,bx,by,bw,bh); break;
            case 1: ui->R_blob_02->setAll(tmp,bx,by,bw,bh); break;
            case 2: ui->R_blob_03->setAll(tmp,bx,by,bw,bh); break;
            case 3: ui->R_blob_04->setAll(tmp,bx,by,bw,bh); break;
            case 4: ui->R_blob_05->setAll(tmp,bx,by,bw,bh); break;
            case 5: ui->R_blob_06->setAll(tmp,bx,by,bw,bh); break;
            case 6: ui->R_blob_07->setAll(tmp,bx,by,bw,bh); break;
            case 7: ui->R_blob_08->setAll(tmp,bx,by,bw,bh); break;
            case 8: ui->R_blob_09->setAll(tmp,bx,by,bw,bh); break;
            case 9: ui->R_blob_10->setAll(tmp,bx,by,bw,bh); break;
            default: break;
        }
    }
}

void bpWIdget::halt_all(){

    CDA cda;
    CamBlobData * pCamBlob;
    ObjCognitiveData * pCogMem;

    pCamBlob = (CamBlobData*) cda.getMemPtr(CAM_BLOB_AREA);
    pCogMem = (ObjCognitiveData*) cda.getMemPtr(OBJ_COGNITIVE_AREA);

    pCamBlob->ctrl.loop = halt_cmd; //step;
    pCogMem->ctrl.loop = halt_cmd; //step;
}

void bpWIdget::updateTresholds(){

    ui->sliderAlpha->value();
}
