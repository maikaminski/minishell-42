/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makamins <makamins@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 10:02:10 by makamins          #+#    #+#             */
/*   Updated: 2025/07/11 18:37:39 by makamins         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/minishell.h"
#include "include/garbage_collector.h"

t_env	*copy_env_node(t_env *src, t_garbage **gc);
void	insert_sorted_env_node(t_env **sorted, t_env *new_node);

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

void	print_sorted_env(t_env *env, t_minishell *mini)
{
	t_env	*sorted_list;
	t_env	*curr;
	t_env	*node_copy;
	
	if (!env)
		return ;
	sorted_list = NULL;
	curr = env;
	while (curr)
	{
		node_copy = copy_env_node(curr, &mini->gc);
		insert_sorted_env_node(&sorted_list, node_copy);
		curr = curr->next;
	}
	curr = sorted_list;
	while (curr)
	{
		printf("declare -x %s", curr->key);
		if (curr->value != NULL)
			printf("=\"%s\"", curr->value);
		printf("\n");
		curr = curr->next;
	}
}

void	export_variable(char *arg, t_minishell *mini)
{
	int	i;

	i = 0;
	if (!arg || arg[0] == '\0')
		return ;
	while (arg[i] != '=')
	{
		
	}
	
}

int	ft_export(char **argv, t_minishell *mini)
{
	
}

