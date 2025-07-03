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

char *ft_strjoin_and_free(char *s1, char *s2, int free_s)
{
	char	*str;

	if (!s1 || !s2)
		return (NULL);
	str = ft_strjoin(s1, s2);
	if (free_s & 1)
		free(s1);
	if (free_s & 2)
		free(s2);
	return (str);
}