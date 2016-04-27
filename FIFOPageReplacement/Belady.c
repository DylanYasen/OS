//
//  Belays.c
//  FIFOPageReplacement
//
//  Created by Yadikaer Yasheng on 4/15/16.
//  Copyright © 2016 Yadikaer Yasheng. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct PageFrame{
    int pageName;
    int requestTime;
};

int fakeTime = 0;

int getRandom(int max);
void initPageTable(struct PageFrame* table, int frameCount);
int comp (const void * elem1, const void * elem2);
int hasFreePageframe(struct PageFrame* table,int frameCount);
int pageRequest(struct PageFrame* table, int frameCount, int pageName);
void pageSwap(struct PageFrame* table, int frameCount, int pageNameSwapWith);
void handlePageFault(struct PageFrame* table,int frameCount, int requestedPageName);
void printFrame(struct PageFrame* table, int frameCount);
void printResult(int size1, int fault1, int size2,int fault2,int totalPageRequest);
int checkBelady(int size1, int fault1, int size2,int fault2);
int pagingRand(int frameCount,int totalPageCount,bool printFrame);
int paging(int frameCount,int totalPageCount, int pageNames[]);

int main(int argc, char * argv[]) {

    char * mode = argv[1];
    
    if(strcmp(mode, "random") == 0){
        
        int faultCount1 = pagingRand(3, 20,false);
        int faultCount2 = pagingRand(4, 20,false);
        printResult(3, faultCount1, 4, faultCount2, 20);
    }
    else if (strcmp(mode, "ludicrous") == 0){
        
        int belayCount = 0;
        int i;
        for (i = 0; i < 1000; i++) {
            int falutCount1 = pagingRand(3, 20,false);
            int falutCount2 = pagingRand(4, 20,false);
            
            if(falutCount2 > falutCount1)
                belayCount++;
        }
        
        printf("Using a frame size of 3 and 4, five possible pages, Belady’s Anomaly occurred %d times and did not occur %d times.\n",belayCount,(1000-belayCount));
    }
    else{
        int totalPageCount = argc - 1;
        int *data = (int *)malloc(totalPageCount * sizeof(*data));
        int i;
        for (i = 0; i < totalPageCount; i++)
            data[i] = atoi(argv[i+1]);

        int faultCount1 = paging(3, totalPageCount, data);
        int faultCount2 = paging(4,totalPageCount, data);
        printResult(3, faultCount1, 4, faultCount2, totalPageCount);
        
        free(data);
    }
    
    return 0;
}

int pagingRand(int frameCount,int totalPageCount, bool outputFrame){
    int pageFaultCount = 0;
    
    struct PageFrame* pageTabel = (struct PageFrame*)malloc(frameCount * sizeof(struct PageFrame));
    initPageTable(pageTabel,frameCount);
    
    int i;
    for (i = 0; i < totalPageCount; i++) {
        int pageName = getRandom(4);
        
        int pageFrameIndex = pageRequest(pageTabel, frameCount, pageName);
        
        // page fault occur
        if(pageFrameIndex == -1){
            pageFaultCount ++;
            
            handlePageFault(pageTabel,frameCount,pageName);
        }
    }
    
    qsort(pageTabel, frameCount, sizeof(struct PageFrame), comp);
    
    if (outputFrame) {
        printFrame(pageTabel, frameCount);
    }
    
    free(pageTabel);
    
    return pageFaultCount;
}

int paging(int frameCount,int totalPageCount, int pageNames[]){
    int pageFaultCount = 0;
    
    struct PageFrame* pageTabel = (struct PageFrame*)malloc(frameCount * sizeof(struct PageFrame));
    initPageTable(pageTabel,frameCount);
    
    int i;
    for (i = 0; i < totalPageCount; i ++) {
        int pageName = pageNames[i];
        
        int pageFrameIndex = pageRequest(pageTabel, frameCount, pageName);
        
        // page fault occur
        if(pageFrameIndex == -1){
            pageFaultCount ++;
            
            handlePageFault(pageTabel,frameCount,pageName);
        }
    }
    
    printFrame(pageTabel, frameCount);
    
    free(pageTabel);

    return pageFaultCount;
}

void initPageTable(struct PageFrame* table, int frameCount){
    int i;
    for (i = 0; i < frameCount; i++) {
        table[i].pageName = -1;
        table[i].requestTime = 999999;
    }
}

int pageRequest(struct PageFrame* table, int frameCount, int pageName){
    
    // update emulated system time
    fakeTime ++;
    
    int i = 0;
    for (i = 0; i < frameCount; i++) {
        
        if (table[i].pageName == pageName) {
            return i;
        }
    }
    
    // page fault
    return -1;
}

void handlePageFault(struct PageFrame* table,int frameCount, int requestedPageName){
    
    int freeFrameIndex = hasFreePageframe(table, frameCount);
    
    // has free space insert
    if (freeFrameIndex >= 0) {
        table[freeFrameIndex].pageName = requestedPageName;
        table[freeFrameIndex].requestTime = fakeTime;
    }
    else{
        pageSwap(table,frameCount,requestedPageName);
    }
}

int hasFreePageframe(struct PageFrame* table,int frameCount){
    
    int i;
    for (i = 0; i < frameCount; i++) {
        if (table[i].pageName == -1) {
            return i;
        }
    }
    
    return -1;
}

// fifo swap
void pageSwap(struct PageFrame* table, int frameCount, int pageNameSwapWith){
    
    qsort(table, frameCount, sizeof(struct PageFrame), comp);
    
    // swap out oldest
    table[frameCount-1].pageName = -1;
    table[frameCount-1].requestTime = 99999;
    
    // swap in new
    table[frameCount-1].pageName = pageNameSwapWith;
    table[frameCount-1].requestTime = fakeTime;
}

int getRandom(int max){
    return rand() % max;
}

void printFrame(struct PageFrame* table, int frameCount){
    printf("Frame Size %d\n",frameCount);
    
    qsort(table, frameCount, sizeof(struct PageFrame), comp);
    
    int i;
    for (i = 0; i < frameCount; i++) {
        printf("\tFrame %d = %d\n",i,table[i].pageName);
    }
}

void printResult(int size1, int fault1, int size2,int fault2,int totalPageRequest){
    
    printf("Frame Size %d had %d page faults\n",size1,fault1);
    printf("Frame Size %d had %d page faults\n",size2,fault2);
    printf("Total number of pages: %d\n",totalPageRequest);

    if (checkBelady(size1, size2, fault1, fault2))
        printf("Belady’s Anomaly occurred.\n");
    else
        printf("Belady’s Anomaly didn't occur.\n");
}

int checkBelady(int size1, int fault1, int size2,int fault2){
    
    // occur
    if (size2 > size1) {
        if(fault1 > fault2)
            return 1;
    }
    
    return 0;
}

int comp (const void * elem1, const void * elem2)
{
    struct PageFrame f = *((struct PageFrame*)elem1);
    struct PageFrame s = *((struct PageFrame*)elem2);
    if (f.requestTime > s.requestTime) return  -1;
    if (f.requestTime < s.requestTime) return 1;
    
    return 0;
}



