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
		if (t->paused && y == 8)
			append_string(buffer, &pos, "      PAUSED      ");
		else if (t->paused && y == 10)
		{
			if (t->menu_selection == 0)
				append_string(buffer, &pos, "     > Return     ");
			else
				append_string(buffer, &pos, "       Return     ");
		}
		else if (t->paused && y == 11)
		{
			if (t->menu_selection == 1)
				append_string(buffer, &pos, "     > Exit       ");
			else
				append_string(buffer, &pos, "       Exit       ");
		}
		else if (t->paused && (y == 9 || y == 12))
			append_string(buffer, &pos, "                  ");
		else
		{
			x = 0;
			while (x < t->board_w)
			{
				if (t->board[y][x] || check_active_piece(t, x, y))
					append_string(buffer, &pos, "[]");
				else
					append_string(buffer, &pos, "  ");
				x++;
			}
		}
		append_string(buffer, &pos, "|");
		
		// Right Side Info (Next Piece)
		if (y == 0)
			append_string(buffer, &pos, " Next:");
		else if (y >= 2 && y < 6)
		{
			append_string(buffer, &pos, " ");
			int py = y - 2;
			int px = 0;
			while (px < 4)
			{
				if (t->pieces[t->next_piece][0][py][px])
					append_string(buffer, &pos, "[]");
				else
					append_string(buffer, &pos, "  ");
				px++;
			}
		}
		
		append_string(buffer, &pos, "\033[K\n");
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
	append_string(buffer, &pos, "+\033[K\n");

	// Score and Level
	append_string(buffer, &pos, "Score: ");
	append_int(buffer, &pos, t->score);
	append_string(buffer, &pos, "\033[K\nLevel: ");
	append_int(buffer, &pos, t->level);
	append_string(buffer, &pos, "\033[K\n");

	write(STDOUT_FILENO, buffer, pos);
}
