# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/01 19:58:48 by makamins          #+#    #+#              #
#    Updated: 2025/08/11 19:00:00 by sabsanto          ###   ########.fr        #
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
GC_DIR = gc

MAIN_SRC = $(SRC_DIR)/minishell_main.c

PARSER_SRC = $(SRC_DIR)/parser/parser.c \
             $(SRC_DIR)/parser/parser_utils.c \
             $(SRC_DIR)/parser/quote_handlers.c \
             $(SRC_DIR)/parser/variable_expansion.c \
             $(SRC_DIR)/parser/variable_expansion_utils.c \
             $(SRC_DIR)/parser/string_utils.c

BUILTINS_SRC = $(SRC_DIR)/builtins/echo.c \
               $(SRC_DIR)/builtins/cd.c \
               $(SRC_DIR)/builtins/pwd.c \
               $(SRC_DIR)/builtins/export.c \
               $(SRC_DIR)/builtins/unset.c \
               $(SRC_DIR)/builtins/env.c \
               $(SRC_DIR)/builtins/exit.c

EXEC_SRC = $(SRC_DIR)/exec/exec_cmd.c \
           $(SRC_DIR)/exec/exec_paths.c

ENV_SRC = $(SRC_DIR)/env/envp.c

UTILS_SRC = $(SRC_DIR)/utils/utils.c \
            $(SRC_DIR)/utils/utils_env.c \
            $(SRC_DIR)/utils/utils_export.c \
            $(SRC_DIR)/utils/utils_builtins.c \
            $(SRC_DIR)/utils/utils_exec.c

GC_SRC = $(GC_DIR)/garbage.c

SRC = $(MAIN_SRC) $(PARSER_SRC) $(BUILTINS_SRC) $(EXEC_SRC) \
      $(ENV_SRC) $(UTILS_SRC) $(GC_SRC)

OBJS = $(SRC:%.c=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

LIBFT = $(LIBFT_DIR)/libft.a

BLUE = \033[36m
PINK = \033[35m
NC = \033[0m
GREEN = \033[0;32m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m

all: $(LIBFT) $(NAME)

$(LIBFT):
	@echo "$(YELLOW)Compiling libft...$(NC)"
	@make -C $(LIBFT_DIR) --no-print-directory

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@echo "$(YELLOW)Linking $(NAME)...$(NC)"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -o $(NAME) $(OBJS) -L$(LIBFT_DIR) -lft -lreadline
	@chmod +x $(NAME)
	@echo "$(PINK)"
	@echo "🔥🔥🔥     😈😈😈   💀💀💀     😈😈     💀💀💀   😈😈😈     🔥🔥🔥"
	@echo "███╗   ███╗██╗███╗   ██╗██╗███████╗██╗  ██╗███████╗██╗     ██╗     "
	@echo "████╗ ████║██║████╗  ██║██║██╔════╝██║  ██║██╔════╝██║     ██║     "
	@echo "██╔████╔██║██║██╔██╗ ██║██║███████╗███████║█████╗  ██║     ██║     "
	@echo "██║╚██╔╝██║██║██║╚██╗██║██║╚════██║██╔══██║██╔══╝  ██║     ██║     "
	@echo "██║ ╚═╝ ██║██║██║ ╚████║██║███████║██║  ██║███████╗███████╗███████╗"
	@echo "╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚═╝╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝"
	@echo "🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥"
	@echo "$(NC)"
	@echo "$(GREEN)🔥 Compilation successful! 😈$(NC)"

clean:
	@echo "$(GREEN)Cleaning objects...$(NC)"
	@rm -rf $(OBJ_DIR)
	@make -C $(LIBFT_DIR) clean --no-print-directory

fclean: clean
	@echo "$(BLUE)Removing $(NAME)...$(NC)"
	@rm -f $(NAME)
	@make -C $(LIBFT_DIR) fclean --no-print-directory

re: fclean all

valgrind: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes \
	         --suppressions=readline.supp ./$(NAME)

norm:
	@echo "$(YELLOW)Checking Norm...$(NC)"
	@norminette $(SRC_DIR) include $(GC_DIR) $(LIBFT_DIR) | grep -v "OK!" || echo "$(GREEN)✓ Norm OK!$(NC)"

.PHONY: all clean fclean re valgrind norm

-include $(DEPS)
