#include "display.h"
#include "tkjhat/sdk.h"
#include <string.h>

void display_text(const char *text) {
  clear_display();

  size_t length = strlen(text);
  size_t curr = 0;
  int y = 0;

  while (curr < length && y + 8 < 64) {
    char line[22];
    size_t count = length - curr;

    // target only 21 bytes of text
    if (count > 21) {
      count = 21;
    }

    memcpy(line, text + curr, count);
    line[count] = '\0';

    write_text_xy(0, y, line);

    curr += count;
    y += 10;
  }
}

void display_menu(MenuSelection selection) {
  clear_display();
  write_text_xy(0, 0, selection == TILT ? "> Tilt" : "  Tilt");
  write_text_xy(0, 16, selection == TAP ? "> Tap" : "  Tap");
  write_text_xy(
      0, 32, selection == LAST_MESSAGE ? "> Last message" : "  Last message");
}
