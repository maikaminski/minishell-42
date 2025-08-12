/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_exit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 19:38:23 by makamins          #+#    #+#             */
/*   Updated: 2025/08/12 02:09:18 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

void	child_exit(t_minishell *mini, int exit_code)
{
	if (mini)
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
	}
	exit(exit_code);
}
