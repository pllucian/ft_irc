/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pllucian <pllucian@21-school.ru>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/11 17:21:12 by pllucian          #+#    #+#             */
/*   Updated: 2022/04/15 11:26:34 by pllucian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"
# include <string>
# include <list>

class Client;

class Channel
{
	public:
		Channel(void);
		Channel(const std::string& name);
		Channel(const Channel& src);
		~Channel(void);

		Channel&	operator=(const Channel& rhs);

		std::string&		get_name(void);
		std::list<Client*>&	get_clients(void);
		std::string			get_names(void);
		void				set_name(const std::string& name);
		void				add_client(Client& client);
		void				del_client(Client& client);

	private:
		std::string			_name;
		std::list<Client*>	_clients;
};

#endif
