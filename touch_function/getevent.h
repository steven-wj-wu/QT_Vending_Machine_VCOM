#ifndef GETEVENT_H
#define GETEVENT_H

extern "C"
{
    #include <unistd.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <fcntl.h>
    #include <limits.h>
    #include <sys/types.h>
    #include <sys/stat.h>

    #include <linux/input.h>

    typedef struct struct_point{
        unsigned x;
        unsigned y;
    } Point;


    int openPort(fd_set* readfds,bool set_driver);
    void touchEvent(int fd, fd_set *readfds, int *px, int *py,bool *b_break);

}

#endif // GETEVENT_H
