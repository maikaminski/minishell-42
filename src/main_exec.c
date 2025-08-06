/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 00:53:09 by sabsanto          #+#    #+#             */
/*   Updated: 2025/08/06 00:53:16 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: assistant <assistant@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 15:30:00 by assistant         #+#    #+#             */
/*   Updated: 2025/08/06 15:30:00 by assistant         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

// Declarações das funções auxiliares
int		is_builtin(char *cmd);
int		is_parent_builtin(char *cmd);
int		execute_builtin(t_commands *cmd, t_minishell *mini);
void	execute_external_cmd(t_commands *cmd, t_minishell *mini);
int		setup_redirections(t_redir *redir_list, t_minishell *mini);

// Fork e executa comando com redirecionamentos
int	fork_and_execute(t_commands *cmd, t_minishell *mini)
{
	pid_t	pid;
	int		status;
	int		saved_stdin;
	int		saved_stdout;
	
	// Salva FDs originais
	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	
	// Aplica redirecionamentos antes do fork (se houver)
	if (cmd->redir && setup_redirections(cmd->redir, mini) == -1)
	{
		dup2(saved_stdin, STDIN_FILENO);
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdin);
		close(saved_stdout);
		return (1);
	}
	
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		dup2(saved_stdin, STDIN_FILENO);
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdin);
		close(saved_stdout);
		return (1);
	}
	else if (pid == 0)
	{
		// Processo filho
		close(saved_stdin);
		close(saved_stdout);
		
		// Reseta sinais no filho
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		
		// Se for builtin, executa e sai
		if (is_builtin(cmd->argv[0]))
			exit(execute_builtin(cmd, mini));
		
		// Comando externo
		execute_external_cmd(cmd, mini);
		exit(127); // Se chegou aqui, comando não foi encontrado
	}
	
	// Processo pai
	// Restaura FDs originais
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
	
	// Espera o filho terminar
	waitpid(pid, &status, 0);
	
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

// Executa um único comando (sem pipes)
int	execute_single_cmd(t_commands *cmd, t_minishell *mini)
{
	int	status;
	
	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (0);
	
	// Verifica se é um builtin que deve ser executado no processo pai
	if (is_parent_builtin(cmd->argv[0]))
	{
		// Para builtins do pai com redirecionamento, 
		// precisamos fazer fork também
		if (cmd->redir)
			return (fork_and_execute(cmd, mini));
		else
			return (execute_builtin(cmd, mini));
	}
	
	// Cria processo filho para comandos externos ou builtins que modificam FDs
	status = fork_and_execute(cmd, mini);
	return (status);
}

// Processa e executa o input
int	process_input(char *input, t_minishell *mini)
{
	t_token		*tokens;
	t_commands	*cmd_list;
	int			status;

	// Tokeniza o input
	tokens = tokenize(input, mini);
	if (!tokens)
		return (0);
	
	// Converte tokens em estrutura de comandos
	cmd_list = parse_tokens_to_commands(tokens, &mini->gc);
	if (!cmd_list)
	{
		mini->last_exit = 2; // Erro de sintaxe
		return (2);
	}
	
	// Debug: imprime a estrutura (remover na versão final)
	// print_command_structure(cmd_list);
	
	// Salva lista de comandos na estrutura principal
	mini->commands = cmd_list;
	
	// Executa o comando ou pipeline
	if (cmd_list->next)
		status = execute_pipeline(cmd_list, mini);
	else
		status = execute_single_cmd(cmd_list, mini);
	
	mini->last_exit = status;
	
	// Limpa lista de comandos
	mini->commands = NULL;
	
	return (status);
}
