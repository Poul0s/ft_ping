CC			= cc

CFLAGS		= -Wall -Wextra -Werror -MD -g

SRCS		=	main.c \
				get_flags.c

NAME		=	ft_ping

OBJS		=	$(addprefix .build/, $(SRCS:.c=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ 

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