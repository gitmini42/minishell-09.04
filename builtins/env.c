/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:07:07 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/09 20:04:12 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h" // Adjust to your project's header file

char	**ft_sort_env(char **newenv, int env_count)
{
	char	*temp;
	int		i;
	int		j;

	i = 0;
	while (i < env_count - 1)
	{
		j = i + 1;
		while (j < env_count)
		{
			if (ft_strcmp(newenv[i], newenv[j]) > 0)
			{
				temp = newenv[i];
				newenv[i] = newenv[j];
				newenv[j] = temp;
			}
			j++;
		}
		i++;
	}
	return (newenv);
}

void	ft_print_env(char **sorted_env)
{
	int	i;

	i = 0;
	while (sorted_env[i])
	{
		printf("%s\n", sorted_env[i]);
		i++;
	}
}

void	ft_env(char **envp)
{
	char	**newenv;
	int		env_count;
	int		i;

	i = 0;
	env_count = 0;
	while (envp[env_count])
		env_count++;
	newenv = malloc((env_count + 1) * sizeof(char *));
	if (!newenv)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	while (i < env_count)
	{
		newenv[i] = envp[i];
		i++;
	}
	newenv[env_count] = NULL;
	ft_sort_env(newenv, env_count);
	ft_print_env(newenv);
	free(newenv);
	g_exit_status = 0;
}
