/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:07:46 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/15 11:15:33 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	handle_invalid_identifier(char *name)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(name, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	return (1);
}

static void	update_env_var(char *name, char *value, char ***envp)
{
	char	*temp;
	char	*new_entry;
	int		index;

	temp = ft_strjoin(name, "=");
	new_entry = ft_strjoin(temp, value);
	free(temp);
	if (!*envp)
	{
		*envp = malloc(sizeof(char *) * 2);
		if (!*envp)
			return (free(new_entry));
		(*envp)[0] = ft_strdup(new_entry);
		(*envp)[1] = NULL;
		return (free(new_entry));
	}
	index = find_env_var_index(*envp, name);
	*envp = copy_envp_with_update(*envp, new_entry, index);
	free(new_entry);
}

int	handle_export_with_value(char *arg, t_var **vars, char ***envp)
{
	char	*equal;
	char	*name;
	char	*value;
	t_var	*var;

	equal = ft_strchr(arg, '=');
	if (!equal)
		return (1);
	*equal = '\0';
	name = arg;
	value = equal + 1;
	if (!is_valid_var_name(name))
		return (handle_invalid_identifier(name));
	store_var(name, value, vars);
	var = *vars;
	while (var && ft_strcmp(var->name, name) != 0)
		var = var->next;
	if (var)
	{
		var->exported = 1;
		update_env_var(name, value, envp);
	}
	return (0);
}

int	handle_export_without_value(char *arg, t_var **vars, char ***envp)
{
	t_var	*var;

	if (!is_valid_var_name(arg))
		return (handle_invalid_identifier(arg));
	var = *vars;
	while (var && ft_strcmp(var->name, arg) != 0)
		var = var->next;
	if (var)
	{
		var->exported = 1;
		update_env_var(var->name, var->value, envp);
	}
	else
	{
		ft_putstr_fd("minishell: export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putstr_fd("': not found\n", 2);
		return (1);
	}
	return (0);
}

int	ft_export(char **args, t_var **vars, char ***envp)
{
	int		i;
	int		status;
	char	*equal;

	status = 0;
	if (!args[1])
		return (print_exported_env(*envp));
	i = 1;
	while (args[i])
	{
		equal = ft_strchr(args[i], '=');
		if (equal)
			status |= handle_export_with_value(args[i], vars, envp);
		else
			status |= handle_export_without_value(args[i], vars, envp);
		i++;
	}
	return (status);
}
