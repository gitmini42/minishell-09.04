/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:45:24 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/15 11:36:01 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/stat.h>

int	g_exit_status = 0;

void	sigint_handler(int signum)
{
	(void)signum;
	rl_replace_line("", 0);
	printf("\n");
	rl_on_new_line();
	rl_redisplay();
	fflush(stdout);
	g_exit_status = 130;
}

void	setup_signals(void)
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

static void	expand_and_validate(char **tokens, char *quote_types,
		char ***envp, t_var **vars)
{
	char			**expanded_args;
	char			*home;
	char			*tmp;
	t_CommandData	data;
	size_t			len;
	struct stat		st;

	int i = 0;
	if (!tokens || !tokens[0])
	{
		g_exit_status = 0;
		return;
	}
	expanded_args = malloc(sizeof(char *) * (count_args(tokens) + 1));
	if (!expanded_args)
	{
		g_exit_status = 1;
		return;
	}
	while (tokens[i])
	{
		if (i > 0 && ft_strcmp(tokens[i - 1], "<<") == 0)
		{
			expanded_args[i] = ft_strdup(tokens[i]); // Preserve heredoc delimiters
		}
		else
		{
			expanded_args[i] = expand_variables(tokens[i], *vars, *envp, quote_types[i]);
			if (!expanded_args[i])
				expanded_args[i] = ft_strdup(tokens[i]);
			if (quote_types[i] == '"' || quote_types[i] == '\'')
			{
				tmp = expanded_args[i];
				len = ft_strlen(tmp);
				if (len >= 2 && tmp[0] == quote_types[i] && tmp[len - 1] == quote_types[i])
				{
					expanded_args[i] = ft_strndup(tmp + 1, len - 2);
					free(tmp);
				}
			}
		}
		i++;
	}
	expanded_args[i] = NULL;
	if (expanded_args[0])
	{
		if (ft_strcmp(expanded_args[0], "~") == 0)
		{
			home = getenv("HOME");
			free(expanded_args[0]);
			expanded_args[0] = ft_strdup(home ? home : "");
		}
		if (ft_strcmp(expanded_args[0], ".") == 0 || ft_strcmp(expanded_args[0], "..") == 0)
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(expanded_args[0], STDERR_FILENO);
			ft_putstr_fd(": command not found\n", STDERR_FILENO);
			g_exit_status = 127;
			free_args(expanded_args, NULL);
			return;
		}
		if (stat(expanded_args[0], &st) == 0 && S_ISDIR(st.st_mode))
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(expanded_args[0], STDERR_FILENO);
			ft_putstr_fd(": Is a directory\n", STDERR_FILENO);
			g_exit_status = 126;
			free_args(expanded_args, NULL);
			return;
		}
	}
	ft_memset(&data, 0, sizeof(t_CommandData));
	parse_input(expanded_args, count_args(expanded_args), &data);
	execute_commands(&data, envp, vars);
	free_args(expanded_args, &data);
}

char	*get_user_input(void)
{
	char	*input;

	if (isatty(fileno(stdin)))
		return readline("minishell> ");
	else
	{
		char *line = get_next_line(fileno(stdin));
		if (!line)
			return NULL;
		input = ft_strtrim(line, "\n");
		free(line);
		return (input);
	}
}

int	process_special_cases(char *input, t_var **vars)
{
	if (!input)
		return (1);
	if (ft_strcmp(input, "exit") == 0)
	{
		free(input);
		exit(g_exit_status);
	}
	if (free_and_continue_if_empty(input))
		return (1);
	add_history(input);
	if (handle_and_continue_if_var_assignment(input, vars))
		return (1);
	return (0);
}

void	cleanup_and_exit(char **envp, t_var *vars)
{
	free_args(envp, NULL);
	free_all_vars(&vars);
	clear_history();
}

int	main(int argc, char *argv[], char *envp[])
{
	(void)argc;
	(void)argv;
	char			**my_envp;
	char			*input;
	t_var			*vars;
	t_parse_result	parsed;

	my_envp = copy_envp(envp);
	vars = NULL;
	setup_signals();
	while ((input = get_user_input()))
	{
		if (process_special_cases(input, &vars))
			continue;
		parsed = parse_command(input);
		if (!parsed.args)
			continue;
		expand_and_validate(parsed.args, parsed.quote_types, &my_envp, &vars);
		free_args(parsed.args, NULL);
		free(parsed.quote_types);
		free(input);
	}
	cleanup_and_exit(my_envp, vars);
	return (g_exit_status);
}
