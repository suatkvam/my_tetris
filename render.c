/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akivam <akivam@student.42istanbul.com.tr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 19:36:28 by akivam            #+#    #+#             */
/*   Updated: 2026/01/18 19:36:28 by akivam           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tetris.h"

#define BUFFER_SIZE 4096

static void	append_string(char *buffer, int *pos, const char *str)
{
	while (*str && *pos < BUFFER_SIZE - 1)
		buffer[(*pos)++] = *str++;
	buffer[*pos] = '\0';
}

static void	append_int(char *buffer, int *pos, int n)
{
	char	temp[16];
	int		i;

	if (n == 0)
	{
		append_string(buffer, pos, "0");
		return ;
	}
	i = 0;
	while (n > 0)
	{
		temp[i++] = (n % 10) + '0';
		n /= 10;
	}
	while (i > 0)
	{
		if (*pos < BUFFER_SIZE - 1)
			buffer[(*pos)++] = temp[--i];
		else
			break;
	}
}

static int	check_active_piece(t_tetris *t, int x, int y)
{
	int	px;
	int	py;

	py = y - t->pos_y;
	px = x - t->pos_x;
	if (px >= 0 && px < 4 && py >= 0 && py < 4)
	{
		if (t->pieces[t->current_piece][t->rotation][py][px])
			return (1);
	}
	return (0);
}

void	render(t_tetris *t)
{
	char	buffer[BUFFER_SIZE];
	int		pos;
	int		x;
	int		y;
	int		i;

	pos = 0;
	append_string(buffer, &pos, HOME);

	// Top Border
	append_string(buffer, &pos, "  +");
	i = 0;
	while (i < t->board_w)
	{
		append_string(buffer, &pos, "--");
		i++;
	}
	append_string(buffer, &pos, "+\n");

	// Board Rows
	y = 0;
	while (y < t->board_h)
	{
		append_string(buffer, &pos, "  |");
		x = 0;
		while (x < t->board_w)
		{
			if (t->board[y][x] || check_active_piece(t, x, y))
				append_string(buffer, &pos, "[]");
			else
				append_string(buffer, &pos, "  ");
			x++;
		}
		append_string(buffer, &pos, "|\n");
		y++;
	}

	// Bottom Border
	append_string(buffer, &pos, "  +");
	i = 0;
	while (i < t->board_w)
	{
		append_string(buffer, &pos, "--");
		i++;
	}
	append_string(buffer, &pos, "+\n");

	// Score and Level
	append_string(buffer, &pos, "Score: ");
	append_int(buffer, &pos, t->score);
	append_string(buffer, &pos, "\nLevel: ");
	append_int(buffer, &pos, t->level);
	append_string(buffer, &pos, "\n");

	write(STDOUT_FILENO, buffer, pos);
}
