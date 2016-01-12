#include <pebble.h>
#define SIZE 10
#define MAXCOLS 5

//TODO
//basically, every iteration its going to create a square outwards where all the cells
//in the square are the same color, so the recursion can be directly pushed outwards to
//the edges of the square

static const int BOARD_SIZE = SIZE;
static const int MOVES = 30;
static const int PLAY = 0;
static const int WIN = 1;
static const int LOSE = 2;
//static const int MENU = 3;

static Window *window;
static TextLayer *text_layer;
static Layer *graphics_layer;
static int active_color;
static int board[SIZE][SIZE];
static int colors[MAXCOLS];
static int moves;
static int game_state;
static int inner_square;
static bool recur;
static char* move_string;
//static Game *game;

//debug
static int ree;

static void generate_string(int n) {
  char* newstring = malloc(sizeof(char) * 5);
  if (n >= 10) {
    newstring[0] = (n / 10) + '0';
    newstring[1] = (n % 10) + '0';
  } else {
    newstring[0] = ' ';
    newstring[1] = n + '0';
  }
  newstring[2] = '/';
  newstring[3] = (MOVES / 10) + '0';
  newstring[4] = (MOVES % 10) + '0';
  strcpy(move_string, newstring);
  free(newstring);
}

bool game_won() {
  for (int i = 0; i < 6; i++) {
    if (colors[i] == BOARD_SIZE * BOARD_SIZE) {
      return true;
    }
  }
  return false;
}

bool game_over() {
  if (moves == MOVES) {
    return true;
  }
  return false;
}

static void set_state() {
  if (game_won()) {
    game_state = WIN;
  } else if (game_over()) {
    game_state = LOSE;
  } else {
    game_state = PLAY;
  }
}

static void init_board() {
  recur = false;
  moves = 0;
  game_state = PLAY;
  inner_square = 0;
  for (int i = 0; i < MAXCOLS; i++) {
    colors[i] = 0;
  }
  for (int r = 0; r < BOARD_SIZE; r++) {
    for (int c = 0; c < BOARD_SIZE; c++) {
      int col = rand() % MAXCOLS;
      board[r][c] = col;
      colors[col]++;
    }
  }
  set_state();
}

static void set_cell_color(int r, int c, int color, int prev_c) {
  int prev = prev_c;
  board[r][c] = color;
  colors[prev]--;
  colors[color]++;
  
  ree++; //DEBUG
  
  if (r < BOARD_SIZE - 1 && board[r+1][c] == prev) {
    set_cell_color(r + 1, c, color, prev);
  }
  if (r > 0 && board[r-1][c] == prev) {
    set_cell_color(r - 1, c, color, prev);
  }
  if (c < BOARD_SIZE - 1 && board[r][c+1] == prev) {
    set_cell_color(r, c + 1, color, prev);
  }
  if (c > 0 && board[r][c-1] == prev) {
    set_cell_color(r, c - 1, color, prev);
  }
}

static void check_square() {
  int checker = board[0][0];
  bool bigger = true;
  while(bigger) {
    for (int r = 0; r < inner_square + 1; r++) {
      for (int c = 0; c < inner_square + 1; c++) {
        if (board[r][c] != checker) {
          bigger = false;
          break;
        }
      }
    }
    if (bigger) {
      inner_square++;
    }
  }
}

int set_inner_square(int color) {
  int prev = board[0][0];
  for (int r = 0; r < inner_square - 1; r++) {
    for (int c = 0; c < inner_square - 1; c++) {
      colors[board[r][c]]--;
      colors[color]++;
      board[r][c] = color;
    }
  }
  return prev;
}

static void graphics_layer_update_proc(Layer *layer, GContext *ctx) {
  //GColor game_colors[6] = {GColorRed, GColorChromeYellow, GColorYellow, GColorGreen, GColorPictonBlue, GColorPurple};
  GColor game_colors[5] = {GColorRed, GColorChromeYellow, GColorYellow, GColorGreen, GColorPictonBlue};
  int offset_x = 12; //old2 left and right border size
  int offset_y = 36; //old26 top border size such that bottom border is 2px
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, GCornerNone);
  
  /* Draws game board */
  for (int r = 0; r < BOARD_SIZE; r++) {
    for (int c = 0; c < BOARD_SIZE; c++) {
      graphics_context_set_fill_color(ctx, game_colors[board[r][c]]);
      graphics_fill_rect(ctx, GRect(offset_x + (c * 12), offset_y + (r * 12), 12, 12), 0, GCornerNone);
    }
  }
  
  /* Draws color selector */
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(12 + (active_color * 10), 8, 10, 10), 0, GCornerNone);
  for (int i = 0; i < MAXCOLS; i++) {
    graphics_context_set_fill_color(ctx, game_colors[i]);
    graphics_fill_rect(ctx, GRect(13 + (i * 10), 9, 8, 8), 0, GCornerNone);
  }
  
  if (game_state == WIN) {
    text_layer_set_text(text_layer, "Winner");
    active_color = 0;
  } else if (game_state == LOSE) {
    text_layer_set_text(text_layer, "Game over");
    active_color = 0;
  } else {
    if (moves == 0) {
      text_layer_set_text(text_layer, "Flood");
    } else {
      generate_string(moves);
      text_layer_set_text(text_layer, move_string);
    }
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (game_state == PLAY) {
    if (!recur) {
      ree = 0; //DEBUG
      recur = true;
      int prev;
      check_square();
      prev = set_inner_square(active_color);
      set_cell_color(inner_square - 1, inner_square - 1, active_color, prev);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Inner square: %d   Recursions: %d", inner_square, ree); //DEBUG
      moves++;
      set_state();
    }
  } else {
    init_board();
  }
  layer_mark_dirty(graphics_layer);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  recur = false;
  if (active_color > 0) {
    active_color--;
  } else {
    active_color = MAXCOLS - 1;
  }
  layer_mark_dirty(graphics_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  recur = false;
  if (active_color < MAXCOLS - 1) {
    active_color++;
  } else {
    active_color = 0;
  }
  layer_mark_dirty(graphics_layer);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  //GRect bounds = layer_get_bounds(window_layer);
  
  move_string = malloc(sizeof(char) * 6);
  //game = game_create();
  //game_initialize(game);
  init_board();

  graphics_layer = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(graphics_layer, graphics_layer_update_proc);
  
  text_layer = text_layer_create(GRect(80, 3, 64, 23));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(text_layer, "Flood");
  
  layer_add_child(window_layer, graphics_layer);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
  layer_destroy(graphics_layer);
  free(move_string);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}