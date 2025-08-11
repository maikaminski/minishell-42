/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_main2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makamins <makamins@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 13:54:54 by makamins          #+#    #+#             */
/*   Updated: 2025/08/11 19:00:50 by makamins         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

static void	handle_parent_process(pid_t pid, t_minishell *mini)
{
	int	status;

	if (waitpid(pid, &status, 0) == -1)
	{
		if (errno != ECHILD && errno != EINTR)
			perror("waitpid");
		mini->last_exit = 1;
	}
	else
		handle_child_exit_status(status, mini);
	setup_signals_interactive();
}

void	execute_external_command(t_commands *cmd, t_minishell *mini)
{
	pid_t	pid;

	setup_signals_ignore();
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		mini->last_exit = 1;
		setup_signals_interactive();
		return ;
	}
	else if (pid == 0)
		child_process_exec(cmd, mini);
	else
		handle_parent_process(pid, mini);
}

void	execute_simple_command(t_commands *cmd, t_minishell *mini)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
		return ;
	if (is_builtin_cmd(cmd->argv[0]))
		execute_builtin_with_redirections(cmd, mini);
	else
		execute_external_command(cmd, mini);
}

void	handle_child_exit_status(int status, t_minishell *mini)
{
	int	sig;

	if (WIFEXITED(status))
		mini->last_exit = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
			write(STDOUT_FILENO, "\n", 1);
		else if (sig == SIGQUIT)
			write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
		mini->last_exit = 128 + sig;
	}
}
