NAME        = push_swap

CC          = cc
CFLAGS      = -Wall -Wextra -Werror
RM          = rm -f

SRCS        = start_point.c \
              solv.c \
              medimum.c \
              helpers.c \
              opreter.c \
              push_swap_utiles.c \
              files_for_medium.c \
              Complex.c \
              brench.c \
              ft_dprintf.c \
              chek_arg.c \
			  
BONUS_SRCS = checker.c \
             checker_helper.c \
             helpers.c \
             get_next_line.c \
             get_next_line_utils.c

BONUS_OBJS = $(BONUS_SRCS:.c=.o)

OBJS        = $(SRCS:.c=.o)

all:        $(NAME)

$(NAME):    $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) libftprintf.a -o $(NAME)

%.o:        %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(BONUS_OBJS)

fclean:     clean
	$(RM) $(NAME) checker

re:         fclean all

bonus: checker

checker: $(BONUS_OBJS)
	$(CC) $(CFLAGS) $(BONUS_OBJS) -o checker

.PHONY: all clean fclean re bonus