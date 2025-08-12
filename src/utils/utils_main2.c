/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_main2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 13:54:54 by makamins          #+#    #+#             */
/*   Updated: 2025/08/12 03:13:45 by sabsanto         ###   ########.fr       */
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

void	child_process_exec(t_commands *cmd, t_minishell *mini)
{
	char	**envp;
	char	*cmd_path;

	setup_signals_child();
	if (cmd->redir && handle_redirections(cmd->redir, mini) == -1)
		child_exit(mini, 1);
	if (!cmd->argv[0] || is_str_empty_or_whitespace(cmd->argv[0]))
		child_exit(mini, 0);
	envp = env_list_to_array(mini->env, &mini->gc_temp);
	if (!envp)
		child_exit(mini, 1);
	cmd_path = get_cmd_path(cmd->argv[0], mini->env, &mini->gc_temp);
	if (!cmd_path)
		handle_cmd_not_found(mini, cmd->argv[0]);
	cleanup_memory_and_exec(cmd, mini, cmd_path, envp);
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
