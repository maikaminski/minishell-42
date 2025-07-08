#ifndef MINISHELL_H

# define MINISHELL_H

# include "libft/libft.h"
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <stdbool.h>
# include "include/garbage_collector.h"

//struct de redirecionamentos
typedef enum e_redir_type
{
	REDIR_IN, // <
	REDIR_OUT, // >
	REDIR_APPEND, // >>
	REDIR_HEREDOC // <<
}	t_redir_type;

// possíveis tipos
typedef enum	e_tokens
{
	T_WORD, //palavra normal (comando ou argumento)
	T_PIPE, // |
	T_REDIR_IN,
	T_REDIR_OUT,
	T_REDIR_APPEND,
	T_REDIR_HEREDOC
}	t_tokens;

typedef struct	s_redir
{
	t_redir_type type; //tipo de redirecionamento
	char         *file; //arquivo de entrada/saída ou delimitador (heredoc)
	struct s_redir *next; //// Lista encadeada de redirecionamentos (pode ter vários)
}	t_redir;
// struct de comandos
typedef struct	s_comands
{
	char	**argv; // vetor de strings: nome do comando e argumentos
	t_redir	*redir; // lista de redirecionamentos
	struct s_comands *next; // próximo comando (para a pipeline)
}	t_comands;

// struct variáveis de ambiente
typedef struct	s_env
{
	char	*key; //nome da variável (ex: PATH)
	char	*value; // valor da variável (ex: "/usr/bin:/bin")
	struct s_env 	*next; //lista encadeada
}	t_env;

// struct geral
typedef struct	s_minishell
{
	t_comands	*comands; // lista de comandos (um por pipe)
	t_env		*env; // lista de variáveis de ambiente
	int			last_exit; //código de saída do último comando executado
	int			in_fd; // STDIN original (para restaurar depois)
	int			out_fd; // STDOUT original (para restaurar depois)
	t_garbage	*gc;
}	t_minishell;


bool	is_numeric_arg(char *str);
int	ft_atol(const char *nbr);
char	is_space(char c);
char	is_operator(char c);
char *ft_strjoin_and_free(char *s1, char *s2, int free_s);


void	update_env_value(t_env *found,
	const char *value, t_garbage **gc);
void	append_env_node(t_env **env, t_env *new);
void	set_env_value(t_env **env,
	const char *key, const char *value, t_garbage **gc);
	t_env	*find_env_key(t_env *env, const char *key);
t_env	*create_env_node(const char *key,
	const char *value, t_garbage **gc);
#endif