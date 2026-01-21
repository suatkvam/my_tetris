#include "tetris.h"

int	load_scores(t_score_entry scores[MAX_SCORES])
{
	FILE	*file;
	int		count;

	file = fopen(SCORES_FILE, "rb");
	if (!file)
		return (0);
	count = fread(scores, sizeof(t_score_entry), MAX_SCORES, file);
	fclose(file);
	return (count);
}

void	save_scores(t_score_entry scores[MAX_SCORES], int count)
{
	FILE	*file;

	if (count > MAX_SCORES)
		count = MAX_SCORES;
	file = fopen(SCORES_FILE, "wb");
	if (!file)
		return ;
	fwrite(scores, sizeof(t_score_entry), count, file);
	fclose(file);
}

static void	insert_score(t_score_entry scores[MAX_SCORES], int *count,
				t_score_entry new_entry)
{
	int	i;
	int	j;

	i = 0;
	while (i < *count && scores[i].score > new_entry.score)
		i++;
	if (i >= MAX_SCORES)
		return ;
	j = ((*count < MAX_SCORES) ? *count : MAX_SCORES - 1);
	while (j > i)
	{
		scores[j] = scores[j - 1];
		j--;
	}
	scores[i] = new_entry;
	if (*count < MAX_SCORES)
		(*count)++;
}

void	add_score(int score, int level, const char *name)
{
	t_score_entry	scores[MAX_SCORES];
	t_score_entry	new_entry;
	int				count;

	count = load_scores(scores);
	strncpy(new_entry.name, name, MAX_NAME_LEN);
	new_entry.name[MAX_NAME_LEN] = '\0';
	new_entry.score = score;
	new_entry.level = level;
	new_entry.timestamp = time(NULL);
	insert_score(scores, &count, new_entry);
	save_scores(scores, count);
}

static void	render_score_line(int rank, t_score_entry *entry)
{
	char	buffer[100];
	int		len;

	len = snprintf(buffer, sizeof(buffer),
			"  ║ %2d. %-16s %6d pts  Lvl %-2d ║\n",
			rank, entry->name, entry->score, entry->level);
	write(1, buffer, len);
}

void	render_high_scores(t_tetris *t)
{
	t_score_entry	scores[MAX_SCORES];
	int				count;
	int				i;

	(void)t;
	write(1, CLEAR, 4);
	write(1, HOME, 3);
	write(1, "\n\n", 2);
	write(1, "  ╔════════════════════════════════════════╗\n", 47);
	write(1, "  ║        HIGH SCORES (TOP 10)           ║\n", 47);
	write(1, "  ╠════════════════════════════════════════╣\n", 47);

	count = load_scores(scores);
	if (count == 0)
	{
		write(1, "  ║                                        ║\n", 47);
		write(1, "  ║       No scores yet. Play a game!      ║\n", 47);
		write(1, "  ║                                        ║\n", 47);
	}
	else
	{
		i = 0;
		while (i < count)
		{
			render_score_line(i + 1, &scores[i]);
			i++;
		}
	}

	write(1, "  ╚════════════════════════════════════════╝\n", 47);
	write(1, "\n", 1);
	write(1, "  Press ESC to return to menu\n", 31);
}

int	read_name_input(char *name, int max_len)
{
	char	c;
	int		len;

	len = 0;
	name[0] = '\0';

	write(1, CLEAR, 4);
	write(1, HOME, 3);
	write(1, "\n\n  Enter your name (max 16 chars): ", 36);

	while (1)
	{
		if (read(STDIN_FILENO, &c, 1) != 1)
		{
			simple_delay();
			continue ;
		}
		if (c == '\n')
			break ;
		if (c == 127 || c == 8)
		{
			if (len > 0)
			{
				len--;
				name[len] = '\0';
				write(1, "\b \b", 3);
			}
		}
		else if (len < max_len && (c >= 32 && c <= 126))
		{
			name[len] = c;
			len++;
			name[len] = '\0';
			write(1, &c, 1);
		}
	}
	if (len == 0)
	{
		strcpy(name, "Anonymous");
		return (9);
	}
	return (len);
}
