#include "../../../include/graphics.h"
#include "ncurses.h"

WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);

    wrefresh(local_win);

    wattron(local_win,COLOR_PAIR(1));
    wborder(local_win, ' ', ' ', ' ', '-', ' ', ' ', '-', '-');
    wrefresh(local_win);
    wattroff(local_win, COLOR_PAIR(1));

    wrefresh(local_win);

    return local_win;
}

void printLogMsg(WINDOW *win,char *msg) {
    wprintw(win, "%s", msg);
    wrefresh(win);
}

void printHelp(WINDOW *print, WINDOW *clear) {
    attron(COLOR_PAIR(2));
    printLogMsg(print, "\n:help - get help\n");
    printLogMsg(print, ":q - exit program\n");
    wclear(clear);
}

void initNcurses() {
    initscr();
    refresh();
    start_color();
    use_default_colors();
    keypad(stdscr,TRUE);

    init_pair(1, COLOR_GREEN, -1); // Зеленый цвет
    init_pair(2, -1, -1); // Дефолтный цвет
    init_pair(3, COLOR_MAGENTA, -1); // Розовый цвет
    init_pair(4, COLOR_CYAN, -1); // Синий цвет
}

void updateBorder(WINDOW *local_win) {
    wattron(local_win,COLOR_PAIR(1));
    wborder(local_win, ' ', ' ', ' ', '-', ' ', ' ', '-', '-');
    wrefresh(local_win);
}