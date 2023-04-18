#include "touchthread.h"
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>
#include "touch_function/getevent.h"

int x = 320;

//DRINK REGION
QRegion colaElliRegion (x+201, 236, 125, 125, QRegion::Ellipse);
QRegion colaRectRegion (x+189, 371, 150, 316, QRegion::Rectangle);

QRegion sodaElliRegion (x+389, 228,125,125, QRegion::Ellipse);
QRegion sodaRectRegion (x+374, 448, 154, 308, QRegion::Rectangle);

QRegion coffeeElliRegion (x+568, 268, 125, 125, QRegion::Ellipse);
QRegion coffeeRectRegion (x+566, 404, 141, 283, QRegion::Rectangle);

QRegion orangeElliRegion (x+734, 137, 125, 125, QRegion::Ellipse);
QRegion orangeRectRegion (x+742, 272, 111, 409, QRegion::Rectangle);

QRegion teaElliRegion (x+896, 118, 125, 125, QRegion::Ellipse);
QRegion teaRectRegion (x+891, 256, 133, 432, QRegion::Rectangle);

QRegion waterElliRegion (x+1067, 126, 125, 125, QRegion::Ellipse);
QRegion waterRectRegion (x+1062, 265, 132, 415, QRegion::Rectangle);

//FOOD REGION
QRegion chickenElliRegion (x+77, 311, 150, 150, QRegion::Ellipse);
QRegion chickenRectRegion (x+7, 485, 273, 204, QRegion::Rectangle);

QRegion friesElliRegion (x+326, 300, 150, 150, QRegion::Ellipse);
QRegion friesRectRegion (x+294, 457, 206, 93, QRegion::Rectangle);

QRegion burgerElliRegion (x+561, 282, 150, 150, QRegion::Ellipse);
QRegion burgerRectRegion (x+521, 448, 243, 224, QRegion::Rectangle);

QRegion onionElliRegion (x+842, 319, 150, 150, QRegion::Ellipse);
QRegion onionRectRegion (x+785, 464, 244, 104, QRegion::Rectangle);

QRegion nuggetsElliRegion (x+1063, 339, 150, 150, QRegion::Ellipse);
QRegion nuggetsRectRegion (x+1043, 494, 223, 172, QRegion::Rectangle);


QRegion payElliRegion (x+703, 288, 262, 262, QRegion::Ellipse);
QRegion cancelRectRegion(x+66, 612, 256, 70, QRegion::Rectangle);

// parse angle of point of volume button
double touchthread ::get_angle(double x1,double y1,double x2,double y2,double x3,double y3){
    double theta = atan2(x1-x3,y1-y3)-atan2(x2-x3,y2-y3);
    //qDebug()<<atan2(x2-x3,y2-y3);
    if(theta>M_PI)
        theta -= -2*M_PI;
   if(theta<-M_PI)
       theta += 2*M_PI;

   theta = theta*180.0/M_PI;
   return theta;
}

//if point is in volume area
bool touchthread ::in_circle(double x1,double y1){
    double radius = 100;
    double cx=140,cy=655;
    double dis = sqrt((x1-cx)*(x1-cx)+(y1-cy)*(y1-cy));
    if(dis<=radius){
        return true;
    }
    else {
        return false;
    }
}


void touchthread :: run() {
    double cx=140,cy=665; //circle center of air-con button
    double x2=140,y2=570; //radius = 570-665
    int fd, openFlag, px, py, vaild;
    int volume_times=0;
    double difference=0.0;
    int volume_last_x=-1;
    fd_set readfds;
    QPoint touchPoint;

   //initial  device
   fd = openPort(&readfds,0);
        if (fd>=0)
            openFlag = 1;
        else
            openFlag = 0;
        emit portOpen(openFlag);

     touchLock=0;
     videoLock=1;
     touchStep=-1;

     if (fd>=0)
         {
             while(1)
             {
                 //get touch point
                 touchEvent(fd, &readfds, &px, &py, &break_btouch);

                 //seprate_off
                 if(end_sep &&SEP_TP_thread ){
                     end_sep=false;
                     system("sudo rmmod focaltech.ko.xz");
                     system("i2cset -y 1 0x38 0x9E 0X00");
                     fd = openPort(&readfds,1); //initial device again
                 }

                 touchPoint.setX(px);
                 touchPoint.setY(py);

                 //qDebug()<<"[TOUCH] touchLock:"<<px<<","<<py;

                 //food and drink select area
                 if((videoLock && touchLock == 0) && touchPoint.x()>320 )
                 {   //STOP HEAD
                     if(drive_scene){
                         emit getTouch(301,0);
                         drive_scene=false;
                         continue;
                        }
                     else{
                        if(current_mode==1){//FOOD
                           emit getTouch(1,0); //play food head
                           videoLock = 0;
                           if(SEP_TP_thread ){
                               end_sep=false;
                               system("sudo rmmod focaltech.ko.xz");
                               system("i2cset -y 1 0x38 0x9E 0X00");
                               fd = openPort(&readfds,1);
                           }
                           continue;
                        }
                        else if(current_mode==2){//DRINK
                            emit getTouch(101,0); //play drink head
                            videoLock = 0;
                            if(SEP_TP_thread ){
                                end_sep=false;
                                system("sudo rmmod focaltech.ko.xz");
                                system("i2cset -y 1 0x38 0x9E 0X00");
                                fd = openPort(&readfds,1);
                            }
                            continue;
                        }
                     }
                 }

                 //point report in volume button area
                 if(touchPoint.x()>50 && touchPoint.x()<250 && touchPoint.y()>400 && touchPoint.y()<535){
                     if(volume_last_x!=999){
                         difference = (touchPoint.x()-70)/8;
                         //qDebug()<<difference;
                         emit getTouch(999,difference); //change voulme ui
                         //qDebug()<<"Difference:"<<(int)difference<<touchPoint.x()<<touchPoint.y();
                     }
                     volume_last_x=touchPoint.x();
                     continue;
                 }
                 else{
                     volume_last_x=-1;
                 }

                 if(in_circle((double)px,(double)py)){
                         //qDebug()<<px<<","<<py;
                     double angle = get_angle((double)px,(double)py,x2,y2,cx,cy);
                     if(angle>-150&&angle<150)
                     //qDebug()<<std::ceil(angle/16.667);
                     //qDebug()<<get_angle(115,700,167,700,140,655);
                     emit getTouch(888,(int)(angle/16.667));
                 }



                 if( touchLock == 0)
                 {
                     switch(touchStep)
                     {
                     // wait choose food
                         case 2 :
                             //printf("\t [TOUCH] wait choose food\n");

                             if( touchPoint.x()<620 && touchPoint.y()<200 && touchPoint.x()>320 )
                             {
                                 printf("\t [TOUCH] restart\n");
                                 emit getTouch(300,0);
                                 videoLock = 1;
                                 drive_scene=true;
                                 current_mode=1;
                             }

                             drinkKind = foodChoose(touchPoint);


                             //printf("%d\n",drinkKind);
                             if( drinkKind != -1 )
                             {
                                 emit getTouch(10+drinkKind,0);
                                 //printf("\t [TOUCH] food %d\n", drinkKind);
                                 //sep_on
                                 if(SEP_TP_thread){
                                     system("sudo rmmod focaltech.ko.xz");
                                     system("i2cset -y 1 0x38 0x9E 0X01");
                                     fd = openPort(&readfds,1);
                                     qDebug()<<"reset dri"<<fd;
                                 }
                             }
                         break;


                       // wait choose drinks
                     case 102 :
                         //printf("\t [TOUCH] wait choose drink\n");

                         if( touchPoint.x()<620 && touchPoint.y()<200 && touchPoint.x()>320 )
                         {
                             //printf("\t [TOUCH] restart\n");
                             emit getTouch(300,0);
                             videoLock = 1;
                             drive_scene=true;
                             current_mode=1;
                         }

                         drinkKind = drinksChoose(touchPoint);
                         //printf("%d\n",drinkKind);
                         if( drinkKind != -1 )
                         {
                             printf("\t [TOUCH] drink %d\n", drinkKind);
                             emit getTouch(10+drinkKind,0);
                             //sep_on
                             if(SEP_TP_thread){
                             system("sudo rmmod focaltech.ko.xz");
                             system("i2cset -y 1 0x38 0x9E 0X01");
                             fd = openPort(&readfds,1);
                             qDebug()<<"reset dri"<<fd;
                             }
                             //
                         }
                     break;


                     // wait pay or cancel
                         case 1 :
                             printf("\t [TOUCH] pay or cancel\n");
                             vaild = cancelback(touchPoint);
                             if(vaild==1)
                             {
                                 emit getTouch(2,0);
                                 printf("\t [TOUCH] cancel\n");

                                 //sep off
                                 if(SEP_TP_thread){
                                 system("sudo rmmod focaltech.ko.xz");
                                 system("i2cset -y 1 0x38 0x9E 0X00");
                                 fd = openPort(&readfds,1);
                                 qDebug()<<"reset dri"<<fd;
                                 }
                                 //



                             }
                             else if(vaild == 2)
                             {

                                printf("\t [TOUCH] pay\n");
                                //emit  getTouch(20+drinkKind);

                             }
                         break;

                     //-----------------------------------drink cancel
                     case 101 :
                         printf("\t [TOUCH] pay or cancel\n");
                         vaild = cancelback(touchPoint);
                         if(vaild==1)
                         {
                             printf("\t [TOUCH] cancel\n");
                             emit getTouch(102,0);
                             //sep off
                             if(SEP_TP_thread){
                             system("sudo rmmod focaltech.ko.xz");
                             system("i2cset -y 1 0x38 0x9E 0X00");
                             fd = openPort(&readfds,1);
                             }
                             //
                         }

                         /*
                         else if(vaild == 2)
                         {
                            printf("\t [TOUCH] pay\n");
                            emit  getTouch(20+drinkKind);
                         }
                         */
                     break;
                     }
                 }

             }
         }


}

int touchthread ::foodChoose(QPoint tp)
{


    if( chickenElliRegion.contains(tp) || chickenRectRegion.contains(tp))
    {
        return CHICKEN;
    }
    if( friesElliRegion.contains(tp) || friesRectRegion.contains(tp))
    {
        return FRIES;
    }
    if( burgerElliRegion.contains(tp) || burgerRectRegion.contains(tp))
    {
        return BURGER;
    }
    if( onionElliRegion.contains(tp) || onionRectRegion.contains(tp))
    {
        return ONION;
    }
    if( nuggetsElliRegion.contains(tp) || nuggetsRectRegion.contains(tp))
    {
        return NUGGETS;
    }

    return -1;
}



int touchthread ::drinksChoose(QPoint tp)
{

    if( colaElliRegion.contains(tp) || colaRectRegion.contains(tp))
    {
        return COLA;
    }
    if( sodaElliRegion.contains(tp) || sodaRectRegion.contains(tp))
    {
        return SODA;
    }
    if( coffeeElliRegion.contains(tp) || coffeeRectRegion.contains(tp))
    {
        return COFFEE;
    }

    if( orangeElliRegion.contains(tp) || orangeRectRegion.contains(tp))
    {
        return ORANGE;
    }
    if( teaElliRegion.contains(tp) || teaRectRegion.contains(tp))
    {
        return TEA;
    }
    if( waterElliRegion.contains(tp) || waterRectRegion.contains(tp))
    {
        return WATER;
    }


    return -1;
}

int touchthread::cancelback(QPoint tp)
{

    if( cancelRectRegion.contains(tp) )  // cancel button
        return 1;

    if( payElliRegion.contains(tp) )  // pay button
        return 2;

    return 0;
}
