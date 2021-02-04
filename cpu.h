#include <stdio.h>

struct CPU {
    int IP;
    char IR[1000];
    int quanta;
    int offset;
};

extern struct CPU *cpu;
int run(int quanta);
void setIP(int instrIndex);
void createCPU(int quanta);
