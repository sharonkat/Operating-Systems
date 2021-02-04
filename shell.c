/* This file contains the main() and parse() functions. 
 * Starting code is from the slides. Work needs to be done. TODO */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
#include "shellmemory.h"

int shellUI() {
    
    printf("Welcome to the Sharon Shell!\nVersion 3.0 Created April 2020\n");

    shell_memory_initialize();

    while (!feof(stdin)) 
    {
        printf("$ ");
        fflush(stdout);
    
        char *line = NULL;
        size_t linecap = 0;
        if (getline(&line, &linecap, stdin) == -1) {
            freopen("/dev/tty", "r", stdin);
            // break;
        } else {
            (void)interpreter(line);
            free(line);
        }
    }

    shell_memory_destroy();

    return 0;
}
