/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_execute_builtins.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:06:42 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/09 18:53:02 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int check_builtin(char *command)
{
	if (command == NULL)
		return (0);
	if (ft_strcmp(command, "echo") == 0 ||
		ft_strcmp(command, "pwd") == 0 ||
		ft_strcmp(command, "cd") == 0 ||
		ft_strcmp(command, "export") == 0 ||
		ft_strcmp(command, "unset") == 0 ||
		ft_strcmp(command, "exit") == 0 ||
		ft_strcmp(command, "env") == 0)
		return (1);
	return (0);
}

static void restore_fds(int original_stdin, int original_stdout)
{
	dup2(original_stdin, STDIN_FILENO);
	dup2(original_stdout, STDOUT_FILENO);
	close(original_stdin);
	close(original_stdout);
}

int	execute_builtin(char *command, char **args, int *i, char ***envp,
		t_var **vars, t_CommandData *data)
{
	int	fd_in;
	int	fd_out;
	int	original_stdin;
	int	original_stdout;

	fd_in = -1;
	fd_out = -1;
	original_stdin = dup(STDIN_FILENO);
	original_stdout = dup(STDOUT_FILENO);
	if (*i == 0 && data->input_file)
	{
		fd_in = open(data->input_file, O_RDONLY);
		if (fd_in == -1)
		{
			perror("open input file");
			g_exit_status = 1;
			restore_fds(original_stdin, original_stdout);
			return (1);
		}
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	if (*i == data->num_commands - 1 && data->output_file)
	{
		fd_out = open(data->output_file, O_WRONLY | O_CREAT
			| (data->append_output ? O_APPEND : O_TRUNC), 0644);
		if (fd_out == -1)
		{
			perror("open output file");
			g_exit_status = 1;
			restore_fds(original_stdin, original_stdout);
			return (1);
		}
		dup2(fd_out, STDOUT_FILENO);
		close(fd_out);
	}
	if (ft_strcmp(command, "echo") == 0)
		ft_echo(args);
	else if (ft_strcmp(command, "pwd") == 0)
		ft_pwd();
	else if (ft_strcmp(command, "cd") == 0)
		ft_cd(args, i, *envp);
	else if (ft_strcmp(command, "export") == 0)
		g_exit_status = ft_export(args, vars, envp);
	else if (ft_strcmp(command, "unset") == 0)
		g_exit_status = ft_unset(args, vars, envp);
	else if (ft_strcmp(command, "env") == 0)
		ft_env(*envp);
	else if (ft_strcmp(command, "exit") == 0)
		ft_exit(args);

	else
	{
		restore_fds(original_stdin, original_stdout);
		return (0);
	}
	restore_fds(original_stdin, original_stdout);
	return (1);
}
