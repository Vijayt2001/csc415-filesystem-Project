/**************************************************************
* Class:  CSC-415-01 Summer 2023
* Names: Vijayraj Tolnoorkar, Phillip Ma, Janvi Patel, Leo Wu
* Student IDs: 922110069, 920556972, 917944864, 917291133
* GitHub Name:
* Group Name: Vancouver
* Project: Basic File System
*
* File: fsInit.c
*
* Description: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
*
**************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "fsLow.h"
#include "mfs.h"

#define MAGIC_NUMBER 0xf0f03410


void cleanUpMemory(char **buffer, vcb **vcbVar, uint64_t **freeSpace, fdDir **directory);
int VCB(uint64_t, uint64_t, uint64_t);
int freeSpace();
int rootDirectory();

int configureFileSystem(uint64_t blockAmount, uint64_t blockSize) {
    printf("Setting up File System with %ld blocks, each of size %ld\n", blockAmount, blockSize);

    uint64_t vcbBlocks = (sizeof(vcb) + blockSize - 1) / blockSize;

    char *readBuffer = calloc(vcbBlocks, blockSize);
    if (!readBuffer) return -1;

    LBAread(readBuffer, vcbBlocks, 0);

    ourVCB = calloc(1, sizeof(vcb));
    if (!ourVCB) return cleanUpMemory(&readBuffer, NULL, NULL, NULL), -1;

    memcpy(ourVCB, readBuffer, sizeof(vcb));

    free(readBuffer); readBuffer = NULL;

    // any one here??????



    if (MAGIC_NUMBER == ourVCB->VCB) {

        readBuffer = calloc(ourVCB->freespaceBlockCount, ourVCB->blockSize);
        if (!readBuffer) return cleanUpMemory(NULL, &ourVCB, NULL, NULL), -1;

        LBAread(readBuffer, ourVCB->freespaceBlockCount, ourVCB->freespaceBlockCount);

        freespace = calloc(1, ourVCB->numberOfBlocks);
        if (!freespace) return cleanUpMemory(&readBuffer, &ourVCB, NULL, NULL), -1;

        memcpy(freespace, readBuffer, ourVCB->numberOfBlocks);

        free(readBuffer); readBuffer = NULL;

        readBuffer = calloc(getBlockCount(sizeof(fdDir)), ourVCB->blockSize);
        if (!readBuffer) return cleanUpMemory(NULL, &ourVCB, &freespace, NULL), -1;

        LBAread(readBuffer, getBlockCount(sizeof(fdDir)), ourVCB->rootDirectory);

        fsCWD = calloc(1, sizeof(fdDir));
        if (!fsCWD) return cleanUpMemory(&readBuffer, &ourVCB, &freespace, NULL), -1;

        memcpy(fsCWD, readBuffer, sizeof(fdDir));

        free(readBuffer); readBuffer = NULL;
    }
    else {
        if (VCB(blockAmount, blockSize, vcbBlocks) 
            || freeSpace() 
            || rootDirectory()) return -1;

        updateOurVCB();
    }

    return 0;
}

void exitFileSystem() {
    printf("System exiting\n");
}

void cleanUpMemory(char **buffer, vcb **vcbVar, uint64_t **freeSpace, fdDir **directory) {
    if (buffer && *buffer) { free(*buffer); *buffer = NULL; }
    if (vcbVar && *vcbVar) { free(*vcbVar); *vcbVar = NULL; }
    if (freeSpace && *freeSpace) { free(*freeSpace); *freeSpace = NULL; }
    if (directory && *directory) { free(*directory); *directory = NULL; }
}

int VCB(uint64_t blockAmount, uint64_t blockSize, uint64_t vcbBlocks) {
    memset(ourVCB, 0, sizeof(vcb));

    ourVCB->VCB = MAGIC_NUMBER;
    ourVCB->numberOfBlocks = blockAmount;
    ourVCB->blockSize = blockSize;
    ourVCB->freespaceBlockCount = vcbBlocks;

    uint64_t bytes = (blockAmount + 7) / 8; // Round up to nearest byte.
    ourVCB->freespaceBlockCount = (bytes + blockSize - 1) / blockSize; // Round up to nearest block.

    return 0;
}

int freeSpace() {
    freespace = calloc(1, ourVCB->numberOfBlocks);
    if (!freespace) return -1;
    return 0;
}

int prepareRootDirectory() {
    fdDir *newDirectory = createDirectory(NULL, "/");
    if (!newDirectory) return -1;

    updateDirectory(newDirectory);

    fsCWD = newDirectory;
    ourVCB->rootDirectory = fsCWD->directoryStartLocation;
    return 0;
}
