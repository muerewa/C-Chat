#include "../../../include/graphics.h"
#include "ncurses.h"

void printLogMsg(char *msg) {
    printw("%s", msg);
    refresh();
}