/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 18:30:58 by makamins          #+#    #+#             */
/*   Updated: 2025/08/11 18:14:36 by sabsanto         ###   ########.fr       */
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
	int			status;
	int			last_status;
	int			i;

	setup_initial_vars(&data, cmd_list);
	
	// Configura sinais para ignorar durante execução do pipeline
	setup_signals_ignore();
	
	while (data.cmd)
	{
		if (exec_single_command(&data, mini) == 1)
		{
			// Restaura sinais antes de retornar em caso de erro
			setup_signals_interactive();
			return (1);
		}
	}
	
	// Espera pelos processos filhos (exceto o último)
	i = data.num_cmds - 1;
	while (i > 0)
	{
		waitpid(-1, &status, 0);
		i--;
	}
	
	// Espera pelo último processo e captura seu status
	last_status = 0;
	if (waitpid(data.last_pid, &last_status, 0) == -1)
	{
		// Só imprime erro se não for ECHILD ou EINTR
		if (errno != ECHILD && errno != EINTR)
			perror("waitpid");
	}
	else
	{
		// Processa o status do último comando
		if (WIFEXITED(last_status))
			mini->last_exit = WEXITSTATUS(last_status);
		else if (WIFSIGNALED(last_status))
		{
			int sig = WTERMSIG(last_status);
			mini->last_exit = 128 + sig;
		}
		else
			mini->last_exit = 1;
	}
	
	if (data.prev_read_fd != -1)
		close(data.prev_read_fd);
	
	// IMPORTANTE: Restaura sinais interativos aqui
	setup_signals_interactive();
	
	return (0);
}
