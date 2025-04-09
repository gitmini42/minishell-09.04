/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:08:13 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/09 16:08:14 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../minishell.h"

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

static void	remove_from_env(char *name, char ***envp)
{
	int	i;
	int	j;
	int	len;

	if (!*envp)
		return ;
	len = 0;
	while ((*envp)[len])
		len++;
	i = 0;
	while ((*envp)[i])
	{
		if (ft_strncmp((*envp)[i], name, ft_strlen(name)) == 0
			&& (*envp)[i][ft_strlen(name)] == '=')
		{
			free((*envp)[i]);
			j = i;
			while (j < len - 1)
			{
				(*envp)[j] = (*envp)[j + 1];
				j++;
			}
			(*envp)[len - 1] = NULL;
			return ;
		}
		i++;
	}
}

int	ft_unset(char **args, t_var **vars, char ***envp)
{
	int	i;
	int	status;

	status = 0;
	if (!args[1])
	{
		fprintf(stderr, "minishell: unset: not enough arguments\n");//preciso tirar fprintf
		return (1);
	}
	i = 1;
	while (args[i])
	{
		if (!is_valid_var_name(args[i]))
		{
			fprintf(stderr, "minishell: unset: `%s': not a valid identifier\n", args[i]);//preciso tirar fprintf
			status = 1;
			i++;
			continue ;
		}
		remove_from_vars(args[i], vars);
		remove_from_env(args[i], envp);
		i++;
	}
	return (status);
}
