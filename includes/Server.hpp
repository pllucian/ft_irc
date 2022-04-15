/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pllucian <pllucian@21-school.ru>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/14 17:03:45 by pllucian          #+#    #+#             */
/*   Updated: 2022/04/11 16:45:30 by pllucian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "IRC.hpp"
# include "Client.hpp"
# include <csignal>
# include <netdb.h>
# include <sys/epoll.h>
# include <arpa/inet.h>

# define MAX_EVENTS	10000

class Server : public IRC
{
	public:
		Server(void);
		Server(const char* port, const char* password);
		Server(const Server& src);
		~Server(void);

		Server&	operator=(const Server& rhs);

		void	start(void);
		void	stop(void);

	private:

		const char*	_port;
		const char*	_passwd;
		int			_servfd;
		int			_epfd;

		void	init_server(void);
		void	init_epoll(void);
		void	main_loop(void);
		void	accept_client(void);
		void	handle_client(int fd);
		void	remove_server(void);
		void	throw_error(const char* error);
};

#endif
