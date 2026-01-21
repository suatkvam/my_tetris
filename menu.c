#include "tetris.h"

void	render_main_menu(t_tetris *t)
{
	const char	*options[] = {
		"Single Player",
		"Local 2 Players",
		"Multiplayer",
		"High Scores",
		"Settings",
		"Exit"
	};
	int			i;

	write(1, CLEAR, 4);
	write(1, HOME, 3);
	write(1, "\n\n", 2);
	write(1, "  ╔════════════════════════════════════╗\n", 44);
	write(1, "  ║                                    ║\n", 44);
	write(1, "  ║           T E T R I S              ║\n", 44);
	write(1, "  ║                                    ║\n", 44);
	write(1, "  ╚════════════════════════════════════╝\n", 44);
	write(1, "\n", 1);

	i = 0;
	while (i < 6)
	{
		if (i == t->menu_selection)
			write(1, "        > ", 10);
		else
			write(1, "          ", 10);
		write(1, options[i], strlen(options[i]));
		write(1, "\n", 1);
		i++;
	}

	write(1, "\n\n", 2);
	write(1, "  Use W/S or Arrow keys to navigate\n", 37);
	write(1, "  Press ENTER or SPACE to select\n", 34);
	write(1, "  Press ESC to exit\n", 21);
}

void	handle_menu_input(t_tetris *t, char c)
{
	if (c == 'w' || c == 'W')
	{
		if (t->menu_selection > 0)
			t->menu_selection--;
	}
	else if (c == 's' || c == 'S')
	{
		if (t->menu_selection < 5)
			t->menu_selection++;
	}
	else if (c == '\n' || c == ' ')
	{
		if (t->menu_selection == 0)
		{
			t->mode = MODE_SINGLE_PLAYER;
			t->is_multiplayer = 0;
		}
		else if (t->menu_selection == 1)
			t->mode = MODE_LOCAL_2P;
		else if (t->menu_selection == 2)
			t->mode = MODE_MULTIPLAYER_MENU;
		else if (t->menu_selection == 3)
			t->mode = MODE_HIGH_SCORES;
		else if (t->menu_selection == 4)
			t->mode = MODE_SETTINGS;
		else if (t->menu_selection == 5)
			t->running = 0;
	}
}

void	handle_menu_escape(t_tetris *t)
{
	char	seq[2];

	if (read(STDIN_FILENO, &seq[0], 1) != 1)
	{
		t->running = 0;
		return ;
	}
	if (seq[0] != '[')
	{
		t->running = 0;
		return ;
	}
	if (read(STDIN_FILENO, &seq[1], 1) != 1)
		return ;

	if (seq[1] == KEY_ARROW_UP)
	{
		if (t->menu_selection > 0)
			t->menu_selection--;
	}
	else if (seq[1] == KEY_ARROW_DOWN)
	{
		if (t->menu_selection < 5)
			t->menu_selection++;
	}
}
