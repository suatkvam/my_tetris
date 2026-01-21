#include "tetris.h"

int	get_local_ip(char *buffer, int size)
{
	struct ifaddrs	*ifaddr;
	struct ifaddrs	*ifa;
	int				found;

	found = 0;
	if (getifaddrs(&ifaddr) == -1)
		return (-1);
	ifa = ifaddr;
	while (ifa != NULL)
	{
		if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET)
		{
			if (strcmp(ifa->ifa_name, "lo") != 0)
			{
				inet_ntop(AF_INET,
					&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr,
					buffer, size);
				found = 1;
				break ;
			}
		}
		ifa = ifa->ifa_next;
	}
	freeifaddrs(ifaddr);
	if (!found)
		strncpy(buffer, "127.0.0.1", size);
	return (0);
}

t_network	*init_network(void)
{
	t_network	*net;

	net = malloc(sizeof(t_network));
	if (!net)
		return (NULL);
	net->socket_fd = -1;
	net->is_host = 0;
	net->connected = 0;
	net->opponent_ip[0] = '\0';
	net->port = 12345;
	net->start_time = time(NULL);
	return (net);
}

int	network_host(t_network *net, int port)
{
	struct sockaddr_in	server_addr;
	int					opt;

	opt = 1;
	net->port = port;
	net->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (net->socket_fd < 0)
		return (-1);
	if (setsockopt(net->socket_fd, SOL_SOCKET, SO_REUSEADDR,
		&opt, sizeof(opt)) < 0)
	{
		close(net->socket_fd);
		return (-1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	if (bind(net->socket_fd, (struct sockaddr *)&server_addr,
		sizeof(server_addr)) < 0)
	{
		close(net->socket_fd);
		return (-1);
	}
	if (listen(net->socket_fd, 1) < 0)
	{
		close(net->socket_fd);
		return (-1);
	}
	fcntl(net->socket_fd, F_SETFL, O_NONBLOCK);
	net->is_host = 1;
	net->start_time = time(NULL);
	return (0);
}

int	network_accept(t_network *net)
{
	struct sockaddr_in	client_addr;
	socklen_t			client_len;
	int					client_fd;

	client_len = sizeof(client_addr);
	client_fd = accept(net->socket_fd, (struct sockaddr *)&client_addr,
		&client_len);
	if (client_fd < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return (1);
		return (-1);
	}
	close(net->socket_fd);
	net->socket_fd = client_fd;
	net->connected = 1;
	inet_ntop(AF_INET, &client_addr.sin_addr, net->opponent_ip,
		sizeof(net->opponent_ip));
	return (0);
}

int	network_join(t_network *net, const char *ip, int port)
{
	struct sockaddr_in	server_addr;

	net->port = port;
	net->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (net->socket_fd < 0)
		return (-1);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0)
	{
		close(net->socket_fd);
		return (-1);
	}
	if (connect(net->socket_fd, (struct sockaddr *)&server_addr,
		sizeof(server_addr)) < 0)
	{
		close(net->socket_fd);
		return (-1);
	}
	net->connected = 1;
	net->is_host = 0;
	strncpy(net->opponent_ip, ip, sizeof(net->opponent_ip) - 1);
	return (0);
}

void	network_close(t_network *net)
{
	if (net)
	{
		if (net->socket_fd >= 0)
			close(net->socket_fd);
		free(net);
	}
}

int	network_send_state(t_network *net, t_tetris *t)
{
	char	buffer[256];
	int		len;

	if (!net->connected)
		return (-1);
	len = snprintf(buffer, sizeof(buffer),
		"STATE|%d|%d|%d|%d|%d|%d|%d\n",
		t->pos_x, t->pos_y, t->rotation, t->current_piece,
		t->score, t->lines, t->level);
	if (send(net->socket_fd, buffer, len, 0) < 0)
	{
		net->connected = 0;
		return (-1);
	}
	return (0);
}

int	network_send_game_over(t_network *net, int final_score)
{
	char	buffer[64];
	int		len;

	if (!net->connected)
		return (-1);
	len = snprintf(buffer, sizeof(buffer), "GAMEOVER|%d\n", final_score);
	if (send(net->socket_fd, buffer, len, 0) < 0)
	{
		net->connected = 0;
		return (-1);
	}
	return (0);
}

int	network_receive(t_network *net, char *buffer, int size)
{
	int	bytes;

	if (!net->connected)
		return (-1);
	bytes = recv(net->socket_fd, buffer, size - 1, MSG_DONTWAIT);
	if (bytes <= 0)
	{
		if (bytes == 0 || (errno != EAGAIN && errno != EWOULDBLOCK))
		{
			net->connected = 0;
			return (-1);
		}
		return (0);
	}
	buffer[bytes] = '\0';
	return (bytes);
}
