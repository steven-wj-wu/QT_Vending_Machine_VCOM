#include "widget.h"
#include "ui_widget.h"
#include <cmath>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <ctime>

using namespace std;




Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    sleep(2);
    ui->setupUi(this);

    this->setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);

//Create Touch Area Label on Top
    label = new QLabel(0,Qt::SplashScreen|Qt::WindowStaysOnTopHint);
    label->setGeometry(321,0,1280,720);


//set base UI object----------------------------------------------------------
    QPixmap icon_rain;
    QPixmap icon_air;
    QPixmap icon_location;
    QPixmap icon_volume1;
    QPixmap icon_volume2;
    QPixmap icon_aircon;
    icon_rain.load("/home/steven/qt_widget/weather/rain.png");
    icon_air.load("/home/steven/qt_widget/weather/air.png");
    icon_aircon.load("/home/steven/qt_widget/pannel/air.png");
    icon_location.load("/home/steven/qt_widget/pannel/location.png");
    icon_volume1.load("/home/steven/qt_widget/pannel/volume1.png");
    icon_volume2.load("/home/steven/qt_widget/pannel/volume2.png");
    icon_location=icon_location.scaled(30,30);
    icon_rain=icon_rain.scaled(50, 50);
    icon_air=icon_air.scaled(50, 50);
    icon_volume1=icon_volume1.scaled(50, 50);
    icon_volume2=icon_volume2.scaled(50, 50);
    icon_aircon=icon_aircon.scaled(50, 50);
//Icon Label Set
    ui->label_18->setPixmap(icon_location); //land mark
    ui->label_5->setPixmap(icon_rain);  //rain pop
    ui->label_13->setPixmap(icon_air);  //aqi
    ui->label_16->setPixmap(icon_volume1);  //volume small
    ui->label_16->setStyleSheet("background-color: rgba(0,0,0,0%)");
    ui->label_17->setPixmap(icon_volume2);  //volume loud
    ui->label_17->setStyleSheet("background-color: rgba(0,0,0,0%)");
    ui->label_19->setPixmap(icon_aircon);   //aircon
    ui->dial->setRange(16,32);
//Text Label
    ui->label_3->setStyleSheet("color:White"); //location
    ui->label_12->setStyleSheet("color:White"); //:
    ui->label_6->setStyleSheet("color:White"); //AQI
    ui->label_14->setStyleSheet("color:White"); //AQI_val
    ui->label_11->setStyleSheet("color:White"); //PM2.5
    ui->label_15->setStyleSheet("color:White"); //PM2.5 val

    ui->label_20->setStyleSheet("color:White"); //"C
    ui->label_7->setStyleSheet("color:White;background-color: rgba(0,0,0,0%)"); //temp
    ui->label_10->setStyleSheet("color:White;background-color: rgba(0,0,0,0%)"); //weather

//Volume
    ui->horizontalSlider->setValue(4);
    ui->horizontalSlider->setEnabled(false);
    ui->dial->setEnabled(false);



//與PN5190連線--------------------------------------
    //serial_port_connect();

//GPIO Set-------------------------------------------
    wiringPiSetup();

    /* If TP IC is not autorun,set to open at program begining
    pinMode (CS, OUTPUT);
    pinMode (SCL, OUTPUT);
    pinMode (Data, OUTPUT);
    digitalWrite(CS, HIGH);delayMicroseconds (10);
    digitalWrite(SCL, HIGH);delayMicroseconds (10);
    digitalWrite(Data, HIGH);delayMicroseconds (10);
    sendcommand(reg_shift);
    delay(1);
    sendcommand(TP_on);
    sleep(2);
    */


    if(!serialport_is_available)
        serial_port_connect();
//Network test--------------------------------------------------------
    qDebug()<<"QSSLsocket="<<QSslSocket::sslLibraryBuildVersionString();
    qDebug()<<"SSL situatuion"<<QSslSocket::supportsSsl();
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    QNetworkRequest request;
    qDebug()<<nam->supportedSchemes();

//Request Data : Weather-----------------------------------

    nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyfinished(QNetworkReply*)));
    nam->get(QNetworkRequest{QUrl{"https://opendata.cwb.gov.tw/api/v1/rest/datastore/F-C0032-001?Authorization=CWB-816A0D63-6736-4F79-8D4D-C7FF31E67A19&format=JSON&locationName=%E6%96%B0%E7%AB%B9%E5%B8%82"}});

    QTimer *w = new QTimer(this);
    w->setInterval(300000);
        connect(w, &QTimer::timeout, [&]() {
            nam = new QNetworkAccessManager(this);
            connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyfinished(QNetworkReply*)));
            nam->get(QNetworkRequest{QUrl{"https://opendata.cwb.gov.tw/api/v1/rest/datastore/F-C0032-001?Authorization=CWB-816A0D63-6736-4F79-8D4D-C7FF31E67A19&format=JSON&locationName=%E6%96%B0%E7%AB%B9%E5%B8%82"}});

        } );
    w->start();


//Request Data: AQI-----------------------------------



    AQI_manager = new QNetworkAccessManager(this);
    connect(AQI_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyfinished_aqi(QNetworkReply*)));
    AQI_manager->get(QNetworkRequest{QUrl{"https://data.epa.gov.tw/api/v2/aqx_p_432?api_key=e8dd42e6-9b8b-43f8-991e-b3dee723a52d&limit=1000&sort=ImportDate%20desc&format=JSON"}});

    QTimer *a = new QTimer(this);
    a->setInterval(300000);
    connect(a, &QTimer::timeout, [&]() {
                AQI_manager = new QNetworkAccessManager(this);
                connect(AQI_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyfinished_aqi(QNetworkReply*)));
                AQI_manager->get(QNetworkRequest{QUrl{"https://data.epa.gov.tw/api/v2/aqx_p_432?api_key=e8dd42e6-9b8b-43f8-991e-b3dee723a52d&limit=1000&sort=ImportDate%20desc&format=JSON"}});

    } );
    a->start();

//Clock set------------------------------------------------
    QTimer *t = new QTimer(this);
    t->setInterval(1000);
        connect(t, &QTimer::timeout, [&]() {

            if(!serialport_is_available)
                serial_port_connect();

            QTime qtimeObj = QTime::currentTime();
            QDateTime dateObj = QDateTime::currentDateTime();
            ui->label_2->setText(qtimeObj.toString("hh mm"));
            ui->label_2->setStyleSheet("color:White;text-align: center");
            ui->label_9->setText(dateObj.toString("MM月dd日"));
            ui->label_9->setStyleSheet("color:White;text-align: center");
            if(time_ani){
                ui->label_12->setText("");
                time_ani=false;
            }else{
                ui->label_12->setText(":");
                time_ani=true;
            }
        } );
    t->start();

//Botton Set--------------------------------------------------

    /*IMage button set old
    QPixmap botton_back_img;
    botton_back_img.load("/home/steven/qt_widget/button_background.jpg");
    botton_back_img = botton_back_img.scaled(320,240);
    QPalette palette;
    palette.setBrush(ui->pushButton_3->backgroundRole(), QBrush(botton_back_img));
    palette.setBrush(ui->pushButton_2->backgroundRole(), QBrush(botton_back_img));
    palette.setBrush(ui->pushButton->backgroundRole(), QBrush(botton_back_img));
    ui->pushButton_3->setFlat(true);
    ui->pushButton_3->setAutoFillBackground(true);
    ui->pushButton_3->setPalette(palette);
    ui->pushButton_2->setFlat(true);
    ui->pushButton_2->setAutoFillBackground(true);
    ui->pushButton_2->setPalette(palette);
    ui->pushButton->setFlat(true);
    ui->pushButton->setAutoFillBackground(true);
    ui->pushButton->setAutoFillBackground(true);
    ui->pushButton->setPalette(palette);
    */

    QPixmap img;
    QPixmap Botton1_img;
    QPixmap Botton2_img;
    QPixmap Botton3_img;
    mapper_btw = new QSignalMapper(this); //botton signle map


    img.load("/home/steven/qt_widget/food/all_food.jpg");
    Botton1_img.load("/home/steven/qt_widget/food.png");
    Botton1_img.scaled(180,180);
    Botton2_img.load("/home/steven/qt_widget/drink.png");
    Botton3_img.load("/home/steven/qt_widget/dessert_sold_out.png"); 
    ui->pushButton->setIcon(Botton1_img);
    ui->pushButton_2->setIcon(Botton2_img);
    ui->pushButton_3->setIcon(Botton3_img);


//Connect Event
    connect(ui->pushButton,SIGNAL(clicked()),mapper_btw,SLOT(map()));
    mapper_btw->setMapping(ui->pushButton,1);
    connect(ui->pushButton_2,SIGNAL(clicked()),mapper_btw,SLOT(map()));
    mapper_btw->setMapping(ui->pushButton_2,2);
    connect(ui->pushButton,SIGNAL(clicked()),mapper_btw,SLOT(map()));
    mapper_btw->setMapping(ui->pushButton_3,3);
    connect(mapper_btw,SIGNAL(mapped(int)),this,SLOT(set_btn(int)));


//Thread Initial-----------------------------------------------------------
    tThread = new touchthread(this);
    vThread = new videothread(this);
    connect( tThread, SIGNAL(portOpen(int)), this, SLOT(startPaymant(int)) ); //get touch event
    connect( tThread, SIGNAL(getTouch(int,int)), this, SLOT(onGetTouch(int,int)) ); //deal touchinfo
    connect( this, SIGNAL(VideoChange(int)), vThread, SLOT(onVideoChange(int)) ); //play video
    connect( this, SIGNAL(ImageChange(int)), this, SLOT(onImageChange(int)) ); //touch area image change
    //connect( this, SIGNAL(ContentChange(int)), this, SLOT(onGetTouch(int,int))); //
    connect( this, SIGNAL(GetPay(int,int)), this, SLOT(onGetTouch(int,int))); //5190 process signal

    tThread->start();
    tThread->current_mode=1;

//Video Thread Timer Control-------------------------------------------------------
//Switch Touch Area Image and Touch step After Timer Set
//---------------------------------------------------------------------------------

//wait pay tep
    drink_step_map =  new QSignalMapper(this);
    food_step_map =  new QSignalMapper(this);
    connect(drink_step_map,SIGNAL(mapped(int)),this,SLOT(cola_image_change(int)));  //drink link map
    connect(food_step_map,SIGNAL(mapped(int)),this,SLOT(chicken_image_change(int))); //food kind map

    tempTimer= new QTimer(this);
    tempTimer->setInterval(800);
    tempTimer->setSingleShot(true);
    connect(tempTimer,SIGNAL(timeout()),drink_step_map,SLOT(map()));

    tempTimer_food= new QTimer(this);
    tempTimer_food->setInterval(800);
    tempTimer_food->setSingleShot(true);
    connect(tempTimer_food,SIGNAL(timeout()),food_step_map,SLOT(map()));


//pay step
    drink_switch_Timer = new QTimer(this);
    drink_switch_Timer->setSingleShot(true);
    drink_switch_Timer->setInterval(2000);
    QObject::connect(drink_switch_Timer, SIGNAL(timeout()),this, SLOT(drink_image_change())); //back to

    food_switch_Timer = new QTimer(this);
    food_switch_Timer->setSingleShot(true);
    food_switch_Timer->setInterval(1000);
    QObject::connect(food_switch_Timer, SIGNAL(timeout()),this, SLOT(food_image_change()));



//App start: At playing navigation video---------------------------------------------------
    tThread->drive_scene=true;
    vThread->start();
    label->show();
    emit VideoChange(300);
    vThread->initial=false;

}

Widget::~Widget()
{
    if (serialPort->isOpen())
        serialPort->close();

    delete ui;
}


void Widget:: startPaymant(int openFlag)
{
        if (openFlag)
        {
            sleep(2);           
        }
        else
        {
            QMessageBox::information(this,"Error!","Don't Detect Panel!");
        }
}

//Touch Action Control----------------------------------------------
//Action With Value From Touch Thread With Different Touch Step
//------------------------------------------------------------------

void Widget:: onGetTouch(int step,int diff)
{
    QPixmap img;
//------------------------------------
       //Touch Status Sample:
       //          0                                   0                                     0
       // content(food 0,drink 1 drive 3)     step(select 0,pay 1,pay_finish 2)     type(cola,chicken)
       //ex: [0 0 0]-> open food head video
       //ex: [0 0 1]-> Stop food head video
       //ex: [0 0 2]-> food cancel from pay step
       //ex: [0 1 0-4] ->Switch food pay
       //ex: [1 0 0]-> Switch drink head
       //ex: [1 0 1]-> Stop drink head
       //ex: [0 2 0-4] -> Switch food pay finish video
       //999 is for volume UI
       //888 is for AIR CON UI
//----------------------------------

    switch(step)
    {


//volume UI
    case 999:
        ui->horizontalSlider->setValue(diff);
        break;
//AIR con UI
   case 888:
         ui->dial->setValue(24-diff);
        break;


//drive video stop
        case 301:
            emit VideoChange(301); //mplayer stop video
            emit VideoChange(666); //play food head
            on_choose_btn=1;       //set food mode
            tThread->touchLock=0;
            tThread->videoLock=1;
            tThread->touchStep=-1;
            pay_page=false;
            food_switch_Timer->start();

            break;

//drive video start
        case 300:
            qDebug()<<"play start";
            //emit ImageChange(7);
            emit VideoChange(300);
        break;


// stop head video
        case 1 :
            emit VideoChange(1001);
            img.load("/home/steven/qt_widget/food/all_food.jpg");
            tThread->touchStep = 2;
            tThread->touchLock = 0;
            img = img.scaled(winWidth, winHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            //ui->label->setPixmap(img);
            label->setPixmap(img);
        break;



//FOOD---------------------------------------------

// start head video
          case 0 :
               tThread->touchLock = 1;
               emit VideoChange(0);
               //sleep(2);
               tThread->touchLock = 0;

          break;


// back to wait choose food
            case 2 :
                tThread->touchStep = 2;
                tThread->touchLock = 1;
                //nThread->nfcLock = 1;
                emit VideoChange(1);   //close video
                emit ImageChange(2);   //food image
                //usleep(10000);
                pay_page=false;
                tThread->touchLock = 0;
                //WriteCMD("C");
            break;



// show wait pay video
            case 10 :
            case 11 :
            case 12 :
            case 13 :
            case 14 :
                tThread->touchLock = 1;
                emit VideoChange(20+tThread->drinkKind);
                food_step_map->setMapping(tempTimer_food,step);
                tempTimer_food->start();
                break;

// show pay video
            case 20 :
            case 21 :
            case 22 :
            case 23 :
            case 24 :
            emit VideoChange(30+tThread->drinkKind);
            food_switch_Timer->start();
            tThread->end_sep=true;
            tThread->break_btouch=true;
            able_pay=false;
            last_5190='F';
            tThread->touchLock = 1;
            printf("play pay video\n");
            while(vThread->playFinished == 0){usleep(100000);}
            break;


//--------------------------------Drink

//play drink head
            case 100 :
                tThread->touchLock = 1;
                emit VideoChange(100);
                tThread->touchLock = 0;
            break;

//stop drink head
            case 101 :
            img.load("/home/steven/qt_widget/drink/all_drink.jpg");
            tThread->touchStep = 102;
            tThread->touchLock = 0;
            emit VideoChange(101);
            img = img.scaled(winWidth, winHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            label->setPixmap(img);
            break;


//back to wait choose drink
            case 102 :
                tThread->touchStep = 102;
                tThread->touchLock = 1;
                emit VideoChange(1);   //close video
                emit ImageChange(102);   //food image
                pay_page=false;
                tThread->touchLock = 0;
                //WriteCMD("C");
            break;

//show drink wait pay video
            case 110 :
            case 111 :
            case 112 :
            case 113 :
            case 114 :
            case 115 :
                emit ImageChange(1000+step); //video show up image
                tThread->touchLock = 1;
                emit VideoChange(20+tThread->drinkKind);
                while(vThread->playFinished == 0){usleep(100000);}
                drink_step_map->setMapping(tempTimer,step);
                tempTimer->start();
             break;


// show pay video
            case 120 :
            case 121 :
            case 122 :
            case 123 :
            case 124 :
            case 125 :
                //wait 2 sec and open TP
                //sleep(2);

                last_5190='F';           
                emit VideoChange(30+tThread->drinkKind);
                //sep_off
                tThread->end_sep=true;
                tThread->break_btouch=true;
                able_pay=false;
                drink_switch_Timer->start();
                tThread->touchLock = 1;           
                printf("play pay video\n");
                while(vThread->playFinished == 0){usleep(100000);}  
                break;

        default :
            break;
    }
}

//Image Chnage in Touch Area
void Widget::onImageChange(int imgIdx)
{
    QPixmap img;

    switch(imgIdx)
    {

    //-------------------------------------------------------------food
        case 2:
             img.load("/home/steven/qt_widget/food/all_food.jpg");
            break;

        case 7:
         img.load("/home/steven/qt_widget/black_background.png");
        break;


        case 10:
            img.load("/home/steven/qt_widget/food/chicken.jpg");
            break;

        case 11:
            img.load("/home/steven/qt_widget/food/fries.jpg");
            break;

        case 12:
            img.load("/home/steven/qt_widget/food/bugger.jpg");
            break;

        case 13:
            img.load("/home/steven/qt_widget/food/onion.jpg");
            break;

        case 14:
            img.load("/home/steven/qt_widget/food/nuggets.jpg");
            break;

    //-----------------------------------------------------------drink
    case 102:
         img.load("/home/steven/qt_widget/drink/all_drink.jpg");
        break;


    case 110:
        img.load("/home/steven/qt_widget/drink/cola.png");
        break;
    case 1110:
        img.load("/home/steven/qt_widget/drink/COLA-1_frame.jpg");
        break;

    case 111:
        img.load("/home/steven/qt_widget/drink/soda.png");
        break;
    case 1111:
        img.load("/home/steven/qt_widget/drink/SODA-1_frame.jpg");
        break;

    case 112:
        img.load("/home/steven/qt_widget/drink/sid2_premiumcoffee.png");
        break;
    case 1112:
        img.load("/home/steven/qt_widget/drink/PREMUMCOFFEE-1_frame.jpg");
        break;

    case 113:
        img.load("/home/steven/qt_widget/drink/sid2_orangejuice.png");
        break;
    case 1113:
        img.load("/home/steven/qt_widget/drink/ORANGEJUICE-1_frame.jpg");
        break;

    case 114:
        img.load("/home/steven/qt_widget/drink/sid2_greentea.png");
        break;
    case 1114:
        img.load("/home/steven/qt_widget/drink/GREENTEA-1_frame.jpg");
        break;

    case 115:
        img.load("/home/steven/qt_widget/drink/sid2_mineralwater.png");
        break;
    case 1115:
        img.load("/home/steven/qt_widget/drink/MINERALWATER-1_frame.jpg");
        break;

    case 666:
        img.load("/home/steven/qt_widget/drink/COLA-1_frame.jpg");
        break;

    }

    img = img.scaled(winWidth, winHeight);
    if(imgIdx==7)
        img = img.scaled(winWidth, winHeight);
    //ui->label->setPixmap(img);
    label->setPixmap(img);
    //label->show();
}


//Select Botton function-------------------------------------------
void Widget::set_btn(int btn_num){

    QPixmap img;

    if(btn_num==1){
        if(on_choose_btn!=1){
//Action is same as start from food head
            emit VideoChange(1);
            img.load("/home/steven/qt_widget/black_background.png");          
            on_choose_btn=1;
//Reset Touch Step
            tThread->touchLock=0;
            tThread->videoLock=1;
            tThread->touchStep=-1;
            pay_page=false;
            emit VideoChange(0);
            img = img.scaled(winWidth, winHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            ui->label->setPixmap(img);
            label->setPixmap(img);
            tThread->current_mode = btn_num;

        }
    }

    else if(btn_num==2){
        if(on_choose_btn!=2){
            emit VideoChange(1);
            img.load("/home/steven/qt_widget/black_background.png");  
            on_choose_btn=2;
            tThread->touchLock=0;
            tThread->videoLock=1;
            tThread->touchStep=-1;
            pay_page=false;
            emit VideoChange(100);
            img = img.scaled(winWidth, winHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            ui->label->setPixmap(img);
            label->setPixmap(img);
            tThread->current_mode = btn_num;
        }    

    }

}


//5190 function---------------------------------------------
void Widget::serial_port_connect()
{
    // 配置與 PN5190 的ComPort
    //serialport_is_available=false;
    serial_port_name="";
    serialPort = new QSerialPort();  
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier()&&serialPortInfo.hasProductIdentifier()){
            //qDebug()<<"serialPortInfo.portName():"<<serialPortInfo.portName();
            //qDebug()<<"serialPortInfo.vendorIdentifier():"<<serialPortInfo.vendorIdentifier();
            //qDebug()<<"serialPortInfo.productIdentifier():"<<serialPortInfo.productIdentifier();
            //serial_port_name=serialPortInfo.portName();
            //serialport_is_available=true;
            if(serialPortInfo.vendorIdentifier() == pn5190_vendorID){
                if(serialPortInfo.productIdentifier() == pn5190_productID){
                    serial_port_name=serialPortInfo.portName();
                    serialport_is_available=true;
                }
            }
        }
    }

    if(serialport_is_available)
    {
        serialPort->setPortName(serial_port_name);
        serialPort->open(QSerialPort::ReadWrite);
        serialPort->setBaudRate(QSerialPort::Baud115200);
        serialPort->setDataBits(QSerialPort::Data8);
        serialPort->setParity(QSerialPort::NoParity);
        serialPort->setStopBits(QSerialPort::OneStop);
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
        connect(serialPort,SIGNAL(readyRead()),this,SLOT(receive_pn5190()));        
        qDebug()<<"Connect"<<endl;
    }else{
        qDebug()<<"Connect Failed"<<endl;
        //QMessageBox::warning(this,"Port error","Couldn't find the pn5190");
    }
}
void Widget::WriteCMD(QString command)
{

    if(serialPort->isWritable()){
        //qDebug()<<"CMD";
        serialPort->write(command.toStdString().c_str());
    }else{
        qDebug()<<"Couldn't write to serial";
    }
}
void Widget::receive_pn5190()
{

    // 收集來自pn5190的資訊
    //qDebug() << "Serial Port Works!!\n";
    //QMessageBox::information(this, "Serial Port Works", "Opened serial port to pn5190.");
    char *buffer;
    std::string id_buffer;
    QByteArray serialData = serialPort->readAll();
    QString temp = QString::fromStdString(serialData.toStdString());


    //Change UID label
    if(serialData.size()>3){
        if(get_uid){
            get_uid = false;
            ui->uuid->setText("UID:");
            buffer = serialData.data();
            QString temp ;
            for(int i=0;i<serialData.size();i++){
                 //printf("%d\t",buffer[i]);
                 id_buffer = hexify((unsigned char)buffer[i]);
                 temp += (QString::fromStdString(id_buffer)+" ");
             }
             qDebug()<<temp;
             ui->uuid->setText("UID: "+temp);
             ui->uuid->setStyleSheet("color:red");
        }
    }


    if(pay_page == true){ //if is pay step
        if(able_pay==true){ //if is not canceling

             for(int j=0;j<serialData.size();j++){

                qDebug() <<"receive_pn5190:"<< temp[j];

                if(temp[j]=='A'){
                   //if last message is D => pay failed and open TP
                    /*
                    if(last_5190=='D'){

                        pinMode (CS, OUTPUT);
                        pinMode (SCL, OUTPUT);
                        pinMode (Data, OUTPUT);
                        digitalWrite(CS, HIGH);delayMicroseconds (10);
                        digitalWrite(SCL, HIGH);delayMicroseconds (10);
                        digitalWrite(Data, HIGH);delayMicroseconds (10);
                        sendcommand(reg_shift);
                        delay(1);
                        sendcommand(TP_on);
                        sleep(2);
                        last_5190=temp;
                    }
                     */

                   //else send P to start payment with 5190
                   last_5190=temp;
                   WriteCMD("P");
                }

                else if(temp[j]=='D'){
               //Close TP------------------------------------------------
                    /*
                        pinMode (CS, OUTPUT);
                        pinMode (SCL, OUTPUT);
                        pinMode (Data, OUTPUT);
                        digitalWrite(CS, HIGH);delayMicroseconds (10);
                        digitalWrite(SCL, HIGH);delayMicroseconds (10);
                        digitalWrite(Data, HIGH);delayMicroseconds (10);
                        sendcommand(reg_shift);
                        delay(1);
                        sendcommand(TP_off);
                        last_5190=temp;
                      */

                    //if(clock_start){
                       // clock_t start_nfc= clock();
                        //clock_start = 0;
                    //}

                }

                /*
                else if(temp[0]=='F'){//buying finish
                }
                */
                else if(temp[j]=='U'){//Get UID
                    get_uid = true;
                    nfc_switch=true; //nfc_close
                    GetPay(20+tThread->drinkKind,0);
                    usleep(50000);
                }
                else if(temp[j]=='T'){//NFC open
                    nfc_switch=false;
                }
            }
        }
    }

}

void Widget::sendcommand(const unsigned *buff)
{
        digitalWrite(CS,LOW);
        delayMicroseconds(1);
        //printf(sizeof(buf));

        for(int i=0;i<18;i++){
          digitalWrite(SCL,LOW);
          delayMicroseconds(10);
          digitalWrite(Data,buff[i]);
          delayMicroseconds(10);
          digitalWrite(SCL,HIGH);
          delayMicroseconds(20);
        }
        digitalWrite(CS,HIGH);
}

//UI Function------------------------------------------------------
//LCD Number Change
void Widget::on_dial_valueChanged(int value)
{
    ui->lcdNumber->display(value);
}


//Select Drink to pay Drink Step
void Widget::cola_image_change(int step)
{
    emit ImageChange(step);
    tThread->touchStep = 101;
    tThread->touchLock = 0;
    pay_page=true;
    able_pay=true;
    if(nfc_switch){
        //if nfc is close pass R
        WriteCMD("R");
    }
}

//Select food to pay food Step
void Widget::chicken_image_change(int step)
{
    emit ImageChange(step);
    tThread->touchStep = 1;
    tThread->touchLock = 0;
    pay_page=true;
    able_pay=true;
    if(nfc_switch){
        //if nfc is close pass R
        WriteCMD("R");
    }
}


//pay finish back to select page
void Widget::drink_image_change()
{
   emit ImageChange(102);
   tThread->touchStep = 102;
   tThread->touchLock = 0;
   pay_page = false;
   /* pay finish TP on
   pinMode (CS, OUTPUT);
   pinMode (SCL, OUTPUT);
   pinMode (Data, OUTPUT);
   digitalWrite(CS, HIGH);delayMicroseconds (10);
   digitalWrite(SCL, HIGH);delayMicroseconds (10);
   digitalWrite(Data, HIGH);delayMicroseconds (10);
   sendcommand(reg_shift);
   delay(1);
   sendcommand(TP_on);
   */
   

}

//pay finish back to select page
void Widget::food_image_change()
{
   emit ImageChange(2);
   tThread->touchStep = 2;
   tThread->touchLock = 0;
   pay_page = false;
   /* pay finish TP on
   pinMode (CS, OUTPUT);
   pinMode (SCL, OUTPUT);
   pinMode (Data, OUTPUT);
   digitalWrite(CS, HIGH);delayMicroseconds (10);
   digitalWrite(SCL, HIGH);delayMicroseconds (10);
   digitalWrite(Data, HIGH);delayMicroseconds (10);
   sendcommand(reg_shift);
   delay(1);
   sendcommand(TP_on);
   */
}

//Weather JSON Data Process
void Widget::onReplyfinished(QNetworkReply* reply)
{
    if (reply->error()) {
        QPixmap weather_img;
        weather_img.load("/home/steven/qt_widget/weather/no_internet.png");
        qDebug() << reply->errorString();
        weather_img=weather_img.scaled(150, 150);
        ui->label_4->setPixmap(weather_img);
        ui->label_7->setText("Non");
        ui->label_8->setText("Non");
        ui->label_10->setText("No Internet");
        return;
    }

    QString answer = reply->readAll();
    //qDebug() << answer;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
    QPixmap weather_img;
    QTime c_timeObj = QTime::currentTime();
    int time_step = 0;
    int hour = c_timeObj.toString("hh").toInt();
    if(hour>18)
        time_step=1;

    QJsonObject jsonObj = jsonResponse.object();
    qDebug()<<jsonObj["success"].toString(); //test API
    QJsonObject obj_records = jsonObj["records"].toObject();
    QJsonArray records_datas = obj_records["location"].toArray();
    foreach (const QJsonValue & value, records_datas) {
        QJsonObject obj_in_datas = value.toObject();
        //qDebug()<<obj;
        QJsonArray weather_elements = obj_in_datas["weatherElement"].toArray(); //all of weather temp rain datas
        foreach (const QJsonValue & value_weather,  weather_elements) {
             QJsonObject obj_weather = value_weather.toObject();

             if(obj_weather["elementName"].toString()=="Wx"){ //weather description
                QJsonArray Wx_times = obj_weather["time"].toArray(); //36 hours datas separate in 3
                QJsonObject Wx_time_1= Wx_times[time_step].toObject(); //use current time
                QJsonObject Wx_time_1_data = Wx_time_1["parameter"].toObject();
                QString Wx_time_1_data_toString = Wx_time_1_data["parameterValue"].toString();
                QString Wx_time_1_data_name_toString = Wx_time_1_data["parameterName"].toString();
                 qDebug()<<"wx:"<< Wx_time_1_data_toString; //天氣

                 if(Wx_time_1_data_toString.toInt()<2){
                     //ui->label_4->setText(Wx_time_1_data_toString);
                     //ui->label_4->setStyleSheet("color:White");
                     weather_img.load("/home/steven/qt_widget/weather/sunny.png");
                     weather_img=weather_img.scaled(121, 121);
                     ui->label_4->setPixmap(weather_img);

                 }
                 else if(Wx_time_1_data_toString.toInt()>1 && Wx_time_1_data_toString.toInt()<5 ){
                     weather_img.load("/home/steven/qt_widget/weather/cloudy.png");
                     weather_img=weather_img.scaled(121, 121);
                     ui->label_4->setPixmap(weather_img);
                 }
                 else if(Wx_time_1_data_toString.toInt()>4 && Wx_time_1_data_toString.toInt()<8){
                     weather_img.load("/home/steven/qt_widget/weather/cloudy2.png");
                     weather_img=weather_img.scaled(121, 121);
                     ui->label_4->setPixmap(weather_img);
                 }
                 else if(Wx_time_1_data_toString.toInt()>7 && Wx_time_1_data_toString.toInt()<15){
                     weather_img.load("/home/steven/qt_widget/weather/rainy.png");
                     weather_img=weather_img.scaled(121, 121);
                     ui->label_4->setPixmap(weather_img);
                 }

                 ui->label_10->setText(Wx_time_1_data_name_toString);
                 ui->label_10->setStyleSheet("color:White");

             }

             else if(obj_weather["elementName"].toString()=="PoP"){ //Rain pop
                QJsonArray pop_times = obj_weather["time"].toArray(); //36 hours datas separate in 3
                QJsonObject pop_time_1= pop_times[time_step].toObject(); //use current time
                QJsonObject pop_time_1_data = pop_time_1["parameter"].toObject();
                QString pop_time_1_data_toString = pop_time_1_data["parameterName"].toString();
                 qDebug()<<"pop:"<< pop_time_1_data_toString ; //降雨機率

                 ui->label_8->setText(pop_time_1_data_toString +"%");
                 ui->label_8->setStyleSheet("color:White;background-color: rgba(0,0,0,0%)");

             }

             else if(obj_weather["elementName"].toString()=="MaxT"){
                 QJsonArray temp_times = obj_weather["time"].toArray(); //36 hours datas separate in 3
                 QJsonObject temp_time_1= temp_times[time_step].toObject(); //use current time
                 QJsonObject temp_time_1_data = temp_time_1["parameter"].toObject();
                 QString temp_time_1_data_toString = temp_time_1_data["parameterName"].toString();
                 qDebug()<<"Max Temp:"<< temp_time_1_data_toString ; //highest temp
                 ui->label_7->setText(temp_time_1_data_toString +"°C");
                 ui->label_7->setStyleSheet("color:White;background-color: rgba(0,0,0,0%)");

             }
        }
    }
}

//AQI JSON Data Process
void Widget::onReplyfinished_aqi(QNetworkReply* reply){

                                         if (reply->error()) {
                                             qDebug() << reply->errorString();
                                             ui->label_14->setText("Non");
                                             ui->label_15->setText("Non");
                                             return;
                                         }


                                         QString answer = reply->readAll();
                                         QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
                                         QJsonObject AQIObj = jsonResponse.object();

                                         QJsonArray aqi_record = AQIObj["records"].toArray();

                                         foreach (const QJsonValue & value, aqi_record) {
                                             QJsonObject record_obj = value.toObject();

                                             if(record_obj["siteid"].toString().toInt()==24){ //24 == Hsinchu
                                                 //qDebug()<< record_obj["siteid"].toString().toInt();
                                                 ui->label_14->setText(record_obj["aqi"].toString());
                                                 if(record_obj["aqi"].toString().toInt()<51){
                                                     ui->label_14->setStyleSheet("color:#B7FF4A");
                                                 }
                                                 else if(record_obj["aqi"].toString().toInt()>50 && record_obj["aqi"].toString().toInt()<101){
                                                     ui->label_14->setStyleSheet("color:#FFA042");
                                                 }
                                                 else if(record_obj["aqi"].toString().toInt()>50 && record_obj["aqi"].toString().toInt()<101){
                                                     ui->label_14->setStyleSheet("color:#FF5809");
                                                 }

                                                 ui->label_15->setText(record_obj["pm2.5"].toString());
                                                 if(record_obj["pm2.5"].toString().toInt()<51){
                                                     ui->label_15->setStyleSheet("color:#B7FF4A");
                                                 }
                                                 else if(record_obj["pm2.5"].toString().toInt()>50 && record_obj["pm2.5"].toString().toInt()<101){
                                                     ui->label_15->setStyleSheet("color:#FFA042");
                                                 }
                                                 else if(record_obj["pm2.5"].toString().toInt()>50 && record_obj["pm2.5"].toString().toInt()<101){
                                                     ui->label_15->setStyleSheet("color:#FF5809");
                                                 }

                                             }

                                         }


}

//parse UUID hex data
std::string Widget::hexify(unsigned int n)
{
  std::string res;

  do
  {
    res += "0123456789ABCDEF"[n % 16];
    n >>= 4;
  } while(n);

  return std::string(res.rbegin(), res.rend());
}

/* Request weather Single time sample
QObject::connect(nam, &QNetworkAccessManager::finished,
            this, [=](QNetworkReply *reply) {
                if (reply->error()) {
                    QPixmap weather_img;
                    weather_img.load("/home/steven/qt_widget/weather/no_internet.png");
                    qDebug() << reply->errorString();
                    weather_img=weather_img.scaled(150, 150);
                    ui->label_4->setPixmap(weather_img);
                    ui->label_7->setText("Non");
                    ui->label_8->setText("Non");
                    ui->label_10->setText("No Internet");



                    return;
                }

                QString answer = reply->readAll();
                //qDebug() << answer;
                QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
                QPixmap weather_img;
                QTime c_timeObj = QTime::currentTime();
                int time_step = 0;
                int hour = c_timeObj.toString("hh").toInt();
                if(hour>18)
                    time_step=1;

                QJsonObject jsonObj = jsonResponse.object();
                qDebug()<<jsonObj["success"].toString(); //test API
                QJsonObject obj_records = jsonObj["records"].toObject();
                QJsonArray records_datas = obj_records["location"].toArray();
                foreach (const QJsonValue & value, records_datas) {
                    QJsonObject obj_in_datas = value.toObject();
                    //qDebug()<<obj;
                    QJsonArray weather_elements = obj_in_datas["weatherElement"].toArray(); //all of weather temp rain datas
                    foreach (const QJsonValue & value_weather,  weather_elements) {
                         QJsonObject obj_weather = value_weather.toObject();

                         if(obj_weather["elementName"].toString()=="Wx"){ //weather description
                            QJsonArray Wx_times = obj_weather["time"].toArray(); //36 hours datas separate in 3
                            QJsonObject Wx_time_1= Wx_times[time_step].toObject(); //use current time
                            QJsonObject Wx_time_1_data = Wx_time_1["parameter"].toObject();
                            QString Wx_time_1_data_toString = Wx_time_1_data["parameterValue"].toString();
                            QString Wx_time_1_data_name_toString = Wx_time_1_data["parameterName"].toString();
                             qDebug()<<"wx:"<< Wx_time_1_data_toString; //天氣

                             if(Wx_time_1_data_toString.toInt()<2){
                                 //ui->label_4->setText(Wx_time_1_data_toString);
                                 //ui->label_4->setStyleSheet("color:White");
                                 weather_img.load("/home/steven/qt_widget/weather/sunny.png");
                                 weather_img=weather_img.scaled(150, 150);
                                 ui->label_4->setPixmap(weather_img);

                             }
                             else if(Wx_time_1_data_toString.toInt()>1 && Wx_time_1_data_toString.toInt()<5 ){
                                 weather_img.load("/home/steven/qt_widget/weather/cloudy.png");
                                 weather_img=weather_img.scaled(150, 150);
                                 ui->label_4->setPixmap(weather_img);
                             }
                             else if(Wx_time_1_data_toString.toInt()>4 && Wx_time_1_data_toString.toInt()<8){
                                 weather_img.load("/home/steven/qt_widget/weather/cloudy2.png");
                                 weather_img=weather_img.scaled(150, 150);
                                 ui->label_4->setPixmap(weather_img);
                             }
                             else if(Wx_time_1_data_toString.toInt()>7 && Wx_time_1_data_toString.toInt()<15){
                                 weather_img.load("/home/steven/qt_widget/weather/rainy.png");
                                 weather_img=weather_img.scaled(150, 150);
                                 ui->label_4->setPixmap(weather_img);
                             }

                             ui->label_10->setText(Wx_time_1_data_name_toString);
                             ui->label_10->setStyleSheet("color:White");

                         }

                         else if(obj_weather["elementName"].toString()=="PoP"){ //Rain pop
                            QJsonArray pop_times = obj_weather["time"].toArray(); //36 hours datas separate in 3
                            QJsonObject pop_time_1= pop_times[time_step].toObject(); //use current time
                            QJsonObject pop_time_1_data = pop_time_1["parameter"].toObject();
                            QString pop_time_1_data_toString = pop_time_1_data["parameterName"].toString();
                             qDebug()<<"pop:"<< pop_time_1_data_toString ; //降雨機率

                             ui->label_8->setText(pop_time_1_data_toString +"%");
                             ui->label_8->setStyleSheet("color:White;background-color: rgba(0,0,0,0%)");

                         }

                         else if(obj_weather["elementName"].toString()=="MaxT"){
                             QJsonArray temp_times = obj_weather["time"].toArray(); //36 hours datas separate in 3
                             QJsonObject temp_time_1= temp_times[time_step].toObject(); //use current time
                             QJsonObject temp_time_1_data = temp_time_1["parameter"].toObject();
                             QString temp_time_1_data_toString = temp_time_1_data["parameterName"].toString();
                             qDebug()<<"Max Temp:"<< temp_time_1_data_toString ; //highest temp
                             ui->label_7->setText(temp_time_1_data_toString +"°C");
                             ui->label_7->setStyleSheet("color:White;background-color: rgba(0,0,0,0%)");

                         }
                    }
                }

            });
//request.setUrl(QUrl("https://opendata.cwb.gov.tw/api/v1/rest/datastore/F-C0032-001?Authorization=CWB-539CBA7F-E6BB-4C06-9124-B4F2F24FF173&format=JSON&locationName=%E6%96%B0%E7%AB%B9%E5%B8%82"));
//QNetworkReply * reply = nam->get(request);
*/

/* Request AQI Single Time Sample
QObject::connect(AQI_manager, &QNetworkAccessManager::finished,
        this, [=](QNetworkReply *reply) {


            if (reply->error()) {
                qDebug() << reply->errorString();
                ui->label_14->setText("Non");
                ui->label_15->setText("Non");
                return;
            }


            QString answer = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
            QJsonObject AQIObj = jsonResponse.object();

            QJsonArray aqi_record = AQIObj["records"].toArray();

            foreach (const QJsonValue & value, aqi_record) {
                QJsonObject record_obj = value.toObject();

                if(record_obj["siteid"].toString().toInt()==24){ //24 == Hsinchu
                    //qDebug()<< record_obj["siteid"].toString().toInt();
                    ui->label_14->setText(record_obj["aqi"].toString());
                    if(record_obj["aqi"].toString().toInt()<51){
                        ui->label_14->setStyleSheet("color:#B7FF4A");
                    }
                    else if(record_obj["aqi"].toString().toInt()>50 && record_obj["aqi"].toString().toInt()<101){
                        ui->label_14->setStyleSheet("color:#FFA042");
                    }
                    else if(record_obj["aqi"].toString().toInt()>50 && record_obj["aqi"].toString().toInt()<101){
                        ui->label_14->setStyleSheet("color:#FF5809");
                    }

                    ui->label_15->setText(record_obj["pm2.5"].toString());
                    if(record_obj["pm2.5"].toString().toInt()<51){
                        ui->label_15->setStyleSheet("color:#B7FF4A");
                    }
                    else if(record_obj["pm2.5"].toString().toInt()>50 && record_obj["pm2.5"].toString().toInt()<101){
                        ui->label_15->setStyleSheet("color:#FFA042");
                    }
                    else if(record_obj["pm2.5"].toString().toInt()>50 && record_obj["pm2.5"].toString().toInt()<101){
                        ui->label_15->setStyleSheet("color:#FF5809");
                    }

                }

            }
          });

//QNetworkAccessManager *AQI_manager = new QNetworkAccessManager(this);
//QNetworkRequest request_aqi;
//request_aqi.setUrl(QUrl("https://data.epa.gov.tw/api/v2/aqx_p_432?api_key=e8dd42e6-9b8b-43f8-991e-b3dee723a52d&limit=1000&sort=ImportDate%20desc&format=JSON"));
//AQI_manager->get(request_aqi);

*/
