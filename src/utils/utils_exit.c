/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_exit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makamins <makamins@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 19:38:23 by makamins          #+#    #+#             */
/*   Updated: 2025/08/11 20:17:17 by makamins         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

void	child_exit(t_minishell *mini, int exit_code)
{
	if (mini && mini->gc_temp)
	{
		gc_free_all(&mini->gc_temp);
		mini->gc_temp = NULL;
	}
	exit(exit_code);
}
