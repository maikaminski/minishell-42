/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 14:25:19 by makamins          #+#    #+#             */
/*   Updated: 2025/08/06 21:05:18 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

// Função init_env_list - inicializa lista de ambiente
void	init_env_list(t_minishell *mini, char **envp)
{
	int		i;
	char	*eq_pos;
	char	*key;
	char	*value;

	if (!mini || !envp)
		return ;
	
	mini->env = NULL;
	i = 0;
	
	while (envp[i])
	{
		eq_pos = ft_strchr(envp[i], '=');
		if (eq_pos)
		{
			// Extrai chave
			key = gc_malloc(eq_pos - envp[i] + 1, &mini->gc_persistent);
			if (!key)
				return ;
			ft_strlcpy(key, envp[i], eq_pos - envp[i] + 1);
			
			// Extrai valor
			value = gc_malloc(ft_strlen(eq_pos + 1) + 1, &mini->gc_persistent);
			if (!value)
				return ;
			ft_strlcpy(value, eq_pos + 1, ft_strlen(eq_pos + 1) + 1);
			
			// Adiciona à lista
			set_env_value(&mini->env, key, value, &mini->gc_persistent);
		}
		i++;
	}
	
	// Atualiza SHLVL
	update_shlvl(mini);
}

// Função auxiliar para criar string "key=value"
static char	*make_env_entry(t_env *node, t_garbage **gc)
{
	char	*entry;
	size_t	key_len;
	size_t	val_len;
	size_t	total_len;

	if (!node || !node->key)
		return (NULL);
	
	key_len = ft_strlen(node->key);
	val_len = node->value ? ft_strlen(node->value) : 0;
	total_len = key_len + 1 + val_len + 1; // key + '=' + value + '\0'
	
	entry = gc_malloc(total_len, gc);
	if (!entry)
		return (NULL);
	
	ft_strlcpy(entry, node->key, total_len);
	ft_strlcat(entry, "=", total_len);
	if (node->value)
		ft_strlcat(entry, node->value, total_len);
	
	return (entry);
}

// Função env_list_to_array - converte lista para array de strings
char	**env_list_to_array(t_env *env, t_garbage **gc)
{
	char	**array;
	t_env	*curr;
	int		count;
	int		i;

	// Conta elementos
	count = 0;
	curr = env;
	while (curr)
	{
		if (curr->value)  // Só exporta variáveis com valor
			count++;
		curr = curr->next;
	}
	
	// Aloca array
	array = gc_malloc(sizeof(char *) * (count + 1), gc);
	if (!array)
		return (NULL);
	
	// Preenche array
	i = 0;
	curr = env;
	while (curr)
	{
		if (curr->value)
		{
			array[i] = make_env_entry(curr, gc);
			if (!array[i])
				return (NULL);
			i++;
		}
		curr = curr->next;
	}
	array[i] = NULL;
	
	return (array);
}
