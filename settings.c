#include "tetris.h"

void	init_settings(t_settings *settings)
{
	settings->speed_base = SPEED_BASE_DEFAULT;
	settings->speed_min = SPEED_MIN_DEFAULT;
	settings->speed_level_step = SPEED_LEVEL_STEP_DEFAULT;
	settings->lock_delay = LOCK_DELAY_DEFAULT;
	settings->ghost_piece_enabled = GHOST_PIECE_DEFAULT;
}

void	render_settings(t_shell *shell)
{
	char	buffer[128];
	int		len;
	int		sel;

	sel = shell->tetris->menu_selection;
	write(1, CLEAR, 4);
	write(1, HOME, 3);
	write(1, "\n\n", 2);
	write(1, "  ╔════════════════════════════════════╗\n", 44);
	write(1, "  ║                                    ║\n", 44);
	write(1, "  ║           S E T T I N G S          ║\n", 44);
	write(1, "  ║                                    ║\n", 44);
	write(1, "  ╚════════════════════════════════════╝\n", 44);
	write(1, "\n", 1);

	len = snprintf(buffer, sizeof(buffer), "  %c Speed Base: %d\n",
		sel == 0 ? '>' : ' ', shell->settings.speed_base);
	write(1, buffer, len);

	len = snprintf(buffer, sizeof(buffer), "  %c Speed Min: %d\n",
		sel == 1 ? '>' : ' ', shell->settings.speed_min);
	write(1, buffer, len);

	len = snprintf(buffer, sizeof(buffer), "  %c Speed Level Step: %d\n",
		sel == 2 ? '>' : ' ', shell->settings.speed_level_step);
	write(1, buffer, len);

	len = snprintf(buffer, sizeof(buffer), "  %c Lock Delay: %d frames\n",
		sel == 3 ? '>' : ' ', shell->settings.lock_delay);
	write(1, buffer, len);

	len = snprintf(buffer, sizeof(buffer), "  %c Ghost Piece: %s\n",
		sel == 4 ? '>' : ' ', shell->settings.ghost_piece_enabled ? "ON" : "OFF");
	write(1, buffer, len);

	write(1, "  ", 2);
	if (sel == 5)
		write(1, "> ", 2);
	else
		write(1, "  ", 2);
	write(1, "Back\n", 5);

	write(1, "\n", 1);
	write(1, "  Use W/S or Arrows: Navigate\n", 31);
	write(1, "  Use A/D or Left/Right: Change value\n", 39);
	write(1, "  Press ESC to go back\n", 24);
}

void	handle_settings_input(t_shell *shell, char c)
{
	t_tetris	*t;
	int			*sel;

	t = shell->tetris;
	sel = &t->menu_selection;

	if (c == 'w' || c == 'W')
	{
		if (*sel > 0)
			(*sel)--;
	}
	else if (c == 's' || c == 'S')
	{
		if (*sel < 5)
			(*sel)++;
	}
	else if (c == 'a' || c == 'A')
		handle_settings_decrease(shell);
	else if (c == 'd' || c == 'D')
		handle_settings_increase(shell);
	else if (c == '\n' || c == ' ')
	{
		if (*sel == 5)
		{
			t->mode = MODE_MENU;
			*sel = 0;
		}
	}
}

void	handle_settings_escape(t_shell *shell)
{
	char	seq[2];

	if (read(STDIN_FILENO, &seq[0], 1) != 1)
	{
		shell->tetris->mode = MODE_MENU;
		shell->tetris->menu_selection = 0;
		return ;
	}
	if (seq[0] != '[')
	{
		shell->tetris->mode = MODE_MENU;
		shell->tetris->menu_selection = 0;
		return ;
	}
	if (read(STDIN_FILENO, &seq[1], 1) != 1)
		return ;

	if (seq[1] == KEY_ARROW_UP)
	{
		if (shell->tetris->menu_selection > 0)
			shell->tetris->menu_selection--;
	}
	else if (seq[1] == KEY_ARROW_DOWN)
	{
		if (shell->tetris->menu_selection < 5)
			shell->tetris->menu_selection++;
	}
	else if (seq[1] == KEY_ARROW_LEFT)
		handle_settings_decrease(shell);
	else if (seq[1] == KEY_ARROW_RIGHT)
		handle_settings_increase(shell);
}

void	handle_settings_decrease(t_shell *shell)
{
	int	sel;

	sel = shell->tetris->menu_selection;
	if (sel == 0 && shell->settings.speed_base > 3)
		shell->settings.speed_base--;
	else if (sel == 1 && shell->settings.speed_min > 1)
		shell->settings.speed_min--;
	else if (sel == 2 && shell->settings.speed_level_step > 1)
		shell->settings.speed_level_step--;
	else if (sel == 3 && shell->settings.lock_delay > 0)
		shell->settings.lock_delay--;
	else if (sel == 4)
		shell->settings.ghost_piece_enabled = 0;
}

void	handle_settings_increase(t_shell *shell)
{
	int	sel;

	sel = shell->tetris->menu_selection;
	if (sel == 0 && shell->settings.speed_base < 30)
		shell->settings.speed_base++;
	else if (sel == 1 && shell->settings.speed_min < 10)
		shell->settings.speed_min++;
	else if (sel == 2 && shell->settings.speed_level_step < 10)
		shell->settings.speed_level_step++;
	else if (sel == 3 && shell->settings.lock_delay < 30)
		shell->settings.lock_delay++;
	else if (sel == 4)
		shell->settings.ghost_piece_enabled = 1;
}
