#include "../../include/clientHandlers.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

int commandHandler(char *buffer, char *MAGENTA, char *RESET) {
    if (!strcmp(buffer, ":help\n")) {
        printf("%s", MAGENTA);
        printf("\t:help to get help\n");
        printf("%s", RESET);
        fflush(stdout);
        return 1;
    } else if (!strcmp(buffer, ":q\n")) {
        exit(0);
    }
    return 0;
}
