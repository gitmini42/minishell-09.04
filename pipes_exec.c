/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 18:12:49 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/14 11:10:52 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


//I don't think this is used anymore
/* char **finalize_args(t_parse *state)
{
    if (state->in_quotes)
    {
        ft_putstr_fd("Error: unclosed quotes\n", STDERR_FILENO);
		g_exit_status = 1;
        free(state->args);
        free(state->quote_types);
        return (NULL);
    }
    state->args[state->args_count] = NULL;
    free(state->quote_types);
    return (state->args);
} */

void initialize_state(t_parse *state, const char *cmd)
{
    size_t max_tokens;
	
	if (cmd)
    	max_tokens = ft_strlen(cmd) / 2 + 2;
	else
    	max_tokens = 4;
    ft_memset(state, 0, sizeof(t_parse));
    state->cmd = cmd;
    state->args = malloc(sizeof(char *) * max_tokens);
    state->quote_types = malloc(sizeof(char) * max_tokens);
    if (!state->args || !state->quote_types)
    {
        free(state->args);
        free(state->quote_types);
        state->args = NULL;
        state->quote_types = NULL;
        return;
    }
    ft_memset(state->args, 0, sizeof(char *) * max_tokens);
    ft_memset(state->quote_types, 0, sizeof(char) * max_tokens);
}

/* static void handle_quoted_string(t_parse *state)
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
} */

t_parse_result parse_command(const char *cmd)
{
    t_parse         state;
    t_parse_result  result;
    int             last_was_operator;
	int is_double;
	char op;
	size_t max_tokens;

	if (cmd)
    	max_tokens = ft_strlen(cmd) / 2 + 2;
	else
    	max_tokens = 4;

	last_was_operator = 0;
    result.args = NULL;
    result.quote_types = NULL;
    initialize_state(&state, cmd);
    if (!state.args || !state.quote_types)
    {
        g_exit_status = 1;
        return (result);
    }
    if (cmd && cmd[0] == '|')
    {
        ft_putstr_fd("minishell: syntax error near unexpected token '|'\n", STDERR_FILENO);
        g_exit_status = 2;
        free(state.args);
        free(state.quote_types);
        return (result);
    }
    while (state.cmd[state.i])
    {
        while (ft_isspace(state.cmd[state.i]) && !state.in_quotes)
            state.i++;
        if (!state.cmd[state.i])
            break;
        state.start = state.i;
        if ((state.cmd[state.i] == '(' || state.cmd[state.i] == ')') && !state.in_quotes)
        {
            ft_putstr_fd("minishell: syntax error near unexpected token\n", STDERR_FILENO);
            g_exit_status = 2;
            free(state.args);
            free(state.quote_types);
            return (result);
        }
        if (!state.in_quotes && (state.cmd[state.i] == '<' || state.cmd[state.i] == '>' || state.cmd[state.i] == '|'))
        {
            op = state.cmd[state.i];
            is_double = (state.cmd[state.i + 1] == op && (op == '<' || op == '>'));
            if (is_double)
                state.i += 2;
            else
                state.i++;
            if (last_was_operator || (op == '|' && !state.cmd[state.i]))
            {
                ft_putstr_fd("minishell: syntax error near unexpected token '", STDERR_FILENO);
                ft_putstr_fd(op == '|' ? "|" : (op == '<' ? "<" : ">"), STDERR_FILENO);
                ft_putstr_fd("'\n", STDERR_FILENO);
                g_exit_status = 2;
                free(state.args);
                free(state.quote_types);
                return (result);
            }
            add_argument(&state);
            last_was_operator = 1;
            continue;
        }
        while (state.cmd[state.i] && (!ft_isspace(state.cmd[state.i]) || state.in_quotes))
        {
            if ((state.cmd[state.i] == '\'' || state.cmd[state.i] == '"') && !state.in_quotes)
            {
                state.quote_char = state.cmd[state.i];
                state.in_quotes = 1;
                state.i++;
            }
            else if (state.cmd[state.i] == state.quote_char && state.in_quotes && state.brace_count == 0)
            {
                state.in_quotes = 0;
                state.quote_char = '\0';
                state.i++;
            }
            else if (state.in_quotes)
            {
                if (state.cmd[state.i] == '{')
                    state.brace_count++;
                else if (state.cmd[state.i] == '}')
                    state.brace_count--;
                state.i++;
            }
            else if (!state.in_quotes && (state.cmd[state.i] == '<' || state.cmd[state.i] == '>' || state.cmd[state.i] == '|'))
                break;
            else
                state.i++;
        }
        if (state.i > state.start)
        {
            add_argument(&state);
            last_was_operator = 0;
        }
    }
    if (state.in_quotes)
    {
        ft_putstr_fd("minishell: syntax error: unclosed quotes\n", STDERR_FILENO);
        g_exit_status = 2;
        free(state.args);
        free(state.quote_types);
        result.args = NULL;
        result.quote_types = NULL;
    }
    else if (last_was_operator)
    {
        ft_putstr_fd("minishell: syntax error near unexpected token 'newline'\n", STDERR_FILENO);
        g_exit_status = 2;
        free(state.args);
        free(state.quote_types);
        result.args = NULL;
        result.quote_types = NULL;
    }
    else
    {
        max_tokens = (cmd ? ft_strlen(cmd) / 2 + 2 : 4);
        if (state.args_count < (int)max_tokens)
            state.args[state.args_count] = NULL;
        result.args = state.args;
        result.quote_types = state.quote_types;
    }
    return (result);
}
