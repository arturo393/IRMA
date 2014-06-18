#ifndef BPWIDGET_H
#define BPWIDGET_H

#include <QWidget>
#include "blobinfogui.h"

namespace Ui {
    class bpWIdget;
}

class bpWIdget : public QWidget
{
    Q_OBJECT

public:
    explicit bpWIdget(QWidget *parent = 0);
    ~bpWIdget();
    void timerEvent(QTimerEvent*);

private:
    Ui::bpWIdget *ui;
    void refrescar();
    int timerid;
    int cmd;

public slots:
    void pasoCaptura();
    void correCaptura();
    void halt_all();
    void updateTresholds();
};

#endif // BPWIDGET_H
