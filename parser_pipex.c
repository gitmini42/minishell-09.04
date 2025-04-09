/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_pipex.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 17:32:45 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/09 18:54:44 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	setup_heredoc(const char *delimiter)
{
	int		pipefd[2];
	char	*line;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		write(pipefd[1], line, ft_strlen(line));
		write(pipefd[1], "\n", 1);
		free(line);
	}
	close(pipefd[1]);
	return (pipefd[0]);
}

void	execute_command(char *command, char **args, char *const envp[])
{
	char	*full_path;

	full_path = find_command_path(command, envp);
	if (!full_path)
	{
		ft_putstr_fd("Command not found: ", 2);
		ft_putstr_fd(command, 2);
		ft_putstr_fd("\n", 2);
		g_exit_status = 127;
		exit(127);
	}
	execve(full_path, args, envp);
	perror("execve");
	free(full_path);
	exit(g_exit_status);
}

void	setup_pipes_and_redirections(int i, int num_commands, int *pipefd,
		int *prev_pipe_read, t_CommandData *data)
{
	int	fd;
	int	flags;

	if (i > 0)
	{
		dup2(*prev_pipe_read, STDIN_FILENO);
		close(*prev_pipe_read);
	}
	if (i == 0 && data->input_file)
	{
		fd = open(data->input_file, O_RDONLY);
		if (fd < 0)
		{
			perror("open input file");
			exit(1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	if (i < num_commands - 1)
	{
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);
	}
	if (i == num_commands - 1 && data->output_file)
	{
		flags = O_WRONLY | O_CREAT;
		if (data->append_output)
			flags |= O_APPEND;
		else
			flags |= O_TRUNC;
		fd = open(data->output_file, flags, 0644);
		if (fd < 0)
		{
			perror("open output file");
			exit(1);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
}

void	execute_commands(t_CommandData *data, char ***envp, t_var **vars)
{
	int		pipefd[2];
	int		prev_pipe_read;
	int		i;
	pid_t	*pids;
	int		status;
	int		heredoc_fd;
	int		is_builtin_cmd;
	pid_t	pid;

	prev_pipe_read = -1;
	i = 0;
	heredoc_fd = -1;
	pids = malloc(sizeof(pid_t) * data->num_commands);
	if (!pids)
	{
		perror("malloc");
		exit(1);
	}
	ft_memset(pids, 0, sizeof(pid_t) * data->num_commands);
	while (i < data->num_commands)
	{
		if (i < data->num_commands - 1)
		{
			if (pipe(pipefd) == -1)
			{
				perror("pipe");
				g_exit_status = 1;
				free(pids);
				return ;
			}
		}
		if (data->heredoc_delim && i == 0)
		{
			heredoc_fd = setup_heredoc(data->heredoc_delim);
			if (heredoc_fd == -1)
			{
				g_exit_status = 1;
				free(pids);
				return ;
			}
		}
		is_builtin_cmd = data->commands[i] && check_builtin(data->commands[i]);
		if (data->num_commands > 1 || !is_builtin_cmd)
		{
			pid = fork();
			if (pid == 0)
			{
				if (heredoc_fd != -1)
				{
					dup2(heredoc_fd, STDIN_FILENO);
					close(heredoc_fd);
				}
				setup_pipes_and_redirections(i, data->num_commands, pipefd,
					&prev_pipe_read, data);
				if (is_builtin_cmd)
				{
					execute_builtin(data->commands[i], data->arguments[i], &i,
						envp, vars, data);
					exit(g_exit_status);
				}
				else
				{
					execute_command(data->commands[i], data->arguments[i],
						*envp);
					exit(1);
				}
			}
			else if (pid > 0)
			{
				pids[i] = pid;
				if (prev_pipe_read != -1)
					close(prev_pipe_read);
				if (i < data->num_commands - 1)
				{
					prev_pipe_read = pipefd[0];
					close(pipefd[1]);
				}
				if (heredoc_fd != -1)
				{
					close(heredoc_fd);
					heredoc_fd = -1;
				}
			}
			else
			{
				perror("fork");
				g_exit_status = 1;
				free(pids);
				return ;
			}
		}
		else if (is_builtin_cmd)
		{
			if (heredoc_fd != -1)
			{
				dup2(heredoc_fd, STDIN_FILENO);
				close(heredoc_fd);
				heredoc_fd = -1;
			}
			execute_builtin(data->commands[i], data->arguments[i], &i, envp,
				vars, data);
		}
		i++;
	}
	i = 0;
	while (i < data->num_commands)
	{
		if (pids[i] > 0)
		{
			if (waitpid(pids[i], &status, 0) > 0)
			{
				if (WIFEXITED(status))
					g_exit_status = WEXITSTATUS(status);
				else if (WIFSIGNALED(status))
					g_exit_status = 128 + WTERMSIG(status);
			}
		}
		i++;
	}
	free(pids);
}

/* int main() {
	// Example input: "ls -l | grep .c > output.txt"
	char *tokens[] = {"ls", "-l", "|", "grep", ".c", ">", "output.txt"};
	int count = sizeof(tokens) / sizeof(tokens[0]);

	CommandData data;
	parse_input(tokens, count, &data);

	// Print parsed data
	printf("Number of commands: %d\n", data.num_commands);
	printf("Number of pipes: %d\n", data.num_pipes);
	printf("Input file: %s\n", data.input_file ? data.input_file : "None");
	printf("Output file: %s\n", data.output_file ? data.output_file : "None");
	printf("Append output: %d\n", data.append_output);

	for (int i = 0; i < data.num_commands; i++) {
		printf("Command %d: %s\n", i, data.commands[i]);
		for (int j = 0; data.arguments[i][j] != NULL; j++) {
			printf("  Argument %d: %s\n", j, data.arguments[i][j]);
		}
	}

	// Execute the commands
	char *envp[] = {NULL}; // Empty environment (can be customized)
	execute_commands(&data, envp);

	// Free allocated memory
	free_command_data(&data);

	return (0);
} */
