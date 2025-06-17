#include "../include/minishell.h"

char	is_space(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r'
		|| c == '\f' || c == '\v');
}

char	is_operator(char c)
{
	return (c == '>' || c == '<' || c == '|');
}