/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makamins <makamins@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 10:02:10 by makamins          #+#    #+#             */
/*   Updated: 2025/07/11 13:28:37 by makamins         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/minishell.h"
#include "include/garbage_collector.h"

bool	is_valid_id_export(const char *key)
{
	int	i;

	i = 0;
	if (!key || key[0] == '\0')
		return (false);
	while (key[i] && key[i] != '=')
	{
		if (i == 0)
		{
			if (!(ft_isalpha(key[0]) || key[0] == '_'))
				return (false);
		}
		else
		{
			if (!(ft_isalnum(key[i]) || key[i] != '_'))
				return (false);	
		}
		i++;
	}
	if (i == 0)
		return (false);
	return (true);
}

void	print_sorted_env(t_env *env)
{
	t_env	*sorted_list;
	t_env	*curr;
	t_env	*node_copy;
	
	sorted_list = NULL;
	if (!env)
		return ;
}

void	export_variable(char *arg, t_minishell *mini)
{
	
}

int	ft_export(char **argv, t_minishell *mini)
{
	
}

