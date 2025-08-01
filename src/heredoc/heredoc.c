/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makamins <makamins@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 15:47:21 by makamins          #+#    #+#             */
/*   Updated: 2025/08/01 17:32:45 by makamins         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

char	*read_line_stdin(t_garbage **gc)
{
	char	buff[1];
	char	*line;
	char	*temp;
	int		i;
	int		len;

	len = 0;
	line = gc_malloc(1, gc);
	if (!line)
		return (NULL);
	line[0] = '\0';
	while ((i = read(STDIN_FILENO, buff, 1)) > 0)
	{
		if (buff[0] == '\n')
			break ;
		temp = gc_malloc(len + 2, gc);
		if (!temp)
			return (NULL);
	}
}
