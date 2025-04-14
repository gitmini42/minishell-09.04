/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:07:46 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/14 12:03:26 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	print_exported_env(char **envp)
{
	int		i;
	char	*equal;

	i = 0;
	while (envp[i])
	{
		equal = ft_strchr(envp[i], '=');
		if (equal)
		{
			*equal = '\0';
			printf("declare -x %s=\"%s\"\n", envp[i], equal + 1);
			*equal = '=';
		}
		i++;
	}
	return (0);
}

int	find_env_var_index(char **envp, const char *name)
{
	int	i;
	int	len;

	if (!envp || !name)
		return (-1);
	len = ft_strlen(name);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], name, len) == 0 && envp[i][len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

void	fill_updated_envp(char **envp, char **new_envp, char *new_entry, int replace_index)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (envp[i])
	{
		if (i == replace_index)
			new_envp[j++] = ft_strdup(new_entry);
		else
			new_envp[j++] = ft_strdup(envp[i]);
		free(envp[i]);
		i++;
	}
	if (replace_index == -1)
		new_envp[j++] = ft_strdup(new_entry);
	new_envp[j] = NULL;
}

char	**copy_envp_with_update(char **envp, char *new_entry, int replace_index)
{
	int		count;
	char	**new_envp;

	count = 0;
	while (envp && envp[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + (replace_index == -1 ? 2 : 1)));
	if (!new_envp)
		return (NULL);
	fill_updated_envp(envp, new_envp, new_entry, replace_index);
	free(envp);
	return (new_envp);
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
int	handle_invalid_identifier(char *name)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(name, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	return (1);
}
int	handle_export_with_value(char *arg, t_var **vars, char ***envp)
{
	char	*equal = ft_strchr(arg, '=');
	char	*name;
	char	*value;
	t_var	*var;

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
		return handle_invalid_identifier(arg);
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
	int	status;
	int	i;
	char *equal;

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
