
/**************************************************************
* Class:  CSC-415-01 Summer 2023
* Names: Vijayraj Tolnoorkar, Phillip Ma, Janvi Patel, Leo Wu
* Student IDs: 922110069, 920556972, 917944864, 917291133
* GitHub Name:
* Group Name: Vancouver
* Project: Basic File System
*
* File: mfs.c
*
* Description:The file contians functions and logic to interacct witth the file system.
*
**************************************************************/


#include "mfs.h"
#include "b_io.h"


#define MAX_FILES 1024
#define MAX_FILEPATH_SIZE 1024
#define MAX_DIRECTORY_DEPTH 256
#define MAX_FILENAME_SIZE 256

typedef struct {
    char* path;
    fdDir* dir;
} file_entry;

file_entry file_table[MAX_FILES];
// Hi I do not have access to edit the file and terminal 
void initialize_file_table() {
    for (int i = 0; i < MAX_FILES; i++) {
        file_table[i].path = NULL;
        file_table[i].dir = NULL;
    }
}

file_entry* get_file_entry(char* path) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].path != NULL && strcmp(file_table[i].path, path) == 0) {
            return &file_table[i];
        }
    }
    return NULL;
}

void add_file_entry(char* path, fdDir* dir) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].path == NULL) {
            file_table[i].path = strdup(path);
            file_table[i].dir = dir;
            break;
        }
    }
}


// holding our current directory's path
char currentDirectoryPath[MAX_FILEPATH_SIZE];
char currentDirectoryPathArray[MAX_DIRECTORY_DEPTH][MAX_FILENAME_SIZE];
int currentDirectoryPathArraySize = 0;

// hold each level of a path
char requestedFilePath[MAX_FILEPATH_SIZE];
char requestedFilePathArray[MAX_DIRECTORY_DEPTH][MAX_FILENAME_SIZE];
int requestedFilePathArraySize = 0;

char * parseFilePath(const char *pathname)
{
    // clear previous arrays
    requestedFilePath[0] = '\0';
    requestedFilePathArraySize = 0;

    /*make mutable copy of pathname*/
    char * pathnameCopy = strdup(pathname);

    // setup tokenizer
    char *token = strtok(pathnameCopy, "/");

    // create pathnames
    int isAbsolute = pathname[0] == '/';
    int isSelfRelative = !strcmp(token, ".");
    int isParentRelative = !strcmp(token, "..");

    if (token && !isAbsolute)
    {
        int max = isParentRelative ? currentDirectoryPathArraySize - 1 : currentDirectoryPathArraySize;
        for (int i = 0; i < max; i++)
        {
            strcpy(requestedFilePathArray[i], currentDirectoryPathArray[i]);
            strcat(requestedFilePathArray[i], "/");
            strcat(requestedFilePathArray[i], token);
            requestedFilePathArraySize++;
        }
    }

    /* Discard '.' or '..'. */
    if (isSelfRelative || isParentRelative)
    {
        token = strtok(NULL, "/");
    }

    while (token && requestedFilePathArraySize < MAX_DIRECTORY_DEPTH)
    {
        strcpy(requestedFilePathArray[requestedFilePathArraySize], token);
        strcat(requestedFilePathArray[requestedFilePathArraySize], "/");
        requestedFilePathArraySize++;
        token = strtok(NULL, "/");

        // printf for debugging
        printf("\t%s\n", requestedFilePathArray[requestedFilePathArraySize]);
    }

    printf("Output: %s\n", requestedFilePath);
    free(pathnameCopy);
    return requestedFilePath;
}

int fs_mkdir(const char *pathname, mode_t mode)
{
    // return 0 for sucsess and -1 if not
    printf("fs_mkdir\n");

    // Parses file name, adds info for inode fdDir
    // Adds info to parent if necessary and checks if the folder already exists
    char *parentPath = parseFilePath(pathname);
    if (!parentPath)
    {
        return -1;
    }

    fdDir * parentDir = fs_opendir(parentPath);

    char *childName = requestedFilePathArray[MAX_FILEPATH_SIZE - 1];

 
    // Use strtok_r() to parse the path once, and then use strstr() to check if the directory already exists.
    // This avoids the need to iterate through the parent directory's children list twice.
    char *token = strtok_r(parentPath, "/", &parentPath);
    while (token)
    {
        if (strstr(token, childName))
        {
            printf("Folder already exists!\n");

            return -1;
        }

        token = strtok_r(NULL, "/", &parentPath);
    }


    return 0;
}

fdDir *fs_opendir(const char *fileName)
{
    int ret = b_open((char *)fileName, 0);
    if (ret < 0)
    {
        return NULL;
    }
    
    fdDir *dir = malloc(sizeof(fdDir));
    dir->d_reclen = 0;
    dir->directoryStartLocation = ret;
    dir->dirEntryAmount = 0;
    strcpy(dir->dirName, fileName);

    return dir;
}

int fs_closedir(fdDir *dirp)
{
    if (dirp == NULL)
    {
        return -1;
    }
    free(dirp);
    return 0;
}

int fs_stat(const char *path, struct fs_stat *buf)
{
    fdDir *dir = fs_opendir(path);
    if (dir == NULL)
    {
        return -1;
    }
    
    for (int i = 0; i < MAX_AMOUNT_OF_ENTRIES; i++)
    {
        
        buf->st_size = openedDir->entryList[i].size;
        buf->st_blksize = ourVCB->blockSize;
        buf->st_blocks = ((openedDir->entryList[i].size + ourVCB->blockSize - 1) / ourVCB->blockSize);
        buf->st_accesstime = openedDir->entryList[i].last_access_time;
        buf->st_modtime = openedDir->entryList[i].modified_time;
        buf->st_createtime = openedDir->entryList[i].create_time;
    }

    free(dir);
    return 0;
}

int fs_isDir(char *pathname)
{
    fdDir *dir = fs_opendir(pathname);
    if (dir == NULL)
    {
        return -1;
    }
    free(dir);
    return 0;
}


char *fs_getcwd(char *buf, size_t size) {
    // Allocating an array to hold directory names as we traverse the hierarchy
    const int max_dirs = 128;  // Assume a reasonable limit to the directory depth
    char *dirs[max_dirs];
    int dir_count = 0;

    // Start with the current working directory
    fdDir *currentDir = fsCWD;

    // Traverse the directory hierarchy up to the root, storing each directory name
    while (currentDir->directoryStartLocation != ourVCB->rootDirectory && dir_count < max_dirs) {
        dirs[dir_count++] = strdup(currentDir->dirName);
        currentDir = getDirByEntry(currentDir->entryList + 1);  // Get the parent directory
    }

    // Construct the full path from root to cwd by popping directory names off the stack
    strcpy(buf, ".");
    while (dir_count > 0) {
        // Check that the new directory name will fit in the buffer, including the '/' separator and null terminator
        if (strlen(buf) + strlen(dirs[dir_count - 1]) + 2 > size) {
            for (int i = 0; i < dir_count; i++) {
                free(dirs[i]);  // Free any directory names we've duplicated
            }
            return NULL;  // The buffer isn't big enough
        }
        
        strcat(buf, "/");
        strcat(buf, dirs[--dir_count]);
        free(dirs[dir_count]);  // We're done with this directory name
    }

    return buf;
}

int fs_setcwd(char *buf)
{
    // Check if the provided buffer is not NULL
    if (buf == NULL)
    {
        dprintf("Invalid buffer: NULL\n");
        return -1;
    }

    // Get the directory to set as the current working directory
    fdDir *newCWD = getDirByPath(buf);
    if (newCWD == NULL)
    {
        dprintf("Unable to get directory by path: %s\n", buf);
        return -1;
    }

    dprintf("Previous fsCWD: %s\n", fsCWD->dirName);

    // Allocate a temporary pointer to hold the current working directory
    fdDir *oldCWD = fsCWD;

    // Set the current working directory to the new directory
    fsCWD = newCWD;

    // Now that we've successfully updated fsCWD, deallocate the old working directory
    free(oldCWD);

    dprintf("Current fsCWD: %s\n", fsCWD->dirName);

    return 0;
}

int fs_isFile(char *path) {
    file_entry* entry = get_file_entry(path);
    if (entry != NULL) {
        return 1; // File exists
    } else {
        return 0; // File does not exist
    }
}

struct fs_diriteminfo *fs_readdir(fdDir *dirp)
{
    if (dirp == NULL) // Check if the directory is valid
    {
        return NULL;
    }
    // Check if we've reached the end of the directory
    if (dirp->entryList[openedDirEntryIndex].fileType == 0)
    {
        return NULL;
    }
    // Create a new directory item info struct
    struct fs_diriteminfo *dirItemInfo = malloc(sizeof(struct fs_diriteminfo));
    // Populate the directory item info struct
    dirItemInfo->d_reclen = sizeof(struct fs_diriteminfo);
    dirItemInfo->fileType = dirp->entryList[openedDirEntryIndex].fileType;
    dirItemInfo->size = dirp->entryList[openedDirEntryIndex].size;
    strcpy(dirItemInfo->d_name, dirp->entryList[openedDirEntryIndex].d_name);

    // Increment the opened directory entry index
    openedDirEntryIndex++;
    return dirItemInfo;
}



int fs_delete(char *filename)
{
    // search for the file entry
    file_entry *entry = get_file_entry(filename);
}



int fs_rmdir(const char *pathname)
{
    fdDir *dir = fs_opendir(pathname); // Open the directory
    if (dir == NULL)                  // Check if the path is valid
    {
        return -1;
    }
    if (dir->entryList[openedDirEntryIndex].fileType != 1) // Check if the path is a directory
    {
        return -1;
    }
    if (dir->entryList[openedDirEntryIndex].size != 0) // Check if the directory is empty
    {
        return -1;
    }
    
    int ret = b_delete(pathname); // Delete the directory
    if (ret < 0)                  // Check if the directory was deleted successfully
    {
        return -1;
    }
    return 0; // Directory was deleted successfully
}
