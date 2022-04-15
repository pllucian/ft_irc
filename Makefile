# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pllucian <pllucian@21-school.ru>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/03/14 17:00:03 by pllucian          #+#    #+#              #
#    Updated: 2022/04/19 19:18:09 by pllucian         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS =	Server.cpp \
		IRC.cpp \
		Client.cpp \
		Channel.cpp \
		ircserv.cpp

INCL =	Server.hpp \
		IRC.hpp \
		Client.hpp \
		Channel.hpp

OBJS = $(addprefix objs/, $(SRCS:.cpp=.o))

DEPS = $(wildcard objs/*.d)

NAME = ircserv

CXX = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98

objs/%.o:	srcs/%.cpp
			@ [ -d objs ] || mkdir -p objs
			$(CXX) $(CFLAGS) -MMD -c $< -o $@ -I./includes

$(NAME):	$(OBJS)
			$(CXX) -o $@ $^

all:		$(NAME)

clean:
			$(RM)r $(OBJS) $(DEPS) objs

fclean:		clean
			$(RM) $(NAME)

re:			fclean all

.PHONY:		all clean fclean re

include		$(DEPS)
