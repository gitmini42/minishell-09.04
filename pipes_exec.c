/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 18:12:49 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/10 10:48:54 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"



char **finalize_args(t_parse *state)
{
    if (state->in_quotes)
    {
        ft_putstr_fd("Error: unclosed quotes\n", STDERR_FILENO);
        free(state->args);
        free(state->quote_types);
        return (NULL);
    }
    state->args[state->args_count] = NULL;
    free(state->quote_types);
    return (state->args);
}

void	initialize_state(t_parse *state, const char *cmd)
{
	state->args = malloc(sizeof(char *) * 2);
    state->quote_types = malloc(sizeof(char) * 2);
	if (!state->args)
	{
		ft_putstr_fd("Error: malloc failed\n", STDERR_FILENO);
		exit(1);
	}
	state->args_count = 0;
    state->in_quotes = false;
    state->quote_char = '\0';
    state->i = 0;
    state->start = 0;
    state->brace_count = 0;
    state->cmd = cmd;
}

static void handle_quoted_string(t_parse *state)
{
    state->in_quotes = true;
    state->quote_char = state->cmd[state->i];
    state->start = state->i;
    state->i++;
}

static void handle_braces_and_escapes(t_parse *state)
{
    if (state->cmd[state->i] == '{')
        state->brace_count++;
    else if (state->cmd[state->i] == '}')
        state->brace_count--;
    state->i++;
}

t_parse_result parse_command(const char *cmd)
{
    t_parse			state;
    t_parse_result	result;

	result.args = NULL;
	result.quote_types = NULL;
    initialize_state(&state, cmd);
    while (state.cmd[state.i])
    {
        if ((state.cmd[state.i] == '(' || state.cmd[state.i] == ')') && !state.in_quotes)
        {
            ft_putstr_fd("minishell: syntax error near unexpected token\n", STDERR_FILENO);
            free(state.args);
            free(state.quote_types);
            return (result);
        }
        if ((state.cmd[state.i] == '\'' || state.cmd[state.i] == '"') && !state.in_quotes)
            handle_quoted_string(&state);
        else if (state.cmd[state.i] == state.quote_char && state.in_quotes && state.brace_count == 0)
            handle_end_of_quoted_string(&state);
        else if (state.in_quotes)
            handle_braces_and_escapes(&state);
        else if (state.cmd[state.i] == ' ' && !state.in_quotes)
            handle_non_quoted_space(&state);
        else
            state.i++;
    }
    if (state.i > state.start)
        add_argument(&state);
    if (state.in_quotes)
    {
        ft_putstr_fd("Error: unclosed quotes\n", STDERR_FILENO);
        free(state.args);
        free(state.quote_types);
    }
    else
    {
        state.args[state.args_count] = NULL;
        result.args = state.args;
        result.quote_types = state.quote_types;
    }
    return (result);
}
