/* A struct having an IP, IR, and quanta field. IP points to the next instruction
 * to execute from ram[]. The currently executing instruction is stored in
 * the IR. The IR stores the instruction that will be sent to the interpreter()
 * for execution. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pcb.h"
#include "ram.h"
#include "kernel.h"
#include "shell.h"
#include "interpreter.h"
#include "memorymanager.h"

struct CPU {
    int IP;
    char IR[1000];
    int quanta;
    int offset;
};

struct CPU *cpu = NULL;

void createCPU(int quanta) {
    cpu->quanta = quanta; 
    cpu->IP = -1;
}

void setQuanta(int quanta) {
    cpu->quanta = quanta;
}

int getIP() {
    return cpu->IP;
}

void setIP(int instrIndex) {
    cpu->IP = instrIndex;
}

void setIR(char* instruction) {
    strcpy(cpu->IR, instruction);
}

int findMin(int a, int b) {
    if (a < b) return a;
    else return b;
}

int run(int quanta) {
    int errCode = 0;

    cpu->offset = head->PC_offset;
    
    while(quanta > 0) {
        
        // Check to see if there is another frame to this program
        if (ram[cpu->IP+cpu->offset] == NULL) {
            errCode = -2;
            return errCode;
        }
        // Instruction register reads a line from RAM to run it
        setIR(strdup(readRAM(cpu->IP+cpu->offset)));

        interpreter(cpu->IR);
        quanta--;
        cpu->offset++;
    }
    // When offset is 4, we have completed one page (4 lines)
    if (cpu->offset == PAGE_SIZE) {
        errCode = pageFault(head);
        return errCode;
    }

    // Update PC for PCB
    head->PC = cpu->IP;
    // Save the offset at the end of the quanta
    head->PC_offset = cpu->offset;
    
    return errCode;
}
