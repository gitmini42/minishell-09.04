/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:45:24 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/10 10:58:11 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_exit_status = 0;

void	sigint_handler()
{
	rl_replace_line("", 0);
	printf("\n");
	rl_on_new_line();
	rl_redisplay();
	fflush(stdout);
}

void	setup_signals()
{
	struct sigaction	sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("sigaction for SIGINT");
		exit(1);
	}
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
	{
		perror("sigaction for SIGQUIT");
		exit(1);
	}
}

void	free_args(char **args2, t_CommandData *data)
{
	int	i;

	i = 0;
	while (args2[i] != NULL)
	{
		free(args2[i]);
		i++;
	}
	free(args2);
	free_command_data(data);
}

static void expand_and_validate(char **tokens, char *quote_types, char **envp,
                                t_var **vars)
{
    char **expanded_args;
    t_CommandData data;
    int i;

	i = 0;
    expanded_args = malloc(sizeof(char *) * (count_args(tokens) + 1));
    if (!expanded_args)
        return;
    while (tokens[i])
    {
        expanded_args[i] = expand_variables(tokens[i], *vars, envp, quote_types[i]);
		printf("Expanded: %s\n", expanded_args[i]);
        if (!expanded_args[i])
            expanded_args[i] = ft_strdup(tokens[i]);
        i++;
    }
    expanded_args[i] = NULL;
    parse_input(expanded_args, count_args(expanded_args), &data);
    execute_commands(&data, &envp, vars);
    free_args(expanded_args, &data);
}

int main(int argc, char *argv[], char *envp[])
{
	(void)argc;
	(void)argv;
    char **my_envp;
    char *input;
    t_var *vars;
    t_parse_result parsed;

    vars = NULL;
    my_envp = copy_envp(envp);
    setup_signals();
    while (1)
    {
        input = readline("minishell> ");
        if (!input)
            break;
        if (free_and_continue_if_empty(input))
            continue;
		add_history(input);
        if (handle_and_continue_if_var_assignment(input, &vars))
            continue;
        parsed = parse_command(input);
        if (!parsed.args)
        {
            free(input);
            continue;
        }
        expand_and_validate(parsed.args, parsed.quote_types, my_envp, &vars);
        free_args(parsed.args, NULL);
        free(parsed.quote_types);
        free(input);
    }
    free_args(my_envp, NULL);
    free_all_vars(&vars);
    return (0);
}

/* int main(int argc, char *argv[], char *envp[])
{
	(void)argc;
	(void)argv;
    char **my_envp = copy_envp(envp);
    t_var *vars = NULL;
    char *input; // Rename to prompt for clarity
    char *expanded;

    setup_signals();
    while (1)
    {
        if (isatty(fileno(stdin)))
        {
            // Interactive mode
            input = readline("minishell> ");
            if (!input) // EOF or Ctrl+D
                break;
        }
        else
        {
            // Tester mode
            char *line = get_next_line(fileno(stdin));
            if (!line) // End of input
                break;
            input = ft_strtrim(line, "\n");
            free(line);
            if (!input) // ft_strtrim failed
                continue;
        }
        if (free_and_continue_if_empty(input))
            continue;
        if (handle_and_continue_if_var_assignment(input, &vars))
            continue;
        expanded = expand_variables(input, vars, my_envp);
        if (!expanded)
        {
            free(input);
            continue;
        }
        parse_and_validate_input(expanded, my_envp, &vars);
        if (isatty(fileno(stdin)))
            add_history(input); // Only add history in interactive mode
        free(expanded);
        free(input);
    }
    free_envp(my_envp);
    free_all_vars(&vars);
    return 0;
} */
