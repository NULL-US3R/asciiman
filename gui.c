#include "gui.h"
#include "level.h"
#include <ctype.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Helper functions */
int is_numeric(const char *str) {
  for (int i = 0; str[i]; i++) {
    if (!isdigit(str[i]))
      return 0;
  }
  return 1;
}

int is_valid_seed(const char *str) {
  if (strlen(str) != 45)
    return 0;
  for (int i = 0; i < 45; i++) {
    if (!isdigit(str[i]))
      return 0;
  }
  return 1;
}

void draw_credits(WINDOW *win, int logo_start_y) {
  int max_y, max_x;
  getmaxyx(win, max_y, max_x);
  const char *credits = "Maksim Bespalov, Sergey Petrov, 2025";
  mvwprintw(win, max_y - 1, (max_x - strlen(credits)) / 2, "%s", credits);
}

/* Random parameters input screen */
int get_random_params(WINDOW *win, level_t *level) {

  level_t input = {0};

  werase(win);
  int max_y, max_x;
  getmaxyx(win, max_y, max_x);
  int logo_start_y = (max_y - LOGO_HEIGHT) / 2;
  char error_msg[64] = {0};
  int current_field = 0;
  char tmp[6];

  const char *labels[] = {
      "Height:          ", "Width:           ", "Paths:           ",
      "Max path length: ", "Points:          ", "[ Play ]"};

  while (1) {
    werase(win);
    for (int i = 0; i < LOGO_HEIGHT; i++) {
      int x = (max_x - strlen(logo[i])) / 2;
      mvwprintw(win, logo_start_y + i, x, "%s", logo[i]);
    }
    draw_credits(win, logo_start_y);

    // Draw error message
    if (error_msg[0]) {
      wattron(win, COLOR_PAIR(COLOR_ERROR));
      mvwprintw(win, max_y - 3, (max_x - strlen(error_msg)) / 2, "%s",
                error_msg);
      wattroff(win, COLOR_PAIR(COLOR_ERROR));
      error_msg[0] = '\0';
    }

    int start_y = logo_start_y + LOGO_HEIGHT + 2;
    int start_x = (max_x - 25) / 2;

    for (int i = 0; i < 6; i++) {
      if (i == current_field)
        wattron(win, A_REVERSE);

      uint16_t param;
      switch (i) {
      case 0:
        param = input.height;
        break;
      case 1:
        param = input.width;
        break;
      case 2:
        param = input.paths;
        break;
      case 3:
        param = input.max_path_length;
        break;
      case 4:
        param = input.points;
        break;
      }

      if (i < 5) {
        mvwprintw(win, start_y + i, start_x, "%s", labels[i]);
        mvwprintw(win, start_y + i, start_x + 16, "%5hu", param);
      } else {
        mvwprintw(win, start_y + i, start_x + 8, "%s", labels[i]);
      }

      if (i == current_field)
        wattroff(win, A_REVERSE);
    }

    wrefresh(win);

    int ch = wgetch(win);
    switch (ch) {
    case KEY_UP:
      current_field = (current_field - 1 + 6) % 6;
      break;
    case KEY_DOWN:
      current_field = (current_field + 1) % 6;
      break;
    case 10: // Enter
      if (current_field == 5)
        goto random_params_exit;
      echo();
      curs_set(1);
      mvwgetnstr(win, start_y + current_field, start_x + 16, tmp, 5);
      noecho();
      curs_set(0);

      if (!is_numeric(tmp)) {
        snprintf(error_msg, sizeof(error_msg), "Invalid number: '%s'", tmp);
        break;
      }

      long val = atol(tmp);
      if (val < 0 || val > UINT16_MAX) {
        snprintf(error_msg, sizeof(error_msg), "Value must be between 0-%d",
                 UINT16_MAX);
        break;
      }

      switch (current_field) {
      case 0:
        input.height = val;
        break;
      case 1:
        input.width = val;
        break;
      case 2:
        input.paths = val;
        break;
      case 3:
        input.max_path_length = val;
        break;
      case 4:
        input.points = val;
        break;
      }

      break;
    case 27: // ESC
      return 0;
    }
  }
random_params_exit:
  init_level(level, input.height, input.width, input.paths,
             input.max_path_length, input.points);
  return 1;
}

/* Seed input screen */
int get_seed_input(WINDOW *win, level_t *level) {
  werase(win);
  int max_y, max_x;
  getmaxyx(win, max_y, max_x);
  int logo_start_y = (max_y - LOGO_HEIGHT) / 2;
  char error_msg[64] = {0};

  char buf[SEED_BUFFER_SIZE];

  while (1) {
    werase(win);
    for (int i = 0; i < LOGO_HEIGHT; i++) {
      int x = (max_x - strlen(logo[i])) / 2;
      mvwprintw(win, logo_start_y + i, x, "%s", logo[i]);
    }
    draw_credits(win, logo_start_y);

    int start_y = logo_start_y + LOGO_HEIGHT + 2;
    int start_x = (max_x - 50) / 2;

    if (error_msg[0]) {
      wattron(win, COLOR_PAIR(COLOR_ERROR));
      mvwprintw(win, start_y + 3, start_x, "%s", error_msg);
      wattroff(win, COLOR_PAIR(COLOR_ERROR));
      error_msg[0] = '\0';
    }

    mvwprintw(win, start_y, start_x, "Enter seed (exactly 45 digits):");
    mvwprintw(win, start_y + 1, start_x, "> ");
    mvwprintw(win, start_y + 2, start_x,
              "[Press Enter to confirm, ESC to cancel]");

    echo();
    curs_set(1);
    mvwgetnstr(win, start_y + 1, start_x + 2, buf, SEED_BUFFER_SIZE);
    noecho();
    curs_set(0);

    if (strlen(buf) == 0)
      return -1;

    if (!is_valid_seed(buf)) {
      snprintf(error_msg, sizeof(error_msg),
               "Invalid seed! Must be exactly 45 digits (got %zu)",
               strlen(buf));
      continue;
    }

    init_level_seeded(level, buf);
    return 1;
  }
}

/* Main menu function */
int draw_menu_and_init_level(WINDOW *win, level_t *level) {
  werase(win);
  int max_y, max_x;
  getmaxyx(win, max_y, max_x);
  int logo_start_y = (max_y - LOGO_HEIGHT) / 2;

  // Blinking text phase
  bool text_visible = true;
  const char *message = "Press any key to continue";
  int msg_y = (max_y / 2) + LOGO_HEIGHT + 2;
  int msg_x = (max_x - strlen(message)) / 2;
  int msg_length = strlen(message);

  nodelay(win, TRUE);
  keypad(win, TRUE);

  int ch;
  while ((ch = wgetch(win)) == ERR) {
    werase(win);
    for (int i = 0; i < LOGO_HEIGHT; i++) {
      int x = (max_x - strlen(logo[i])) / 2;
      mvwprintw(win, logo_start_y + i, x, "%s", logo[i]);
    }

    if (text_visible) {
      mvwprintw(win, msg_y, msg_x, "%s", message);
    } else {
      mvwprintw(win, msg_y, msg_x, "%*s", msg_length, " ");
    }

    draw_credits(win, logo_start_y);
    wrefresh(win);
    text_visible = !text_visible;
    napms(500);
  }

  // Main menu phase
  nodelay(win, FALSE);
  const char *menu_items[] = {"1. Play random seed", "2. Play set seed   ",
                              "Q. Quit            "};
  const int menu_size = sizeof(menu_items) / sizeof(menu_items[0]);
  int selection = 0;

  while (1) {
    werase(win);
    for (int i = 0; i < LOGO_HEIGHT; i++) {
      int x = (max_x - strlen(logo[i])) / 2;
      mvwprintw(win, logo_start_y + i, x, "%s", logo[i]);
    }
    draw_credits(win, logo_start_y);

    int menu_start_y = logo_start_y + LOGO_HEIGHT + 2;
    for (int i = 0; i < menu_size; i++) {
      int x = (max_x - strlen(menu_items[i])) / 2;
      if (i == selection) {
        wattron(win, A_REVERSE);
        mvwprintw(win, menu_start_y + i, x, "%s", menu_items[i]);
        wattroff(win, A_REVERSE);
      } else {
        mvwprintw(win, menu_start_y + i, x, "%s", menu_items[i]);
      }
    }

    wrefresh(win);

    switch ((ch = wgetch(win))) {
    case KEY_UP:
      selection = (selection - 1 + menu_size) % menu_size;
      break;
    case KEY_DOWN:
      selection = (selection + 1) % menu_size;
      break;
    case '1':
    case '2':
    case 'q':
    case 'Q':
    case 10: // Enter
      if (ch == '1' || (ch == 10 && selection == 0)) {
        if (get_random_params(win, level))
          return 1;
      } else if (ch == '2' || (ch == 10 && selection == 1)) {
        if (get_seed_input(win, level))
          return 1;
      } else if (ch == 'q' || ch == 'Q' || (ch == 10 && selection == 2)) {
        return 0;
      }
      break;
    case 27: // ESC
      return 0;
    }
  }
}

void process_fog(const level_t *level_src, level_t *level_res,
                 const pacman_t *pacman) {
  for (int i = 0; i < level_src->height; i++) {
    for (int j = 0; j < level_src->width; j++) {
      if (abs(pacman->y - i) > pacman->fog_radius ||
          abs(pacman->x - j) > pacman->fog_radius) {
        level_res->level_arr[i][j] = object_chars[FOG];
      }
    }
  }
}

FILE *log_init(const level_t *level, const pacman_t *pacman) {
  time_t t = time(NULL);
  char filename[23];
  struct tm *tp = localtime(&t);
  strftime(filename, sizeof(filename), "%d_%m_%Y-%H:%M:%S.log", tp);
  FILE *file = fopen(filename, "w+");
  char seed[45];
  get_seed((level_t *)level, seed, sizeof(seed));
  fprintf(file, "%s\n", asctime(tp));
  fprintf(file, "seed: %s\n", seed);
  return file;
}

void log_movement(FILE *file, const pacman_t *pacman) {
  fprintf(file, "x: %d y: %d score: %d move_direction: %s\n", pacman->x,
          pacman->y, pacman->score,
          move_direction_names[pacman->move_direction]);
}

void init_pacman(pacman_t *pacman, level_t *level) {
  do {
    pacman->x = rand() % level->width;
    pacman->y = rand() % level->height;
  } while (level->level_arr[pacman->y][pacman->x] == object_chars[BORDER]);
  pacman->score = 0;
  pacman->move_direction = NO_MOVE;
  pacman->fog_radius = 20;
}

void move_pacman(pacman_t *pacman, level_t *level) {
  int32_t new_x = pacman->x + move_dx[pacman->move_direction];
  int32_t new_y = pacman->y + move_dy[pacman->move_direction];
  if (new_x >= 0 && new_x < level->width && new_y >= 0 &&
      new_y < level->height &&
      level->level_arr[new_y][new_x] != object_chars[BORDER]) {
    pacman->x = new_x;
    pacman->y = new_y;
  } else {
    pacman->move_direction = NO_MOVE;
  }
  if (level->level_arr[pacman->y][pacman->x] == object_chars[FOOD]) {
    level->level_arr[pacman->y][pacman->x] = object_chars[PATH];
    pacman->score++;
  }
}

void draw_game(WINDOW *gamemap, WINDOW *sidebar, level_t *level,
               pacman_t *pacman, FILE *f) {
  level_t level_fogged = {0};
  char seed[SEED_BUFFER_SIZE];
  get_seed(level, seed, sizeof(seed));
  mvwprintw(sidebar, 2, 0, "%s", seed);

  while (1) {
    wrefresh(gamemap);
    wrefresh(sidebar);
    level_copy(&level_fogged, level);

    process_fog(level, &level_fogged, pacman);

    for (int i = 0; i < level_fogged.height; i++) {
      mvwprintw(gamemap, i, 0, "%s", level_fogged.level_arr[i]);
    }
    wattron(gamemap, COLOR_PAIR(COLOR_PACMAN) | A_BOLD);
    mvwaddch(gamemap, pacman->y, pacman->x, '@');
    wattroff(gamemap, COLOR_PAIR(COLOR_PACMAN) | A_BOLD);

    int key = getch();
    switch (key) {
    case 'q':
      goto game_exit;
    case '+':
      pacman->fog_radius++;
      break;
    case '-':
      pacman->fog_radius--;
      break;
    case 'w':
      pacman->move_direction = UP;
      log_movement(f, pacman);
      break;
    case 's':
      pacman->move_direction = DOWN;
      log_movement(f, pacman);
      break;
    case 'a':
      pacman->move_direction = LEFT;
      log_movement(f, pacman);
      break;
    case 'd':
      pacman->move_direction = RIGHT;
      log_movement(f, pacman);
      break;
    }

    move_pacman(pacman, &level_fogged);

    mvwprintw(sidebar, 0, 0, "x: %0d y: %0d", pacman->x, pacman->y);
    mvwprintw(sidebar, 1, 0, "score: %d", pacman->score);

    wtimeout(gamemap, 100);
  }
game_exit:
  free_level(&level_fogged);
}

int main() {
  initscr();
  start_color();
  cbreak();
  noecho();
  curs_set(0);
  init_pair(COLOR_ERROR, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_PROMPT, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_PACMAN, COLOR_YELLOW, COLOR_BLACK);

  WINDOW *menu = newwin(LINES, COLS, 0, 0);

  level_t level = {0};
  level.level_arr = NULL;

  draw_menu_and_init_level(menu, &level);
  delwin(menu);

  generate_level(&level);

  pacman_t pacman = {0};
  init_pacman(&pacman, &level);

  FILE *f = log_init(&level, &pacman);

  WINDOW *gamemap =
      newwin(level.height, level.width, (LINES - level.height) / 2,
             (COLS - level.width) / 2);
  wborder(gamemap, '#', '#', '#', '#', '#', '#', '#', '#');

  WINDOW *sidebar = newwin(4, 15, 0, 0);

  draw_game(gamemap, sidebar, &level, &pacman, f);

  delwin(gamemap);
  delwin(sidebar);
  free_level(&level);
  fclose(f);
  endwin();
  return 0;
}