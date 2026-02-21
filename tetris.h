#ifndef TETRIS_H
# define TETRIS_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <termios.h>
# include <time.h>
# include <string.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <errno.h>
# include <ifaddrs.h>
# include <net/if.h>

# define KEY_ROTATE 'w'
# define KEY_LEFT 'a'
# define KEY_RIGHT 'd'
# define KEY_SOFT_DROP 's'
# define KEY_HARD_DROP ' '
# define KEY_QUIT 'q'
# define KEY_ESC 27
# define KEY_CTRL_C 3
# define KEY_CTRL_D 4
# define KEY_ARROW_UP 'A'
# define KEY_ARROW_DOWN 'B'
# define KEY_ARROW_RIGHT 'C'
# define KEY_ARROW_LEFT 'D'

# define HOME "\033[H"
# define CLEAR "\033[2J"

# define MAX_NAME_LEN 16
# define MAX_SCORES 10
# define SCORES_FILE ".tetris_scores.dat"

# define SPEED_BASE_DEFAULT 12
# define SPEED_MIN_DEFAULT 3
# define SPEED_LEVEL_STEP_DEFAULT 5
# define LOCK_DELAY_DEFAULT 1
# define GHOST_PIECE_DEFAULT 1

typedef enum e_game_mode
{
	MODE_MENU,
	MODE_SINGLE_PLAYER,
	MODE_LOCAL_2P,
	MODE_MULTIPLAYER_MENU,
	MODE_MULTIPLAYER_HOST,
	MODE_MULTIPLAYER_JOIN,
	MODE_MULTIPLAYER_GAME,
	MODE_HIGH_SCORES,
	MODE_SETTINGS
}	t_game_mode;

typedef struct s_score_entry
{
	char	name[MAX_NAME_LEN + 1];
	int		score;
	int		level;
	time_t	timestamp;
}	t_score_entry;

typedef struct s_network
{
	int		socket_fd;
	int		is_host;
	int		connected;
	char	opponent_ip[16];
	int		port;
	time_t	start_time;
}	t_network;

typedef struct s_opponent_state
{
	int		pos_x;
	int		pos_y;
	int		rotation;
	int		current_piece;
	int		score;
	int		lines;
	int		level;
	int		game_over;
}	t_opponent_state;

typedef struct s_settings
{
	int		speed_base;
	int		speed_min;
	int		speed_level_step;
	int		lock_delay;
	int		ghost_piece_enabled;
}	t_settings;

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
	t_game_mode     mode;
	t_network       *network;
	int             is_multiplayer;
	t_opponent_state opponent;
	int             lock_delay_counter;
	t_settings      *settings;
}   t_tetris;

typedef struct s_shell
{
	t_tetris    *tetris;
	t_tetris    *player2;
	t_settings  settings;
	void        *cmd_arena;
}   t_shell;

void    init_tetris(t_tetris *t, void *arena, t_settings *settings);
void    reset_game(t_tetris *t);
int     calc_speed(int level, t_settings *settings);
void    init_settings(t_settings *settings);
void    clear_lines(t_tetris *t);
int     check_collision(t_tetris *t, int new_x, int new_y, int new_rot);
void    lock_piece(t_tetris *t);
void    render(t_tetris *t);
void    render_multiplayer(t_tetris *t);
void    spawn_piece(t_tetris *t);
void    rotate_piece(t_tetris *t);
void    move_left(t_tetris *t);
void    move_right(t_tetris *t);
void    soft_drop(t_tetris *t);
void    hard_drop(t_tetris *t);
const int (*get_tetromino_shapes(void))[4][4][4];
void    setup_tetris_terminal(struct termios *old);
void    simple_delay(void);
unsigned int xorshift32(unsigned int *state);
unsigned int get_random_seed(void);

/* Menu functions */
void    render_main_menu(t_tetris *t);
void    handle_menu_input(t_tetris *t, char c);
void    handle_menu_escape(t_tetris *t);

/* Settings functions */
void    render_settings(t_shell *shell);
void    handle_settings_input(t_shell *shell, char c);
void    handle_settings_escape(t_shell *shell);
void    handle_settings_decrease(t_shell *shell);
void    handle_settings_increase(t_shell *shell);

/* Scoreboard functions */
int     load_scores(t_score_entry scores[MAX_SCORES]);
void    save_scores(t_score_entry scores[MAX_SCORES], int count);
void    add_score(int score, int level, const char *name);
void    render_high_scores(t_tetris *t);
int     read_name_input(char *name, int max_len);

/* Network functions */
int     get_local_ip(char *buffer, int size);
t_network *init_network(void);
int     network_host(t_network *net, int port);
int     network_accept(t_network *net);
int     network_join(t_network *net, const char *ip, int port);
void    network_close(t_network *net);
int     network_send_state(t_network *net, t_tetris *t);
int     network_send_game_over(t_network *net, int final_score);
int     network_receive(t_network *net, char *buffer, int size);

/* Local 2P functions */
void    render_local_2p(t_shell *shell);
void    init_player2(t_shell *shell);
void    update_local_2p(t_shell *shell);

/* Multiplayer menu functions */
void    render_multiplayer_menu(t_tetris *t);
void    handle_multiplayer_menu_input(t_tetris *t, char c);
int     read_ip_input(char *ip, int max_len);
void    render_host_waiting(t_tetris *t);
void    render_join_connecting(t_tetris *t);
void    render_connection_failed(void);

/* Macros for compatibility */
# define gc_malloc(arena, size) malloc(size)

#endif
