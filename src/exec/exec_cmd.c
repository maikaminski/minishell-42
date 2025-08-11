/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makamins <makamins@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 19:13:48 by makamins          #+#    #+#             */
/*   Updated: 2025/08/11 20:34:07 by makamins         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

int	has_slash(char *cmd)
{
	int	i;

	i = 0;
	while (cmd[i])
	{
		if (cmd[i] == '/')
			return (1);
		i++;
	}
	return (0);
}

static void	print_exec_error(char *cmd)
{
	write(2, cmd, ft_strlen(cmd));
	if (has_slash(cmd))
		write(2, ": No such file or directory\n", 29);
	else
		write(2, ": command not found\n", 20);
}

int	fork_and_exec(char *cmd_path, char **args, char **envp, t_minishell *mini)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
	{
		execve(cmd_path, args, envp);
		perror("execve");
		child_exit(mini, 1);
	}
	if (waitpid(pid, &status, 0) == -1)
	{
		perror("waitpid");
		return (1);
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}

int	exec_cmd(char **args, t_env *env, t_garbage **gc, t_minishell *mini)
{
	char	*cmd_path;
	char	**env_array;

	if (!args || !args[0] || args[0][0] == '\0')
		return (1);
	env_array = env_list_to_array(env, gc);
	if (!env_array)
		return (1);
	cmd_path = get_cmd_path(args[0], env, gc);
	if (!cmd_path)
	{
		print_exec_error(args[0]);
		return (127);
	}
	return (fork_and_exec(cmd_path, args, env_array, mini));
}

char	*find_cmd_in_paths(char **paths, char *cmd, t_garbage **gc)
{
	char	*full_path;
	int		i;

	i = 0;
	while (paths[i])
	{
		full_path = construct_path(paths[i], cmd, gc);
		if (full_path && access(full_path, X_OK) == 0)
			return (full_path);
		i++;
	}
	return (NULL);
}
