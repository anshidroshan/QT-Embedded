#ifndef READ_ANALYZER_H
#define READ_ANALYZER_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<string.h>
#include <sys/ioctl.h>


int open_AN(void);
int data_onport(void);
int read_data_an(char *ptr);
int count,td;

#endif // READ_ANALYZER_H
