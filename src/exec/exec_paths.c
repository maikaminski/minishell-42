/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_paths.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 15:07:43 by makamins          #+#    #+#             */
/*   Updated: 2025/08/06 01:40:41 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

char	*construct_path(char *dir, char *cmd, t_garbage **gc)
{
	size_t	dir_len;
	size_t	cmd_len;
	size_t	total_len;
	char	*full_path;

	if (!dir || !cmd)
		return (NULL);
	dir_len = ft_strlen(dir);
	cmd_len = ft_strlen(cmd);
	total_len = dir_len + cmd_len + 2;
	full_path = gc_malloc(total_len, gc);
	if (!full_path)
		return (NULL);
	ft_strlcpy(full_path, dir, total_len);
	if (dir_len > 0 && dir[dir_len - 1] != '/')
		ft_strlcat(full_path, "/", total_len);
	ft_strlcat(full_path, cmd, total_len);
	return (full_path);
}

char	*find_cmd_in_paths(char **paths, char *cmd, t_garbage **gc)
{
	char	*full_path;
	int		i;

	if (!paths || !cmd)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full_path = construct_path(paths[i], cmd, gc);
		if (!full_path)
		{
			i++;
			continue ;
		}
		if (access(full_path, F_OK) == 0 && access(full_path, X_OK) == 0)
			return (full_path);
		i++;
	}
	return (NULL);
}

static char	**split_and_register_paths(char *path_value, t_garbage **gc)
{
	char	**paths;
	int		i;

	paths = ft_split(path_value, ':');
	if (!paths)
		return (NULL);
	if (!gc_add_ptr(paths, gc))
	{
		free_array(paths);
		return (NULL);
	}
	i = 0;
	while (paths[i])
	{
		if (!gc_add_ptr(paths[i], gc))
			return (NULL);
		i++;
	}
	return (paths);
}

char	**get_paths(t_env *env, t_garbage **gc)
{
	t_env	*path_node;
	char	**paths;

	if (!env || !gc)
		return (NULL);
	path_node = find_env_key(env, "PATH");
	if (!path_node || !path_node->value || !path_node->value[0])
		return (NULL);
	paths = split_and_register_paths(path_node->value, gc);
	return (paths);
}

static int	is_absolute_or_relative_path(char *cmd)
{
	if (!cmd)
		return (0);
	if (cmd[0] == '/' || (cmd[0] == '.' && (cmd[1] == '/' 
		|| (cmd[1] == '.' && cmd[2] == '/'))))
		return (1);
	if (ft_strchr(cmd, '/'))
		return (1);
	return (0);
}

static char	*handle_direct_path(char *cmd, t_garbage **gc)
{
	char	*cmd_path;

	if (access(cmd, F_OK) != 0)
		return (NULL);
	if (access(cmd, X_OK) != 0)
		return (NULL);
	cmd_path = ft_strdup(cmd);
	if (!cmd_path)
		return (NULL);
	if (!gc_add_ptr(cmd_path, gc))
	{
		free(cmd_path);
		return (NULL);
	}
	return (cmd_path);
}

char	*get_cmd_path(char *cmd, t_env *env, t_garbage **gc)
{
	char	**paths;
	char	*cmd_path;

	if (!cmd || !cmd[0] || !env || !gc)
		return (NULL);
	if (is_absolute_or_relative_path(cmd))
		return (handle_direct_path(cmd, gc));
	paths = get_paths(env, gc);
	if (!paths)
		return (NULL);
	cmd_path = find_cmd_in_paths(paths, cmd, gc);
	return (cmd_path);
}
