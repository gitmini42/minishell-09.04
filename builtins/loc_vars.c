/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loc_vars.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:07:57 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/10 17:30:01 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_var	*g_vars = NULL;

void	store_var(char *name, char *value, t_var **vars)
{
	t_var	*var;
	t_var	*new_var;

	var = *vars;
	while (var)
	{
		if (ft_strcmp(var->name, name) == 0)
		{
			free(var->value);
			var->value = ft_strdup(value);
			return ;
		}
		var = var->next;
	}
	new_var = malloc(sizeof(t_var));
	if (!new_var)
		return ;
	new_var->name = ft_strdup(name);
	new_var->value = ft_strdup(value);
	new_var->exported = 0;
	new_var->next = *vars;
	*vars = new_var;
}

void	free_all_vars(t_var **vars)
{
	t_var	*var;
	t_var	*next;

	var = *vars;
	while (var)
	{
		next = var->next;
		free(var->name);
		free(var->value);
		free(var);
		var = next;
	}
	*vars = NULL;
}

int	is_var_assignment(const char *input)
{
	const char	*equal;
	const char	*ptr;

	equal = ft_strchr(input, '=');
	if (!equal || equal == input)
		return (0);
	ptr = input;
	while (ptr < equal)
	{
		if (isspace(*ptr))
			return (0);
		ptr++;
	}
	return (1);
}

int	is_valid_var_name(const char *name)
{
	if (!name || !*name || (!ft_isalpha(*name) && *name != '_'))
		return (0);
	name++;
	while (*name)
	{
		if (!ft_isalnum(*name) && *name != '_')
			return (0);
		name++;
	}
	return (1);
}

void	handle_var_assignment(const char *input, t_var **vars)
{
	char	*name;
	char	*equal;
	char	*value;

	name = ft_strdup(input);
	equal = ft_strchr(name, '=');
	if (!equal)
	{
		free(name);
		return ;
	}
	*equal = '\0';
	value = equal + 1;
	while (*value && ft_isspace(*value))
		value++;
	if (is_valid_var_name(name))
		store_var(name, value, vars);
	else
	{
		ft_putstr_fd("Command not found: ", 2);
		ft_putstr_fd(name, 2);
		ft_putstr_fd("\n", 2);
		g_exit_status = 127;
	}
	free(name);
}
