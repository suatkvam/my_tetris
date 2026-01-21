#include "tetris.h"

void	setup_tetris_terminal(struct termios *old)
{
	struct termios	new;

	tcgetattr(STDIN_FILENO, old);
	new = *old;
	new.c_lflag &= ~(ICANON | ECHO);
	new.c_cc[VMIN] = 0;
	new.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &new);
	// Hide cursor
	write(1, "\033[?25l", 6);
}

void	simple_delay(void)
{
	usleep(50000); // 50ms delay
}

unsigned int	simple_rand(unsigned int *seed)
{
	*seed = *seed * 1103515245 + 12345;
	return (*seed / 65536) % 32768;
}

// Declaration from tetris.c
void	start_game(char **args, t_shell *shell);

int	main(void)
{
	t_shell	shell;

	// Need to zero out shell or at least cmd_arena since we pass it to gc_malloc macro
	shell.cmd_arena = NULL;
	
	// Seed initialization
	srand(time(NULL));

	start_game(NULL, &shell);

	return (0);
}