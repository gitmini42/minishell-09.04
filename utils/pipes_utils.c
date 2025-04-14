/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 18:16:20 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/14 12:13:55 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	malloc_failed(void)
{
	ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
	g_exit_status = 1;
	exit(1);
}

char	*char_malloc_filed(void)
{
	ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
	g_exit_status = 1;
	return (NULL);
}

void	add_argument(t_parse *state)
{
	int		len;
	size_t	max_tokens;
	char	quote;

	len = state->i - state->start;
	if (state->cmd != NULL)
		max_tokens = ft_strlen(state->cmd) / 2 + 2;
	else
		max_tokens = 4;
	if (state->args_count < 0 || state->args_count >= (int)(max_tokens - 1))
	{
		ft_putstr_fd("minishell: error: too many tokens\n", STDERR_FILENO);
		return ;
	}
	state->args[state->args_count] = ft_strndup(&state->cmd[state->start], len);
	if (state->args[state->args_count])
	{
		if (state->cmd[state->start] == '"' || state->cmd[state->start] == '\'')
			quote = state->cmd[state->start];
		else
			quote = '\0';
		state->quote_types[state->args_count] = quote;
		state->args_count++;
	}
	state->start = state->i;
}

void	handle_non_quoted_space(t_parse *state)
{
	if (state->i > state->start)
		add_argument(state);
	state->start = state->i + 1;
	state->i++;
}

void	handle_end_of_quoted_string(t_parse *state)
{
	state->in_quotes = 0;
	state->quote_char = '\0';
	if (state->i > state->start)
		add_argument(state);
	state->start = state->i + 1;
	state->i++;
}
