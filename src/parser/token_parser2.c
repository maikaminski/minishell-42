/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_parser2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 14:03:26 by makamins          #+#    #+#             */
/*   Updated: 2025/08/11 22:54:18 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

int	add_arg_to_command(t_commands *cmd, char *arg, t_garbage **gc)
{
	char	**new_argv;
	int		count;
	int		i;

	if (!cmd || !arg || !gc)
		return (1);
	count = 0;
	while (cmd->argv && cmd->argv[count])
		count++;
	new_argv = gc_malloc(sizeof(char *) * (count + 2), gc);
	if (!new_argv)
		return (1);
	i = 0;
	while (i < count)
	{
		new_argv[i] = cmd->argv[i];
		i++;
	}
	new_argv[count] = arg;
	new_argv[count + 1] = NULL;
	cmd->argv = new_argv;
	return (0);
}
