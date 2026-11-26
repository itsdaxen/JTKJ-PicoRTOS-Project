#include "editor.h"

bool editor_append(Editor *editor, char symbol) {
  if (editor->len >= MESSAGE_CAPACITY) {
    return false;
  }
  editor->text[editor->len] = symbol;
  editor->len++;
  editor->text[editor->len] = '\0';
  return true;
}

bool editor_backspace(Editor *editor) {
  if (editor->len == 0) {
    return false;
  }
  editor->len--;
  editor->text[editor->len] = '\0';
  return true;
}

void editor_clear(Editor *editor) {
  editor->text[0] = '\0';
  editor->len = 0;
}
