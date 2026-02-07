/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_tetris.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: harici <harici@student.42istanbul.com.tr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 14:07:12 by harici            #+#    #+#             */
/*   Updated: 2026/02/07 14:07:13 by harici           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tetris.h"

static int	**allocate_board(int h, int w)
{
	int	**board;
	int	i;
	int	j;

	/* TODO: Restore gc_malloc(arena, ...) for Minishell integration */
	board = (int **)malloc(sizeof(int *) * h);
	if (!board)
		return (NULL);
	i = 0;
	while (i < h)
	{
		/* TODO: Restore gc_malloc(arena, ...) for Minishell integration */
		board[i] = (int *)malloc(sizeof(int) * w);
		if (!board[i])
			return (NULL);
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

void	init_tetris(t_tetris *t, void *arena)
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
	t->speed = calc_speed(t->level);
	t->random_number_seed = get_random_seed();
	t->running = 1;
	t->paused = 0;
	t->menu_selection = 0;
	t->game_over = 0;
	t->next_piece = xorshift32(&t->random_number_seed) % 7;
	spawn_piece(t);
}
