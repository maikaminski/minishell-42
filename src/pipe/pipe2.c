/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makamins <makamins@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 18:30:58 by makamins          #+#    #+#             */
/*   Updated: 2025/08/08 11:00:13 by makamins         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

void	parent_procces_logic(int *prev_read_fd,
	int pipe_fd[2], t_commands *cmd)
{
	if (*prev_read_fd != -1)
		close(*prev_read_fd);
	if (cmd->next)
	{
		close(pipe_fd[1]);
		*prev_read_fd = pipe_fd[0];
	}
}

void	wait_all_children(int n)
{
	int	status;
	int	i;

	i = 0;
	while (i < n)
	{
		wait(&status);
		i++;
	}
}

int	execute_pipeline(t_commands *cmd_list, t_minishell *mini)
{
	t_exec_data	data;
	int			last_status;

	setup_initial_vars(&data, cmd_list);
	while (data.cmd)
	{
		if (exec_single_command(&data, mini) == 1)
			return (1);
	}
	while (data.num_cmds > 1)
	{
		wait(NULL);
		data.num_cmds--;
	}
	last_status = 0;
	if (waitpid(data.last_pid, &last_status, 0) == -1)
		perror("waitpid");
	if (data.prev_read_fd != -1)
		close(data.prev_read_fd);
	if (WIFEXITED(last_status))
		mini->last_exit = WEXITSTATUS(last_status);
	else
		mini->last_exit = 1;
	return (0);
}
