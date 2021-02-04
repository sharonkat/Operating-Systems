/* This is a data structure used to load at most three programs when the exec 
 * command is used. Each line from the source file is loaded into its own cell
 * in the array of RAM. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define RAM_SIZE 40 //1000

char *ram[RAM_SIZE];

// RAM must read NULL if no program is in it.
void initializeRAM()
{
    for (int i = 0; i < RAM_SIZE; i++)
    {
        ram[i] = NULL;
    }
}

void freeRAM(int start, int end)
{
    int i;
    for (i = start; i <= end; i++)
    {
        ram[i] = NULL;
    }
}

// Add program to RAM
int addToRAM(char *line, int start)
{
    ram[start] = strdup(line);
    // printf("RAM[%d]: %s\n", start, ram[start]);

    return 0;
}

// Returns instruction found in RAM at specified location
char *readRAM(int memAddress)
{
    return ram[memAddress];
}