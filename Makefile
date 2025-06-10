NAME = minishell
CC = cc
FLAGS = -Wall -Werror -Wextra -g3
SRC = 

OBJS = $(SRC:%.c=%.o)

all: $(NAME)

	
$(NAME): $(OBJS)
	$(CC) $(FLAGS) -o $(NAME) $(OBJS)
	chmod +x $(NAME) 
	@echo	"███    ███ ██ ███    ██ ██ ███████ ██   ██ ███████ ██      ██     "
	@echo	"████  ████ ██ ████   ██ ██ ██      ██   ██ ██      ██      ██     "
	@echo	"██ ████ ██ ██ ██ ██  ██ ██ ███████ ███████ █████   ██      ██     "
	@echo	"██  ██  ██ ██ ██  ██ ██ ██      ██ ██   ██ ██      ██      ██     "
	@echo	"██      ██ ██ ██   ████ ██ ███████ ██   ██ ███████ ███████ ███████"

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re 
