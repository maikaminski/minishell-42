# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/06 19:58:48 by makamins          #+#    #+#              #
#    Updated: 2025/08/06 16:00:00 by assistant         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

# Compilador e flags
CC = cc
CFLAGS = -Wall -Werror -Wextra -g3
INCS = include libft
CPPFLAGS = $(addprefix -I,$(INCS)) -MMD -MP

# Diretórios
SRC_DIR = src
OBJ_DIR = obj
LIBFT_DIR = libft

# Arquivos fonte organizados por categoria
MAIN_SRC = $(SRC_DIR)/minishell.c \
           $(SRC_DIR)/main_exec.c

PARSER_SRC = $(SRC_DIR)/parser/parser.c \
             $(SRC_DIR)/parser/parser_utils.c \
             $(SRC_DIR)/parser/quote_handlers.c \
             $(SRC_DIR)/parser/variable_expansion.c \
             $(SRC_DIR)/parser/variable_expansion_utils.c \
             $(SRC_DIR)/parser/string_utils.c \
             $(SRC_DIR)/parser/token_parser.c \
             $(SRC_DIR)/parser/command_builder.c

BUILTINS_SRC = $(SRC_DIR)/builtins/echo.c \
               $(SRC_DIR)/builtins/cd.c \
               $(SRC_DIR)/builtins/pwd.c \
               $(SRC_DIR)/builtins/export.c \
               $(SRC_DIR)/builtins/unset.c \
               $(SRC_DIR)/builtins/env.c \
               $(SRC_DIR)/builtins/exit.c

EXEC_SRC = $(SRC_DIR)/exec/exec_cmd.c \
           $(SRC_DIR)/exec/exec_paths.c \
           $(SRC_DIR)/exec/exec_helpers.c

PIPE_SRC = $(SRC_DIR)/pipe/pipe.c \
           $(SRC_DIR)/pipe/pipe2.c

REDIRECT_SRC = $(SRC_DIR)/redirect/redirect_in.c \
               $(SRC_DIR)/redirect/redirect_out.c

HEREDOC_SRC = $(SRC_DIR)/heredoc/heredoc.c

ENV_SRC = $(SRC_DIR)/env/envp.c

UTILS_SRC = $(SRC_DIR)/utils/utils.c \
            $(SRC_DIR)/utils/utils_env.c \
            $(SRC_DIR)/utils/utils_export.c \
            $(SRC_DIR)/utils/utils_builtins.c \
            $(SRC_DIR)/utils/utils_exec.c

SIGNAL_SRC = $(SRC_DIR)/signal/signal.c

GC_SRC = gc/garbage.c

# Todos os arquivos fonte
SRC = $(MAIN_SRC) $(PARSER_SRC) $(BUILTINS_SRC) $(EXEC_SRC) \
      $(PIPE_SRC) $(REDIRECT_SRC) $(HEREDOC_SRC) $(ENV_SRC) \
      $(UTILS_SRC) $(SIGNAL_SRC) $(GC_SRC)

# Objetos
OBJS = $(SRC:%.c=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

# Biblioteca libft
LIBFT = $(LIBFT_DIR)/libft.a

# Cores para output
GREEN = \033[0;32m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m

all: $(LIBFT) $(NAME)

# Compila libft
$(LIBFT):
	@echo "$(YELLOW)Compiling libft...$(NC)"
	@make -C $(LIBFT_DIR) --no-print-directory

# Cria diretórios necessários
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compila o minishell
$(NAME): $(OBJS)
	@echo "$(YELLOW)Linking $(NAME)...$(NC)"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -o $(NAME) $(OBJS) -L$(LIBFT_DIR) -lft -lreadline
	@chmod +x $(NAME)
	@echo "$(GREEN)"
	@echo "███    ███ ██ ███    ██ ██ ███████ ██   ██ ███████ ██      ██     "
	@echo "████  ████ ██ ████   ██ ██ ██      ██   ██ ██      ██      ██     "
	@echo "██ ████ ██ ██ ██ ██  ██ ██ ███████ ███████ █████   ██      ██     "
	@echo "██  ██  ██ ██ ██  ██ ██ ██      ██ ██   ██ ██      ██      ██     "
	@echo "██      ██ ██ ██   ████ ██ ███████ ██   ██ ███████ ███████ ███████"
	@echo "$(NC)"
	@echo "$(GREEN)✓ Compilation successful!$(NC)"

clean:
	@echo "$(RED)Cleaning objects...$(NC)"
	@rm -rf $(OBJ_DIR)
	@make -C $(LIBFT_DIR) clean --no-print-directory

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(NC)"
	@rm -f $(NAME)
	@make -C $(LIBFT_DIR) fclean --no-print-directory

re: fclean all

# Regra para testar o minishell com valgrind
valgrind: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes \
	         --suppressions=readline.supp ./$(NAME)

# Regra para testar com norminette
norm:
	@echo "$(YELLOW)Checking Norm...$(NC)"
	@norminette $(SRC_DIR) $(INCS) $(LIBFT_DIR) gc | grep -v "OK!" || echo "$(GREEN)✓ Norm OK!$(NC)"

.PHONY: all clean fclean re valgrind norm

# Include dependencies
-include $(DEPS)