/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 18:49:25 by sabsanto          #+#    #+#             */
/*   Updated: 2025/08/06 01:29:20 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

volatile sig_atomic_t	g_signal = 0;

static void	handle_sigint(int sig)
{
	(void)sig;
	g_signal = SIGINT;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

static void	setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_handler = handle_sigint;
	sa_int.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa_int, NULL);
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_handler = SIG_IGN;
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

static void	init_minishell(t_minishell *mini, char **envp)
{
	mini->commands = NULL;
	mini->env = NULL;
	mini->last_exit = 0;
	mini->in_fd = dup(STDIN_FILENO);
	mini->out_fd = dup(STDOUT_FILENO);
	mini->gc = NULL;
	init_env_list(mini, envp);
}

static void	cleanup_minishell(t_minishell *mini)
{
	if (mini->in_fd >= 0)
		close(mini->in_fd);
	if (mini->out_fd >= 0)
		close(mini->out_fd);
	gc_free_all(&mini->gc);
	rl_clear_history();
}

int	main(int argc, char **argv, char **envp)
{
	t_minishell	mini;
	char		*input;

	(void)argc;
	(void)argv;
	init_minishell(&mini, envp);
	setup_signals();
	while (1)
	{
		g_signal = 0;
		input = readline("minishell> ");
		if (!input)
		{
			write(1, "exit\n", 5);
			break ;
		}
		if (*input)
		{
			add_history(input);
			process_input(input, &mini);
		}
		free(input);
	}
	cleanup_minishell(&mini);
	return (mini.last_exit);
}
