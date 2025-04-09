/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 19:03:17 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/09 19:33:06 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_data_commands(char **commands, int num_commands)
{
	int	i;

	if (commands)
	{
		i = 0;
		while (i < num_commands)
		{
			if (commands[i])
				free(commands[i]);
			i++;
		}
		free(commands);
	}
}

void	free_data_arguments(char ***arguments, int num_commands)
{
	int	i;
	int	j;

	if (!arguments)
		return ;
	i = 0;
	while (i < num_commands)
	{
		if (arguments[i])
		{
			j = 0;
			while (arguments[i][j])
			{
				free(arguments[i][j]);
				j++;
			}
			free(arguments[i]);
		}
		i++;
	}
	free(arguments);
}
