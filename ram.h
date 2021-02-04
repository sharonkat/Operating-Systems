#define RAM_SIZE 40

extern char* ram[RAM_SIZE];
// int addToRAM(FILE *p, int *start);
int addToRAM(char *line, int start);
void freeRAM(int start, int end);
void initializeRAM();
char *readRAM(int memAddress);
