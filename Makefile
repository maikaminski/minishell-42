NAME = minishell

CC = cc
CFLAGS = -Wall -Werror -Wextra -g3

INCS = include libft # Colocar as pastas onde tem .h
CPPFLAGS = $(addprefix -I,$(INCS)) -MMD -MP # Facilitar o include das pastas
SRC = 	gc/garbage.c \
		src/parser/variable_expansion_utils.c \
		src/parser/token_parser2.c \
		src/parser/token_parser.c \
		src/parser/command_builder.c \
		src/parser/variable_expansion.c \
		src/parser/string_utils.c \
		src/parser/parser_utils.c \
		src/parser/parser.c \
		src/parser/parser_utils2.c \
		src/parser/quote_handlers.c \
		src/builtins/pwd.c \
		src/builtins/cd.c \
		src/builtins/unset.c \
		src/builtins/exit.c \
		src/builtins/export.c \
		src/builtins/echo.c \
		src/builtins/env.c \
		src/signal/signal.c \
		src/env/envp.c \
		src/minishell_main.c \
		src/heredoc/heredoc.c \
		src/pipe/pipe_utils.c \
		src/pipe/pipe2.c \
		src/pipe/pipe.c \
		src/pipe/pipe_utils2.c \
		src/pipe/pipe_utils3.c \
		src/redirect/redirect_out.c \
		src/redirect/redirect_in.c \
		src/exec/exec_cmd.c \
		src/exec/exec_paths.c \
		src/utils/utils_helpers.c \
		src/utils/utils_exec.c \
		src/utils/utils_env.c \
		src/utils/utils_cleanup.c \
		src/utils/utils_export.c \
		src/utils/utils_builtins.c \
		src/utils/utils_exit.c \
		src/utils/utils_main2.c \
		src/utils/utils_main.c \
		src/utils/utils.c \
		src/utils/ft_itoa_buf.c

OBJS = $(SRC:%.c=%.o)

LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

all: $(LIBFT) $(NAME)

# Compila libft primeiro
$(LIBFT):
	make -C $(LIBFT_DIR)

# Compila o minishell
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(NAME) $(OBJS) -L$(LIBFT_DIR) -lft -lreadline
	chmod +x $(NAME)
	rm -f $(OBJS) $(OBJS:.o=.d)
	echo	"███    ███ ██ ███    ██ ██ ███████ ██   ██ ███████ ██      ██     "
	echo	"████  ████ ██ ████   ██ ██ ██      ██   ██ ██      ██      ██     "
	echo	"██ ████ ██ ██ ██ ██  ██ ██ ███████ ███████ █████   ██      ██     "
	echo	"██  ██  ██ ██ ██  ██ ██ ██      ██ ██   ██ ██      ██      ██     "
	echo	"██      ██ ██ ██   ████ ██ ███████ ██   ██ ███████ ███████ ███████"

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

val: $(NAME) 
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=readline.supp --log-file=valgrind_log.txt ./$(NAME)
clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re 

.SILENT: