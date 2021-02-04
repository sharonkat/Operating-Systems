/* For this assignment, a very simple PCB. It will contain only three variables:
 * a program counter, the program's start address, and the program's ending address.
 * The PC is an integer that refers to the cell number of ram[] containing the
 * instruction to execute. The start variable contains the cell number of ram[]
 * of the first instruction of the program. The end variable contains the cell 
 * number of ram[] of the last instruction of the program. */
#include <stdlib.h>

int PID = 0;

// I think you need to replay start and end with the other vals
struct PCB {
    int PC;
    // int start;
    // int end;
    struct PCB* next;
    int PC_page;
    int PC_offset;
    int pages_max;      // total number of pages in this program
    int pageTable[10];
    int PID;
};

// PC is only updated after a task switch.
// When a program is launched, a PCB is created and the PCB's PC field points
// to the first line of the program. The PCB's PC is updated after the quanta
// is finished. 

/* The ready queue is a simple linked list with head and tail pointers.
 * FIFO and RR. Head points to the first PCB in the list. Tail points to the
 * last PCB in the list. New PCBs appended at the tail. */

struct PCB* makePCB(int pages_max) {
    struct PCB* pcb = ((struct PCB*) malloc (sizeof(struct PCB)));

    pcb->PC = 0;
    pcb->PC_offset = 0;
    pcb->PC_page = 0;
    pcb->pages_max = pages_max;
    pcb->next = NULL;

    for (int i=0; i<10; i++) {
        pcb->pageTable[i] = -1;
    }
    
    pcb->PID = PID;
    PID++;
    return pcb;
}

int getCurrentPID() {
    return PID;
}
