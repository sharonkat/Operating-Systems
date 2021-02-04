#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pcb.h"
#include "ram.h"
#include "kernel.h"

#define PAGE_SIZE 4

struct PCB *pcb;
int victimFlag = -1;
int frameFlag = -1;

/* Helper function for countTotalPages */
double countLines(FILE *f) {
    char codeLine[1000];
    double lineCount = 0;
    rewind(f);
    while (fgets(codeLine, 1000, f) != NULL) {
            lineCount++; 
    }
    return lineCount;
}

int countTotalPages(FILE *f)
{
    double lineCount = countLines(f);
    if(lineCount == 0) {
        return 0;
    }

    if(ceil(lineCount / PAGE_SIZE) <= 1) {
        return 1;
    } 
    return ceil(lineCount / PAGE_SIZE);
}

/* Searches the page table of PCB pcb for the frame victim. 
 * If it does not find it, returns -1. */
int findFrameInPT(struct PCB* pcb, int victim) {
    int frame;
    for (int i=0; i<pcb->pages_max; i++){
        if (pcb->pageTable[i] == victim) {
            frame = pcb->pageTable[i];
            return frame;
        }
    }
    return -1;    
}

/* Iterates through the queue of PCBs and looks in each page table for
 * the victim frame. If found, updates the page table accordingly. */
void updateVictimPCB(int victimFrame) {
    struct PCB* temp = head;
    while (temp) {
        int pageTableIndx = findFrameInPT(temp, victimFrame);
        if (pageTableIndx != -1) {
            temp->pageTable[pageTableIndx] = -1;
            break;
        }
        temp = temp->next;
    }
}

int findVictim(struct PCB *p)
{
    // When findFrame returns -1, use a random number generator
    // to pick a random frame number. if this frame number is not already
    // in the page table, then return it. Otherwise, incremenet until
    // you get a good frame number.
    int victim;
    int r = rand() % 10; // random int between 0 and 9
    victim = p->pageTable[r];
    while (victim != -1)
    {
        r = (r + 1) % 10;
        victim = p->pageTable[r];
    }
    return r;
}

int findFrame()
{
    // Search ram[] for a frame that is equal to NULL. If one exists, return its index,
    // otherwise return -1 i.e. the RAM is full. Then findVictim()

    int index;

    for (index = 0; index < RAM_SIZE; index+4)      // needs to count every 4
    {
        if (ram[index] == NULL)
        {
            if (index == (RAM_SIZE - 1)) {
                return -1;
            } else {
                return (int) index/PAGE_SIZE;
            }
        }
        else {
            index++;
        }
    }
    return -1;
}

int updatePageTable(struct PCB *p, int pageNumber, int frameNumber, int victimFrame)
{
    if (frameNumber != -1 && victimFrame == -1)
    {
        p->pageTable[pageNumber] = frameNumber;
    }
    else if (victimFrame != -1 && frameNumber == -1)
    {
        p->pageTable[pageNumber] = victimFrame;
        updateVictimPCB(victimFrame);
    }
    else if (frameNumber == -1 && victimFrame == -1)
    {
        printf("Error updating page table.\n");
    }
    return 0;
}

void loadPage(int pageNumber, FILE *p, int frameNumber)
{
    rewind(p);

    int currPage = 0;
    int offset = 0;
    char line[256];

    for (currPage; currPage != pageNumber; currPage++) {
        int count = 0;
        while (fgets(line, sizeof(line), p) != NULL && count < 3) {
            count++;
        }
    }

    int cell = frameNumber*PAGE_SIZE;
    char *tmp;

    while (fgets(line, sizeof(line), p) != NULL && offset <= 3) {
        tmp = &line[0];
        addToRAM(tmp, cell+offset);
        offset++;
    }
    rewind(p);
}

/* Helper function for pageFault. Called when a new page needs
 * to be loaded in RAM. */
int findPage(int PID, int page) {
    char id[32];
    char filePath[1000] = "./BackingStore/";

    sprintf(id, "%d", PID);

    const char *filename = strcat(id, ".txt");
    strcat(filePath, filename);

    FILE *f = fopen(filePath, "r");
    if (f == NULL)
    {
        fclose(f);
        printf("Cannot open %s.\n", filePath);
        return 0;
    }


    int frame = findFrame();

    frameFlag = frame;
    if (frame == -1)
    {
        frame = findVictim(pcb);    // needs to be head of pcb at ready queue
        victimFlag = frame;
    }
    loadPage(page, f, frame);
    updatePageTable(pcb, page, frameFlag, victimFlag);

    // /BackingStore/PID.txt is the file we need from!
    // And the page*PAGE_SIZE is the line we need
    
    page++;
    fclose(f);
    return page;
}

int launcher(FILE *f)
{
    char filePath[1000] = "./BackingStore/";
    char PID[32], c;
    sprintf(PID, "%d", getCurrentPID());        // PID takes the value of PID
    const char *filename = strcat(PID, ".txt"); // creates the file
    strcat(filePath, filename);

    FILE *target = fopen(filePath, "w+");

    if (target == NULL)
    {
        fclose(f);
        printf("Cannot open %s.\n", filePath);
        return 0;
    }

    // Copy the file to execute into the file in the backing store
    c = fgetc(f);
    while (c != EOF)
    {
        fputc(c, target);
        c = fgetc(f);
    }
    fclose(f);
    fclose(target);
    FILE *fp = fopen(filePath, "r"); // now only reading permissions


    // Now load max two pages of the program into RAM.
    int pages, frame;
    pages = countTotalPages(fp);
    if (pages > 10) {
        printf("WARNING: Program file too large. MAX SIZE: 8 lines.\n");
    }

    pcb = myInit(pages); // create pcb from the file in backing store

    int counter = 0;
    if (pages >= 2) {
        counter = 2;
    } else if (pages == 1) {
        counter = 1;
    }

    for (int i = 0; i < counter; i++)
    {
        frame = findFrame();

        frameFlag = frame;
        if (frame == -1)
        {
            frame = findVictim(pcb);    // needs to be head of pcb at ready queue
            victimFlag = frame;
        }

        loadPage(i, fp, frame);
        updatePageTable(pcb, i, frameFlag, victimFlag);
    }

    pcb->pages_max = pages;
    fclose(fp);
    return 1;
}