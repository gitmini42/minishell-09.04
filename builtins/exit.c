/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 16:05:58 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/13 05:45:54 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	is_numeric(const char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

void	ft_exit(char **args)
{
	int	exit_code;

	if (args[1])
	{
		if (!is_numeric(args[1]))
		{
			printf("exit\nminishell: exit: %s: numeric argument required\n" \
			, args[1]);
			exit(1);
		}
		else if (args[2])
		{
			printf("exit\nminishell: exit: too many arguments\n");
			return ;
		}
		if (ft_atoi (args[1]) > 255)
			exit_code = ft_atoi(args[1]) % 255;
		else
			exit_code = ft_atoi(args[1]);
	}
	else
		exit_code = 0;
	printf("Exiting minishell...\n");
	free_args(args, NULL);
	exit(exit_code);
}
