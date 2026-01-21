#include "tetris.h"

static void	draw_cell(int value)
{
	if (value == 0)
		write(1, "  ", 2);
	else
		write(1, "██", 6);
}

static void	render_player_board(t_tetris *t, int offset_x)
{
	char	buffer[100];
	int		i;
	int		j;
	int		len;

	i = 0;
	while (i < t->board_h)
	{
		len = snprintf(buffer, sizeof(buffer), "\033[%d;%dH", i + 3, offset_x);
		write(1, buffer, len);
		write(1, "║", 3);
		j = 0;
		while (j < t->board_w)
		{
			draw_cell(t->board[i][j]);
			j++;
		}
		write(1, "║", 3);
		i++;
	}
}

static void	render_player_piece(t_tetris *t, int offset_x, int is_ghost)
{
	char	buffer[100];
	int		i;
	int		j;
	int		y_pos;
	int		len;

	y_pos = t->pos_y;
	if (is_ghost)
	{
		while (!check_collision(t, t->pos_x, y_pos + 1, t->rotation))
			y_pos++;
	}
	i = 0;
	while (i < 4)
	{
		j = 0;
		while (j < 4)
		{
			if (t->pieces[t->current_piece][t->rotation][i][j])
			{
				len = snprintf(buffer, sizeof(buffer), "\033[%d;%dH",
					y_pos + i + 3, offset_x + 3 + (t->pos_x + j) * 2);
				write(1, buffer, len);
				if (is_ghost)
					write(1, "░░", 6);
				else
					write(1, "██", 6);
			}
			j++;
		}
		i++;
	}
}

static void	render_player_info(t_tetris *t, int offset_x, int offset_y,
	const char *label)
{
	char	buffer[100];
	int		len;

	len = snprintf(buffer, sizeof(buffer), "\033[%d;%dH%s",
		offset_y, offset_x, label);
	write(1, buffer, len);
	len = snprintf(buffer, sizeof(buffer), "\033[%d;%dHScore: %d",
		offset_y + 2, offset_x, t->score);
	write(1, buffer, len);
	len = snprintf(buffer, sizeof(buffer), "\033[%d;%dHLevel: %d",
		offset_y + 3, offset_x, t->level);
	write(1, buffer, len);
	len = snprintf(buffer, sizeof(buffer), "\033[%d;%dHLines: %d",
		offset_y + 4, offset_x, t->lines);
	write(1, buffer, len);
}

static void	render_opponent_info(t_opponent_state *opp, int offset_x, int offset_y)
{
	char	buffer[100];
	int		len;

	len = snprintf(buffer, sizeof(buffer), "\033[%d;%dH--- OPPONENT ---",
		offset_y, offset_x);
	write(1, buffer, len);
	len = snprintf(buffer, sizeof(buffer), "\033[%d;%dHScore: %d",
		offset_y + 2, offset_x, opp->score);
	write(1, buffer, len);
	len = snprintf(buffer, sizeof(buffer), "\033[%d;%dHLevel: %d",
		offset_y + 3, offset_x, opp->level);
	write(1, buffer, len);
	len = snprintf(buffer, sizeof(buffer), "\033[%d;%dHLines: %d",
		offset_y + 4, offset_x, opp->lines);
	write(1, buffer, len);
	if (opp->game_over)
	{
		len = snprintf(buffer, sizeof(buffer), "\033[%d;%dH[GAME OVER]",
			offset_y + 6, offset_x);
		write(1, buffer, len);
	}
}

void	render_multiplayer(t_tetris *t)
{
	int	board_offset;
	int	info_offset;

	board_offset = 5;
	info_offset = 30;

	write(1, CLEAR, 4);
	write(1, HOME, 3);
	write(1, "\n", 1);
	write(1, "            MULTIPLAYER MODE\n\n", 31);

	render_player_board(t, board_offset);
	if (!t->game_over)
	{
		render_player_piece(t, board_offset, 1);
		render_player_piece(t, board_offset, 0);
	}
	render_player_info(t, info_offset, 5, "--- YOU ---");
	render_opponent_info(&t->opponent, info_offset, 12);

	if (t->game_over)
	{
		write(1, "\033[25;10H", 9);
		write(1, "YOU LOST! Press ENTER to continue...", 37);
	}
	else if (t->opponent.game_over)
	{
		write(1, "\033[25;10H", 9);
		write(1, "YOU WON! Press ENTER to continue...", 36);
	}
}
