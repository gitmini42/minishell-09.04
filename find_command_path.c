/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_command_path.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:45:50 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/14 11:55:03 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*get_path_from_env(char *const envp[])
{
	int	i;

	i = 0;
	while (envp && envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i] + 5);
		i++;
	}
	return (NULL);
}

static char	*check_direct_executable(char *command)
{
	if (access(command, X_OK) == 0)
		return (ft_strdup(command));
	return (NULL);
}

static char	*search_in_path(char *command, char **path_dirs)
{
	int		i;
	char	*temp;
	char	*full_path;

	i = 0;
	while (path_dirs[i])
	{
		temp = ft_strjoin(path_dirs[i++], "/");
		if (!temp)
			return (char_malloc_filed());
		full_path = ft_strjoin(temp, command);
		free(temp);
		if (!full_path)
			return (char_malloc_filed());
		if (access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
	}
	return (NULL);
}

void	free_command_args(char **args)
{
	int	i;

	i = 0;
	while (args[i] != NULL)
	{
		free(args[i]);
		i++;
	}
	free(args);
}

char	*find_command_path(char *command, char *const envp[])
{
	char	*path;
	char	**path_dirs;
	char	*result;

	result = check_direct_executable(command);
	if (result)
		return (result);
	path = get_path_from_env(envp);
	if (!path)
		return (NULL);
	path_dirs = ft_split(path, ':');
	if (!path_dirs)
	{
		ft_putstr_fd("Error: ft_split failed\n", STDERR_FILENO);
		g_exit_status = 1;
		exit(1);
	}
	result = search_in_path(command, path_dirs);
	free_command_args(path_dirs);
	if (result)
		return (result);
	else
		return (check_direct_executable(command));
}
