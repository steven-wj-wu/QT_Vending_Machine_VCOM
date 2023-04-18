#include "videothread.h"



void videothread :: onVideoChange(int video)
{

    static QProcess *proc_video = new QProcess(this);
    static QProcess *proc_video2 = new QProcess(this);
    static QProcess *proc_video3 = new QProcess(this);
    static QProcess *proc_mpl = new QProcess(this); //choose and finishvideo
    static QProcess *proc_mpl_head = new QProcess(this); //drive video
    static QProcess *proc_mpl_food = new QProcess(this); //food  video
    static QProcess *proc_mpl_drink = new QProcess(this); //drink  video
    static QProcess *tp_driver = new QProcess(this);

    if(initial){
    proc_mpl->start("mplayer -slave -quiet -idle -really-quiet -noborder -osdlevel 0 -geometry 1280x720+320-0");
    proc_mpl_head->start("mplayer -slave -quiet -idle -really-quiet  -noborder -osdlevel 0 -geometry 1600x720+320-0");
    proc_mpl_drink->start("mplayer -slave -quiet -idle -really-quiet  -noborder -osdlevel 0 -geometry 1280x720+320-0");
    proc_mpl_food->start("mplayer -slave -quiet -idle -really-quiet  -noborder -osdlevel 0 -geometry 1280x720+320-0");
    }

    playFinished = 0;

    switch(video){

        case 0:
            //proc_video->start("ffplay -an -sn -hide_banner -fast -nostats -alwaysontop -x 1280 -y 720 -left 320 -top -30 -codec:v h264_v4l2m2m /home/steven/qt_widget/food/head_food.mp4 -loop 0");
            //proc_video->start("mplayer -noborder -ontop -loop 0 -geometry  320:0 -x 1280 -y 720 /home/steven/qt_widget/food/head_food.mp4");
            proc_mpl_food->write("loadfile /home/steven/qt_widget/food/head_food.mp4\n");
            proc_mpl_food->write("switch_ratio 1.77778 \n"); //16:9 scale size
            proc_mpl_food->write("vo_ontop 1\n");
            proc_mpl_food->write("loop 1\n");
        break;

        case 1001:
        proc_mpl_food->write("stop\n");
         break;


        case 1:
                /*
                printf("video1 state: %u\n", proc_video->state());
                if(proc_video->state() != 0)
                {
                    proc_video->write("q");
                    proc_video->waitForStarted();
                    proc_video->close();
                }
                */
                proc_mpl->write("stop\n");

        break;


        case 2:
                printf("video2 state: %u\n", proc_video2->state());
                if(proc_video2->state() != 0)
                {
                    proc_video2->write("q");
                    proc_video2->waitForStarted();
                    proc_video2->close();
                    printf("start video close");
                }
        break;


        //wait pay video
        case 20:
                //proc_video->start("mplayer -noborder -ontop -geometry  320:-30 -x 1280 -y 720 /home/steven/qt_widget/food/chicken_buy.mp4");
                //proc_video->waitForStarted();
                //msleep(1000);
                proc_mpl->write("loadfile /home/steven/qt_widget/food/chicken_buy.mp4\n");
                proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
                proc_mpl->write("vo_ontop 1\n");
                playFinished = 1;
            break;

        case 21:
               // proc_video->start("mplayer -noborder -ontop -geometry  320:-30 -x 1280 -y 720  /home/steven/qt_widget/food/fries_buy.mp4");
                //proc_video->start("ffplay -an -sn -hide_banner -fast -nostats -alwaysontop -autoexit -x 1280 -y 720 -left 320 -top -30 -codec:v h264_v4l2m2m /home/steven/qt_widget/food/fries_buy.mp4");
                //proc_video->waitForStarted();
                //msleep(1000);
                proc_mpl->write("loadfile /home/steven/qt_widget/food/fries_buy.mp4\n");
                proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
                proc_mpl->write("vo_ontop 1\n");
                playFinished = 1;
            break;

        case 22:
                //proc_video->start("mplayer -noborder -ontop -geometry  320 :-30 -x 1280 -y 720  /home/steven/qt_widget/food/burger_buy.mp4");
                //proc_video->waitForStarted();
                //msleep(1000);
                proc_mpl->write("loadfile /home/steven/qt_widget/food/burger_buy.mp4\n");
                proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
                proc_mpl->write("vo_ontop 1\n");

                playFinished = 1;
            break;

        case 23:
            //proc_video->start("mplayer -noborder -ontop -geometry  320:-30 -x 1280 -y 720 /home/steven/qt_widget/food/onion_buy.mp4");
            //proc_video->waitForStarted();
            //msleep(1000);
                proc_mpl->write("loadfile /home/steven/qt_widget/food/onion_buy.mp4\n");
                proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
                proc_mpl->write("vo_ontop 1\n");
            playFinished = 1;
        break;

        case 24:
            //proc_video->start("mplayer -noborder -ontop -geometry  320:-30 -x 1280 -y 720  /home/steven/qt_widget/food/nuggets_buy.mp4");
            //proc_video->waitForStarted();
            //msleep(1000);
                proc_mpl->write("loadfile /home/steven/qt_widget/food/nuggets_buy.mp4\n");
                proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
                proc_mpl->write("vo_ontop 1\n");
            playFinished = 1;
        break;

        //pay finished video
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
            //proc_video2->start("mplayer -noborder -ontop -geometry  320 :0 -x 1280 -y 720  /home/steven/qt_widget/food/finish.mp4");
            //proc_video2->waitForStarted();
            //msleep(2000);
                proc_mpl->write("loadfile /home/steven/qt_widget/food/finish.mp4\n");
                proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
                proc_mpl->write("vo_ontop 1\n");
            playFinished = 1;
        break;


        case 100:
        //proc_video->start("mplayer -noborder -ontop -loop 0 -geometry  320:0 -x 1280 -y 720 /home/steven/qt_widget/drink/head_drink.mp4");
        proc_mpl_drink->write("loadfile /home/steven/qt_widget/drink/head_drink.mp4\n");
        proc_mpl_drink->write("switch_ratio 1.77778 \n"); //16:9 scale size
        proc_mpl_drink->write("vo_ontop 1\n");
        proc_mpl_drink->write("loop 1\n");
        break;

        case 103:
        proc_video2->start("mplayer -noborder -ontop -loop 0 -geometry  320:0 -x 1280 -y 720 /home/steven/qt_widget/drink/head_drink.mp4 ");
        break;



        //wait pay video
        case 120:
                //proc_video->start("mpv --force-seekable=yes --untimed --really-quiet --no-ytdl --no-audio --no-border --no-input-cursor --geometry=1280x720+320-0  --no-keepaspect /home/steven/qt_widget/drink/COLA-1.mp4");
                //proc_video->start("mplayer -vo xv -really-quiet -noborder -ontop -geometry 1280x720+320-0  /home/steven/qt_widget/drink/COLA-1.mp4");
                //proc_video->start("ffplay -an -noborder -x 1280 -y 720 -left  320  -autoexit  /home/steven/qt_widget/drink/COLA-1.mp4");
                //proc_video->waitForStarted();
                //playFinished = 1;
                proc_mpl->write("loadfile /home/steven/qt_widget/drink/COLA-1.mp4\n");
                proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
                proc_mpl->write("vo_ontop 1\n");
                playFinished = 1;

            break;

        case 121:
                //proc_video->start("mplayer -noborder -ontop -geometry  320:0 -x 1280 -y 720 /home/steven/qt_widget/drink/SODA-1.mp4");
                //proc_video->waitForStarted();
                //msleep(800);
                //playFinished = 1;
                proc_mpl->write("loadfile /home/steven/qt_widget/drink/SODA-1.mp4\n");
                proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
                proc_mpl->write("vo_ontop 1\n");
                playFinished = 1;

            break;

        case 122:
                //proc_video->start("mplayer -noborder -ontop -geometry  320:0 -x 1280 -y 720  /home/steven/qt_widget/drink/PREMIUMCOFFEE-1.mp4");
                //proc_video->waitForStarted();
                //msleep(1000);
                proc_mpl->write("loadfile /home/steven/qt_widget/PREMIUMCOFFEE-1.mp4\n");
                proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
                proc_mpl->write("vo_ontop 1\n");
                playFinished = 1;
            break;

    case 123:
            //proc_video->start("mplayer -noborder -ontop -geometry  320:0 -x 1280 -y 720  /home/steven/qt_widget/drink/ORANGEJUICE-1.mp4");
            //proc_video->waitForStarted();
            //msleep(1000);
            proc_mpl->write("loadfile /home/steven/qt_widget/ORANGEJUICE-1.mp4\n");
            proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
            proc_mpl->write("vo_ontop 1\n");
            playFinished = 1;
        break;

    case 124:
            //proc_video->start("mplayer -noborder -ontop -geometry  320:0 -x 1280 -y 720  /home/steven/qt_widget/drink/GREENTEA-1.mp4");
            //proc_video->waitForStarted();
            //msleep(1000);
            proc_mpl->write("loadfile /home/steven/qt_widget/GREENTEA-1.mp4\n");
            proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
            proc_mpl->write("vo_ontop 1\n");
            playFinished = 1;
        break;

    case 125:
            //proc_video->start("ffplay -an -x 1280 -y 720 -left  320  -top -30 -autoexit  /home/steven/qt_widget/drink/MINERALWATER-1.mp4");
            //proc_video->start("mplayer -noborder -ontop -geometry  320:0 -x 1280 -y 720  /home/steven/qt_widget/drink/MINERALWATER-1.mp4");
            //proc_video->waitForStarted();
            //msleep(2000);
            proc_mpl->write("loadfile /home/steven/qt_widget/MINERALWATER-1.mp4\n");
            proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
            proc_mpl->write("vo_ontop 1\n");
            playFinished = 1;
        break;


    case 130:
        //proc_video2->start("mplayer -noborder -ontop -geometry  320:0 -x 1280 -y 720 /home/steven/qt_widget/drink/COLA-2.mp4");
        //proc_video2->waitForStarted();
        //msleep(3500);
        proc_mpl->write("loadfile /home/steven/qt_widget/COLA-2.mp4\n");
        proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
        proc_mpl->write("vo_ontop 1\n");
        playFinished = 1;
        break;
    case 131:
        //proc_video2->start("mplayer -noborder -ontop -geometry  320:0 -x 1280 -y 720 /home/steven/qt_widget/drink/SODA-2.mp4");
        //proc_video2->waitForStarted();
        //msleep(3500);
        proc_mpl->write("loadfile /home/steven/qt_widget/SODA-2.mp4\n");
        proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
        proc_mpl->write("vo_ontop 1\n");
        playFinished = 1;
        break;
    case 132:
        //proc_video2->start("mplayer -noborder -ontop -geometry  320:0 -x 1280 -y 720  /home/steven/qt_widget/drink/PREMIUMCOFFEE-2.mp4");
        //proc_video2->waitForStarted();
        //msleep(3500);
        proc_mpl->write("loadfile /home/steven/qt_widget/PREMIUMCOFFEE-2.mp4\n");
        proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
        proc_mpl->write("vo_ontop 1\n");
        playFinished = 1;
        break;
    case 133:
        //proc_video2->start("mplayer -noborder -ontop -geometry  320:0 -x 1280 -y 720  /home/steven/qt_widget/drink/ORANGEJUICE-2.mp4");
        //proc_video2->waitForStarted();
        //msleep(3500);
        proc_mpl->write("loadfile /home/steven/qt_widget/ORANGEJUICE-2.mp4\n");
        proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
        proc_mpl->write("vo_ontop 1\n");
        playFinished = 1;
        break;
    case 134:
        //proc_video2->start("mplayer -noborder -ontop -geometry  320:0 -x 1280 -y 720  /home/steven/qt_widget/drink/GREENTEA-2.mp4");
        //proc_video2->waitForStarted();
        //msleep(3500);
        proc_mpl->write("loadfile /home/steven/qt_widget/GREENTEA-2.mp4\n");
        proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
        proc_mpl->write("vo_ontop 1\n");
        playFinished = 1;
        break;
    case 135:
        //proc_video2->start("mplayer -noborder -ontop -geometry  320:0 -x 1280 -y 720  /home/steven/qt_widget/drink/MINERALWATER-2.mp4");
        //proc_video2->waitForStarted();
        //msleep(3500);
        proc_mpl->write("loadfile /home/steven/qt_widget/MINERALWATER-2.mp4\n");
        proc_mpl->write("switch_ratio 1.77778 \n"); //16:9 scale size
        proc_mpl->write("vo_ontop 1\n");
        playFinished = 1;
        break;


     //navigation video
     case 300:
        proc_mpl_head->write("loadfile /home/steven/qt_widget/navi_Trim.mp4\n");
        proc_mpl_head->write("switch_ratio 2.22222 \n"); //16:9 scale size
        proc_mpl_head->write("vo_ontop 1\n");
        proc_mpl_head->write("loop 1\n");
//Hide Food Head in Back
        proc_mpl_food->write("loadfile /home/steven/qt_widget/food/head_food.mp4\n");
        proc_mpl_food->write("switch_ratio 1.77778 \n"); //16:9 scale size
        proc_mpl_food->write("pause 1\n");
        break;

    case 301:
       proc_mpl_head->write("stop 0\n");
       //show food head
       proc_mpl_food->write("vo_ontop 1\n");
       proc_mpl_food->write("loop 1\n");
       break;

     case 101:
              proc_mpl_drink->write("stop 0\n");
              break;

    }

}

