/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pllucian <pllucian@21-school.ru>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/14 17:03:30 by pllucian          #+#    #+#             */
/*   Updated: 2022/04/15 13:27:07 by pllucian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool	g_status;

void	sighandler(int signum)
{
	if (signum == SIGINT)
	{
		g_status = false;
		std::cout << '\n';
	}
}

Server::Server(void) : _port("6667"), _passwd("password"), _servfd(-1), _epfd(-1) {}

Server::Server(const char* port, const char* password) \
	: _port(port), _passwd(password), _servfd(-1), _epfd(-1) {}

Server::Server(const Server& src) : IRC(src)
{
	*this = src;
}

Server::~Server(void)
{
	remove_server();
}

Server&	Server::operator=(const Server& rhs)
{
	if (this != &rhs)
	{
		_port = rhs._port;
		_passwd = rhs._passwd;
		_servfd = rhs._servfd;
		_epfd = rhs._epfd;
	}
	return *this;
}

void	Server::start(void)
{
	g_status = true;
	signal(SIGINT, &sighandler);
	init_server();
	init_epoll();
	main_loop();
}

void	Server::stop(void)
{
	g_status = false;
}

void	Server::init_server(void)
{
	addrinfo	hints;
	addrinfo*	servinfo;
	addrinfo*	p;
	int			optval = 1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, _port, &hints, &servinfo) != 0)
		throw_error("Error: getaddrinfo");
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		_servfd = socket(p->ai_family, p->ai_socktype | SOCK_NONBLOCK, p->ai_protocol);
		if (_servfd < 0)
			continue ;
		if (setsockopt(_servfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		{
			freeaddrinfo(servinfo);
			throw_error("Error: setsockopt");
		}
		if (bind(_servfd, p->ai_addr, p->ai_addrlen) < 0)
		{
			freeaddrinfo(servinfo);
			throw_error("Error: bind");
		}
		break ;
	}
	freeaddrinfo(servinfo);
	if (!p)
		throw_error("Error: socket");
	if (listen(_servfd, SOMAXCONN) < 0)
		throw_error("Error: listen");
}

void	Server::init_epoll(void)
{
	epoll_event	ev;

	_epfd = epoll_create(MAX_EVENTS);
	if (_epfd < 0)
		throw_error("Error: epoll_create");
	memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN;
	ev.data.fd = _servfd;
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, _servfd, &ev) < 0)
		throw_error("Error: epoll_ctl");
}

void	Server::main_loop(void)
{
	int			nfds;
	epoll_event	events[MAX_EVENTS];

	while (g_status)
	{
		nfds = epoll_wait(_epfd, events, MAX_EVENTS, -1);
		if (nfds < 0)
		{
			if (!g_status)
				break ;
			throw_error("Error: epoll_wait");
		}
		for (int i = 0; i < nfds ; i++)
		{
			if (events[i].events & EPOLLERR \
				|| events[i].events & EPOLLHUP)
				close(events[i].data.fd);
			if (events[i].data.fd == _servfd)
				accept_client();
			else
				handle_client(events[i].data.fd);
		}
	}
	remove_server();
}

void	Server::accept_client(void)
{
	int			fd;
	sockaddr	addr;
	socklen_t	addrlen;
	epoll_event	ev;
	char		buf[INET6_ADDRSTRLEN];
	std::string	address;
	int			port;

	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);
	fd = accept(_servfd, &addr, &addrlen);
	if (fd < 0)
		throw_error("Error: accept");
	memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) < 0)
		throw_error("Error: epoll_ctl");
	memset(buf, 0, INET6_ADDRSTRLEN);
	if (addr.sa_family == AF_INET)
	{
		address = inet_ntoa(((sockaddr_in*)&addr)->sin_addr);
		port = ntohs(((sockaddr_in*)&addr)->sin_port);
	}
	else
	{
		address = inet_ntop(AF_INET6, &(((sockaddr_in6*)&addr)->sin6_addr), buf, INET6_ADDRSTRLEN);
		port = ntohs(((sockaddr_in6*)&addr)->sin6_port);
	}
	_clients[fd] = Client(fd, address, _passwd);
	std::cout << "New client #" << fd << " from " << address << ":" << port << '\n';
}

void	Server::handle_client(int fd)
{
	int		ret;
	char	buf[BUFFER_SIZE];
	Client&	client = _clients[fd];

	ret = recv(fd, buf, BUFFER_SIZE, MSG_DONTWAIT | MSG_NOSIGNAL);
	if (!ret)
	{
		client.get_buffer().clear();
		client.get_buffer().append("quit\r\n", 6);
	}
	else
		client.get_buffer().append(buf, ret);
	exec_command(client);
}

void	Server::remove_server(void)
{
	std::map<int, Client>::iterator	it;

	for (it = _clients.begin(); it != _clients.end(); ++it)
		close(it->first);
	if (_epfd > 0)
		close(_epfd);
	close(_servfd);
}

void	Server::throw_error(const char* error)
{
	remove_server();
	throw std::runtime_error(error);
}
