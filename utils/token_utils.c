/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 18:27:28 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/11 09:24:39 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	initialize_command_data(t_CommandData *data)
{
	data->commands = NULL;
	data->arguments = NULL;
	data->num_commands = 0;
	data->num_pipes = 0;
	data->input_file = NULL;
	data->output_file = NULL;
	data->append_output = 0;
	data->heredoc_delim = NULL;
	data->heredoc_quoted = 0;
}

void	is_pipe(t_CommandData **data, int *command_index, int *arg_index)
{
	(*data)->num_pipes++;
	(*command_index)++;
	*arg_index = 0;
}

void	redirect_input(char **args, t_CommandData *data, int *i)
{
	if (args[*i + 1] == NULL)
	{
		write(STDERR_FILENO, "Error: No input file specified after '<'\n", 41);
		g_exit_status = 1;
		return ;
	}
	data->input_file = ft_strdup(args[*i + 1]);
	(*i)++;
}

void	redirect_output(char **args, t_CommandData *data, int *i)
{
	if (args[*i + 1] == NULL)
	{
		write(STDERR_FILENO, "Error: No output file specified after '>'\n", 42);
		g_exit_status = 1;
		return ;
	}
	data->output_file = ft_strdup(args[*i + 1]);
	(*i)++;
}

void	redirect_output_append(char **args, t_CommandData *data, int *i)
{
	if (args[*i + 1] == NULL)
	{
		write(STDERR_FILENO, "Error: No output file specified after '>>'\n",
			43);
		g_exit_status = 1;
		return ;
	}
	data->output_file = ft_strdup(args[*i + 1]);
	data->append_output = 1;
	(*i)++;
}
