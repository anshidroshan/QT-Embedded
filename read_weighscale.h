#ifndef READ_WEIGHSCALE_H
#define READ_WEIGHSCALE_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<string.h>
#include <sys/ioctl.h>


int open_WS(void);
int data_onport2(void);
int read_data_ws(void);
char weight_array[100];
int fd,n;

#endif // READ_WEIGHSCALE_H
