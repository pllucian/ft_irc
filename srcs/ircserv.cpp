/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pllucian <pllucian@21-school.ru>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/14 17:12:17 by pllucian          #+#    #+#             */
/*   Updated: 2022/03/29 20:28:11 by pllucian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"

int	main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Error: need two arguments: <port> <password>\n";
		return 1;
	}

	Server	server(argv[1], argv[2]);

	try
	{
		server.start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return 0;
}
