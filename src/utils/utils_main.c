/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 13:12:19 by makamins          #+#    #+#             */
/*   Updated: 2025/08/11 23:07:03 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

void	save_std_fds(int *saved_stdin, int *saved_stdout)
{
	*saved_stdin = dup(STDIN_FILENO);
	*saved_stdout = dup(STDOUT_FILENO);
}

void	restore_std_fds(int saved_stdin, int saved_stdout)
{
	if (saved_stdin != -1)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout != -1)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
}

void	execute_builtin_with_redirections(t_commands *cmd, t_minishell *mini)
{
	int	saved_stdin;
	int	saved_stdout;

	saved_stdin = -1;
	saved_stdout = -1;
	if (cmd->redir)
		save_std_fds(&saved_stdin, &saved_stdout);
	if (cmd->redir && handle_redirections(cmd->redir, mini) == -1)
	{
		mini->last_exit = 1;
		restore_std_fds(saved_stdin, saved_stdout);
		return ;
	}
	mini->last_exit = execute_builtin(cmd, mini);
	restore_std_fds(saved_stdin, saved_stdout);
}

static void	handle_command_not_found(t_minishell *mini, char *cmd)
{
	write(2, cmd, ft_strlen(cmd));
	if (has_slash(cmd))
		write(2, ": No such file or directory\n", 29);
	else
		write(2, ": command not found\n", 20);
	child_exit(mini, 127);
}

void	child_process_exec(t_commands *cmd, t_minishell *mini)
{
	char	**envp;
	char	*cmd_path;

	// IMPORTANTE: NÃO liberar gc_temp aqui porque ainda precisamos dos dados do comando
	// O processo filho vai terminar com exit(), então a memória será automaticamente liberada
	
	setup_signals_child();
	if (cmd->redir && handle_redirections(cmd->redir, mini) == -1)
		child_exit(mini, 1);
	if (!cmd->argv[0] || is_str_empty_or_whitespace(cmd->argv[0]))
		child_exit(mini, 0);
	
	// Agora sim, podemos criar novas alocações para envp e cmd_path
	envp = env_list_to_array(mini->env, &mini->gc_temp);
	if (!envp)
		child_exit(mini, 1);
	cmd_path = get_cmd_path(cmd->argv[0], mini->env, &mini->gc_temp);
	if (!cmd_path)
		handle_command_not_found(mini, cmd->argv[0]);
	execve(cmd_path, cmd->argv, envp);
	perror(cmd->argv[0]);
	child_exit(mini, 126);
}
