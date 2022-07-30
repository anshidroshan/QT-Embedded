#include "read_analyzer.h"

//Analyzer Setup

int open_AN(void)
{
    struct termios my_termios;
    int Baudrate=0;

    memset(&my_termios, 0, sizeof(struct termios));
    Baudrate= B9600;
    td = open("/dev/ttyUSB0",O_RDWR|O_NOCTTY|O_NONBLOCK);
    if( td < 0 || Baudrate ==0 ) {  fprintf(stderr,"Unable to open USB port \n Please check the Arguments\n"); return -1; }
    tcflush(td,TCIFLUSH);
    my_termios.c_cflag=Baudrate|CS8|CREAD;//|PARENB;
    my_termios.c_cc[VMIN]=0;
    my_termios.c_cc[VTIME]=0;
    tcsetattr(td,TCSANOW,&my_termios);

    return 0;
}

int data_onport(void)
{
    int ret=-1,count = 0;
    ret = ioctl(td,FIONREAD,&count);
    if (ret != 0)
        return -1;
    return count;
}

int read_data_an(char *ptr)
{
    int read_bytes=0;
    read_bytes = read(td,ptr,1024);
    close(td);
    return read_bytes;
}

