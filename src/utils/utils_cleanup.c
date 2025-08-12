/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_cleanup.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 22:57:42 by sabsanto          #+#    #+#             */
/*   Updated: 2025/08/12 02:11:20 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

void	cleanup_temp_memory(t_minishell *mini)
{
	if (mini && mini->gc_temp)
	{
		gc_free_all(&mini->gc_temp);
		mini->gc_temp = NULL;
	}
}

void	reset_for_next_command(t_minishell *mini)
{
	cleanup_temp_memory(mini);
	mini->commands = NULL;
}

void	emergency_cleanup(t_minishell *mini)
{
	cleanup_temp_memory(mini);
	if (mini)
	{
		mini->commands = NULL;
		if (mini->in_fd != -1)
			dup2(mini->in_fd, STDIN_FILENO);
		if (mini->out_fd != -1)
			dup2(mini->out_fd, STDOUT_FILENO);
	}
}
