/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: makamins <makamins@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 18:18:14 by makamins          #+#    #+#             */
/*   Updated: 2025/08/11 18:19:06 by makamins         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

void	process_operator_token(char *input, int *i,
		t_token **tokens, t_garbage **gc)
{
	t_tokens	type;
	t_token		*new_token;

	type = get_operator_type(input, *i);
	if ((input[*i] == '>' || input[*i] == '<') && input[*i + 1] == input[*i])
		*i += 2;
	else
		(*i)++;
	new_token = create_token(NULL, type, gc);
	if (new_token)
		add_token_to_list(tokens, new_token);
}
