#include "tetris.h"

void	render_multiplayer_menu(t_tetris *t)
{
	const char	*options[] = {
		"Host Game",
		"Join Game",
		"Back"
	};
	int			i;

	write(1, CLEAR, 4);
	write(1, HOME, 3);
	write(1, "\n\n", 2);
	write(1, "  ╔════════════════════════════════════╗\n", 44);
	write(1, "  ║                                    ║\n", 44);
	write(1, "  ║        M U L T I P L A Y E R       ║\n", 44);
	write(1, "  ║                                    ║\n", 44);
	write(1, "  ╚════════════════════════════════════╝\n", 44);
	write(1, "\n", 1);

	i = 0;
	while (i < 3)
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
	write(1, "  Press ESC to go back\n", 24);
}

void	handle_multiplayer_menu_escape(t_tetris *t)
{
	char	seq[2];

	if (read(STDIN_FILENO, &seq[0], 1) != 1)
	{
		t->mode = MODE_MENU;
		t->menu_selection = 0;
		return ;
	}
	if (seq[0] != '[')
	{
		t->mode = MODE_MENU;
		t->menu_selection = 0;
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
		if (t->menu_selection < 2)
			t->menu_selection++;
	}
}

void	handle_multiplayer_menu_input(t_tetris *t, char c)
{
	if (c == KEY_ESC)
	{
		handle_multiplayer_menu_escape(t);
	}
	else if (c == 'w' || c == 'W')
	{
		if (t->menu_selection > 0)
			t->menu_selection--;
	}
	else if (c == 's' || c == 'S')
	{
		if (t->menu_selection < 2)
			t->menu_selection++;
	}
	else if (c == '\n' || c == ' ')
	{
		if (t->menu_selection == 0)
			t->mode = MODE_MULTIPLAYER_HOST;
		else if (t->menu_selection == 1)
			t->mode = MODE_MULTIPLAYER_JOIN;
		else if (t->menu_selection == 2)
		{
			t->mode = MODE_MENU;
			t->menu_selection = 0;
		}
	}
}

int	read_ip_input(char *ip, int max_len)
{
	char	c;
	int		len;

	len = 0;
	ip[0] = '\0';

	write(1, CLEAR, 4);
	write(1, HOME, 3);
	write(1, "\n\n  Enter opponent IP address: ", 32);

	while (1)
	{
		if (read(STDIN_FILENO, &c, 1) != 1)
		{
			simple_delay();
			continue ;
		}
		if (c == '\n')
			break ;
		if (c == KEY_ESC)
			return (-1);
		if (c == 127 || c == 8)
		{
			if (len > 0)
			{
				len--;
				ip[len] = '\0';
				write(1, "\b \b", 3);
			}
		}
		else if (len < max_len && ((c >= '0' && c <= '9') || c == '.'))
		{
			ip[len] = c;
			len++;
			ip[len] = '\0';
			write(1, &c, 1);
		}
	}
	return (len);
}

void	render_host_waiting(t_tetris *t)
{
	char	buffer[128];
	char	ip[16];
	int		len;

	write(1, CLEAR, 4);
	write(1, HOME, 3);
	write(1, "\n\n", 2);
	write(1, "  ╔════════════════════════════════════╗\n", 44);
	write(1, "  ║                                    ║\n", 44);
	write(1, "  ║          HOSTING GAME...           ║\n", 44);
	write(1, "  ║                                    ║\n", 44);
	write(1, "  ╚════════════════════════════════════╝\n", 44);
	write(1, "\n", 1);

	get_local_ip(ip, sizeof(ip));
	len = snprintf(buffer, sizeof(buffer),
		"  Your IP: %s\n", ip);
	write(1, buffer, len);
	len = snprintf(buffer, sizeof(buffer),
		"  Port: %d\n\n", t->network->port);
	write(1, buffer, len);
	write(1, "  Waiting for opponent to connect...\n\n", 39);
	write(1, "  Press ESC to cancel\n", 23);
}

void	render_join_connecting(t_tetris *t)
{
	char	buffer[128];
	int		len;

	write(1, CLEAR, 4);
	write(1, HOME, 3);
	write(1, "\n\n", 2);
	write(1, "  ╔════════════════════════════════════╗\n", 44);
	write(1, "  ║                                    ║\n", 44);
	write(1, "  ║         JOINING GAME...            ║\n", 44);
	write(1, "  ║                                    ║\n", 44);
	write(1, "  ╚════════════════════════════════════╝\n", 44);
	write(1, "\n", 1);

	len = snprintf(buffer, sizeof(buffer),
		"  Connecting to: %s:%d\n\n", t->network->opponent_ip, t->network->port);
	write(1, buffer, len);
	write(1, "  Please wait...\n", 17);
}

void	render_connection_failed(void)
{
	write(1, CLEAR, 4);
	write(1, HOME, 3);
	write(1, "\n\n", 2);
	write(1, "  ╔════════════════════════════════════╗\n", 44);
	write(1, "  ║                                    ║\n", 44);
	write(1, "  ║       CONNECTION FAILED!           ║\n", 44);
	write(1, "  ║                                    ║\n", 44);
	write(1, "  ╚════════════════════════════════════╝\n", 44);
	write(1, "\n", 1);
	write(1, "  Press any key to continue...\n", 31);
}
