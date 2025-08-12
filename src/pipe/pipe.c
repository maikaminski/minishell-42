/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makamins <makamins@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:46:32 by makamins          #+#    #+#             */
/*   Updated: 2025/08/12 14:32:40 by makamins         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

static void	setup_pipes_and_redirections(t_commands *cmd, int prev_read_fd,
	int pipe_fd[2], t_minishell *mini)
{
	if (prev_read_fd != -1)
	{
		dup2(prev_read_fd, STDIN_FILENO);
		close(prev_read_fd);
	}
	if (cmd->next)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
	}
	if (handle_redirections(cmd->redir, mini) == -1)
		child_exit(mini, 1);
}

static void	handle_command_not_found(t_commands *cmd, t_minishell *mini)
{
	if (cmd->argv[0] && !is_str_empty_or_whitespace(cmd->argv[0]))
	{
		write(2, cmd->argv[0], ft_strlen(cmd->argv[0]));
		if (has_slash(cmd->argv[0]))
			write(2, ": No such file or directory\n", 29);
		else
			write(2, ": command not found\n", 20);
	}
	child_exit(mini, 127);
}

static void	cleanup_and_exec(t_commands *cmd, t_minishell *mini, char *cmd_path,
	char **envp)
{
	execve(cmd_path, cmd->argv, envp);
	perror(cmd->argv[0]);
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
	exit(127);
}

static void	execute_external_cmd(t_commands *cmd, t_minishell *mini)
{
	char	**envp;
	char	*cmd_path;

	envp = env_list_to_array(mini->env, &mini->gc_temp);
	if (!envp)
		child_exit(mini, 1);
	cmd_path = get_cmd_path(cmd->argv[0], mini->env, &mini->gc_temp);
	if (!cmd_path)
		handle_command_not_found(cmd, mini);
	cleanup_and_exec(cmd, mini, cmd_path, envp);
}

void	child_procces_logic(t_commands *cmd,
	int prev_read_fd, int pipe_fd[2], t_minishell *mini)
{
	int	status;

	expand_cmd_args(cmd, mini);
	setup_pipes_and_redirections(cmd, prev_read_fd, pipe_fd, mini);
	if (!cmd->argv[0] || is_str_empty_or_whitespace(cmd->argv[0]))
		child_exit(mini, 0);
	if (is_builtin_cmd(cmd->argv[0]))
	{
		status = execute_builtin(cmd, mini);
		child_exit(mini, status);
	}
	execute_external_cmd(cmd, mini);
}
