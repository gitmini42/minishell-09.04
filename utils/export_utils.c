/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 10:59:01 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/15 11:13:42 by scarlos-         ###   ########.fr       */
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

void	fill_updated_envp(char **envp, char **new_envp,
				char *new_entry, int replace_index)
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
	int		new_size;
	char	**new_envp;

	count = 0;
	while (envp && envp[count])
		count++;
	if (replace_index == -1)
		new_size = count + 2;
	else
		new_size = count + 1;
	new_envp = malloc(sizeof(char *) * new_size);
	if (!new_envp)
		return (NULL);
	fill_updated_envp(envp, new_envp, new_entry, replace_index);
	free(envp);
	return (new_envp);
}
