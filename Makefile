NAME		= push_swap
BONUS_NAME	= checker

CC		= cc
CFLAGS		= -Wall -Wextra -Werror
RM		= rm -f

LIB		= libftprintf.a
HEADERS		= push_swap.h ft_printf.h checker.h get_next_line.h

SRCS		= main.c \
		  parse_args.c \
		  parse_args_utiles.c \
		  push_swap_utiles.c \
		  helpers.c \
		  operations.c \
		  adaptive.c \
		  sort_semple.c \
		  sort_medium.c \
		  sort_medium_utils.c \
		  sort_complex.c \
		  bench.c \
		  ft_dprintf.c \
		  ft_split.c \

BONUS_SRCS	= checker_bonus.c \
		  checker_helper_bonus.c \
		  checker_operations_bonus.c \
		  get_next_line.c \
		  get_next_line_utils.c \
		  cheker_bonus_utiles.c \
		  helpers.c \
		  ft_split.c \
			parse_args.c \
		  parse_args_utiles.c \



OBJS		= $(SRCS:.c=.o)
BONUS_OBJS	= $(BONUS_SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB) -o $(NAME)

$(BONUS_NAME): $(BONUS_OBJS)
	$(CC) $(CFLAGS) $(BONUS_OBJS) -o $(BONUS_NAME)

bonus: $(BONUS_NAME)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(BONUS_OBJS)

fclean: clean
	$(RM) $(NAME) $(BONUS_NAME)

re: fclean all

.PHONY: all bonus clean fclean re