#ifndef BLOBINFOGUI_H
#define BLOBINFOGUI_H

#include <QWidget>

namespace Ui {
    class BlobInfoGui;
}

class BlobInfoGui : public QWidget
{
    Q_OBJECT

public:
    explicit BlobInfoGui(QWidget *parent = 0);
    ~BlobInfoGui();
    void setPixmap ( const QPixmap & );
    void setX(int x);
    void setY(int y);
    void setW(int w);
    void setH(int h);
    void setAll(const QPixmap & , int x, int y, int w, int h);
    void setLabels(int x,int y, int w, int h);
    void reset();
private:
    Ui::BlobInfoGui *ui;
};

#endif // BLOBINFOGUI_H
