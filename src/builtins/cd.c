/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sabsanto <sabsanto@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 14:39:05 by makamins          #+#    #+#             */
/*   Updated: 2025/08/06 21:48:05 by sabsanto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "garbage_collector.h"

// Retorna o path alvo do cd: argv[1] se existir, senão busca a HOME
static char *get_target_path(char **argv, t_minishell *mini)
{
    char *home;

    if (argv[1])
        return (argv[1]);
    home = get_env_value(mini->env, "HOME");
    if (!home)
    {
        write(2, "cd: HOME not set\n", 18);
        return (NULL);
    }
    return (home);
}

// CORRIGIDO: Removido parâmetro 'mini' não utilizado
static char *change_dir(const char *path)
{
    char *cwd;

    if (chdir(path) != 0)
    {
        write(2, "cd: ", 4);
        perror(path);
        return (NULL);
    }
    cwd = getcwd(NULL, 0);
    if (!cwd)
    {
        perror("cd");
        return (NULL);
    }
    // NÃO adiciona ao GC aqui - deixa para ft_cd fazer
    return (cwd);
}

static void update_pwd_vars(t_minishell *mini,
    const char *oldpwd, const char *newpwd)
{
    set_env_value(&mini->env, "OLDPWD", oldpwd, &mini->gc_persistent);
    set_env_value(&mini->env, "PWD", newpwd, &mini->gc_persistent);
}

// CORRIGIDO: 'int' ao invés de 'nt'
int ft_cd(char **argv, t_minishell *mini)
{
    char *oldpwd;
    char *target;
    char *newpwd;

    oldpwd = getcwd(NULL, 0);
    if (!oldpwd)
    {
        perror("cd");
        return (1);
    }
    gc_add_ptr(oldpwd, &mini->gc_temp);
    
    target = get_target_path(argv, mini);
    if (!target)
        return (1);
        
    newpwd = change_dir(target);  // CORRIGIDO: Removido parâmetro 'mini'
    if (!newpwd)
        return (1);
    
    gc_add_ptr(newpwd, &mini->gc_temp); // Única adição
    update_pwd_vars(mini, oldpwd, newpwd);
    return (0);
}
