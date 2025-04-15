/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_handling.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 18:22:05 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/15 11:42:09 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_operator(const char *token)
{
	return (ft_strcmp(token, "|") == 0 || ft_strcmp(token, "<") == 0
		|| ft_strcmp(token, ">") == 0 || ft_strcmp(token, ">>") == 0
		|| ft_strcmp(token, "<<") == 0);
}

void	free_command_data(t_CommandData *data)
{
	if (!data)
		return ;
	if (data->commands)
		free_data_commands(data->commands, data->num_commands);
	if (data->arguments)
		free_data_arguments(data->arguments, data->num_commands);
	if (data->input_file)
		free(data->input_file);
	if (data->output_file)
		free(data->output_file);
	if (data->heredoc_delim)
		free(data->heredoc_delim);
	data->num_commands = 0;
	data->num_pipes = 0;
}

static void	handle_operator(char **args, t_CommandData *data,
		int *i, int *command_index)
{
	const char	*token = args[*i];
	size_t		len;
	char		*delim;

	if (ft_strcmp(token, "|") == 0)
	{
		data->num_pipes++;
		(*command_index)++;
	}
	else if (ft_strcmp(token, "<") == 0)
	{
		if (args[*i + 1])
		{
			free(data->input_file);
			data->input_file = ft_strdup(args[*i + 1]);
			(*i)++;
		}
	}
	else if (ft_strcmp(token, ">") == 0)
	{
		if (args[*i + 1])
		{
			free(data->output_file);
			data->output_file = ft_strdup(args[*i + 1]);
			data->append_output = 0;
			(*i)++;
		}
	}
	else if (ft_strcmp(token, ">>") == 0)
	{
		if (args[*i + 1])
		{
			free(data->output_file);
			data->output_file = ft_strdup(args[*i + 1]);
			data->append_output = 1;
			(*i)++;
		}
	}
	else if (ft_strcmp(token, "<<") == 0)
	{
		if (args[*i + 1])
		{
			free(data->heredoc_delim);
			delim = args[*i + 1];
			len = ft_strlen(delim);
			if (len >= 2 && (delim[0] == '"' || delim[0] == '\'') \
				&& delim[len - 1] == delim[0])
			{
				data->heredoc_delim = ft_strndup(delim + 1, len - 2);
				data->heredoc_quoted = 1;
			}
			else
			{
				data->heredoc_delim = ft_strdup(delim);
				data->heredoc_quoted = 0;
			}
			(*i)++;
		}
		else
		{
			write(STDERR_FILENO, "Error: No delimiter specified after '<<'\n",
				 42);
		}
	}
	(*i)++;
}

void parse_input(char **args, int count, t_CommandData *data)
{
	int	arg_index;
	int	*arg_counts;
	int	command_index;
	int	i;

	ft_memset(data, 0, sizeof(t_CommandData));
	arg_counts = malloc(count * sizeof(int));
	if (!arg_counts)
		return;
	ft_memset(arg_counts, 0, sizeof(int) * count);
	command_index = 0;
	i = 0;
	while (i < count)
	{
		if (is_operator(args[i]))
			handle_operator(args, data, &i, &command_index);
		else
		{
			arg_counts[command_index]++;
			i++;
		}
	}
	data->num_commands = command_index + 1;
	data->commands = malloc(data->num_commands * sizeof(char *));
	data->arguments = malloc(data->num_commands * sizeof(char **));
	if (!data->commands || !data->arguments)
	{
		free_command_data(data);
		free(arg_counts);
		return;
	}
	ft_memset(data->commands, 0, data->num_commands * sizeof(char *));
	ft_memset(data->arguments, 0, data->num_commands * sizeof(char **));
	command_index = 0;
	arg_index = 0;
	i = 0;
	while (i < count)
	{
		if (is_operator(args[i]))
		{
			handle_operator(args, data, &i, &command_index);
			arg_index = 0;
		}
		else
		{
			if (arg_index == 0)
			{
				data->commands[command_index] = ft_strdup(args[i]);
				data->arguments[command_index] = malloc((arg_counts[command_index] + 1) * sizeof(char *));
				if (!data->commands[command_index] || !data->arguments[command_index])
				{
					free_command_data(data);
					free(arg_counts);
					return;
				}
			}
			data->arguments[command_index][arg_index] = ft_strdup(args[i]);
			if (!data->arguments[command_index][arg_index])
			{
				free_command_data(data);
				free(arg_counts);
				return;
			}
			arg_index++;
			i++;
		}
	}
	i = 0;
	while (i < data->num_commands)
	{
		if (data->arguments[i])
			data->arguments[i][arg_counts[i]] = NULL;
		i++;
	}
	free(arg_counts);
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

	// Free allocated memory
	free_command_data(&data);

	return (0);
} */
