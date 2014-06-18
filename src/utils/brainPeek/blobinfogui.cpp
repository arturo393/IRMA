#include "blobinfogui.h"
#include "ui_blobinfogui.h"

BlobInfoGui::BlobInfoGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlobInfoGui)
{
    ui->setupUi(this);
}

BlobInfoGui::~BlobInfoGui()
{
    delete ui;
}

void BlobInfoGui::setPixmap ( const QPixmap & pix){
    ui->image->setPixmap(pix);
}

void BlobInfoGui::setX(int x){
    ui->xLabel->setText(QString().setNum(x));
}

void BlobInfoGui::setY(int y){
    ui->yLabel->setText(QString().setNum(y));
}

void BlobInfoGui::setW(int w){
    ui->wLabel->setText(QString().setNum(w));

}
void BlobInfoGui::setH(int h){
    ui->hLabel->setText(QString().setNum(h));

}

void BlobInfoGui::setAll(const QPixmap & pix, int x, int y, int w, int h){
    this->setPixmap(pix);
    this->setX(x);
    this->setY(y);
    this->setW(w);
    this->setH(h);
}

void BlobInfoGui::setLabels(int x,int y, int w, int h){
    this->setX(x);
    this->setY(y);
    this->setW(w);
    this->setH(h);
}

void BlobInfoGui::reset(){
    ui->image->clear();
    ui->xLabel->setText("---");
    ui->yLabel->setText("---");
    ui->wLabel->setText("---");
    ui->hLabel->setText("---");
}
