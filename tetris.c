/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tetris.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akivam <akivam@student.42istanbul.com.tr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 19:36:28 by akivam            #+#    #+#             */
/*   Updated: 2026/01/18 19:36:28 by akivam           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tetris.h"

static void	handle_escape(t_tetris *t)
{
	char	seq[2];

	if (read(STDIN_FILENO, &seq[0], 1) != 1)
	{
		// Only ESC pressed
		if (t->paused)
			t->paused = 0; // Resume
		else
			t->paused = 1; // Pause
		return ;
	}
	if (seq[0] != '[')
		return ;
	if (read(STDIN_FILENO, &seq[1], 1) != 1)
		return ;
	
	if (t->paused)
	{
		// Menu navigation
		if (seq[1] == KEY_ARROW_UP)
			t->menu_selection = 0;
		else if (seq[1] == KEY_ARROW_DOWN)
			t->menu_selection = 1;
	}
	else
	{
		// Game navigation
		if (seq[1] == KEY_ARROW_UP)
			rotate_piece(t);
		else if (seq[1] == KEY_ARROW_DOWN)
			soft_drop(t);
		else if (seq[1] == KEY_ARROW_RIGHT)
			move_right(t);
		else if (seq[1] == KEY_ARROW_LEFT)
			move_left(t);
	}
}

static void	handle_game_input(t_tetris *t, char c)
{
	if (c == KEY_ESC)
		handle_escape(t);
	else if (t->paused)
	{
		if (c == 'w') t->menu_selection = 0;
		if (c == 's') t->menu_selection = 1;
		if (c == '\n' || c == ' ')
		{
			if (t->menu_selection == 0)
				t->paused = 0;
			else if (t->menu_selection == 1)
			{
				t->mode = MODE_MENU;
				t->menu_selection = 0;
			}
		}
	}
	else
	{
		if (c == KEY_ROTATE)
			rotate_piece(t);
		else if (c == KEY_SOFT_DROP)
			soft_drop(t);
		else if (c == KEY_LEFT)
			move_left(t);
		else if (c == KEY_RIGHT)
			move_right(t);
		else if (c == KEY_HARD_DROP)
			hard_drop(t);
	}
}

static void	handle_local_2p_escape(t_shell *shell)
{
	char	seq[2];

	if (read(STDIN_FILENO, &seq[0], 1) != 1)
		return ;
	if (seq[0] != '[')
		return ;
	if (read(STDIN_FILENO, &seq[1], 1) != 1)
		return ;

	if (!shell->player2->game_over)
	{
		if (seq[1] == KEY_ARROW_UP)
			rotate_piece(shell->player2);
		else if (seq[1] == KEY_ARROW_DOWN)
			soft_drop(shell->player2);
		else if (seq[1] == KEY_ARROW_LEFT)
			move_left(shell->player2);
		else if (seq[1] == KEY_ARROW_RIGHT)
			move_right(shell->player2);
	}
}

static void	handle_local_2p_input(t_shell *shell, char c)
{
	t_tetris	*p1;
	t_tetris	*p2;

	p1 = shell->tetris;
	p2 = shell->player2;

	if (c == KEY_ESC)
		handle_local_2p_escape(shell);
	else if (c == 'w' && !p1->game_over)
		rotate_piece(p1);
	else if (c == 'a' && !p1->game_over)
		move_left(p1);
	else if (c == 'd' && !p1->game_over)
		move_right(p1);
	else if (c == 's' && !p1->game_over)
		soft_drop(p1);
	else if (c == ' ' && !p1->game_over)
		hard_drop(p1);
	else if (c == '\n' && !p2->game_over)
		hard_drop(p2);
}

void	handle_input(t_shell *shell)
{
	t_tetris	*t;
	char		c;

	t = shell->tetris;
	while (read(STDIN_FILENO, &c, 1) == 1)
	{
		if (c == KEY_CTRL_C || c == KEY_CTRL_D)
		{
			t->running = 0;
			return ;
		}
		if (t->mode == MODE_MENU)
		{
			if (c == KEY_ESC)
				handle_menu_escape(t);
			else
				handle_menu_input(t, c);
		}
		else if (t->mode == MODE_HIGH_SCORES)
		{
			if (c == KEY_ESC)
			{
				t->mode = MODE_MENU;
				t->menu_selection = 0;
			}
		}
		else if (t->mode == MODE_SETTINGS)
		{
			if (c == KEY_ESC)
				handle_settings_escape(shell);
			else
				handle_settings_input(shell, c);
		}
		else if (t->mode == MODE_MULTIPLAYER_MENU)
		{
			handle_multiplayer_menu_input(t, c);
		}
		else if (t->mode == MODE_MULTIPLAYER_HOST)
		{
			if (c == KEY_ESC)
			{
				if (t->network)
					network_close(t->network);
				t->network = NULL;
				t->mode = MODE_MULTIPLAYER_MENU;
				t->menu_selection = 0;
			}
		}
		else if (t->mode == MODE_LOCAL_2P)
		{
			if (shell->tetris->game_over || shell->player2->game_over)
			{
				if (c == '\n' || c == ' ')
				{
					t->mode = MODE_MENU;
					t->menu_selection = 0;
					reset_game(shell->tetris);
					reset_game(shell->player2);
				}
			}
			else
				handle_local_2p_input(shell, c);
		}
		else if (t->mode == MODE_SINGLE_PLAYER)
		{
			if (t->game_over)
			{
				if (c == KEY_ESC || c == '\n' || c == ' ')
				{
					char name[MAX_NAME_LEN + 1];
					read_name_input(name, MAX_NAME_LEN);
					add_score(t->score, t->level, name);
					t->mode = MODE_MENU;
					t->menu_selection = 0;
					reset_game(t);
				}
			}
			else
				handle_game_input(t, c);
		}
		else if (t->mode == MODE_MULTIPLAYER_GAME)
		{
			if (t->game_over || t->opponent.game_over)
			{
				if (c == '\n' || c == ' ')
				{
					char name[MAX_NAME_LEN + 1];
					read_name_input(name, MAX_NAME_LEN);
					add_score(t->score, t->level, name);
					if (t->network)
						network_close(t->network);
					t->network = NULL;
					t->mode = MODE_MENU;
					t->menu_selection = 0;
					reset_game(t);
				}
			}
			else
				handle_game_input(t, c);
		}
	}
}

static void	handle_host_mode(t_tetris *t)
{
	time_t	current_time;

	if (!t->network)
	{
		t->network = init_network();
		if (!t->network || network_host(t->network, 12345) < 0)
		{
			render_connection_failed();
			if (t->network)
				network_close(t->network);
			t->network = NULL;
			t->mode = MODE_MULTIPLAYER_MENU;
			return ;
		}
	}
	current_time = time(NULL);
	if (current_time - t->network->start_time > 120)
	{
		if (t->network)
			network_close(t->network);
		t->network = NULL;
		write(1, CLEAR, 4);
		write(1, HOME, 3);
		write(1, "\n\n  Connection timeout. Returning to menu...\n", 45);
		simple_delay();
		simple_delay();
		t->mode = MODE_MULTIPLAYER_MENU;
		return ;
	}
	if (network_accept(t->network) == 0)
	{
		t->mode = MODE_MULTIPLAYER_GAME;
		t->is_multiplayer = 1;
		reset_game(t);
	}
}

static void	handle_join_mode(t_tetris *t)
{
	char	ip[16];

	if (read_ip_input(ip, 15) <= 0)
	{
		t->mode = MODE_MULTIPLAYER_MENU;
		return ;
	}
	t->network = init_network();
	if (!t->network || network_join(t->network, ip, 12345) < 0)
	{
		render_connection_failed();
		if (t->network)
			network_close(t->network);
		t->network = NULL;
		t->mode = MODE_MULTIPLAYER_MENU;
		return ;
	}
	t->mode = MODE_MULTIPLAYER_GAME;
	t->is_multiplayer = 1;
	reset_game(t);
}

static void	parse_opponent_state(t_tetris *t, char *buffer)
{
	if (strncmp(buffer, "STATE|", 6) == 0)
	{
		sscanf(buffer + 6, "%d|%d|%d|%d|%d|%d|%d",
			&t->opponent.pos_x, &t->opponent.pos_y,
			&t->opponent.rotation, &t->opponent.current_piece,
			&t->opponent.score, &t->opponent.lines, &t->opponent.level);
	}
	else if (strncmp(buffer, "GAMEOVER|", 9) == 0)
	{
		sscanf(buffer + 9, "%d", &t->opponent.score);
		t->opponent.game_over = 1;
	}
}

static void	handle_multiplayer_network(t_tetris *t)
{
	char	buffer[256];

	if (!t->network || !t->network->connected)
		return ;
	if (network_receive(t->network, buffer, sizeof(buffer)) > 0)
		parse_opponent_state(t, buffer);
	if (t->game_over && !t->opponent.game_over)
		network_send_game_over(t->network, t->score);
	else if (!t->game_over)
		network_send_state(t->network, t);
}

void	update_game(t_shell *shell)
{
	t_tetris	*t;

	t = shell->tetris;
	if (t->mode == MODE_MULTIPLAYER_HOST)
		handle_host_mode(t);
	else if (t->mode == MODE_MULTIPLAYER_JOIN)
		handle_join_mode(t);
	else if (t->mode == MODE_LOCAL_2P)
	{
		if (!shell->player2)
			init_player2(shell);
		update_local_2p(shell);
		return ;
	}
	if (t->mode != MODE_SINGLE_PLAYER && t->mode != MODE_MULTIPLAYER_GAME)
		return ;
	if (t->mode == MODE_MULTIPLAYER_GAME)
		handle_multiplayer_network(t);
	if (t->paused || t->game_over)
		return ;
	t->tick++;
	t->speed = calc_speed(t->level, t->settings);
	if (t->tick >= t->speed)
	{
		if (!check_collision(t, t->pos_x, t->pos_y + 1, t->rotation))
		{
			t->pos_y++;
			t->lock_delay_counter = 0;
		}
		else
		{
			t->lock_delay_counter++;
			if (t->lock_delay_counter >= t->settings->lock_delay)
			{
				lock_piece(t);
				t->lock_delay_counter = 0;
			}
		}
		t->tick = 0;
	}
}

static void	render_current_mode(t_shell *shell)
{
	t_tetris	*t;

	t = shell->tetris;
	if (t->mode == MODE_MENU)
		render_main_menu(t);
	else if (t->mode == MODE_HIGH_SCORES)
		render_high_scores(t);
	else if (t->mode == MODE_SETTINGS)
		render_settings(shell);
	else if (t->mode == MODE_LOCAL_2P)
		render_local_2p(shell);
	else if (t->mode == MODE_MULTIPLAYER_MENU)
		render_multiplayer_menu(t);
	else if (t->mode == MODE_MULTIPLAYER_HOST)
		render_host_waiting(t);
	else if (t->mode == MODE_MULTIPLAYER_JOIN)
		render_join_connecting(t);
	else if (t->mode == MODE_SINGLE_PLAYER)
		render(t);
	else if (t->mode == MODE_MULTIPLAYER_GAME)
		render_multiplayer(t);
}

static void	game_loop(t_shell *shell)
{
	int	frame_counter;

	frame_counter = 0;
	while (shell->tetris->running)
	{
		handle_input(shell);
		update_game(shell);
		if (shell->tetris->mode == MODE_MULTIPLAYER_GAME 
			|| shell->tetris->mode == MODE_LOCAL_2P)
		{
			if (frame_counter % 2 == 0)
				render_current_mode(shell);
		}
		else
			render_current_mode(shell);
		simple_delay();
		frame_counter++;
	}
}

void	start_game(char **args, t_shell *shell)
{
	struct termios	old;

	(void)args;
	setup_tetris_terminal(&old);
	write(1, CLEAR, 4);
	init_settings(&shell->settings);
	shell->tetris = gc_malloc(shell->cmd_arena, sizeof(t_tetris));
	shell->player2 = NULL;
	init_tetris(shell->tetris, shell->cmd_arena, &shell->settings);
	game_loop(shell);
	tcsetattr(STDIN_FILENO, TCSANOW, &old);
	write(1, "\033[?1049l", 8);
	write(1, CLEAR, 4);
	write(1, HOME, 3);
	write(1, "\033[?25h", 6);
}
