#ifndef C_CHAT_GRAPHICS_H
#define C_CHAT_GRAPHICS_H
#include "ncurses.h"

WINDOW *create_newwin(int height, int width, int starty, int startx);
void printLogMsg(WINDOW *win,char *msg);

#endif
