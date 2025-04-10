/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 18:16:20 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/10 17:11:44 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	malloc_failed(void)
{
	ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
	exit(1);
}

void	add_argument(t_parse *state)
{
	char	*arg;
	char	**new_args;
	char	*new_types;
	int		i;

	arg = ft_substr(state->cmd, state->start, state->i - state->start);
	if (!arg)
		malloc_failed();
	new_args = malloc(sizeof(char *) * (state->args_count + 2));
	new_types = malloc(sizeof(char) * (state->args_count + 2));
	if (!new_args || !new_types)
		malloc_failed();
	i = -1;
	while (++i < state->args_count)
	{
		new_args[i] = state->args[i];
		new_types[i] = state->quote_types[i];
	}
	new_args[i] = arg;
	if (state->in_quotes)
		new_types[i] = state->quote_char;
	else
		new_types[i] = '\0';
	new_args[state->args_count + 1] = NULL;
	new_types[state->args_count + 1] = '\0';
	free(state->args);
	free(state->quote_types);
	state->args = new_args;
	state->quote_types = new_types;
	state->args_count++;
}

void	handle_non_quoted_space(t_parse *state)
{
	if (state->i > state->start)
		add_argument(state);
	while (state->cmd[state->i] == ' ')
		state->i++;
	state->start = state->i;
}

void	handle_end_of_quoted_string(t_parse *state)
{
	add_argument(state);
	state->in_quotes = false;
	state->quote_char = '\0';
	state->i++;
	state->start = state->i;
}
