/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_helpers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 00:51:50 by sabsanto          #+#    #+#             */
/*   Updated: 2025/08/06 00:52:15 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

// Verifica se é um builtin
int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strncmp(cmd, "echo", 5) == 0)
		return (1);
	if (ft_strncmp(cmd, "cd", 3) == 0)
		return (1);
	if (ft_strncmp(cmd, "pwd", 4) == 0)
		return (1);
	if (ft_strncmp(cmd, "export", 7) == 0)
		return (1);
	if (ft_strncmp(cmd, "unset", 6) == 0)
		return (1);
	if (ft_strncmp(cmd, "env", 4) == 0)
		return (1);
	if (ft_strncmp(cmd, "exit", 5) == 0)
		return (1);
	return (0);
}

// Verifica se é um builtin que deve rodar no processo pai
int	is_parent_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	// cd, export, unset e exit devem rodar no processo pai
	// pois modificam o ambiente do shell
	if (ft_strncmp(cmd, "cd", 3) == 0)
		return (1);
	if (ft_strncmp(cmd, "export", 7) == 0)
		return (1);
	if (ft_strncmp(cmd, "unset", 6) == 0)
		return (1);
	if (ft_strncmp(cmd, "exit", 5) == 0)
		return (1);
	return (0);
}

// Executa builtin
int	execute_builtin(t_commands *cmd, t_minishell *mini)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (127);
		
	if (ft_strncmp(cmd->argv[0], "echo", 5) == 0)
		return (ft_echo(cmd->argv, mini));
	else if (ft_strncmp(cmd->argv[0], "cd", 3) == 0)
		return (ft_cd(cmd->argv, mini));
	else if (ft_strncmp(cmd->argv[0], "pwd", 4) == 0)
		return (ft_pwd(mini));
	else if (ft_strncmp(cmd->argv[0], "export", 7) == 0)
		return (ft_export(cmd->argv, mini));
	else if (ft_strncmp(cmd->argv[0], "unset", 6) == 0)
		return (ft_unset(cmd->argv, mini));
	else if (ft_strncmp(cmd->argv[0], "env", 4) == 0)
		return (ft_env(mini));
	else if (ft_strncmp(cmd->argv[0], "exit", 5) == 0)
		return (ft_exit(cmd->argv, mini));
	return (127);
}

// Executa comando externo
void	execute_external_cmd(t_commands *cmd, t_minishell *mini)
{
	char	**envp;
	char	*cmd_path;
	
	envp = env_list_to_array(mini->env, &mini->gc);
	if (!envp)
		exit(1);
	
	cmd_path = get_cmd_path(cmd->argv[0], mini->env, &mini->gc);
	if (!cmd_path)
	{
		write(2, cmd->argv[0], ft_strlen(cmd->argv[0]));
		write(2, ": command not found\n", 20);
		exit(127);
	}
	
	execve(cmd_path, cmd->argv, envp);
	perror(cmd->argv[0]);
	exit(127);
}

// Configura redirecionamento de entrada
int	setup_input_redir(char *file)
{
	int	fd;
	
	fd = open(file, O_RDONLY);
	if (fd == -1)
	{
		perror(file);
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

// Configura redirecionamento de saída
int	setup_output_redir(char *file, int append)
{
	int	fd;
	int	flags;
	
	if (append)
		flags = O_CREAT | O_WRONLY | O_APPEND;
	else
		flags = O_CREAT | O_WRONLY | O_TRUNC;
	
	fd = open(file, flags, 0644);
	if (fd == -1)
	{
		perror(file);
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("dup2");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

// Configura todos os redirecionamentos
int	setup_redirections(t_redir *redir_list, t_minishell *mini)
{
	while (redir_list)
	{
		if (redir_list->type == REDIR_IN)
		{
			if (setup_input_redir(redir_list->file) == -1)
				return (-1);
		}
		else if (redir_list->type == REDIR_OUT)
		{
			if (setup_output_redir(redir_list->file, 0) == -1)
				return (-1);
		}
		else if (redir_list->type == REDIR_APPEND)
		{
			if (setup_output_redir(redir_list->file, 1) == -1)
				return (-1);
		}
		else if (redir_list->type == REDIR_HEREDOC)
		{
			if (handle_heredoc_redirection(redir_list, mini) == -1)
				return (-1);
		}
		redir_list = redir_list->next;
	}
	return (0);
}
