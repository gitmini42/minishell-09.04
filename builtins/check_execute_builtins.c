/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_execute_builtins.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:06:42 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/13 06:29:39 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	restore_fds(int original_stdin, int original_stdout)
{
	dup2(original_stdin, STDIN_FILENO);
	dup2(original_stdout, STDOUT_FILENO);
	close(original_stdin);
	close(original_stdout);
}

int	handle_input_redirection(t_CommandData *data, int *i, int original_stdin)
{
	int	fd_in;

	fd_in = -1;
	if (*i == 0 && data->input_file)
	{
		fd_in = open(data->input_file, O_RDONLY);
		if (fd_in == -1)
		{
			perror("open input file");
			g_exit_status = 1;
			restore_fds(original_stdin, STDOUT_FILENO);
			return (1);
		}
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	return (0);
}

int	handle_output_redirection(t_CommandData *data, int *i, int original_stdout)
{
	int	fd_out;

	fd_out = -1;
	if (*i == data->num_commands - 1 && data->output_file)
	{
		if (data->append_output)
			fd_out = open(data->output_file, O_WRONLY | \
				O_CREAT | O_APPEND, 0644);
		else
			fd_out = open(data->output_file, O_WRONLY | \
				O_CREAT | O_TRUNC, 0644);
		if (fd_out == -1)
		{
			perror("open output file");
			g_exit_status = 1;
			restore_fds(STDIN_FILENO, original_stdout);
			return (1);
		}
		dup2(fd_out, STDOUT_FILENO);
		close(fd_out);
	}
	return (0);
}

int	execute_builtin_command(char *command, char **args,
		t_var **vars, char ***envp)
{
	if (ft_strcmp(command, "echo") == 0)
		ft_echo(args);
	else if (ft_strcmp(command, "pwd") == 0)
		ft_pwd();
	else if (ft_strcmp(command, "export") == 0)
		g_exit_status = ft_export(args, vars, envp);
	else if (ft_strcmp(command, "unset") == 0)
		g_exit_status = ft_unset(args, vars, envp);
	else if (ft_strcmp(command, "env") == 0)
		ft_env(*envp);
	else if (ft_strcmp(command, "exit") == 0)
		ft_exit(args);
	else
		return (0);
	return (1);
}

int	execute_builtin(int *i, char ***envp, t_var **vars, t_CommandData *data)
{
	int		original_stdin;
	int		original_stdout;
	char	*command;
	char	**args;

	original_stdin = dup(STDIN_FILENO);
	original_stdout = dup(STDOUT_FILENO);
	command = data->commands[*i];
	args = data->arguments[*i];
	if (handle_input_redirection(data, i, original_stdin))
		return (1);
	if (handle_output_redirection(data, i, original_stdout))
		return (1);
	if (!execute_builtin_command(command, args, vars, envp))
	{
		if (ft_strcmp(command, "cd") == 0)
			ft_cd(args, i, *envp); //feito fora pela quantidade de parametros
		else
			restore_fds(original_stdin, original_stdout);
		return (0);
	}
	restore_fds(original_stdin, original_stdout);
	return (1);
}
