//
//  main.c
//  Threads&Scheduling
//
//  Created by Yadikaer Yasheng on 3/3/16.
//  Copyright © 2016 Yadikaer Yasheng. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

struct Account{
    int balance;
};

struct Transaction{
    int accNumFrom;
    int accNumTo;
    int amount;
    int timeRequired;
};

pthread_mutex_t mutex;
sem_t sem;

struct Account* accounts = NULL;
struct Transaction* transactions = NULL;
int numWorker = 0;
int numAccount = 0;
int numTrans = 0;
int currentJobIndex = 0;

int comp (const void * elem1, const void * elem2)
{
    struct Transaction f = *((struct Transaction*)elem1);
    struct Transaction s = *((struct Transaction*)elem2);
    if (f.timeRequired > s.timeRequired) return  1;
    if (f.timeRequired < s.timeRequired) return -1;
    return 0;
}

void* Transact(void* args){
    
    int workerindex = *(int*)args;
    int randSleepTime = 0;
    int jobIndex;
    
    while (1) {
    
        pthread_mutex_lock(&mutex);
        
        // finished tasks
        if(currentJobIndex >= numTrans){
            pthread_mutex_unlock(&mutex);
            break;
        }
            
        // get the next job
        jobIndex = currentJobIndex;
        struct Transaction job = transactions[currentJobIndex];
        currentJobIndex++;
        
        // deposite
        if (job.accNumTo == 0) {
            
            randSleepTime = rand() % job.timeRequired;
            
            pthread_mutex_unlock(&mutex);
            sleep(randSleepTime);
            
            pthread_mutex_lock(&mutex);
            accounts[job.accNumFrom].balance += job.amount;
            
            printf("Worker %d: Transaction %d: Moving %d from Account %d to %d (required time %d, actual %d)\n",workerindex + 1, jobIndex + 1, job.amount, job.accNumFrom + 1, job.accNumTo + 1,job.timeRequired, randSleepTime);
        }
        
        // withdraw
        else if (job.accNumTo == -1){
            
            randSleepTime = rand() % job.timeRequired;
            
            pthread_mutex_unlock(&mutex);
            sleep(randSleepTime);
            
            pthread_mutex_lock(&mutex);
            accounts[job.accNumFrom].balance -= job.amount;
            
            printf("Worker %d: Transaction %d: Moving %d from Account %d to %d (required time %d, actual %d)\n",workerindex + 1, jobIndex + 1, job.amount, job.accNumFrom + 1, job.accNumTo + 1,job.timeRequired, randSleepTime);
        }
        
        // transfer
        else{
            
            accounts[job.accNumFrom].balance -= job.amount;
            
            randSleepTime = rand() % job.timeRequired;
           
            pthread_mutex_unlock(&mutex);
            sleep(randSleepTime);
            
            pthread_mutex_lock(&mutex);
            accounts[job.accNumTo].balance += job.amount;
            
            printf("Worker %d: Transaction %d: Moving %d from Account %d to %d (required time %d, actual %d)\n",workerindex + 1, jobIndex + 1, job.amount, job.accNumFrom + 1, job.accNumTo + 1,job.timeRequired, randSleepTime);
        }
        
        pthread_mutex_unlock(&mutex);

    }

    pthread_exit((void*) args);

//    return NULL;
}


int main(int argc, char * argv[]) {
    
    char* fileName = "/Users/Yadikaer/Projects/OS/Threads&Scheduling/Threads&Scheduling/data.txt";
    
    //char* filennName = argv[1];
    //int randSeed = atoi(argv[2]);
    
    // int random seed
    unsigned int randSeed = 1;
    srand(randSeed);
    
    // read & parse data
    FILE *fp;
    char str[80];
    fp = fopen(fileName , "r");
    if(fp == NULL)
    {
        perror("Error opening file");
        return(-1);
    }
    
    int line = 0;
    int numParsed = 0;
    while (fgets(str,80,fp) != NULL) {
        
        // init data
        if (line <= 2) {
            numParsed = atoi(str);
            
            if(line == 0){
                numWorker = numParsed;
            }
            else if (line == 1){
                numAccount = numParsed;
                accounts = (struct Account*)malloc(numAccount * sizeof(struct Account));
            }
            else{
                numTrans = numParsed;
                transactions = (struct Transaction*)malloc(numTrans * sizeof(struct Transaction));
            }
        }
        
        // account balance data
        else if (line >= 3 && line <= 2 + numAccount){
            numParsed = atoi(str);
            int accountNum = line - 3;
            accounts[accountNum].balance = numParsed;
        }
        
        // transaction data
        else{
            char * token;
            int charIndex = 0;
            token = strtok (str," ");
            
            int transIndex = line - (2 + numAccount + 1);
            
            printf("trans index %d\n",transIndex);
            
            while (token != NULL)
            {
                if (charIndex == 0) {
                    transactions[transIndex].accNumFrom = atoi(token);
                }
                else if (charIndex == 1){
                    transactions[transIndex].accNumTo= atoi(token);
                }
                else if (charIndex == 2){
                    transactions[transIndex].amount = atoi(token);
                }
                else if (charIndex == 3){
                    transactions[transIndex].timeRequired = atoi(token);
                }
                
                printf ("token: %s\n",token);
                token = strtok (NULL, " ");
                
                charIndex++;
            }
        }
        
        line++;
    }
    
    fclose(fp);
    
    // debug log
    printf("num workers %d\n",numWorker);
    printf("num account %d\n",numAccount);
    printf("num transections %d\n",numTrans);
    for (int i = 0; i < numAccount; i++) {
        printf("Account %d balance: %d \n",i + 1,accounts[i].balance);
    }
    
    // sort transaction by time required to finish
    qsort(transactions, numTrans, sizeof(struct Transaction), comp);
    
    //for (int i = 0; i < numTrans; i++) {
    //    printf("trans need %d time\n",transactions[i].timeRequired);
    //}
    
    // init mutex
    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    
    // init threads
    pthread_t workers[numWorker];
    int i;
    for (i = 0; i < numWorker; i++) {
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&workers[i], NULL,Transact, arg);
    }

    for (int i = 0; i < numWorker; i++) {
        pthread_join(workers[i], NULL);
    }
    
    for (int i = 0; i < numAccount; i++) {
        printf("account %d balance %d\n",i + 1,accounts[i].balance);
    }
    
    // exit
    pthread_mutex_destroy(&mutex);
    free(accounts);
    accounts = NULL;
    return 0;
}
