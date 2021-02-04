#include <stdio.h>

extern struct PCB *head;
extern struct PCB *tail;
int pageFault(struct PCB *pcb);
struct PCB* myInit(int pages_max);
int scheduler();
int kernel();
void boot();