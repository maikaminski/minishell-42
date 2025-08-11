/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makamins <makamins@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 13:21:54 by makamins          #+#    #+#             */
/*   Updated: 2025/08/11 19:39:23 by makamins         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/wait.h>
# include <signal.h>
# include <errno.h>
# include <fcntl.h>
# include <stdbool.h>
# include <sys/stat.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft.h"
# include "garbage_collector.h"

extern volatile sig_atomic_t	g_signal_received;

typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}	t_redir_type;

typedef enum e_tokens
{
	T_WORD,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_REDIR_APPEND,
	T_REDIR_HEREDOC
}	t_tokens;

typedef struct s_token
{
	t_tokens		type;
	char			*value;
	struct s_token	*next;
}	t_token;

typedef struct s_redir
{
	t_redir_type	type;
	char			*file;
	struct s_redir	*next;
}	t_redir;

typedef struct s_commands
{
	char				**argv;
	t_redir				*redir;
	struct s_commands	*next;
}	t_commands;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_minishell
{
	t_commands	*commands;
	t_env		*env;
	int			last_exit;
	int			in_fd;
	int			out_fd;
	int			should_exit;
	t_garbage	*gc_persistent;
	t_garbage	*gc_temp;
}	t_minishell;

typedef struct s_exec_data
{
	int			num_cmds;
	int			i;
	int			pipe_fd[2];
	int			prev_read_fd;
	t_commands	*cmd;
	pid_t		pid;
	pid_t		last_pid;
}	t_exec_data;

void		signal_handler_interactive(int sig);
void		signal_handler_child(int sig);
void		setup_signals_interactive(void);
void		setup_signals_child(void);
void		setup_signals_ignore(void);

int			validate_input_syntax(char *input);

t_token		*tokenize(char *input, t_minishell *mini);
void		process_operator_token(char *input, int *i, t_token **tokens,
				t_garbage **gc);
t_tokens	get_operator_type(char *input, int pos);
t_token		*create_token(char *value, t_tokens type, t_garbage **gc);
void		add_token_to_list(t_token **list, t_token *new_token);

t_commands	*parse_tokens_to_commands(t_token *tokens, t_garbage **gc,
				t_minishell *mini);

char		*extract_quoted_token(char *input, int *i, t_minishell *mini);
char		*extract_raw_content(char *input, int start, int len,
				t_garbage **gc);

char		*process_dollar_sign(char *str, int *i, char *result,
				t_minishell *mini);

char		*handle_single_quotes(char *input, int *i, t_minishell *mini);
char		*handle_double_quotes(char *input, int *i, t_minishell *mini);
char		*expand_variables(char *str, t_minishell *mini);
char		*expand_exit_status(t_minishell *mini);
char		*extract_var_name(char *str, int *pos, t_garbage **gc);
char		*get_var_value(char *var_name, t_minishell *mini);

t_commands	*create_command_node(t_garbage **gc);
t_redir		*create_redir_node(t_redir_type type, char *file,
				t_garbage **gc);
void		add_redir_to_command(t_commands *cmd, t_redir *new_redir);
int			add_arg_to_command(t_commands *cmd, char *arg, t_garbage **gc);
void		add_command_to_list(t_commands **list, t_commands *new_cmd);
t_env		*find_env_key(t_env *env, const char *key);

bool		is_valid_exit_arg(char *str);
bool		is_valid_identifier(const char *key);
bool		is_valid_id_export(const char *key);
bool		is_numeric_arg(char *str);

void		save_std_fds(int *saved_stdin, int *saved_stdout);
void		restore_std_fds(int saved_stdin, int saved_stdout);
void		execute_builtin_with_redirections(t_commands *cmd,
				t_minishell *mini);
void		child_process_exec(t_commands *cmd, t_minishell *mini);
void		handle_child_exit_status(int status, t_minishell *mini);
void		execute_external_command(t_commands *cmd, t_minishell *mini);
void		execute_simple_command(t_commands *cmd, t_minishell *mini);

char		is_space(char c);
char		is_operator(char c);
int			ft_atol(const char *nbr);
void		ft_itoa_buf(int n, char *buf);
void		ft_putnbr_fd(int n, int fd);
int			has_slash(char *cmd);
void		update_shlvl(t_minishell *mini);
char		*join_strings(char *s1, char *s2, t_garbage **gc);
char		*append_char(char *str, char c, t_garbage **gc);
char		*create_empty_string(t_garbage **gc);
bool		is_str_empty_or_whitespace(const char *str);

void		init_env_list(t_minishell *mini, char **envp);
t_env		*create_env_node(const char *key, const char *value,
				t_garbage **gc);
int			set_env_value(t_env **env, const char *key, const char *value,
				t_garbage **gc);
char		*get_env_value(t_env *env, const char *key);
char		**env_list_to_array(t_env *env, t_garbage **gc);

int			ft_echo(char **argv, t_minishell *mini);
int			ft_cd(char **argv, t_minishell *mini);
int			ft_pwd(t_minishell *mini);
int			ft_export(char **argv, t_minishell *mini);
int			ft_unset(char **argv, t_minishell *mini);
int			ft_env(t_minishell *mini);
int			ft_exit(char **argv, t_minishell *mini);
int			is_builtin_cmd(char *cmd);
char		*ft_strjoin_and_free(char *s1, char *s2, int free_s);
size_t		max_strlen(const char *s1, const char *s2);
void		update_env_value(t_env *found, const char *value,
				t_garbage **gc);
void		child_procces_logic(t_commands *cmd,
				int prev_read_fd, int pipe_fd[2], t_minishell *mini);
void		append_env_node(t_env **env, t_env *new);
void		exec_input(char *input, t_minishell *mini);
int			execute_builtin(t_commands *cmd, t_minishell *mini);

t_env		*copy_env_node(t_env *src, t_garbage **gc);
void		insert_sorted_env_node(t_env **list, t_env *new_node);
void		print_env_line(t_env *node);
void		print_export_error(const char *arg);

int			execute_pipeline(t_commands *cmd_list, t_minishell *mini);
int			exec_cmd(char **args, t_env *env, t_garbage **gc);
int			exec_single_command(t_exec_data *data, t_minishell *mini);
void		setup_initial_vars(t_exec_data *data, t_commands *cmd_list);
char		*get_cmd_path(char *cmd, t_env *env, t_garbage **gc);
char		*find_cmd_in_paths(char **paths, char *cmd, t_garbage **gc);
char		*construct_path(char *dir, char *cmd, t_garbage **gc);
int			count_commands(t_commands *cmd_list);
int			create_pipe_if_needed(t_commands *cmd, int pipe_fd[2]);
void		parent_procces_logic(int *prev_read_fd, int pipe_fd[2],
				t_commands *cmd);

int			handle_redirections(t_redir *redir_list, t_minishell *mini);
int			handle_input_redirection(t_redir *redir, t_minishell *mini);
int			setup_output_redirection(t_redir *redir);
int			handle_heredoc_redirection(t_redir *redir, t_minishell *mini);

int			handle_heredoc(const char *delimiter, t_minishell *mini);
char		*read_line_stdin(t_garbage **gc);

void		free_array(char **arr);
void		child_exit(t_minishell *mini, int code);

#endif
