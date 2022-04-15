/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pllucian <pllucian@21-school.ru>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 17:23:29 by pllucian          #+#    #+#             */
/*   Updated: 2022/04/19 19:14:32 by pllucian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(void) : _name("") {}

Channel::Channel(const std::string& name) : _name(name) {}

Channel::Channel(const Channel& src)
{
	*this = src;
}

Channel::~Channel(void) {}

Channel&	Channel::operator=(const Channel& rhs)
{
	if (this != &rhs)
	{
		_name = rhs._name;
		_clients = rhs._clients;
	}
	return *this;
}

std::string&	Channel::get_name(void)
{
	return _name;
}

std::list<Client*>&	Channel::get_clients(void)
{
	return _clients;
}

std::string	Channel::get_names(void)
{
	std::string							ret;
	std::list<Client*>::const_iterator	it;

	ret = get_clients().back()->get_nickname();
	for (it = get_clients().begin(); *it != get_clients().back(); ++it)
		ret += " " + (**it).get_nickname();
	return ret;
}

void	Channel::set_name(const std::string& name)
{
	_name = name;
}

void	Channel::add_client(Client& client)
{
	_clients.push_back(&client);
}

void	Channel::del_client(Client& client)
{
	_clients.remove(&client);
}
