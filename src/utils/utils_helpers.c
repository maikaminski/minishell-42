/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_helpers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 20:44:00 by sabsanto          #+#    #+#             */
/*   Updated: 2025/08/06 21:11:38 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Implementação inline de ft_itoa_buf para uso interno
void	ft_itoa_buf(int n, char *buf)
{
	long	num;
	int		i;
	int		is_neg;

	num = n;
	i = 0;
	is_neg = 0;
	
	if (num < 0)
	{
		is_neg = 1;
		num = -num;
	}
	
	// Converte dígitos (ordem reversa)
	if (num == 0)
		buf[i++] = '0';
	else
	{
		while (num > 0)
		{
			buf[i++] = (num % 10) + '0';
			num /= 10;
		}
	}
	
	if (is_neg)
		buf[i++] = '-';
	
	buf[i] = '\0';
	
	// Inverte a string
	int j = 0;
	i--;
	while (j < i)
	{
		char tmp = buf[j];
		buf[j] = buf[i];
		buf[i] = tmp;
		j++;
		i--;
	}
}

// Implementação inline de ft_putnbr_fd para uso interno
void	ft_putnbr_fd(int n, int fd)
{
	char	buf[12];
	
	ft_itoa_buf(n, buf);
	write(fd, buf, ft_strlen(buf));
}

// Join de duas strings com opção de liberar memória
char	*ft_strjoin_and_free(char *s1, char *s2, int free_s)
{
	char	*result;
	size_t	len1;
	size_t	len2;

	if (!s1 || !s2)
		return (NULL);
	
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	
	result = malloc(len1 + len2 + 1);
	if (!result)
		return (NULL);
	
	ft_strlcpy(result, s1, len1 + 1);
	ft_strlcat(result, s2, len1 + len2 + 1);
	
	if (free_s & 1)
		free(s1);
	if (free_s & 2)
		free(s2);
	
	return (result);
}

/* Retorna o maior tamanho entre duas strings
size_t	max_strlen(const char *s1, const char *s2)
{
	size_t	len1;
	size_t	len2;

	len1 = s1 ? ft_strlen(s1) : 0;
	len2 = s2 ? ft_strlen(s2) : 0;
	
	if (len1 > len2)
		return (len1);
	return (len2);
}*/

// Atualiza o valor SHLVL
void	update_shlvl(t_minishell *mini)
{
	char	*shlvl_str;
	int		shlvl;
	char	new_shlvl[12];

	shlvl_str = get_env_value(mini->env, "SHLVL");
	if (!shlvl_str)
		shlvl = 0;
	else
		shlvl = ft_atoi(shlvl_str);
	
	shlvl++;
	if (shlvl < 0)
		shlvl = 0;
	if (shlvl > 999)
		shlvl = 1;
	
	ft_itoa_buf(shlvl, new_shlvl);
	set_env_value(&mini->env, "SHLVL", new_shlvl, &mini->gc_persistent);
}
