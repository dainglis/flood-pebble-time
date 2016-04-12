#pragma once
#include <pebble.h>

typedef struct pos {
  int row;
  int col;
} Pos;

typedef struct path {
  int size;
  int max_size;
  Pos* pos_path;
} Path;

Path* path_create_path(int max);
void path_add_to_path(Path* p, int row, int col);