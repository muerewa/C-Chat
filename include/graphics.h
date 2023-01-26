#ifndef C_CHAT_GRAPHICS_H
#define C_CHAT_GRAPHICS_H
#include "ncurses.h"

WINDOW *create_newwin(int height, int width, int starty, int startx);
void printLogMsg(WINDOW *win,char *msg);
void initNcurses();
void printHelp(WINDOW *print, WINDOW *clear);
void updateBorder(WINDOW *local_win);

#endif
