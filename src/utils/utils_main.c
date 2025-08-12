/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 13:12:19 by makamins          #+#    #+#             */
/*   Updated: 2025/08/12 03:12:33 by sabsanto         ###   ########.fr       */
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

void	cleanup_memory_and_exec(t_commands *cmd, t_minishell *mini,
	char *cmd_path, char **envp)
{
	if (mini->gc_temp)
	{
		gc_free_all(&mini->gc_temp);
		mini->gc_temp = NULL;
	}
	if (mini->gc_persistent)
	{
		gc_free_all(&mini->gc_persistent);
		mini->gc_persistent = NULL;
	}
	execve(cmd_path, cmd->argv, envp);
	perror(cmd->argv[0]);
	exit(126);
}

void	handle_cmd_not_found(t_minishell *mini, char *cmd)
{
	write(2, cmd, ft_strlen(cmd));
	if (has_slash(cmd))
		write(2, ": No such file or directory\n", 29);
	else
		write(2, ": command not found\n", 20);
	child_exit(mini, 127);
}
