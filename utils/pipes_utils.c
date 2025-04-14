/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 18:16:20 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/14 11:09:18 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	malloc_failed(void)
{
	ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
	g_exit_status = 1;
	exit(1);
}

void add_argument(t_parse *state)
{
    int len = state->i - state->start;
    size_t max_tokens;
	
	if (state->cmd != NULL)
    	max_tokens = ft_strlen(state->cmd) / 2 + 2;
	else 
    	max_tokens = 4;

    if (state->args_count < 0 || state->args_count >= (int)(max_tokens - 1)) // Leave space for NULL
    {
        ft_putstr_fd("minishell: error: too many tokens\n", STDERR_FILENO);
        return;
    }
    state->args[state->args_count] = ft_strndup(&state->cmd[state->start], len);
    if (state->args[state->args_count])
    {
        state->quote_types[state->args_count] = (state->cmd[state->start] == '"' || state->cmd[state->start] == '\'') ? state->cmd[state->start] : '\0';
        state->args_count++;
    }
    state->start = state->i;
}
void handle_non_quoted_space(t_parse *state)
{
    if (state->i > state->start)
        add_argument(state);
    state->start = state->i + 1;
    state->i++;
}

void handle_end_of_quoted_string(t_parse *state)
{
    state->in_quotes = 0;
    state->quote_char = '\0';
    if (state->i > state->start)
        add_argument(state);
    state->start = state->i + 1;
    state->i++;
}
