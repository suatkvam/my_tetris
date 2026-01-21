NAME = tetris
CC = cc
CFLAGS = -Wall -Wextra -Werror

SRCS = calc_speed.c \
	cleare_piece.c \
	collision.c \
	init_tetris.c \
	lock_piece.c \
	main.c \
	movement.c \
	render.c \
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
