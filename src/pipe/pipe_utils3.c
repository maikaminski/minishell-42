/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 02:42:10 by sabsanto          #+#    #+#             */
/*   Updated: 2025/08/12 02:55:19 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

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
