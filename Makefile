CC			= cc

CFLAGS		= -Wall -Wextra -Werror -MD -g

SRCS		=	main.c \
				get_flags.c

LIB_OPT		=	libs/ft_getopt/libgetopt.a

NAME		=	ft_ping

OBJS		=	$(addprefix .build/, $(SRCS:.c=.o))

all: $(LIB_OPT) $(NAME)

$(LIB_OPT):
	make -C $(shell dirname $(LIB_OPT))

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB_OPT) -o $@ 

.build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

-include $(OBJS:.o=.d)

.phony: all clean fclean re