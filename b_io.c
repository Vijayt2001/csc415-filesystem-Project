
/**************************************************************
* Class:  CSC-415-01 Summer 2023
* Names: Vijayraj Tolnoorkar, Phillip Ma, Janvi Patel, Leo Wu
* Student IDs: 922110069, 920556972, 917944864, 917291133
* GitHub Name:
* Group Name: Vancouver
* Project: Basic File System
*
/* File: b_io.c
*
* Description: Basic File System - Key File I/O Operations
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "b_io.h"

#define MAXFCBS 20
#define B_CHUNK_SIZE 512

typedef struct b_fcb
{
    char *buf;  // holds the open file buffer
    int index;  // holds the current position in the buffer
    int buflen; // holds how many valid bytes are in the buffer
    int fd;     // holds file descriptor
} b_fcb;

typedef int b_io_fd; // define the b_io_fd type

b_fcb fcbArray[MAXFCBS];

int startup = 0; // Indicates that this has not been initialized

// Method to initialize our file system
void b_init()
{
    // init fcbArray to all free
    for (int i = 0; i < MAXFCBS; i++)
    {
        fcbArray[i].buf = NULL; // indicates a free fcbArray
    }

    startup = 1;
}

// Method to get a free FCB element
b_io_fd b_getFCB()
{
    for (int i = 0; i < MAXFCBS; i++)
    {
        if (fcbArray[i].buf == NULL)
        {
            return i; // Not thread safe (But do not worry about it for this assignment)
        }
    }
    return (-1); // all in use
}

int b_open(char *filename, int flags)
{
    b_io_fd returnFd;

    if (startup == 0)
        b_init(); // Initialize our system

    returnFd = b_getFCB(); // get our own file descriptor

    return (returnFd);
}

int b_seek(b_io_fd fd, off_t offset, int whence)
{
    if (startup == 0)
        b_init(); // Initialize our system

    if ((fd < 0) || (fd >= MAXFCBS))
    {
        return (-1); // invalid file descriptor
    }

    return (0); // Change this
}

int b_write(b_io_fd fd, char *buffer, int count)
{
    if (startup == 0)
        b_init(); // Initialize our system

    if ((fd < 0) || (fd >= MAXFCBS))
    {
        return (-1); // invalid file descriptor
    }

    return (0); // Change this
}

int b_read(b_io_fd fd, char *buffer, int count)
{
    if (startup == 0)
        b_init(); // Initialize our system

    if ((fd < 0) || (fd >= MAXFCBS))
    {
        return (-1); // invalid file descriptor
    }

    return (0); // Change this
}

void b_close(b_io_fd fd)
{
    if (startup == 0)
        b_init(); // Initialize our system

    if ((fd < 0) || (fd >= MAXFCBS))
    {
        return; // invalid file descriptor
    }
}
