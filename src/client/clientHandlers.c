#include "../../include/clientHandlers.h"
#include "string.h"
#include "stdio.h"

int helpHandler(char *buffer, char *MAGENTA, char *RESET) {
    if (!strcmp(buffer, ":help\n")) {
        printf("%s", MAGENTA);
        printf("\t:help to get help\n");
        printf("%s", RESET);
        fflush(stdout);
        return 1;
    } else {
        return 0;
    }
}