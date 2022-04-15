/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pllucian <pllucian@21-school.ru>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/19 17:09:03 by pllucian          #+#    #+#             */
/*   Updated: 2022/04/19 19:14:52 by pllucian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(void) : _fd(-1), _address(""), _password(""), _status(false), \
	_buffer(""), _nickname(""), _username("") {}

Client::Client(int fd, const std::string& address, const std::string& password) \
	: _fd(fd), _address(address), _password(password), _status(false), \
	_buffer(""), _nickname(""), _username("") {}

Client::Client(const Client& src)
{
	*this = src;
}

Client::~Client(void) {}

Client&	Client::operator=(const Client& rhs)
{
	if (this != &rhs)
	{
		_fd = rhs._fd;
		_address = rhs._address;
		_status = rhs._status;
		_buffer = rhs._buffer;
		_password = rhs._password;
		_nickname = rhs._nickname;
		_username = rhs._username;
	}
	return *this;
}

int	Client::get_fd(void)
{
	return _fd;
}

std::string&	Client::get_address(void)
{
	return _address;
}

bool	Client::get_status(void)
{
	return _status;
}

std::string&	Client::get_buffer(void)
{
	return _buffer;
}

std::string&	Client::get_password(void)
{
	return _password;
}

std::string&	Client::get_nickname(void)
{
	return _nickname;
}

std::string&	Client::get_username(void)
{
	return _username;
}

std::list<Channel*>&	Client::get_channels(void)
{
	return _channels;
}

std::string	Client::get_who(void)
{
	return get_nickname() + "!" + get_username() + "@" + get_address();
}

void	Client::set_address(const std::string& address)
{
	_address = address;
}

void	Client::set_status(bool status)
{
	_status = status;
}

void	Client::set_buffer(const std::string& buffer)
{
	_buffer = buffer;
}

void	Client::set_password(const std::string& password)
{
	_password = password;
}

void	Client::set_nickname(const std::string& nickname)
{
	_nickname = nickname;
}

void	Client::set_username(const std::string& username)
{
	_username = username;
}

void	Client::add_channel(Channel& channel)
{
	_channels.push_back(&channel);
}

void	Client::del_channel(Channel& channel)
{
	_channels.remove(&channel);
}
