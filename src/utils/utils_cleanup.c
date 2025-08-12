/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_cleanup.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 22:57:42 by sabsanto          #+#    #+#             */
/*   Updated: 2025/08/11 22:57:54 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

// Função para limpar completamente a memória temporária
void	cleanup_temp_memory(t_minishell *mini)
{
	if (mini && mini->gc_temp)
	{
		gc_free_all(&mini->gc_temp);
		mini->gc_temp = NULL;
	}
}

// Função para reinicializar estado para próximo comando
void	reset_for_next_command(t_minishell *mini)
{
	cleanup_temp_memory(mini);
	mini->commands = NULL;
}

// Função para limpeza de emergência em caso de erro
void	emergency_cleanup(t_minishell *mini)
{
	cleanup_temp_memory(mini);
	if (mini)
	{
		mini->commands = NULL;
		// Restaurar file descriptors se necessário
		if (mini->in_fd != -1)
			dup2(mini->in_fd, STDIN_FILENO);
		if (mini->out_fd != -1)
			dup2(mini->out_fd, STDOUT_FILENO);
	}
}
