#pragma once

typedef struct grid {
  int size;
  int** cells;
} Grid;

void initialize_grid(Grid* g, int s);