/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:45:24 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/09 18:54:21 by scarlos-         ###   ########.fr       */
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

void	parse_and_validate_input(const char *input, char *const envp[],
		t_var **vars)
{
	t_CommandData	data;
	char		**args2;
	int			count;

	args2 = parse_command(input);
	if (args2 == NULL)
	{
		write(STDERR_FILENO, "Error: Failed to parse input\n", 29);
		return ;
	}
	count = 0;
	while (args2[count] != NULL)
		count++;
	parse_input(args2, count, &data);
	execute_commands(&data, (char ***)&envp, vars);
	free_args(args2, &data);
}

int	main(int argc, char *argv[], char *envp[])
{
	(void)argc;
	(void)argv;
	char	**my_envp;
	char	*input;
	t_var	*vars;
	char	*expanded;

	vars = NULL;
	my_envp = copy_envp(envp);
	setup_signals();
	while (1)
	{
		input = readline("minishell> ");
		if (!input)
			break ;
		if (free_and_continue_if_empty(input))
			continue ;
		if (handle_and_continue_if_var_assignment(input, &vars))
			continue ;
		expanded = expand_variables(input, vars, my_envp);
		if (!expanded)
			free (input);
		parse_and_validate_input(expanded, my_envp, &vars);
		add_history (input);
		free (expanded);
		free (input);
	}
	free_envp(my_envp);
	free_all_vars(&vars);
	return (0);
}
