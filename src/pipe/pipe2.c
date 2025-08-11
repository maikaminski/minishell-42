/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makamins <makamins@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 18:30:58 by makamins          #+#    #+#             */
/*   Updated: 2025/08/11 18:46:43 by makamins         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

void	child_procces_logic(t_commands *cmd,
		int prev_read_fd, int pipe_fd[2], t_minishell *mini);

static void	process_last_exit_status(int last_status, t_minishell *mini)
{
	if (WIFEXITED(last_status))
		mini->last_exit = WEXITSTATUS(last_status);
	else if (WIFSIGNALED(last_status))
		mini->last_exit = 128 + WTERMSIG(last_status);
	else
		mini->last_exit = 1;
}

static void	wait_and_set_exit_status(t_exec_data *data, t_minishell *mini)
{
	int	status;
	int	last_status;
	int	i;

	i = data->num_cmds - 1;
	while (i > 0)
	{
		waitpid(-1, &status, 0);
		i--;
	}
	last_status = 0;
	if (waitpid(data->last_pid, &last_status, 0) == -1)
	{
		if (errno != ECHILD && errno != EINTR)
			perror("waitpid");
	}
	else
		process_last_exit_status(last_status, mini);
}

int	execute_pipeline(t_commands *cmd_list, t_minishell *mini)
{
	t_exec_data	data;

	setup_initial_vars(&data, cmd_list);
	setup_signals_ignore();
	while (data.cmd)
	{
		if (exec_single_command(&data, mini) == 1)
		{
			setup_signals_interactive();
			return (1);
		}
	}
	wait_and_set_exit_status(&data, mini);
	if (data.prev_read_fd != -1)
		close(data.prev_read_fd);
	setup_signals_interactive();
	return (0);
}

int	exec_single_command(t_exec_data *data, t_minishell *mini)
{
	if (create_pipe_if_needed(data->cmd, data->pipe_fd) == -1)
		return (1);
	data->pid = fork();
	if (data->pid < 0)
	{
		perror("fork");
		return (1);
	}
	else if (data->pid == 0)
	{
		child_procces_logic(data->cmd, data->prev_read_fd,
			data->pipe_fd, mini);
	}
	else
	{
		parent_procces_logic(&data->prev_read_fd,
			data->pipe_fd, data->cmd);
		data->last_pid = data->pid;
		data->cmd = data->cmd->next;
		data->i++;
	}
	return (0);
}
