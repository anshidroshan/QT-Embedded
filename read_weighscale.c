#include "read_weighscale.h"

//Weighing Scale Setup

int open_WS(void)
{
    close(fd);
    struct termios newtp;

    fd = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY |O_NDELAY );

    if (fd <0)
    {
        perror("/dev/ttyUSB1");

    }

    fcntl(fd,F_SETFL,0);

    newtp.c_cflag = B9600 | CRTSCTS | CS8 | CLOCAL | CREAD;
    newtp.c_iflag = IGNPAR | ICRNL;
    newtp.c_oflag = 0;
    newtp.c_lflag = ICANON;
    newtp.c_cc[VINTR]    = 0;     /* Ctrl-c */
    newtp.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    newtp.c_cc[VERASE]   = 0;     /* del */
    newtp.c_cc[VKILL]    = 0;     /* @ */
    newtp.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    newtp.c_cc[VMIN]     = 0;     /* blocking read until 1 character arrives */
    newtp.c_cc[VSWTC]    = 0;     /* '\0' */
    newtp.c_cc[VSTART]   = 0;     /* Ctrl-q */
    newtp.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    newtp.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    newtp.c_cc[VEOL]     = 0;     /* '\0' */
    newtp.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    newtp.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    newtp.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    newtp.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    newtp.c_cc[VEOL2]    = 0;     /* '\0' */

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtp);

    return 0;

}

int read_data_ws(void)
{
    int i=0;
    unsigned char buff;
    for(i=0;i<13;i++)
    {
         n =read(fd, &buff,1);
        weight_array[i]=buff;

    }
    weight_array[12]='\0';
    close(fd);

    return 0;

}

int close_WS(void)
{
    close(fd);
}

