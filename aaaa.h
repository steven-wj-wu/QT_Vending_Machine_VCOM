#ifndef AAAA_H
#define AAAA_H


#include<QThread>
#include <QRegion>
#include <QPoint>
#include "touch_function/getevent.h"


#define     COLA        0
#define     SODA        1
#define     COFFEE      2
#define     ORANGE      3
#define     TEA         4
#define     WATER       5


class TouchThread : public QThread
{
    Q_OBJECT

public :
    TouchThread(QObject *parent=0) : QThread(parent){}
    int touchLock;
    int drinkKind;
    int touchStep;
protected :
    void run();
private :
    int drinksChoose(QPoint tp);
    int cancelback(QPoint tp);
signals :
    void portOpen(int);
    void getTouch(int);
};

#endif // AAAA_H
