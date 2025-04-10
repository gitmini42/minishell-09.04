/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:08:13 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/10 17:09:13 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	remove_from_vars(char *name, t_var **vars)
{
	t_var	*current;
	t_var	*prev;

	current = *vars;
	prev = NULL;
	while (current)
	{
		if (ft_strcmp(current->name, name) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				*vars = current->next;
			free(current->name);
			free(current->value);
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

static void	shift_envp(char ***envp, int start_idx, int len)
{
	int	i;

	i = start_idx;
	while (i < len - 1)
	{
		(*envp)[i] = (*envp)[i + 1];
		i++;
	}
	(*envp)[len - 1] = NULL;
}

static void	remove_from_env(char *name, char ***envp)
{
	int	i;
	int	len;

	i = 0;
	if (!*envp)
		return ;
	len = 0;
	while ((*envp)[len])
		len++;
	while ((*envp)[i])
	{
		if (ft_strncmp((*envp)[i], name, ft_strlen(name)) == 0
			&& (*envp)[i][ft_strlen(name)] == '=')
		{
			free((*envp)[i]);
			shift_envp(envp, i, len);
			return ;
		}
		i++;
	}
}

// static void print_error(const char *msg, const char *arg)
// {
// 	write(2, msg, strlen(msg));
// 	write(2, arg, strlen(arg));
// 	write(2, "\n", 1);
// }

static bool	is_invalid_arg(char *arg)
{
	if (!is_valid_var_name(arg))
	{
		//print_error("minishell: unset: ", arg);
		return (1);
	}
	return (0);
}

int	ft_unset(char **args, t_var **vars, char ***envp)
{
	int	i;
	int	status;

	// if (!args[1])
	// {
	// 	print_error("minishell: unset: not enough arguments", "");
	// 	return 1;
	// }
	i = 1;
	status = 0;
	while (args[i])
	{
		if (is_invalid_arg(args[i]))
			status = 1;
		else
		{
			remove_from_vars(args[i], vars);
			remove_from_env(args[i], envp);
		}
		i++;
	}
	return (status);
}
