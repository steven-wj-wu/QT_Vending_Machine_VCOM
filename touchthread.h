#ifndef TOUCHTHREAD_H
#define TOUCHTHREAD_H

#include <QThread>
#include <QRegion>
#include <QPoint>

#include <QDebug>
#include <QMutex>
#define     COLA       100
#define     SODA       101
#define     COFFEE     102
#define     TEA     104
#define     WATER     105
#define     ORANGE     103

#define     CHICKEN       0
#define     FRIES       1
#define     BURGER       2
#define     ONION       3
#define     NUGGETS       4




class touchthread : public QThread
{
    Q_OBJECT

public:
    touchthread(QObject *parent=0) : QThread(parent){}
    QMutex mutex;
    int touchLock;
    int drinkKind;
    int touchStep;
    int videoLock;
    int current_mode;
    bool drive_scene;
    bool SEP_TP_thread=false;
    bool break_btouch=false;
    bool end_sep = false;

protected :
    void run();

private :
    int foodChoose(QPoint tp);
    int drinksChoose(QPoint tp);
    int cancelback(QPoint tp);
    double get_angle(double x1,double y1,double x2,double y2,double x3,double y3);
    bool in_circle(double x1,double y1);

signals :
    void portOpen(int);
    void getTouch(int,int);
    void volume();

};

#endif // TOUCHTHREAD_H
