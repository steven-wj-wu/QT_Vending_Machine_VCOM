#include "getevent.h"
#include <stdio.h>
#include <cmath>
#include <unistd.h>
/*Note: If you couldn't build this source code successfully, it may caused by the old kernel issue.
Please enable below define depending on your kernel version */

//#define KERNEL_2_6_35_DOWNWARD	/* As kernel <= 2.6.35 */
//#define KERNEL_2_6_32_DOWNWARD	/* As kernel <= 2.6.32 */
//#define KERNEL_2_6_29_DOWNWARD	/* As kernel <= 2.6.29 */
bool in_circle(double x1,double y1);

void ParseEvent( input_event ev, int *bTouch, int *position_x, int *position_y)
{

    if( ev.type == EV_KEY ){
        switch( ev.code ){
            case BTN_TOUCH:
                *bTouch = ev.value;
                break;

            //Mouse Event--------------------------------
            case BTN_LEFT: //click mouse left
                 if(ev.value==0){
                   *bTouch = ev.value;
                 }
                break;


            case BTN_RIGHT:
                if(ev.value==0){
                    *bTouch = ev.value;
                    *position_x = 509;
                    *position_y = 371;
                    break;
                }
            case BTN_MIDDLE:
            if(ev.value==0){
                *bTouch = ev.value;
                *position_x = 386;
                *position_y = 612;
                break;
            }

            default:
                break;
        }

    }else if( ev.type == EV_ABS ) {
        switch( ev.code ) {
            case ABS_MT_POSITION_X:
                *position_x = ev.value;
                break;
            case ABS_MT_POSITION_Y:
                *position_y = ev.value;
                break;
            default:
                break;
        }
    }

    //Mosue Event
}



int openPort(fd_set *readfds,bool set_driver)
{

    //open drive
    if(set_driver){
         system("sudo insmod /lib/modules/5.15.92-v7l+/kernel/drivers/input/touchscreen/focaltech_touch/focaltech.ko.xz");
         sleep(1);
    }


    int fd;
    //use path to find certain device name
    char strPort[100] = "/dev/input/by-path/platform-fe804000.i2c-event";
    fd = open(strPort, O_RDONLY );	//open():contact to device, O_RDONLY:only read, if(fd==-1) is error.
    if(fd>=0)
    {
        FD_ZERO( readfds );		//clear fd_set, there doesn't have any fd in set.
        FD_SET( fd , readfds );    	//add fd in fd_set.
    }

    return fd;
}

void touchEvent(int fd, fd_set *readfds, int *px, int *py,bool *b_break)
{
    int bTouch=-1, position_x=-1, position_y=-1, size;
    input_event ev;

    while( bTouch != 0)
    {
        if(*b_break){
            *b_break=false;
            break;
        }
        if( select( fd+1, readfds, NULL, NULL, NULL ) > 0 ) { //test fd has event to process?

            //printf("fd has event \n");
            size = read( fd, &ev, sizeof( struct input_event ));
            if( size >= sizeof( struct input_event ) ) {
                ParseEvent( ev, &bTouch, &position_x, &position_y);

                //get severial point repot with button area
                if((int)position_x>50&&(int)position_x<250&&(int)position_y>400&&(int)position_y<535&&(int)position_x!=-1){
                    break;
                }
                else if(in_circle((double)position_x,(double)position_y)){
                    break;
                }
            }
        }
    }

    *px = (int)position_x; //*0.388;
    *py = (int)position_y; //*0.387;
    return;
}

bool in_circle(double x1,double y1){
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
