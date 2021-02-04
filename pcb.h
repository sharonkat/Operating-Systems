int PID;

struct PCB {
    int PC;
    struct PCB* next;
    int PC_page;
    int PC_offset;
    int pages_max;      // total number of pages in this program
    int pageTable[10];
    int PID;
};

struct PCB* makePCB(int pages_max);
int getCurrentPID();