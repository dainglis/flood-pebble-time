#include <pebble.h>
#include "path.h"

Path* path_create_path(int max) {
  Path* new_path = malloc(sizeof(Path));
  new_path->size = 0;
  new_path->max_size = max;
  new_path->pos_path = malloc(sizeof(Pos) * max);
  return new_path;
}

void path_add_to_path(Path* p, int row, int col) {
  if (p->size < p->max_size) {
    p->pos_path[p->size].row = row;
    p->pos_path[p->size].col = col;
    p->size++;
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "(%d, %d) was added to path", row, col);
  }
}