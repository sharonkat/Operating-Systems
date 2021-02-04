#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <unistd.h>
#include <limits.h>

#include "interpreter.h"
#include "shellmemory.h"
#include "memorymanager.h"
#include "shell.h"
#include "ram.h"
#include "pcb.h"
#include "cpu.h"

struct PCB *head = NULL;
struct PCB *tail = NULL;

/* This is a one-time called function when the program is loaded. It acquires
 * the necessary resources it needs to run. */
void boot() {
    initializeRAM();

    int err;
    char command[50];
    
    
    strcpy(command, "rm -r BackingStore"); 
    err = system(command);      // How can I pipe the error message somewhere other than the command line?
    

    strcpy(command, "mkdir BackingStore");
    err = system(command);      // making the directory
    if (err == 1) {
        printf("System could not be booted.\n");
        exit(-1);
    }
}

// Code from Piazza
int deleteAllFiles(char* dirPath){
    struct dirent *de;

    DIR *dr = opendir(dirPath);

    if (!dr) {
        fprintf(stderr, "Could not open directory %s\n", dirPath);
        return 1;
    }

    #if defined(WIN32)
    char *DIR_SEPARATOR = "\\";
    #else
    char *DIR_SEPARATOR = "/";
    #endif
    while ((de = readdir(dr))){
        if (strstr(de->d_name, ".txt")){
            char filepath[2 * PATH_MAX];
            memset(filepath, '\0', 2 * PATH_MAX); // clear array each loop

            strcat(filepath, dirPath);
            strcat(filepath, DIR_SEPARATOR);
            strcat(filepath, de->d_name);

            unlink(filepath);
        }
    }
    (void) closedir(dr);
    return 0;
}

void addToReady(struct PCB *pcb) {
    if (head == NULL) {
        head = pcb;
    } else {
        tail->next = pcb;
    }
    tail = pcb;
}

// Initiate the program by making it a PCB
struct PCB *myInit(int pages_max)
{
    struct PCB *pcb_ptr = makePCB(pages_max); // makes a PCB using malloc
    addToReady(pcb_ptr);
    return pcb_ptr;
}

struct PCB* removeFromQ(struct PCB* head) {
    struct PCB* temp = head->next;
    freeRAM(head->PC, head->pages_max);

    free(head);
    return temp;
}

int pageFault(struct PCB *pcb) {
    int errCode = 0;

    pcb->PC_page++;         // because we ran 4 lines already

    if (pcb->PC_page >= pcb->pages_max) {    // program is done
        errCode = -3;
        return errCode;
    }
    int frame = pcb->pageTable[pcb->PC_page];

    if (frame != -1) {
        pcb->PC = frame;
        pcb->PC_offset = 0;
        return pcb->PC;
    } else {
        errCode = findPage(pcb->PID, pcb->PC_page);
        pcb->PC_offset = 0;
        pcb->PC = errCode;
        return pcb->PC;
    }
}

int scheduler(){
    int errCode = 0;
    
    while(head != NULL){
        if (cpu == NULL) {
            cpu = (struct CPU*) malloc(sizeof(struct CPU));
        }
        // Set IP to PC for first PCB
        cpu->IP = head->pageTable[head->PC_page]*PAGE_SIZE;     // 0, 4, 8...
        // Run and update PC for head
        errCode = run(2);

        if (errCode == -3) {            // the program needs to be removed
            head = removeFromQ(head);
        } else if (head->next != NULL) {
            struct PCB* newHead = head->next;
            tail->next = head;
            tail = tail->next;
            tail->next = NULL;
            head = newHead;
        }

        if (errCode == -1) {
            return errCode;
        } else if (errCode == -2) {     // the program is done
            return errCode;
        } else if (errCode == 0 || errCode == 1) {
            continue;
        }

    }
    if (head != NULL) {
        removeFromQ(head);
    }
    deleteAllFiles("BackingStore");
}

/* This function is used to call the shell functions which launches the user
 * interface. It also instaniates all the kernel data structures. */
int kernel() {
    // createCPU(2);
    // initializeRAM();
    int errCode = 0;
    printf("Kernel 2.0 loaded!\n");
    do {
        errCode = shellUI();
    } while (errCode != 99);
    // delete();
    return errCode;
}
