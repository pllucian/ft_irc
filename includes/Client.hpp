/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pllucian <pllucian@21-school.ru>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/19 17:08:06 by pllucian          #+#    #+#             */
/*   Updated: 2022/04/19 18:52:43 by pllucian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Channel.hpp"
# include <string>
# include <list>

class Channel;

class Client
{
	public:
		Client(void);
		Client(int fd, const std::string& address, const std::string& password);
		Client(const Client& src);
		~Client(void);

		Client&	operator=(const Client& rhs);

		int						get_fd(void);
		std::string&			get_address(void);
		bool					get_status(void);
		std::string&			get_buffer(void);
		std::string&			get_password(void);
		std::string&			get_nickname(void);
		std::string&			get_username(void);
		std::list<Channel*>&	get_channels(void);
		std::string				get_who(void);
		void					set_address(const std::string& address);
		void					set_status(bool status);
		void					set_buffer(const std::string& buffer);
		void					set_password(const std::string& password);
		void					set_nickname(const std::string& nickname);
		void					set_username(const std::string& username);
		void					add_channel(Channel& channel);
		void					del_channel(Channel& channel);

	private:
		int					_fd;
		std::string			_address;
		std::string			_password;
		bool				_status;
		std::string			_buffer;
		std::string			_nickname;
		std::string			_username;
		std::list<Channel*>	_channels;
};

#endif
