/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:06:32 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/14 12:18:44 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_set_env(char **envp, const char *name, const char *value)
{
	size_t	name_l;
	size_t	value_len;
	int		i;
	char	*new_entry;

	name_l = ft_strlen(name);
	value_len = ft_strlen(value);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], name, name_l) == 0 && envp[i][name_l] == '=')
		{
			new_entry = malloc(name_l + value_len + 2);
			if (!new_entry)
				return (-1);
			ft_strcpy(new_entry, name);
			new_entry[name_l] = '=';
			ft_strcpy(new_entry + name_l + 1, value);
			free(envp[i]);
			envp[i] = new_entry;
			return (0);
		}
		i++;
	}
	return (-1);
}

int	cd_error(const char *msg, char *old_pwd)
{
	perror(msg);
	g_exit_status = 1;
	free(old_pwd);
	return (1);
}

int	gotohome(char **path)
{
	*path = getenv("HOME");
	if (*path == NULL)
	{
		perror("cd: HOME not set\n");
		g_exit_status = 1;
		return (1);
	}
	return (0);
}

int	ft_cd(char **args, int *i, char **envp)
{
	char	*path;
	char	*old_pwd;
	char	*new_pwd;

	old_pwd = getcwd(NULL, 0);
	if (args[1] == NULL || strcmp(args[1], "~") == 0)
		gotohome(&path);
	else
		path = args[1];
	if (chdir(path) != 0)
		return (cd_error("cd", old_pwd));
	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
		return (cd_error("getcwd", old_pwd));
	if (old_pwd)
		ft_set_env(envp, "OLDPWD", old_pwd);
	ft_set_env(envp, "PWD", new_pwd);
	free(old_pwd);
	free(new_pwd);
	(*i)++;
	g_exit_status = 0;
	return (0);
}
