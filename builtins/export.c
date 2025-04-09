/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:07:46 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/09 16:07:48 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_exported_env(char **envp)
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
}

static void	update_env_var(char *name, char *value, char ***envp)
{
	int		i;
	char	*new_entry;

	i = 0;
	new_entry = malloc(ft_strlen(name) + ft_strlen(value) + 2);
	sprintf(new_entry, "%s=%s", name, value); // trocar sprintf
	if (!*envp)
	{
		*envp = malloc(2 * sizeof(char *));
		(*envp)[0] = new_entry;
		(*envp)[1] = NULL;
		return ;
	}
	while ((*envp)[i])
	{
		if (ft_strncmp((*envp)[i], name, ft_strlen(name)) == 0
			&& (*envp)[i][ft_strlen(name)] == '=')
		{
			free((*envp)[i]);
			(*envp)[i] = new_entry;
			return ;
		}
		i++;
	}
	*envp = realloc(*envp, (i + 2) * sizeof(char *)); // trocar realloc
	(*envp)[i] = new_entry;
	(*envp)[i + 1] = NULL;
}

int	ft_export(char **args, t_var **vars, char ***envp)
{
	int		status;
	int		i;
	t_var	*var;
	char	*equal;
	char	*name;
	char	*value;
	int		found;

	status = 0;
	if (!args[1])
	{
		print_exported_env(*envp);
		var = *vars;
		while (var)
		{
			if (var->exported)
			{
				found = 0;
				i = 0;
				while ((*envp)[i])
				{
					if (ft_strncmp((*envp)[i], var->name, ft_strlen(var->name)) == 0
						&& (*envp)[i][ft_strlen(var->name)] == '=')
					{
						found = 1;
						break ;
					}
					i++;
				}
				if (!found)
					printf("export %s=\"%s\"\n", var->name, var->value);
			}
			var = var->next;
		}
		return (0);
	}
	i = 1;
	while (args[i])
	{
		equal = ft_strchr(args[i], '=');
		if (equal)
		{
			*equal = '\0';
			name = args[i];
			value = equal + 1;
			if (!is_valid_var_name(name))
			{
				ft_putstr_fd("minishell: export: `", 2);
				ft_putstr_fd(name, 2);
				ft_putstr_fd("': not a valid identifier\n", 2);
				status = 1;
				i++;
				continue ;
			}
			store_var(name, value, vars);
			var = *vars;
			while (var && ft_strcmp(var->name, name) != 0)
				var = var->next;
			if (var)
			{
				var->exported = 1;
				update_env_var(name, value, envp);
			}
		}
		else
		{
			if (!is_valid_var_name(args[i]))
			{
				ft_putstr_fd("minishell: export: `", 2);
				ft_putstr_fd(args[i], 2);
				ft_putstr_fd("': not a valid identifier\n", 2);
				status = 1;
				i++;
				continue ;
			}
			var = *vars;
			while (var && ft_strcmp(var->name, args[i]) != 0)
				var = var->next;
			if (var)
			{
				var->exported = 1;
				update_env_var(var->name, var->value, envp);
			}
			else
			{
				ft_putstr_fd("minishell: export: `", 2);
				ft_putstr_fd(args[i], 2);
				ft_putstr_fd("': not found\n", 2);
				status = 1;
			}
		}
		i++;
	}
	return (status);
}
