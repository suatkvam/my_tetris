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
		if (seq[1] == 'A') // Up
			t->menu_selection = 0;
		else if (seq[1] == 'B') // Down
			t->menu_selection = 1;
	}
	else
	{
		// Game navigation
		if (seq[1] == 'A')
			rotate_piece(t);
		else if (seq[1] == 'B')
			soft_drop(t);
		else if (seq[1] == 'C')
			move_right(t);
		else if (seq[1] == 'D')
			move_left(t);
	}
}

void	handle_input(t_shell *shell)
{
	t_tetris	*t;
	char		c;

	t = shell->tetris;
	while (read(STDIN_FILENO, &c, 1) == 1)
	{
		if (c == KEY_CTRL_C || c == KEY_CTRL_D || c == KEY_QUIT)
		{
			t->running = 0;
			return ;
		}
		if (t->game_over)
		{
			if (c == KEY_ESC || c == '\n' || c == ' ')
				t->running = 0;
			return ;
		}
		if (c == KEY_ESC)
			handle_escape(t);
		else if (t->paused)
		{
			if (c == 'w') t->menu_selection = 0;
			if (c == 's') t->menu_selection = 1;
			if (c == '\n' || c == ' ') // Enter or Space
			{
				if (t->menu_selection == 0) // Return
					t->paused = 0;
				else if (t->menu_selection == 1) // Exit
					t->running = 0;
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
}

void	update_game(t_shell *shell)
{
	t_tetris	*t;

	t = shell->tetris;
	if (t->paused || t->game_over)
		return ;
	t->tick++;
	t->speed = calc_speed(t->level);
	if (t->tick >= t->speed)
	{
		if (!check_collision(t, t->pos_x, t->pos_y + 1, t->rotation))
			t->pos_y++;
		else
			lock_piece(t);
		t->tick = 0;
	}
}

static void	game_loop(t_shell *shell)
{
	while (shell->tetris->running)
	{
		handle_input(shell);
		update_game(shell);
		render(shell->tetris);
		simple_delay();
	}
}

void	start_game(char **args, t_shell *shell)
{
	struct termios	old;

	(void)args;
	setup_tetris_terminal(&old);
	write(1, CLEAR, 4);
	shell->tetris = gc_malloc(shell->cmd_arena, sizeof(t_tetris));
	init_tetris(shell->tetris, shell->cmd_arena);
	game_loop(shell);
	tcsetattr(STDIN_FILENO, TCSANOW, &old);
	// Show cursor
	write(1, "\033[?25h", 6);
}
