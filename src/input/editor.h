#ifndef EDITOR_H
#define EDITOR_H

#include <stdbool.h>
#include <stddef.h>

#define MESSAGE_CAPACITY 120

typedef struct {
  char text[MESSAGE_CAPACITY + 1];
  size_t len;
} Editor;

bool editor_append(Editor *editor, char symbol);
bool editor_backspace(Editor *editor);
void editor_clear(Editor *editor);

#endif
