/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makamins <makamins@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 14:34:21 by sabsanto          #+#    #+#             */
/*   Updated: 2025/08/11 18:19:02 by makamins         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

static int	is_forbidden_char(char c1, char c2)
{
	if (c1 == '&' && c2 == '&')
		return (1);
	if (c1 == '|' && c2 == '|')
		return (1);
	if (c1 == ';')
		return (1);
	if (c1 == '\\')
		return (1);
	return (0);
}

int	validate_input_syntax(char *input)
{
	int	i;

	i = 0;
	while (input[i])
	{
		if (is_forbidden_char(input[i], input[i + 1]))
		{
			write(2, "minishell: syntax error near unexpected token\n", 47);
			return (1);
		}
		i++;
	}
	return (0);
}

t_tokens	get_operator_type(char *input, int pos)
{
	if (input[pos] == '|')
		return (T_PIPE);
	if (input[pos] == '<')
	{
		if (input[pos + 1] == '<')
			return (T_REDIR_HEREDOC);
		return (T_REDIR_IN);
	}
	if (input[pos] == '>')
	{
		if (input[pos + 1] == '>')
			return (T_REDIR_APPEND);
		return (T_REDIR_OUT);
	}
	return (T_WORD);
}

t_token	*create_token(char *value, t_tokens type, t_garbage **gc)
{
	t_token	*token;

	token = gc_malloc(sizeof(t_token), gc);
	if (!token)
		return (NULL);
	token->value = value;
	token->type = type;
	token->next = NULL;
	return (token);
}

void	add_token_to_list(t_token **list, t_token *new_token)
{
	t_token	*last;

	if (!list || !new_token)
		return ;
	if (!*list)
	{
		*list = new_token;
		return ;
	}
	last = *list;
	while (last->next)
		last = last->next;
	last->next = new_token;
}
