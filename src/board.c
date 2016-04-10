#include <pebble.h>
#include "board.h"

/*
 * initialize_grid(Grid* g, int s)
 * Grid "g" is given size "s" and "s * s" cells
 */
void initialize_grid(Grid* g, int s) {
  g->size = s;
  g->cells = malloc(sizeof(int) * s * s);
}