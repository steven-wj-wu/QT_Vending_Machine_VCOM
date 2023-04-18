#ifndef MESSAGEBOX_THREAD_H
#define MESSAGEBOX_THREAD_H
#include <QObject>
#include <QThread>
#include <QProcess>
#include <QTime>
#include <QTimer>
#include "qmessagebox.h"

class messagethread : public QThread
{
    Q_OBJECT
public:
     messagethread(QObject *parent=0) : QThread(parent){}
     QMessageBox *msgbox;

public slots:
    void Onsendmessage(int);
};




#endif // MESSAGEBOX_THREAD_H
