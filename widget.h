#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>
#include <QSignalMapper>
#include <unistd.h>
#include <sys/time.h>
#include "touchthread.h"
#include "videothread.h"
#include "qmessagebox.h"
#include <QSerialPort>
#include <QSerialPortInfo>

#include <QNetworkReply>
#include <QDateTime>
#include <QTime>
#include <QTimer>
#include <QPalette>
#include <QEventLoop>

#include <QObject>
#include <QThread>

#include <wiringPi.h>
#include <cstdio>
#include <time.h>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

#define CS 4
#define SCL 5
#define Data 6

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void onResult(QNetworkReply *reply);

signals:
    void VideoChange(int);
    void ImageChange(int);
    void GetPay(int,int);
    void sendmessage(int);
    void drinkstep(int);

private:
    Ui::Widget *ui;
    const int winHeight=720;
    const int winWidth=1280;
    touchthread *tThread;
    videothread *vThread;

    QSignalMapper *mapper_btw;
    QSignalMapper *drink_step_map;
    QSignalMapper *food_step_map;
    int on_choose_btn = 1;
    QTimer *tempTimer; //one shot step timer for drink
    QTimer *tempTimer_food; //one shot step timer for drink
    QTimer *drink_switch_Timer; //one shot switch drink all timer
    QTimer *food_switch_Timer; //one shot switch food all timer
    QTimer *driver_Timer; //one shot step timer for drink

    QNetworkAccessManager *nam ;
    QNetworkAccessManager *AQI_manager;
    // Serialport
    QSerialPort *serialPort;
    QString serial_port_name;
    QMessageBox *msgbox;
    QString last_5190;
    QLabel *label ;

    bool serialport_is_available=false;
    bool time_ani=true;
    bool get_uid = false;
    static const qint16 pn5190_vendorID = 8137;
    static const int pn5190_productID = 148;
    void serial_port_connect();
    void WriteCMD(QString command);
    void WriteData(const QByteArray &data);
    void WriteData_char(const char* data);
    char modeSelect[1];
    bool pay_page = false;
    bool able_pay = false;
    int video_proc_num=1;
    void sendcommand(const unsigned *buff);
    std::string hexify(unsigned int n);


    bool clock_start=1;


    //               0 1 2 3 4 5 6 7 8 9 A B C D E F G H
    unsigned HL[18]={0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0};
    //                      C 0 1 2 3 4 5 6 7 D 0 1 2 3 4 5 6 7
    unsigned reg_shift[18]={0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1}; //(0x00,0xE3)
    unsigned TP_on[18]={0,1,1,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0}; //(0xC1,0x30)
    unsigned TP_off[18]={0,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0}; //(0xC1,0x00)
    time_t start, end;
    double diff;
    bool nfc_switch=true;




private slots:
    void onGetTouch(int,int);
    void startPaymant(int);
    void onImageChange(int);
    void set_btn(int btn_num);
    void receive_pn5190();
    void on_dial_valueChanged(int value);

    void cola_image_change(int);
    void chicken_image_change(int);
    void drink_image_change();
    void food_image_change();
    void onReplyfinished(QNetworkReply* reply);
    void onReplyfinished_aqi(QNetworkReply* reply);
};
#endif // WIDGET_H
