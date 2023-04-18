#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QObject>
#include <QThread>
#include <QProcess>
#include <QDebug>

class videothread : public QThread
{
    Q_OBJECT

public:
     videothread(QObject *parent=0) : QThread(parent){}
     int playFinished;
     bool initial=true;



public slots:
    void onVideoChange(int);
};

#endif // VIDEOTHREAD_H
