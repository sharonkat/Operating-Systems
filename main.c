#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

#include <limits.h>

#include "kernel.h"

int main() {
    int error = 0;
    boot();
    error = kernel();
    return error;
}
