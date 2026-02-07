/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_game_over.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: harici <harici@student.42istanbul.com.tr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 18:10:00 by harici            #+#    #+#             */
/*   Updated: 2026/02/07 14:13:22 by harici           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tetris.h"

static void	fill_art_buffer(char *buffer, int *pos)
{
	append_string(buffer, pos, "   ██████   █████  ███    ███ ███████");
	append_string(buffer, pos, "      ██████  ██    ██ ███████");
	append_string(buffer, pos, " ██████   \033[K\n");
	append_string(buffer, pos, "  ██       ██   ██ ████  ████ ██      ");
	append_string(buffer, pos, "    ██    ██ ██    ██ ██      ");
	append_string(buffer, pos, "██   ██ \033[K\n");
	append_string(buffer, pos, "  ██   ███ ███████ ██ ████ ██ █████  ");
	append_string(buffer, pos, "     ██    ██ ██    ██ █████   ");
	append_string(buffer, pos, "██████  \033[K\n");
	append_string(buffer, pos, "  ██    ██ ██   ██ ██  ██  ██ ██     ");
	append_string(buffer, pos, "     ██    ██  ██  ██  ██      ");
	append_string(buffer, pos, "██   ██ \033[K\n");
	append_string(buffer, pos, "   ██████  ██   ██ ██      ██ ███████");
	append_string(buffer, pos, "      ██████    ████   ███████ ");
	append_string(buffer, pos, "██   ██ \033[K\n");
}

void	render_game_over(t_tetris *t)
{
	char	buffer[BUFFER_SIZE];
	int		pos;

	pos = 0;
	append_string(buffer, &pos, CLEAR);
	append_string(buffer, &pos, HOME);
	fill_art_buffer(buffer, &pos);
	append_string(buffer, &pos, "\nFinal Score: ");
	append_int(buffer, &pos, t->score);
	append_string(buffer, &pos, "\033[K\n");
	write(STDOUT_FILENO, buffer, pos);
	t->running = 0;
}
