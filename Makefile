NAME = tetris
CC = cc
CFLAGS = -Wall -Wextra -Werror

SRCS = calc_speed.c \
	cleare_piece.c \
	collision.c \
	init_tetris.c \
	local_2p.c \
	lock_piece.c \
	main.c \
	menu.c \
	movement.c \
	multiplayer_menu.c \
	multiplayer_render.c \
	network.c \
	render.c \
	scoreboard.c \
	settings.c \
	spawn_piece.c \
	tetris.c \
	tetromino.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
