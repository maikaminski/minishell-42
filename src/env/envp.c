/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 14:25:19 by makamins          #+#    #+#             */
/*   Updated: 2025/08/06 20:46:22 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

// Extrai a chave de uma string KEY=VALUE
static char	*extract_key(char *env_str, t_garbage **gc)
{
	char	*equals;
	char	*key;
	size_t	key_len;

	equals = ft_strchr(env_str, '=');
	if (!equals)
		return (ft_strdup(env_str)); // Variável sem valor (apenas exportada)
	
	key_len = equals - env_str;
	key = gc_malloc(key_len + 1, gc);
	if (!key)
		return (NULL);
	
	ft_strlcpy(key, env_str, key_len + 1);
	return (key);
}

// Inicializa a lista de ambiente copiando todas as variáveis do sistema
void	init_env_list(t_minishell *mini, char **envp)
{
	int		i;
	char	*equals;
	char	*key;
	char	*value;

	if (!mini)
		return ;
	
	mini->env = NULL;
	
	// Se não há ambiente, cria um mínimo essencial
	if (!envp || !*envp)
	{
		set_env_value(&mini->env, "PATH", "/usr/bin:/bin", &mini->gc_persistent);
		set_env_value(&mini->env, "PWD", getcwd(NULL, 0), &mini->gc_persistent);
		set_env_value(&mini->env, "SHLVL", "1", &mini->gc_persistent);
		return ;
	}
	
	// Copia todas as variáveis do ambiente
	i = 0;
	while (envp[i])
	{
		key = extract_key(envp[i], &mini->gc_persistent);
		if (!key)
		{
			i++;
			continue;
		}
		
		equals = ft_strchr(envp[i], '=');
		if (equals)
			value = equals + 1;
		else
			value = NULL;
		
		set_env_value(&mini->env, key, value, &mini->gc_persistent);
		i++;
	}
	
	// Incrementa SHLVL
	update_shlvl(mini);
}

// Incrementa o nível do shell
void	update_shlvl(t_minishell *mini)
{
	char	*shlvl_str;
	int		shlvl;
	char	new_shlvl[12];
	
	shlvl_str = get_env_value(mini->env, "SHLVL");
	if (!shlvl_str)
		shlvl = 0;
	else
		shlvl = ft_atoi(shlvl_str);
	
	shlvl++;
	if (shlvl < 0)
		shlvl = 0;
	else if (shlvl > 1000)
	{
		write(2, "minishell: warning: shell level (", 34);
		ft_putnbr_fd(shlvl, 2);
		write(2, ") too high, resetting to 1\n", 28);
		shlvl = 1;
	}
	
	ft_itoa_buf(shlvl, new_shlvl);
	set_env_value(&mini->env, "SHLVL", new_shlvl, &mini->gc_persistent);
}

// Conta o número de variáveis de ambiente
static int	count_env_entries(t_env *env)
{
	int		count;
	t_env	*curr;

	count = 0;
	curr = env;
	while (curr)
	{
		if (curr->key && curr->value)
			count++;
		curr = curr->next;
	}
	return (count);
}

// Cria uma entrada KEY=VALUE para o array
static char	*make_env_entry(t_env *env, t_garbage **gc)
{
	size_t	total_len;
	char	*entry;

	if (!env || !env->key || !env->value || !gc)
		return (NULL);
	
	total_len = ft_strlen(env->key) + ft_strlen(env->value) + 2;
	entry = gc_malloc(total_len, gc);
	if (!entry)
		return (NULL);
	
	ft_strlcpy(entry, env->key, total_len);
	ft_strlcat(entry, "=", total_len);
	ft_strlcat(entry, env->value, total_len);
	return (entry);
}

// Converte a lista de ambiente para array (para execve)
char	**env_list_to_array(t_env *env, t_garbage **gc)
{
	char	**array;
	int		i;
	t_env	*curr;
	char	*entry;

	if (!env || !gc)
		return (NULL);
	
	array = gc_malloc(sizeof(char *) * (count_env_entries(env) + 1), gc);
	if (!array)
		return (NULL);
	
	i = 0;
	curr = env;
	while (curr)
	{
		if (curr->key && curr->value)
		{
			entry = make_env_entry(curr, gc);
			if (entry)
				array[i++] = entry;
		}
		curr = curr->next;
	}
	array[i] = NULL;
	return (array);
}
