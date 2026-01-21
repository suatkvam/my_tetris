#include "tetris.h"

static void render_player_board_2p(t_tetris *t, int offset_x) {
  char buffer[100];
  int i;
  int j;
  int len;

  len = snprintf(buffer, sizeof(buffer), "\033[3;%dH", offset_x);
  write(1, buffer, len);
  write(1, "╔══════════════════════╗\n", 29);
  i = 0;
  while (i < t->board_h) {
    len = snprintf(buffer, sizeof(buffer), "\033[%d;%dH", i + 4, offset_x);
    write(1, buffer, len);
    write(1, "║", 3);
    j = 0;
    while (j < t->board_w) {
      if (t->board[i][j] == 0)
        write(1, "  ", 2);
      else
        write(1, "██", 6);
      j++;
    }
    write(1, "║", 3);
    i++;
  }
  len = snprintf(buffer, sizeof(buffer), "\033[%d;%dH", i + 4, offset_x);
  write(1, buffer, len);
  write(1, "╚══════════════════════╝\n", 29);
}

static void render_player_piece_2p(t_tetris *t, int offset_x) {
  char buffer[100];
  int i;
  int j;
  int len;

  i = 0;
  while (i < 4) {
    j = 0;
    while (j < 4) {
      if (t->pieces[t->current_piece][t->rotation][i][j]) {
        len = snprintf(buffer, sizeof(buffer), "\033[%d;%dH", t->pos_y + i + 4,
                       offset_x + 3 + (t->pos_x + j) * 2);
        write(1, buffer, len);
        write(1, "██", 6);
      }
      j++;
    }
    i++;
  }
}

static void render_player_info_2p(t_tetris *t, int offset_x, const char *name) {
  char buffer[100];
  int len;

  len = snprintf(buffer, sizeof(buffer), "\033[1;%dH%s", offset_x, name);
  write(1, buffer, len);
  len = snprintf(buffer, sizeof(buffer), "\033[2;%dHScore: %d", offset_x,
                 t->score);
  write(1, buffer, len);
  len = snprintf(buffer, sizeof(buffer), "\033[3;%dHLevel: %d  Lines: %d",
                 offset_x, t->level, t->lines);
  write(1, buffer, len);
}

void render_local_2p(t_shell *shell) {
  int p1_offset;
  int p2_offset;

  p1_offset = 5;
  p2_offset = 50;

  write(1, CLEAR, 4);
  write(1, HOME, 3);

  render_player_info_2p(shell->tetris, p1_offset, "PLAYER 1 (WASD)");
  render_player_info_2p(shell->player2, p2_offset, "PLAYER 2 (ARROWS)");

  render_player_board_2p(shell->tetris, p1_offset);
  render_player_board_2p(shell->player2, p2_offset);

  if (!shell->tetris->game_over)
    render_player_piece_2p(shell->tetris, p1_offset);
  if (!shell->player2->game_over)
    render_player_piece_2p(shell->player2, p2_offset);

  if (shell->tetris->game_over || shell->player2->game_over) {
    write(1, "\033[26;20H", 8);
    if (shell->tetris->game_over && !shell->player2->game_over)
      write(1, "PLAYER 2 WINS! Press ENTER...", 30);
    else if (shell->player2->game_over && !shell->tetris->game_over)
      write(1, "PLAYER 1 WINS! Press ENTER...", 30);
    else
      write(1, "DRAW! Both lost! Press ENTER...", 32);
  } else {
    write(1, "\033[26;5H", 7);
    write(1, "P1: SPACE=Drop | P2: ENTER=Drop", 32);
  }
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
