/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pllucian <pllucian@21-school.ru>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/29 16:07:39 by pllucian          #+#    #+#             */
/*   Updated: 2022/04/19 19:13:00 by pllucian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "IRC.hpp"

IRC::IRC(void)
{
	_servname = "ircserv";
	_commands["CAP"] = &IRC::cmd_cap;
	_commands["PING"] = &IRC::cmd_ping;
	_commands["PASS"] = &IRC::cmd_pass;
	_commands["NICK"] = &IRC::cmd_nick;
	_commands["USER"] = &IRC::cmd_user;
	_commands["QUIT"] = &IRC::cmd_quit;
	_commands["JOIN"] = &IRC::cmd_join;
	_commands["PART"] = &IRC::cmd_part;
	_commands["KICK"] = &IRC::cmd_kick;
	_commands["PRIVMSG"] = &IRC::cmd_privmsg;
	_commands["NOTICE"] = &IRC::cmd_notice;
	_commands["MODE"] = &IRC::cmd_mode;
	_commands["WHO"] = &IRC::cmd_who;
}

IRC::IRC(const IRC& src)
{
	*this = src;
}

IRC::~IRC(void) {}

IRC&	IRC::operator=(const IRC& rhs)
{
	if (this != &rhs)
	{
		_servname = rhs._servname;
		_commands = rhs._commands;
		_clients = rhs._clients;
		_reglist = rhs._reglist;
	}
	return *this;
}

void	IRC::exec_command(Client& client)
{
	std::queue<Message>								msgq;
	std::map<std::string, Command>::const_iterator	it;

	parse_buffer(msgq, client.get_buffer());
	while (msgq.size())
	{
		if (client.get_status() \
			|| msgq.front()._command == "CAP" \
			|| msgq.front()._command == "PING" \
			|| msgq.front()._command == "PASS")
		{
			it = _commands.find(msgq.front()._command);
			if (it != _commands.end())
				(this->*(it->second))(client, msgq.front());
			else
				send_error(client, msgq.front(), ERR_UNKNOWNCOMMAND);
		}
		else
			send_error(client, msgq.front(), ERR_NOTREGISTERED);
		msgq.pop();
	}
}

void	IRC::parse_buffer(std::queue<Message>& msgq, std::string& buf)
{
	Message		msg;
	std::string	part;
	size_t		i = 0;
	size_t		j;
	
	while (buf[i])
	{
		j = buf.find("\r\n", i);
		part = buf.substr(i, j - i);
		if (j == std::string::npos)
			break;
		parse_buffer_part(msg, part);
		msgq.push(msg);
		msg._prefix.clear();
		msg._command.clear();
		msg._paramlist.clear();
		i = j + 2;
	}
	if (j == std::string::npos)
		buf = part;
	else
		buf.clear();
}

void	IRC::parse_buffer_part(Message& msg, std::string& part)
{
	size_t	i = 0;
	size_t	j;

	while (i != std::string::npos)
	{
		while (part[i] == ' ')
			++i;
		if (!part[i])
			break;
		j = part.find(' ', i);
		if (msg._prefix.empty() && msg._command.empty() && part[i] == ':')
			msg._prefix = part.substr(i + 1, j);
		else if (msg._command.empty())
			msg._command = part.substr(i, j - i);
		else
		{
			if (part[i] == ':')
				++i;
			msg._paramlist.push_back(part.substr(i, j - i));
		}
		if (msg._paramlist.size() == 15)
			break;
		i = j;
	}
	for (int i = 0; (msg._command)[i]; ++i)
		(msg._command)[i] = toupper((msg._command)[i]);
}

void	IRC::send_error(Client& client, Message& msg, int error)
{
	std::stringstream	ss;
	std::string			code;
	std::string			response;

	ss << error;
	code = ss.str();
	while (code.size() < 3)
		code = "0" + code;
	response = ":" + _servname + " " + code + " ";
	if (client.get_nickname().empty())
		response += client.get_address() + " ";
	else
		response += client.get_nickname() + " ";
	switch (error)
	{
		case ERR_NOSUCHNICK:
			response += msg._paramlist.front() + " :No such nick\r\n";
			break;
		case ERR_NOSUCHCHANNEL:
			response += msg._paramlist.front() + " :No such channel\r\n";
			break;
		case ERR_CANNOTSENDTOCHAN:
			response += msg._paramlist.front() + " :Cannot send to channel\r\n";
			break;
		case ERR_UNKNOWNCOMMAND:
			response += msg._command + " :Unknown command\r\n";
			break;
		case ERR_ERRONEUSNICKNAME:
			response += msg._paramlist.front() + " :Erroneus nickname\r\n";
			break;
		case ERR_NICKNAMEINUSE:
			response += msg._paramlist.front() + " :Nickname is already in use\r\n";
			break;
		case ERR_NOTONCHANNEL:
			response += msg._paramlist.front() + " :You're not on that channel\r\n";
			break;
		case ERR_NOTREGISTERED:
			response += ":You have not registered\r\n";
			break;
		case ERR_NEEDMOREPARAMS:
			response += msg._command + " :Not enough parameters\r\n";
			break;
		case ERR_ALREADYREGISTRED:
			response += ":You may not reregister\r\n";
			break;
		case ERR_PASSWDMISMATCH:
			response += ":Password incorrect\r\n";
			break;
		case ERR_CHANOPRIVSNEEDED:
			response += msg._paramlist.front() + " :You're not channel operator\r\n";
			break;
		default:
			response += ":Unknown error\r\n";
			break;
	}
	send(client.get_fd(), response.c_str(), response.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
}

void	IRC::send_reply(Client& client, Message& msg, int reply)
{
	std::stringstream	ss;
	std::string			code;
	std::string			response;

	ss << reply;
	code = ss.str();
	while (code.size() < 3)
		code = "0" + code;
	response = ":" + _servname + " " + code + " " + client.get_nickname() + " ";
	switch (reply)
	{
		case RPL_WELCOME:
			response += ":Welcome to the " + _servname + ", " + client.get_who() + "\r\n";
			break;
		case RPL_NAMREPLY:
			response += "= " + msg._paramlist.front() + " :" \
				+ _channels[msg._paramlist.front()].get_names() + "\r\n";
			break;
		case RPL_ENDOFNAMES:
			response += msg._paramlist.front() + " :End of /NAMES list\r\n";
			break;
		default:
			response += ":Unknown reply\r\n";
			break;
	}
	send(client.get_fd(), response.c_str(), response.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
}

void	IRC::send_channel(Client& client, std::string& response, Channel& channel, bool self)
{
	std::list<Client*>::const_iterator	it;

	for (it = channel.get_clients().begin(); it != channel.get_clients().end(); ++it)
	{
		if ((**it).get_nickname() != client.get_nickname() || self)
			send(_reglist[(**it).get_nickname()]->get_fd(), response.c_str(), \
				response.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
	}
}

void	IRC::send_all_channels(Client& client, std::string& response, bool self)
{
	std::list<Channel*>::const_iterator	it;

	for (it = client.get_channels().begin(); it != client.get_channels().end(); ++it)
		send_channel(client, response, **it, self);
}

int	IRC::check_nick(const std::string& nick)
{
	if (nick.size() > 9)
		return 1;
	for (size_t i = 0; i < nick.size(); ++i)
	{
		if (('A' <= nick[i] && nick[i] <= 'Z') \
			|| ('a' <= nick[i] && nick[i] <= 'z') \
			|| ('0' <= nick[i] && nick[i] <= '9') \
			|| strchr("-[]\\`^{}|", nick[i]))
			continue;
		return 1;
	}
	return 0;
}

int	IRC::check_channel(const std::string& name)
{
	if (name[0] != '#')
		return 1;
	for (size_t i = 0; i < name.size(); ++i)
	{
		if (name[i] == ' ' || name[i] == 7 || name[i] == ',')
			return 1;
	}
	return 0;
}

void	IRC::cmd_cap(Client& client, Message& msg)
{
	(void)client;
	(void)msg;
}

void	IRC::cmd_ping(Client& client, Message& msg)
{
	std::string								response;
	std::list<std::string>::const_iterator	it;

	response  = "PONG";
	for (it = msg._paramlist.begin(); it != msg._paramlist.end(); ++it)
		response += " " + *it;
	response += "\r\n";
	send(client.get_fd(), response.c_str(), response.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
}

void	IRC::cmd_pass(Client& client, Message& msg)
{
	if (!msg._paramlist.size())
		send_error(client, msg, ERR_NEEDMOREPARAMS);
	else if (client.get_status() && !client.get_nickname().empty())
		send_error(client, msg, ERR_ALREADYREGISTRED);
	else if (!client.get_status() && msg._paramlist.front() != client.get_password())
		send_error(client, msg, ERR_PASSWDMISMATCH);
	else
		client.set_status(true);
}

void	IRC::cmd_nick(Client& client, Message& msg)
{
	std::map<std::string, Client*>::const_iterator	it;
	std::string										response;

	if (msg._paramlist.empty())
		send_error(client, msg, ERR_NEEDMOREPARAMS);
	else if (check_nick(msg._paramlist.front()))
		send_error(client, msg, ERR_ERRONEUSNICKNAME);
	else if ((it = _reglist.find(msg._paramlist.front())) != _reglist.end())
		send_error(client, msg, ERR_NICKNAMEINUSE);
	else
	{
		response = ":" + client.get_who() + " " + msg._command + " :" \
			+ msg._paramlist.front() + "\r\n";
		_reglist.erase(client.get_nickname());
		client.set_nickname(msg._paramlist.front());
		_reglist[client.get_nickname()] = &client;
		send_all_channels(client, response, true);
	}
}

void	IRC::cmd_user(Client& client, Message& msg)
{
	if (client.get_nickname().empty())
		send_error(client, msg, ERR_NOTREGISTERED);
	else if (!msg._paramlist.size())
		send_error(client, msg, ERR_NEEDMOREPARAMS);
	else if (!client.get_username().empty())
		send_error(client, msg, ERR_ALREADYREGISTRED);
	else
	{
		client.set_username(msg._paramlist.front());
		send_reply(client, msg, RPL_WELCOME);
	}
}

void	IRC::cmd_quit(Client& client, Message& msg)
{
	std::list<Channel*>::const_iterator		it;
	std::list<std::string>::const_iterator	it2;
	std::string								response;

	if (!client.get_nickname().empty())
	{
		response = ":" + client.get_who() + " " + msg._command + " :Quit:";
		if (msg._paramlist.size())
		{
			for (it2 = msg._paramlist.begin(); it2 != msg._paramlist.end(); ++it2)
				response += " " + *it2;
		}
		response += "\r\n";
		send_all_channels(client, response, false);
		for (it = client.get_channels().begin(); it != client.get_channels().end(); ++it)
		{
			(**it).del_client(client);
			if (!(**it).get_clients().size())
				_channels.erase((**it).get_name());
		}
		_reglist.erase(client.get_nickname());
	}
	close(client.get_fd());
	std::cout << "Client #" << client.get_fd() << " gone away\n";
	_clients.erase(client.get_fd());
}

void	IRC::cmd_join(Client& client, Message& msg)
{
	std::string	response;

	if (client.get_username().empty())
		send_error(client, msg, ERR_NOTREGISTERED);
	else if (!msg._paramlist.size())
		send_error(client, msg, ERR_NEEDMOREPARAMS);
	else if (check_channel(msg._paramlist.front()))
		send_error(client, msg, ERR_NOSUCHCHANNEL);
	else
	{
		Channel&	channel = _channels[msg._paramlist.front()];

		if (channel.get_name().empty())
			channel.set_name(msg._paramlist.front());
		if (find(client.get_channels().begin(), client.get_channels().end(), &channel) \
			!= client.get_channels().end())
			return;
		client.add_channel(channel);
		channel.add_client(client);
		response = ":" + client.get_who() + " " + msg._command + " " \
			+ msg._paramlist.front() + "\r\n";
		send_channel(client, response, channel, true);
		send_reply(client, msg, RPL_NAMREPLY);
		send_reply(client, msg, RPL_ENDOFNAMES);
	}
}

void	IRC::cmd_part(Client& client, Message& msg)
{
	std::map<std::string, Channel>::iterator	it;
	std::list<Channel*>::const_iterator			it2;
	std::list<std::string>::const_iterator		it3;
	std::string									response;

	if (client.get_username().empty())
		send_error(client, msg, ERR_NOTREGISTERED);
	else if (!msg._paramlist.size())
		send_error(client, msg, ERR_NEEDMOREPARAMS);
	else
	{
		it = _channels.find(msg._paramlist.front());
		if (it == _channels.end())
		{
			send_error(client, msg, ERR_NOSUCHCHANNEL);
			return;
		}
		it2 = find(client.get_channels().begin(), client.get_channels().end(), &(it->second));
		if (it2 == client.get_channels().end())
		{
			send_error(client, msg, ERR_NOTONCHANNEL);
			return;
		}
		response = ":" + client.get_who() + " " + msg._command + " " + msg._paramlist.front();
		if (msg._paramlist.size() > 1)
		{
			it3 = ++msg._paramlist.begin();
			for (; it3 != msg._paramlist.end(); ++it3)
				response += " " + *it3;
		}
		response += "\r\n";
		send_channel(client, response, **it2, true);
		client.del_channel(**it2);
		it->second.del_client(client);
		if (!it->second.get_clients().size())
			_channels.erase(it->second.get_name());
	}
}

void	IRC::cmd_kick(Client& client, Message& msg)
{
	std::map<std::string, Channel>::iterator		it;
	std::map<std::string, Client *>::const_iterator	it2;
	std::list<std::string>::const_iterator			it3;
	std::string										response;

	if (client.get_username().empty())
		send_error(client, msg, ERR_NOTREGISTERED);
	else if (msg._paramlist.size() < 2)
		send_error(client, msg, ERR_NEEDMOREPARAMS);
	else if (check_channel(msg._paramlist.front()))
		send_error(client, msg, ERR_NOSUCHCHANNEL);
	else
	{
		it = _channels.find(msg._paramlist.front());
		if (it == _channels.end())
		{
			send_error(client, msg, ERR_NOSUCHCHANNEL);
			return;
		}
		if (find(client.get_channels().begin(), client.get_channels().end(), &(it->second)) \
			== client.get_channels().end())
		{
			send_error(client, msg, ERR_NOTONCHANNEL);
			return;
		}
		if (it->second.get_clients().front()->get_nickname() != client.get_nickname())
		{
			send_error(client, msg, ERR_CHANOPRIVSNEEDED);
			return;
		}
		it2 = _reglist.find(*(++msg._paramlist.begin()));
		if (it2 != _reglist.end() && find(it2->second->get_channels().begin(), \
			it2->second->get_channels().end(), &(it->second)) != it2->second->get_channels().end())
		{
			response = ":" + client.get_who() + " " + msg._command + " " + msg._paramlist.front() + " ";
			it3 = ++msg._paramlist.begin();
			response += *it3 + " :";
			if (msg._paramlist.size() > 2)
			{
				++it3;
				for (; it3 != msg._paramlist.end(); ++it3)
					response += *it3 + " ";
				response.erase(--response.end());
			}
			else
				response += client.get_nickname();
			response += "\r\n";
			send_channel(client, response, it->second, true);
			it2->second->del_channel(it->second);
			it->second.del_client(*(it2->second));
		}
	}
}

void	IRC::cmd_privmsg(Client& client, Message& msg)
{
	std::map<std::string, Client*>::const_iterator	it;
	std::map<std::string, Channel>::iterator		it2;
	std::list<std::string>::const_iterator			it3;
	std::string										response;

	if (client.get_username().empty())
		send_error(client, msg, ERR_NOTREGISTERED);
	else if (msg._paramlist.size() < 2)
		send_error(client, msg, ERR_NEEDMOREPARAMS);
	else
	{
		response = ":" + client.get_who() + " " + msg._command + " ";
		if (msg._paramlist.front()[0] != '#')
		{
			it = _reglist.find(msg._paramlist.front());
			if (it == _reglist.end())
			{
				send_error(client, msg, ERR_NOSUCHNICK);
				return;
			}
			response += it->second->get_nickname() + " :";
			it3 = ++msg._paramlist.begin();
			for (; it3 != msg._paramlist.end(); ++it3)
				response += *it3 + " ";
			response.erase(--response.end());
			response += "\r\n";
			send(it->second->get_fd(), response.c_str(), \
				response.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		}
		else
		{
			it2 = _channels.find(msg._paramlist.front());
			if (it2 == _channels.end())
			{
				send_error(client, msg, ERR_NOSUCHCHANNEL);
				return;
			}
			if (find(client.get_channels().begin(), client.get_channels().end(), &(it2->second)) \
				== client.get_channels().end())
			{
				send_error(client, msg, ERR_CANNOTSENDTOCHAN);
				return;
			}
			response += it2->second.get_name() + " :";
			it3 = ++msg._paramlist.begin();
			for (; it3 != msg._paramlist.end(); ++it3)
				response += *it3 + " ";
			response.erase(--response.end());
			response += "\r\n";
			send_channel(client, response, it2->second, false);
		}
	}
}

void	IRC::cmd_notice(Client& client, Message& msg)
{
	std::map<std::string, Client*>::const_iterator	it;
	std::map<std::string, Channel>::iterator		it2;
	std::list<std::string>::const_iterator			it3;
	std::string										response;

	if (client.get_username().empty())
		send_error(client, msg, ERR_NOTREGISTERED);
	else if (msg._paramlist.size() < 2)
		send_error(client, msg, ERR_NEEDMOREPARAMS);
	else
	{
		response = ":" + client.get_who() + " " + msg._command + " ";
		if (msg._paramlist.front()[0] != '#')
		{
			it = _reglist.find(msg._paramlist.front());
			if (it == _reglist.end())
			{
				send_error(client, msg, ERR_NOSUCHNICK);
				return;
			}
			response += it->second->get_nickname() + " :";
			it3 = ++msg._paramlist.begin();
			for (; it3 != msg._paramlist.end(); ++it3)
				response += *it3 + " ";
			response.erase(--response.end());
			response += "\r\n";
			send(it->second->get_fd(), response.c_str(), \
				response.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		}
		else
		{
			it2 = _channels.find(msg._paramlist.front());
			if (it2 == _channels.end())
			{
				send_error(client, msg, ERR_NOSUCHCHANNEL);
				return;
			}
			if (find(client.get_channels().begin(), client.get_channels().end(), &(it2->second)) \
				== client.get_channels().end())
			{
				send_error(client, msg, ERR_CANNOTSENDTOCHAN);
				return;
			}
			response += it2->second.get_name() + " :";
			it3 = ++msg._paramlist.begin();
			for (; it3 != msg._paramlist.end(); ++it3)
				response += *it3 + " ";
			response.erase(--response.end());
			response += "\r\n";
			send_channel(client, response, it2->second, false);
		}
	}
}

void	IRC::cmd_mode(Client& client, Message& msg)
{
	(void)client;
	(void)msg;
}

void	IRC::cmd_who(Client& client, Message& msg)
{
	(void)client;
	(void)msg;
}
