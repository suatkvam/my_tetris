#include "tetris.h"

#define RENDER_BUFFER_SIZE 8192

static void append_str(char *buf, int *pos, const char *str) {
  while (*str && *pos < RENDER_BUFFER_SIZE - 1)
    buf[(*pos)++] = *str++;
}

static void render_board_2p_to_buffer(char *buf, int *pos, t_tetris *t,
                                       int offset_x) {
  char line[200];
  int i;
  int j;

  snprintf(line, sizeof(line), "\033[3;%dH╔══════════════════════╗\033[K\n",
           offset_x);
  append_str(buf, pos, line);
  i = 0;
  while (i < t->board_h) {
    snprintf(line, sizeof(line), "\033[%d;%dH║", i + 4, offset_x);
    append_str(buf, pos, line);
    j = 0;
    while (j < t->board_w) {
      if (t->board[i][j] == 0)
        append_str(buf, pos, "  ");
      else
        append_str(buf, pos, "██");
      j++;
    }
    append_str(buf, pos, "║\033[K");
    i++;
  }
  snprintf(line, sizeof(line), "\033[%d;%dH╚══════════════════════╝\033[K\n",
           i + 4, offset_x);
  append_str(buf, pos, line);
}

static void render_piece_2p_to_buffer(char *buf, int *pos, t_tetris *t,
                                       int offset_x) {
  char line[100];
  int i;
  int j;

  i = 0;
  while (i < 4) {
    j = 0;
    while (j < 4) {
      if (t->pieces[t->current_piece][t->rotation][i][j]) {
        snprintf(line, sizeof(line), "\033[%d;%dH██", t->pos_y + i + 4,
                 offset_x + 1 + (t->pos_x + j) * 2);
        append_str(buf, pos, line);
      }
      j++;
    }
    i++;
  }
}

static void render_info_2p_to_buffer(char *buf, int *pos, t_tetris *t,
                                      int offset_x, const char *name) {
  char line[100];

  snprintf(line, sizeof(line), "\033[1;%dH%s\033[K", offset_x, name);
  append_str(buf, pos, line);
  snprintf(line, sizeof(line), "\033[2;%dHScore: %d\033[K", offset_x, t->score);
  append_str(buf, pos, line);
  snprintf(line, sizeof(line), "\033[3;%dHLevel: %d  Lines: %d\033[K",
           offset_x, t->level, t->lines);
  append_str(buf, pos, line);
}

void render_local_2p(t_shell *shell) {
  static char buffer[RENDER_BUFFER_SIZE];
  int pos;
  int p1_offset;
  int p2_offset;

  pos = 0;
  p1_offset = 5;
  p2_offset = 50;

  append_str(buffer, &pos, HOME);

  render_info_2p_to_buffer(buffer, &pos, shell->tetris, p1_offset,
                            "PLAYER 1 (WASD)");
  render_info_2p_to_buffer(buffer, &pos, shell->player2, p2_offset,
                            "PLAYER 2 (ARROWS)");

  render_board_2p_to_buffer(buffer, &pos, shell->tetris, p1_offset);
  render_board_2p_to_buffer(buffer, &pos, shell->player2, p2_offset);

  if (!shell->tetris->game_over)
    render_piece_2p_to_buffer(buffer, &pos, shell->tetris, p1_offset);
  if (!shell->player2->game_over)
    render_piece_2p_to_buffer(buffer, &pos, shell->player2, p2_offset);

  if (shell->tetris->game_over || shell->player2->game_over) {
    append_str(buffer, &pos, "\033[26;20H");
    if (shell->tetris->game_over && !shell->player2->game_over)
      append_str(buffer, &pos, "PLAYER 2 WINS! Press ENTER...\033[K");
    else if (shell->player2->game_over && !shell->tetris->game_over)
      append_str(buffer, &pos, "PLAYER 1 WINS! Press ENTER...\033[K");
    else
      append_str(buffer, &pos, "DRAW! Both lost! Press ENTER...\033[K");
  } else {
    append_str(buffer, &pos, "\033[26;5HP1: SPACE=Drop | P2: ENTER=Drop\033[K");
  }

  write(1, buffer, pos);
}

void init_player2(t_shell *shell) {
  shell->player2 = gc_malloc(shell->cmd_arena, sizeof(t_tetris));
  init_tetris(shell->player2, shell->cmd_arena, &shell->settings);
  shell->player2->mode = MODE_LOCAL_2P;
}

void update_local_2p(t_shell *shell) {
  t_tetris *p1;
  t_tetris *p2;

  p1 = shell->tetris;
  p2 = shell->player2;

  if (!p1->game_over && !p1->paused) {
    p1->tick++;
    p1->speed = calc_speed(p1->level, p1->settings);
    if (p1->tick >= p1->speed) {
      if (!check_collision(p1, p1->pos_x, p1->pos_y + 1, p1->rotation)) {
        p1->pos_y++;
        p1->lock_delay_counter = 0;
      } else {
        p1->lock_delay_counter++;
        if (p1->lock_delay_counter >= p1->settings->lock_delay) {
          lock_piece(p1);
          p1->lock_delay_counter = 0;
        }
      }
      p1->tick = 0;
    }
  }

  if (!p2->game_over && !p2->paused) {
    p2->tick++;
    p2->speed = calc_speed(p2->level, p2->settings);
    if (p2->tick >= p2->speed) {
      if (!check_collision(p2, p2->pos_x, p2->pos_y + 1, p2->rotation)) {
        p2->pos_y++;
        p2->lock_delay_counter = 0;
      } else {
        p2->lock_delay_counter++;
        if (p2->lock_delay_counter >= p2->settings->lock_delay) {
          lock_piece(p2);
          p2->lock_delay_counter = 0;
        }
      }
      p2->tick = 0;
    }
  }
}
