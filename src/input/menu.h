#ifndef MENU_H
#define MENU_H

typedef enum { TILT, TAP, LAST_MESSAGE } MenuSelection;

void print_menu(MenuSelection selection);

#endif
