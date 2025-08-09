/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_paths.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 15:07:43 by makamins          #+#    #+#             */
/*   Updated: 2025/08/09 04:28:19 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

char	*construct_path(char *dir, char *cmd, t_garbage **gc)
{
	size_t	path_len;
	char	*full_path;

	if (!dir || !cmd)
		return (NULL);
	path_len = ft_strlen(dir) + ft_strlen(cmd) + 2;
	full_path = gc_malloc(path_len, gc);
	if (!full_path)
		return (NULL);
	ft_strlcpy(full_path, dir, path_len);
	ft_strlcat(full_path, "/", path_len);
	ft_strlcat(full_path, cmd, path_len);
	return (full_path);
}

static int	process_path_entries(char **paths, t_garbage **gc)
{
	int	i;

	i = 0;
	while (paths[i])
	{
		if (paths[i][0] == '\0')
		{
			free(paths[i]);
			paths[i] = ft_strdup(".");
			if (!paths[i])
				return (0);
		}
		if (!gc_add_ptr(paths[i], gc))
			return (0);
		i++;
	}
	return (1);
}

static char	**split_and_register_paths(char *path_value, t_garbage **gc)
{
	char	**paths;

	paths = ft_split(path_value, ':');
	if (!paths)
		return (NULL);
	if (!gc_add_ptr(paths, gc))
	{
		free_array(paths);
		return (NULL);
	}
	if (!process_path_entries(paths, gc))
		return (NULL);
	return (paths);
}

char	**get_paths(t_env *env, t_garbage **gc)
{
	t_env	*path_node;

	if (!env || !gc)
		return (NULL);
	path_node = find_env_key(env, "PATH");
	if (!path_node || !path_node->value)
		return (NULL);
	return (split_and_register_paths(path_node->value, gc));
}

char	*get_cmd_path(char *cmd, t_env *env, t_garbage **gc)
{
	char	**paths;
	char	*cmd_path;

	if (!cmd || !cmd[0])
		return (NULL);
	if (has_slash(cmd))
	{
		if (access(cmd, X_OK) == 0)
		{
			cmd_path = ft_strdup(cmd);
			if (!cmd_path || !gc_add_ptr(cmd_path, gc))
				return (NULL);
			return (cmd_path);
		}
		return (NULL);
	}
	paths = get_paths(env, gc);
	if (!paths)
		return (NULL);
	cmd_path = find_cmd_in_paths(paths, cmd, gc);
	if (cmd_path && !gc_add_ptr(cmd_path, gc))
		return (NULL);
	return (cmd_path);
}
