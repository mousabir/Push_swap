NAME        = push_swap

CC          = cc
CFLAGS      = -Wall -Wextra -Werror
RM          = rm -f

SRCS        = start_point.c \
              solv.c \
              medimum.c \
              helpers.c \
              opreter.c \
              pri_ar.c \
              push_swap_utiles.c \
			  

OBJS        = $(SRCS:.c=.o)

all:        $(NAME)

$(NAME):    $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) libftprintf.a -o $(NAME)

%.o:        %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean:     clean
	$(RM) $(NAME)

re:         fclean all

.PHONY:     all clean fclean re