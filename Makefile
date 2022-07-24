NAME = ./mini_serv

SRCS = ./main.c \
			 ./utils.c
OBJS = $(SRCS:.c=.o)

CC = gcc
RM = rm -rf
CFLAGS = -Wall -Werror -Wextra -g
INCLUDE = -I ./utils.h

all: 
	$(MAKE) $(NAME) -j4

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)

clean: 
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)
