#include "messagebox_thread.h"

void messagethread::Onsendmessage(int message_type)
{
    msgbox = new QMessageBox();
    QTimer *timer;
    static QProcess *message_proc = new QProcess(this);


    switch(message_type){

        case 1:

            //Paying Message
        printf("123123123123");
        msgbox->setText("Sorry , Pay failed...");
        msgbox->show();
        QTimer::singleShot(5000, msgbox, SLOT(close()));
            break;

        case 2:
        msgbox->setText("Sorry , Pay dddd..");
        msgbox->show();
            //Pay failed Message
            break;


    }

}

