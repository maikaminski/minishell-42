/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_main.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 18:49:25 by sabsanto          #+#    #+#             */
/*   Updated: 2025/08/06 00:54:42 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"
#include <signal.h>

// Variável global para sinais (única permitida)
volatile sig_atomic_t	g_signal = 0;

// Handler para sinais no modo interativo
static void	handle_sigint(int sig)
{
	(void)sig;
	g_signal = SIGINT;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

// Configura os sinais para o modo interativo
static void	setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	// Configura SIGINT (Ctrl+C)
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_handler = handle_sigint;
	sa_int.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa_int, NULL);
	
	// Configura SIGQUIT (Ctrl+\) - deve ser ignorado
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_handler = SIG_IGN;
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

// Inicializa a estrutura minishell
static void	init_minishell(t_minishell *mini, char **envp)
{
	mini->commands = NULL;
	mini->env = NULL;
	mini->last_exit = 0;
	mini->in_fd = dup(STDIN_FILENO);
	mini->out_fd = dup(STDOUT_FILENO);
	mini->gc = NULL;
	
	// Inicializa a lista de variáveis de ambiente
	init_env_list(mini, envp);
}

// Limpa recursos antes de sair
static void	cleanup_minishell(t_minishell *mini)
{
	// Fecha FDs salvos
	if (mini->in_fd >= 0)
		close(mini->in_fd);
	if (mini->out_fd >= 0)
		close(mini->out_fd);
	
	// Libera toda memória alocada
	gc_free_all(&mini->gc);
	
	// Limpa histórico do readline
	rl_clear_history();
}

// Função principal
int	main(int argc, char **argv, char **envp)
{
	t_minishell	mini;
	char		*input;
	
	(void)argc;
	(void)argv;
	
	// Inicializa estruturas e sinais
	init_minishell(&mini, envp);
	setup_signals();
	
	// Loop principal do shell
	while (1)
	{
		// Reseta flag de sinal
		g_signal = 0;
		
		// Lê input do usuário
		input = readline("minishell> ");
		
		// Ctrl+D (EOF)
		if (!input)
		{
			write(1, "exit\n", 5);
			break ;
		}
		
		// Ignora linhas vazias
		if (*input)
		{
			// Adiciona ao histórico
			add_history(input);
			
			// Processa e executa o comando
			process_input(input, &mini);
		}
		
		// Libera o input
		free(input);
	}
	
	// Limpa recursos e sai
	cleanup_minishell(&mini);
	return (mini.last_exit);
}
