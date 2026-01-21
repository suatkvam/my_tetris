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

static int	get_ghost_y(t_tetris *t)
{
	int	ghost_y;

	ghost_y = t->pos_y;
	while (!check_collision(t, t->pos_x, ghost_y + 1, t->rotation))
		ghost_y++;
	return (ghost_y);
}

static int	check_ghost_piece(t_tetris *t, int x, int y, int ghost_y)
{
	int	px;
	int	py;

	py = y - ghost_y;
	px = x - t->pos_x;
	if (px >= 0 && px < 4 && py >= 0 && py < 4)
	{
		if (t->pieces[t->current_piece][t->rotation][py][px])
			return (1);
	}
	return (0);
}

static const char	*get_random_quote(t_tetris *t)
{
	static const char *quotes[] = {
		"Ramen! 🍜",
		"Makarna, Sos ve Kutsal Köfte adına. 🍝",
		"Kutsal Makarna seni sarsın. 🧶",
		"Onun makarnamsı uzantıları üzerinizde olsun. 🐙",
		"Gerçekten de O, bizim için kaynatmıştır. 🥘",
		"Korsanlar kutsaldır, unutma. 🏴‍☠️",
		"Uçan Spagetti Canavarı seni seviyor. 🛸",
		"Soslu günler dilerim. 🍅",
		"Makarna cennetine hoş geldin. ☁️",
		"Biramin! 🍺"
	};
	int count = 10;
	int val;

	if (t->score % 100 == 0)
		val = t->score / 100;
	else
		val = t->score;
	
	int index = (val + t->lines) % count;
	return (quotes[index]);
}

static void	render_game_over(t_tetris *t)
{
	char	buffer[BUFFER_SIZE];
	int		pos;
	int		i;
	static const char *art[] = {
		"⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬛⬛⬜⬜⬜⬜⬛⬛⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜",
		"⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬜⬜⬜⬛⬜⬜⬛⬜⬜⬜⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜",
		"⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬜⬛⬜⬛⬜⬜⬛⬜⬛⬜⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜",
		"⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬜⬜⬜⬛⬜⬜⬛⬜⬜⬜⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜",
		"⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬛⬛⬜⬜⬜⬜⬛⬛⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜",
		"⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬜⬜⬜⬜⬜⬜⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜",
		"⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬜⬜⬜⬜⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜",
		"⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬛⬛⬛⬛⬛⬛⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜",
		"⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬛⬜⬜⬜⬜⬜⬜⬜⬜⬛⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜",
		"⬜⬜⬜⬛⬛⬜⬜⬜⬜⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬜⬜⬜⬜⬛⬛⬜⬜⬜",
		"⬜⬜⬛⬜⬜⬛⬜⬛⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬛⬜⬛⬜⬜⬛⬜⬜",
		"⬛⬜⬜⬛⬜⬜⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬜⬜⬛⬜⬜⬛",
		"⬜⬛⬛⬜⬜⬜⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬜⬜⬜⬛⬛⬜",
		"⬜⬜⬜⬜⬜⬜⬜⬛⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬛⬜⬜⬜⬜⬜⬜⬜",
		"⬜⬜⬜⬜⬜⬜⬜⬛⬜⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬜⬛⬜⬜⬜⬜⬜⬜⬜",
		"⬜⬜⬜⬜⬜⬛⬛⬜⬜⬜⬛⬛⬜⬜⬜⬜⬜⬜⬜⬜⬛⬛⬜⬜⬜⬛⬛⬜⬜⬜⬜⬜",
		"⬜⬜⬜⬜⬛⬜⬜⬜⬜⬜⬜⬛⬛⬛⬛⬛⬛⬛⬛⬛⬛⬜⬜⬜⬜⬜⬜⬛⬜⬜⬜⬜",
		"⬜⬜⬛⬜⬛⬜⬜⬜⬜⬜⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬜⬜⬜⬜⬜⬛⬜⬛⬜⬜",
		"⬜⬜⬜⬛⬜⬜⬜⬜⬜⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬜⬜⬜⬜⬜⬛⬜⬜⬜",
		"⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜",
		"⬜⬜⬜⬜⬜⬜⬜⬛⬛⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜⬛⬛⬜⬜⬜⬜⬜⬜⬜",
		NULL
	};

	pos = 0;
	append_string(buffer, &pos, HOME);
	i = 0;
	while (art[i])
	{
		append_string(buffer, &pos, art[i]);
		append_string(buffer, &pos, "\033[K\n");
		i++;
	}
	append_string(buffer, &pos, "\nFinal Score: ");
	append_int(buffer, &pos, t->score);
	append_string(buffer, &pos, " - ");
	append_string(buffer, &pos, get_random_quote(t));
	append_string(buffer, &pos, "\033[K\n");
	write(STDOUT_FILENO, buffer, pos);
	t->running = 0;
}

void	render(t_tetris *t)
{
	char	buffer[BUFFER_SIZE];
	int		pos;
	int		x;
	int		y;
	int		i;
	int		ghost_y;

	if (t->game_over)
	{
		render_game_over(t);
		return ;
	}

	pos = 0;
	ghost_y = get_ghost_y(t);
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
			append_string(buffer, &pos, "       PAUSED       ");
		else if (t->paused && y == 10)
		{
			if (t->menu_selection == 0)
				append_string(buffer, &pos, "      > Return      ");
			else
				append_string(buffer, &pos, "        Return      ");
		}
		else if (t->paused && y == 11)
		{
			if (t->menu_selection == 1)
				append_string(buffer, &pos, "      > Exit        ");
			else
				append_string(buffer, &pos, "        Exit        ");
		}
		else if (t->paused && (y == 9 || y == 12))
			append_string(buffer, &pos, "                    ");
		else
		{
			x = 0;
			while (x < t->board_w)
			{
				if (t->board[y][x])
					append_string(buffer, &pos, "[]");
				else if (check_active_piece(t, x, y))
					append_string(buffer, &pos, "[]");
				else if (check_ghost_piece(t, x, y, ghost_y))
					append_string(buffer, &pos, "::");
				else
					append_string(buffer, &pos, "  ");
				x++;
			}
		}
		append_string(buffer, &pos, "|");
		
		// Right Side Info (Next Piece, Score, Level)
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
		else if (y == 8)
			append_string(buffer, &pos, " Score:");
		else if (y == 9)
		{
			append_string(buffer, &pos, " ");
			append_int(buffer, &pos, t->score);
		}
		else if (y == 11)
			append_string(buffer, &pos, " Level:");
		else if (y == 12)
		{
			append_string(buffer, &pos, " ");
			append_int(buffer, &pos, t->level);
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

	write(STDOUT_FILENO, buffer, pos);
}
