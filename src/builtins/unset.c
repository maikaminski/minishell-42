/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makamins <makamins@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 16:00:55 by makamins          #+#    #+#             */
/*   Updated: 2025/08/12 14:48:12 by makamins         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

bool	is_valid_identifier(const char *key)
{
	int	i;

	if (!key || key[0] == '\0')
		return (false);
	if (ft_strchr(key, '=') || ft_strchr(key, '+'))
		return (false);
	i = 0;
	while (key[i])
	{
		if (key[i] == ' ' || key[i] == '\t')
			return (false);
		i++;
	}
	return (true);
}

void	remove_env_node(t_env **env, const char *key)
{
	t_env	*curr;
	t_env	*prev;

	if (!env || !*env)
		return ;
	curr = *env;
	prev = NULL;
	while (curr)
	{
		if (ft_strncmp(curr->key, key, ft_strlen(key) + 1) == 0)
		{
			if (!prev)
				*env = curr->next;
			else
				prev->next = curr->next;
			return ;
		}
		prev = curr;
		curr = curr->next;
	}
}

void	print_unset_invalid_identifier(const char *arg)
{
	if (!arg)
		return ;
	write(STDERR_FILENO, "unset: `", 8);
	write(STDERR_FILENO, arg, ft_strlen(arg));
	write(STDERR_FILENO, "': not a valid identifier\n", 27);
}

int	ft_unset(char **argv, t_minishell *mini)
{
	int	i;
	int	status;

	if (!argv || !mini)
		return (1);
	if (!argv[1])
	{
		write(2, "unset: not enough arguments\n", 28);
		return (1);
	}
	i = 1;
	status = 0;
	while (argv[i])
	{
		if (!is_valid_identifier(argv[i]))
		{
			print_unset_invalid_identifier(argv[i]);
			status = 1;
		}
		else
			remove_env_node(&mini->env, argv[i]);
		i++;
	}
	return (status);
}
