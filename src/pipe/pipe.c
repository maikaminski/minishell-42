/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makamins <makamins@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:46:32 by makamins          #+#    #+#             */
/*   Updated: 2025/08/11 20:13:51 by makamins         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

static void	setup_pipes_and_redirections(t_commands *cmd,
	int prev_read_fd, int pipe_fd[2], t_minishell *mini)
{
	if (prev_read_fd != -1)
	{
		dup2(prev_read_fd, STDIN_FILENO);
		close(prev_read_fd);
	}
	if (cmd->next)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
	}
	if (handle_redirections(cmd->redir, mini) == -1)
		child_exit(mini, 1);
}

bool	is_str_empty_or_whitespace(const char *str)
{
	int	i;

	i = 0;
	if (!str)
		return (true);
	while (str[i])
	{
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
			return (false);
		i++;
	}
	return (true);
}

void	expand_cmd_args(t_commands *cmd, t_minishell *mini)
{
	int		i;
	char	*expanded;

	i = 0;
	while (cmd->argv[i])
	{
		expanded = expand_variables(cmd->argv[i], mini);
		if (!expanded)
			expanded = ft_strdup("");
		else
			gc_add_ptr(expanded, &mini->gc_temp);
		cmd->argv[i] = expanded;
		i++;
	}
}

static void	execute_external_cmd(t_commands *cmd, t_minishell *mini)
{
	char	**envp;
	char	*cmd_path;

	envp = env_list_to_array(mini->env, &mini->gc_temp);
	if (!envp)
		child_exit(mini, 1);
	cmd_path = get_cmd_path(cmd->argv[0], mini->env, &mini->gc_temp);
	if (!cmd_path)
	{
		if (cmd->argv[0] && !is_str_empty_or_whitespace(cmd->argv[0]))
		{
			write(2, cmd->argv[0], ft_strlen(cmd->argv[0]));
			if (has_slash(cmd->argv[0]))
				write(2, ": No such file or directory\n", 29);
			else
				write(2, ": command not found\n", 20);
		}
		child_exit(mini, 127);
	}
	execve(cmd_path, cmd->argv, envp);
	perror(cmd->argv[0]);
	child_exit(mini, 127);
}

void	child_procces_logic(t_commands *cmd,
	int prev_read_fd, int pipe_fd[2], t_minishell *mini)
{
	int	status;

	mini->gc_temp = NULL;
	expand_cmd_args(cmd, mini);
	setup_pipes_and_redirections(cmd, prev_read_fd, pipe_fd, mini);
	if (!cmd->argv[0] || is_str_empty_or_whitespace(cmd->argv[0]))
		child_exit(mini, 0);
	if (is_builtin_cmd(cmd->argv[0]))
	{
		status = execute_builtin(cmd, mini);
		child_exit(mini, status);
	}
	execute_external_cmd(cmd, mini);
}
