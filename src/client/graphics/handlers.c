#include "../../../include/graphics.h"
#include "ncurses.h"

WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    wrefresh(local_win);

    return local_win;
}

void printLogMsg(WINDOW *win,char *msg) {
    wprintw(win, "%s", msg);
    wrefresh(win);
}