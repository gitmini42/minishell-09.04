/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 18:16:20 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/09 18:49:15 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*clean_backslashes(const char *arg)
{
	size_t	i;
	size_t	j;
	size_t	len;
	char	*cleaned;

	i = 0;
	j = 0;
	len = ft_strlen(arg);
	cleaned = malloc(len + 1);
	if (!cleaned)
	{
		ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
		exit(1);
	}
	while (arg[i])
	{
		if (arg[i] == '\\' && arg[i + 1] != '\0')
			i++;
		cleaned[j] = arg[i];
		j++;
		i++;
	}
	cleaned[j] = '\0';
	return (cleaned);
}

void	malloc_failed(void)
{
	ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
	exit(1);
}

void	add_argument(t_parse *state)
{
	char	*arg;
	char	*cleaned_arg;
	char	**new_args;
	int		i;

	arg = ft_substr(state->cmd, state->start, state->i - state->start);
	if (!arg)
		malloc_failed();
	cleaned_arg = clean_backslashes(arg);
	free(arg);
	new_args = malloc(sizeof(char *) * (state->args_count + 2));
	if (!new_args)
		malloc_failed();
	i = -1;
	while (++i < state->args_count)
		new_args[i] = state->args[i];
	new_args[state->args_count] = cleaned_arg;
	new_args[state->args_count + 1] = NULL;
	free(state->args);
	state->args = new_args;
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
	state->in_quotes = false;
	add_argument(state);
	state->i++;
	state->start = state->i;
	state->quote_char = '\0';
}
