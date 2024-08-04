# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/31 11:02:12 by psalame           #+#    #+#              #
#    Updated: 2024/08/04 19:20:14 by psalame          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC				=	cc

CFLAGS			=	-Wall -Wextra -Werror

NAME			=	libgetopt.a

SRCS			=	getopt.c 			\
					sort_arg_options.c	\

OBJS			=	$(addprefix .build/, $(SRCS:.c=.o))

INCLUDES_PATH	=	includes

INCLUDES		=	$(INCLUDES_PATH)/getopt_Int.h $(INCLUDES_PATH)/getopt.h

all: $(NAME)

$(NAME): $(OBJS) 
	ar -rc $@ $^ $(INCLUDES)

.build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $^ -o $@ -I $(INCLUDES_PATH)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: bonus all clean fclean re