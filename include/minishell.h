#ifndef MINISHELL_H
# define MINISHELL_H

/* Includes de sistema */
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <stdbool.h>
# include <stdarg.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <signal.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>

/* Includes locais */
# include "libft.h"
# include "garbage_collector.h"

/* Variável global para sinais (única permitida) */
extern volatile sig_atomic_t	g_signal;

/* Variável global para ambiente */
extern char **__environ;

/* Enums para tipos de redirecionamento */
typedef enum e_redir_type
{
	REDIR_IN,      // <
	REDIR_OUT,     // >
	REDIR_APPEND,  // >>
	REDIR_HEREDOC  // <<
}	t_redir_type;

/* Enums para tipos de tokens */
typedef enum e_tokens
{
	T_WORD,         // palavra normal (comando ou argumento)
	T_PIPE,         // |
	T_REDIR_IN,     // <
	T_REDIR_OUT,    // >
	T_REDIR_APPEND, // >>
	T_REDIR_HEREDOC // <<
}	t_tokens;

/* Estrutura para tokens */
typedef struct s_token
{
	char			*value;
	t_tokens		type;
	struct s_token	*next;
}	t_token;

/* Estrutura para redirecionamentos */
typedef struct s_redir
{
	t_redir_type	type;   // tipo de redirecionamento
	char			*file;  // arquivo de entrada/saída ou delimitador (heredoc)
	struct s_redir	*next;  // lista encadeada de redirecionamentos
}	t_redir;

/* Estrutura para comandos */
typedef struct s_commands
{
	char				**argv;  // vetor de strings: nome do comando e argumentos
	t_redir				*redir; // lista de redirecionamentos
	struct s_commands	*next;  // próximo comando (para pipeline)
}	t_commands;

/* Estrutura para variáveis de ambiente */
typedef struct s_env
{
	char			*key;   // nome da variável (ex: PATH)
	char			*value; // valor da variável (ex: "/usr/bin:/bin")
	struct s_env	*next;  // lista encadeada
}	t_env;

/* Estrutura principal do minishell */
typedef struct s_minishell
{
	t_commands	*commands;	// lista de comandos (um por pipe)
	t_env		*env;		// lista de variáveis de ambiente
	int			last_exit;	// código de saída do último comando executado
	int			in_fd;		// STDIN original (para restaurar)
	int			out_fd;		// STDOUT original (para restaurar)
	t_garbage	*gc;		// garbage collector
}	t_minishell;

/* Estrutura para dados de execução de pipeline */
typedef struct s_exec_data
{
	int			num_cmds;      // número total de comandos
	int			i;             // índice do comando atual
	int			prev_read_fd;  // fd de leitura do pipe anterior
	int			pipe_fd[2];    // fds do pipe atual
	pid_t		pid;           // PID do processo filho
	t_commands	*cmd;          // comando atual
}	t_exec_data;

/* ========== PARSER ========== */
/* Tokenização e parsing */
t_token		*tokenize(char *input, t_minishell *mini);
t_commands	*parse_tokens_to_commands(t_token *tokens, t_garbage **gc);
void		print_command_structure(t_commands *cmd_list);

/* Manipulação de quotes */
char		*handle_single_quotes(char *input, int *i, t_minishell *mini);
char		*handle_double_quotes(char *input, int *i, t_minishell *mini);
char		*extract_quoted_token(char *input, int *i, t_minishell *mini);

/* Expansão de variáveis */
char		*expand_variables(char *str, t_minishell *mini);
char		*extract_var_name(char *str, int *pos, t_garbage **gc);
char		*get_var_value(char *var_name, t_minishell *mini);
char		*extract_raw_content(char *input, int start, int len, t_garbage **gc);
char		*process_dollar_sign(char *str, int *i, char *result, t_minishell *mini);

/* Manipulação de strings */
char		*join_strings(char *s1, char *s2, t_garbage **gc);
char		*append_char(char *str, char c, t_garbage **gc);

/* Construção de comandos */
t_commands	*create_command_node(t_garbage **gc);
t_redir		*create_redir_node(t_redir_type type, char *file, t_garbage **gc);
void		add_redir_to_command(t_commands *cmd, t_redir *new_redir);
int			add_arg_to_command(t_commands *cmd, char *arg, t_garbage **gc);
void		add_command_to_list(t_commands **list, t_commands *new_cmd);

/* ========== EXECUÇÃO ========== */
/* Funções principais de execução */
int			process_input(char *input, t_minishell *mini);
int			execute_single_cmd(t_commands *cmd, t_minishell *mini);
int			fork_and_execute(t_commands *cmd, t_minishell *mini);

/* Execução de comandos */
int			execute_pipeline(t_commands *cmd_list, t_minishell *mini);
int			exec_single_command(t_exec_data *data, t_minishell *mini);
int			exec_cmd(char **args, t_env *env, t_garbage **gc);

/* Funções auxiliares de execução */
int			is_builtin(char *cmd);
int			is_parent_builtin(char *cmd);
int			execute_builtin(t_commands *cmd, t_minishell *mini);
void		execute_external_cmd(t_commands *cmd, t_minishell *mini);
//void		execute_builtin_in_child(t_commands *cmd, t_minishell *mini);

/* Funções de pipe */
int			count_commands(t_commands *cmd_list);
void		setup_initial_vars(t_exec_data *data, t_commands *cmd_list);
int			create_pipe_if_needed(t_commands *cmd, int pipe_fd[2]);
void		child_procces_logic(t_commands *cmd, int prev_read_fd, 
				int pipe_fd[2], t_minishell *mini);
void		parent_procces_logic(int *prev_read_fd, int pipe_fd[2], 
				t_commands *cmd);

/* Redirecionamentos */
int			handle_redirections(t_redir *redir_list, t_minishell *mini);
int			setup_redirections(t_redir *redir_list, t_minishell *mini);
int			setup_output_redirection(t_redir *redir);
int			setup_input_redir(char *file);
int			setup_output_redir(char *file, int append);
int			handle_input_redirection(t_redir *redir, t_minishell *mini);
int			setup_input_redirections(t_commands *cmd, t_minishell *mini);

/* Heredoc */
int			handle_heredoc(const char *delimiter, t_minishell *mini);
int			handle_heredoc_redirection(t_redir *redir, t_minishell *mini);
char		*read_line_stdin(t_garbage **gc);

/* Paths e comandos externos */
char		*get_cmd_path(char *cmd, t_env *env, t_garbage **gc);
char		**get_paths(t_env *env, t_garbage **gc);
char		*find_cmd_in_paths(char **paths, char *cmd, t_garbage **gc);
char		*construct_path(char *dir, char *cmd, t_garbage **gc);

/* ========== BUILTINS ========== */
int			ft_echo(char **argv, t_minishell *mini);
int			ft_cd(char **argv, t_minishell *mini);
int			ft_env(t_minishell *mini);
int			ft_exit(char **argv, t_minishell *mini);
int			ft_export(char **argv, t_minishell *mini);
int			ft_pwd(t_minishell *mini);
int			ft_unset(char **argv, t_minishell *mini);

/* Funções auxiliares de builtins */
bool		is_valid_id_export(const char *key);
bool		is_valid_identifier(const char *key);
void		print_sorted_env(t_env *env, t_minishell *mini);
void		print_env_line(t_env *node);
void		print_export_error(const char *arg);
int			process_key(char *arg, t_minishell *mini);
int			export_variable(char *arg, t_minishell *mini);
void		remove_env_node(t_env **env, const char *key);
t_env		*copy_env_node(t_env *src, t_garbage **gc);
void		insert_sorted_env_node(t_env **sorted, t_env *new_node);
size_t		max_strlen(const char *s1, const char *s2);

/* ========== AMBIENTE ========== */
void		init_env_list(t_minishell *mini, char **envp);
void		set_env_value(t_env **env, const char *key, 
				const char *value, t_garbage **gc);
t_env		*find_env_key(t_env *env, const char *key);
t_env		*create_env_node(const char *key, const char *value, t_garbage **gc);
char		*get_env_value(t_env *env, const char *key);
void		update_env_value(t_env *found, const char *value, t_garbage **gc);
void		append_env_node(t_env **env, t_env *new);
char		**env_list_to_array(t_env *env, t_garbage **gc);
char		*make_env_entry(t_env *env, t_garbage **gc);

/* ========== UTILITÁRIOS ========== */
char		is_space(char c);
char		is_operator(char c);
bool		is_numeric_arg(char *str);
int			ft_atol(const char *nbr);
char		*ft_strjoin_and_free(char *s1, char *s2, int free_s);
void		exec_input(char *input, t_minishell *mini);
void		free_array(char **arr);

/* ========== SINAIS ========== */
void		signal_handler_interactive(int sig);
void		setup_signals_interactive(void);

#endif // MINISHELL_H
