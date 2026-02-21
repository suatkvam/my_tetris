#include "tetris.h"

#define RENDER_BUFFER_SIZE 8192

static void	append_str(char *buf, int *pos, const char *str)
{
	while (*str && *pos < RENDER_BUFFER_SIZE - 1)
		buf[(*pos)++] = *str++;
}



static void	render_board_to_buffer(char *buf, int *pos, t_tetris *t, int offset_x)
{
	char	line[200];
	int		i;
	int		j;

	i = 0;
	while (i < t->board_h)
	{
		snprintf(line, sizeof(line), "\033[%d;%dH║", i + 3, offset_x);
		append_str(buf, pos, line);
		j = 0;
		while (j < t->board_w)
		{
			if (t->board[i][j] == 0)
				append_str(buf, pos, "  ");
			else
				append_str(buf, pos, "██");
			j++;
		}
		append_str(buf, pos, "║\033[K\n");
		i++;
	}
}

static void	render_piece_to_buffer(char *buf, int *pos, t_tetris *t,
	int offset_x, int is_ghost)
{
	char	line[100];
	int		i;
	int		j;
	int		y_pos;

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
				snprintf(line, sizeof(line), "\033[%d;%dH",
					y_pos + i + 3, offset_x + 1 + (t->pos_x + j) * 2);
				append_str(buf, pos, line);
				if (is_ghost)
					append_str(buf, pos, "░░");
				else
					append_str(buf, pos, "██");
			}
			j++;
		}
		i++;
	}
}

static void	render_info_to_buffer(char *buf, int *pos, t_tetris *t,
	int offset_x, int offset_y, const char *label)
{
	char	line[100];

	snprintf(line, sizeof(line), "\033[%d;%dH%s\033[K", offset_y, offset_x, label);
	append_str(buf, pos, line);
	snprintf(line, sizeof(line), "\033[%d;%dHScore: %d\033[K", offset_y + 2, offset_x, t->score);
	append_str(buf, pos, line);
	snprintf(line, sizeof(line), "\033[%d;%dHLevel: %d\033[K", offset_y + 3, offset_x, t->level);
	append_str(buf, pos, line);
	snprintf(line, sizeof(line), "\033[%d;%dHLines: %d\033[K", offset_y + 4, offset_x, t->lines);
	append_str(buf, pos, line);
}

static void	render_opponent_to_buffer(char *buf, int *pos,
	t_opponent_state *opp, int offset_x, int offset_y)
{
	char	line[100];

	snprintf(line, sizeof(line), "\033[%d;%dH--- OPPONENT ---\033[K", offset_y, offset_x);
	append_str(buf, pos, line);
	snprintf(line, sizeof(line), "\033[%d;%dHScore: %d\033[K", offset_y + 2, offset_x, opp->score);
	append_str(buf, pos, line);
	snprintf(line, sizeof(line), "\033[%d;%dHLevel: %d\033[K", offset_y + 3, offset_x, opp->level);
	append_str(buf, pos, line);
	snprintf(line, sizeof(line), "\033[%d;%dHLines: %d\033[K", offset_y + 4, offset_x, opp->lines);
	append_str(buf, pos, line);
	if (opp->game_over)
	{
		snprintf(line, sizeof(line), "\033[%d;%dH[GAME OVER]\033[K", offset_y + 6, offset_x);
		append_str(buf, pos, line);
	}
	else
	{
		snprintf(line, sizeof(line), "\033[%d;%dH\033[K", offset_y + 6, offset_x);
		append_str(buf, pos, line);
	}
}

void	render_multiplayer(t_tetris *t)
{
	static char	buffer[RENDER_BUFFER_SIZE];
	int			pos;
	int			board_offset;
	int			info_offset;

	pos = 0;
	board_offset = 5;
	info_offset = 30;

	append_str(buffer, &pos, HOME);
	append_str(buffer, &pos, "\n            MULTIPLAYER MODE\033[K\n\033[K\n");

	render_board_to_buffer(buffer, &pos, t, board_offset);
	if (!t->game_over)
	{
		render_piece_to_buffer(buffer, &pos, t, board_offset, 1);
		render_piece_to_buffer(buffer, &pos, t, board_offset, 0);
	}
	render_info_to_buffer(buffer, &pos, t, info_offset, 5, "--- YOU ---");
	render_opponent_to_buffer(buffer, &pos, &t->opponent, info_offset, 12);

	if (t->game_over)
		append_str(buffer, &pos, "\033[25;10HYOU LOST! Press ENTER to continue...\033[K");
	else if (t->opponent.game_over)
		append_str(buffer, &pos, "\033[25;10HYOU WON! Press ENTER to continue...\033[K");
	else
		append_str(buffer, &pos, "\033[25;10H\033[K");

	write(1, buffer, pos);
}
