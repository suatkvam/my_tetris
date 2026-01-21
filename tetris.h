#ifndef TETRIS_H
# define TETRIS_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <termios.h>
# include <time.h>
# include <string.h>

# define KEY_ROTATE 'w'
# define KEY_LEFT 'a'
# define KEY_RIGHT 'd'
# define KEY_SOFT_DROP 's'
# define KEY_HARD_DROP ' '
# define KEY_QUIT 'q'
# define KEY_ESC 27
# define KEY_CTRL_C 3
# define KEY_CTRL_D 4

# define HOME "\033[H"
# define CLEAR "\033[2J"

typedef struct s_tetris
{
	int             **board;
	int             board_w;
	int             board_h;
	const int       (*pieces)[4][4][4];
	int             score;
	int             level;
	int             lines;
	int             tick;
	int             speed;
	unsigned int    random_number_seed;
	int             running;
	int             pos_x;
	int             pos_y;
	int             rotation;
	int             current_piece;
	int             next_piece;
	int             paused;
	int             menu_selection;
	int             game_over;
}   t_tetris;

typedef struct s_shell
{
	t_tetris    *tetris;
	void        *cmd_arena;
}   t_shell;

void    init_tetris(t_tetris *t, void *arena);
int     calc_speed(int level);
void    clear_lines(t_tetris *t);
int     check_collision(t_tetris *t, int new_x, int new_y, int new_rot);
void    lock_piece(t_tetris *t);
void    render(t_tetris *t);
void    spawn_piece(t_tetris *t);
void    rotate_piece(t_tetris *t);
void    move_left(t_tetris *t);
void    move_right(t_tetris *t);
void    soft_drop(t_tetris *t);
void    hard_drop(t_tetris *t);
const int (*get_tetromino_shapes(void))[4][4][4];
void    setup_tetris_terminal(struct termios *old);
void    simple_delay(void);
unsigned int simple_rand(unsigned int *seed);

/* Macros for compatibility */
# define gc_malloc(arena, size) malloc(size)

#endif
