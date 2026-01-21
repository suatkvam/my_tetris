/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_tetris.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akivam <akivam@student.42istanbul.com.tr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 19:36:28 by akivam            #+#    #+#             */
/*   Updated: 2026/01/18 19:36:28 by akivam           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tetris.h"

static int	**allocate_board(int h, int w)
{
	int	**board;
	int	i;
	int	j;

	board = (int **)gc_malloc(NULL, sizeof(int *) * h);
	i = 0;
	while (i < h)
	{
		board[i] = (int *)gc_malloc(NULL, sizeof(int) * w);
		j = 0;
		while (j < w)
		{
			board[i][j] = 0;
			j++;
		}
		i++;
	}
	return (board);
}

void	init_tetris(t_tetris *t, void *arena, t_settings *settings)
{
	(void)arena;
	t->board_w = 10;
	t->board_h = 20;
	t->board = allocate_board(t->board_h, t->board_w);
	t->pieces = get_tetromino_shapes();
	t->score = 0;
	t->level = 1;
	t->lines = 0;
	t->tick = 0;
	t->settings = settings;
	t->speed = calc_speed(t->level, settings);
	t->random_number_seed = get_random_seed();
	t->running = 1;
	t->paused = 0;
	t->menu_selection = 0;
	t->game_over = 0;
	t->mode = MODE_MENU;
	t->network = NULL;
	t->is_multiplayer = 0;
	t->opponent.pos_x = 0;
	t->opponent.pos_y = 0;
	t->opponent.rotation = 0;
	t->opponent.current_piece = 0;
	t->opponent.score = 0;
	t->opponent.lines = 0;
	t->opponent.level = 1;
	t->opponent.game_over = 0;
	t->lock_delay_counter = 0;
	t->next_piece = xorshift32(&t->random_number_seed) % 7;
	spawn_piece(t);
}

void	reset_game(t_tetris *t)
{
	int	i;
	int	j;

	i = 0;
	while (i < t->board_h)
	{
		j = 0;
		while (j < t->board_w)
		{
			t->board[i][j] = 0;
			j++;
		}
		i++;
	}
	t->score = 0;
	t->level = 1;
	t->lines = 0;
	t->tick = 0;
	t->speed = calc_speed(t->level, t->settings);
	t->random_number_seed = get_random_seed();
	t->paused = 0;
	t->game_over = 0;
	t->opponent.pos_x = 0;
	t->opponent.pos_y = 0;
	t->opponent.rotation = 0;
	t->opponent.current_piece = 0;
	t->opponent.score = 0;
	t->opponent.lines = 0;
	t->opponent.level = 1;
	t->opponent.game_over = 0;
	t->lock_delay_counter = 0;
	t->next_piece = xorshift32(&t->random_number_seed) % 7;
	spawn_piece(t);
}
