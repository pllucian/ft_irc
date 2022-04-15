/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pllucian <pllucian@21-school.ru>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/29 16:07:36 by pllucian          #+#    #+#             */
/*   Updated: 2022/04/15 12:46:46 by pllucian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
# define IRC_HPP

# include "Client.hpp"
# include "Channel.hpp"
# include <map>
# include <queue>
# include <sstream>
# include <sys/types.h>
# include <sys/socket.h>
# include <cstring>
# include <unistd.h>
# include <iostream>
# include <algorithm>

# define BUFFER_SIZE	512

# define RPL_WELCOME			001
# define RPL_NAMREPLY			353
# define RPL_ENDOFNAMES			366

# define ERR_NOSUCHNICK			401
# define ERR_NOSUCHCHANNEL		403
# define ERR_CANNOTSENDTOCHAN	404
# define ERR_UNKNOWNCOMMAND		421
# define ERR_ERRONEUSNICKNAME	432
# define ERR_NICKNAMEINUSE		433
# define ERR_NOTONCHANNEL		442
# define ERR_NOTREGISTERED		451
# define ERR_NEEDMOREPARAMS		461
# define ERR_ALREADYREGISTRED	462
# define ERR_PASSWDMISMATCH		464
# define ERR_CHANOPRIVSNEEDED	482

class IRC
{
	public:
		IRC(void);
		IRC(const IRC& src);
		~IRC(void);

		IRC&	operator=(const IRC& rhs);

	private:
		struct Message
		{
			std::string				_prefix;
			std::string				_command;
			std::list<std::string>	_paramlist;
		};

	protected:
		typedef	void (IRC::*Command)(Client& client, IRC::Message& msg);

		std::string						_servname;
		std::map<std::string, Command>	_commands;
		std::map<int, Client>			_clients;
		std::map<std::string, Client*>	_reglist;
		std::map<std::string, Channel>	_channels;

		void	exec_command(Client& client);

	private:
		void	parse_buffer(std::queue<Message>& msgq, std::string& buf);
		void	parse_buffer_part(Message& msg, std::string& part);
		void	send_error(Client& client, Message& msg, int error);
		void	send_reply(Client& client, Message& msg, int reply);
		void	send_channel(Client& client, std::string& response, Channel& channel, bool self);
		void	send_all_channels(Client& client, std::string& response, bool self);
		int		check_nick(const std::string& nick);
		int		check_channel(const std::string& name);

		void	cmd_cap(Client& client, Message& msg);
		void	cmd_ping(Client& client, Message& msg);
		void	cmd_pass(Client& client, Message& msg);
		void	cmd_nick(Client& client, Message& msg);
		void	cmd_user(Client& client, Message& msg);
		void	cmd_quit(Client& client, Message& msg);
		void	cmd_join(Client& client, Message& msg);
		void	cmd_part(Client& client, Message& msg);
		void	cmd_kick(Client& client, Message& msg);
		void	cmd_privmsg(Client& client, Message& msg);
		void	cmd_notice(Client& client, Message& msg);
		void	cmd_mode(Client& client, Message& msg);
		void	cmd_who(Client& client, Message& msg);
};

#endif
