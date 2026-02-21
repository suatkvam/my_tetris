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
	// Hide cursor and enable alternate screen buffer
	write(1, "\033[?25l", 6);
	write(1, "\033[?1049h", 8);
}

void	simple_delay(void)
{
	volatile long long	i;

	i = 0;
	while (i < 35000000)
		i++;
}

unsigned int	get_random_seed(void)
{
	unsigned int	seed;
	int				fd;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return (42); // Fallback if urandom fails
	read(fd, &seed, sizeof(seed));
	close(fd);
	return (seed);
}

unsigned int	xorshift32(unsigned int *state)
{
	unsigned int	x;

	x = *state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	*state = x;
	return (x);
}

// Declaration from tetris.c
void	start_game(char **args, t_shell *shell);

int	main(void)
{
	t_shell	shell;

	// Need to zero out shell or at least cmd_arena since we pass it to gc_malloc macro
	shell.cmd_arena = NULL;

	start_game(NULL, &shell);

	return (0);
}