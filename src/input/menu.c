#include "menu.h"
#include <stdio.h>

void print_menu(MenuSelection selection) {
  printf("Menu: (Press button1 to navigate or button2 to confirm)\n");
  printf(selection == TILT ? "--> Tilt mode\n" : "    Tilt mode\n");
  printf(selection == TAP ? "--> Tap mode\n" : "    Tap mode\n");
  printf(selection == LAST_MESSAGE ? "--> Last message\n"
                                   : "    Last message\n");
}
