//
//  main.c
//  Processes&Threads
//
//  Created by Yadikaer Yasheng on 2/22/16.
//  Copyright © 2016 Yadikaer Yasheng. All rights reserved.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, const char * argv[]) {
    
    pid_t PID = getpid();
    
    // argv validation
    if(argc<1){
        printf("Invalid Arguments. %zu",PID);
        exit(0);
    }

    char* fileName = "/Users/Yadikaer/Projects/OS/Processes&Threads/Processes&Threads/datafile.txt";
    int sideOfChunk = 10;
    
    // open file
    int fileDes = open(fileName,O_RDONLY,S_IREAD);
    if(fileDes == -1){
        printf("failed to open the file. %zu \n");
        return 0;
    }
    
    // get file info
    struct stat fileStat;
    fstat(fileDes,&fileStat);
    
    // calculate how many child processes we need
    int childProcessNum = (int)fileStat.st_size / sideOfChunk;
    if (fileStat.st_size % sideOfChunk != 0) {
        childProcessNum++;
    }

    printf("Parent Process %zu: File %s contains %d bytes.\n", PID,fileName,(int)fileStat.st_size);
    printf("Parent Process %zu: %d child processes will be forked.\n",PID,childProcessNum);
  
    // create child process
    for (int i = 0; i < childProcessNum; i++) {
        int forkResult = fork();
        
        if(forkResult == -1){
            printf("fork failed.\n");
            exit(0);
        }
        else if (forkResult == 0){
            char buffer[sideOfChunk];
            size_t bytesToRead;
            ssize_t bytesRead;
            bytesToRead = sizeof(buffer);
            bytesRead = read(fileDes, buffer, bytesToRead);
            
            printf("Child Process %zu PPID %zu:<%s> \n",getpid(),getppid(),buffer);
          
            exit(0);
        }
        else{
            wait(&forkResult);
        }
    }
    
    return 0;
}
