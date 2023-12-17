/**************************************************************
* Class: CSC-415-01  Summer 2023
* Names: Vijayraj Tolnoorkar, Phillip Ma, Janvi Patel, Leo Wu
* Student IDs: 922110069, 920556972, 917944864, 917291133
* GitHub Name:
* Group Name: Vancouver
* Project: Basic File System
*
* File: b_io.h
*
* Description: Interface of basic I/O functions
*
**************************************************************/

#ifndef _B_IO_H
#define _B_IO_H
#include <fcntl.h>

int b_open(char *filename, int flags);
int b_read(int argfd, char *buffer, int count);
int b_write(int argfd, char *buffer, int count);
void b_close(int argfd);
void writeIntoVolume(int argfd);

#endif
