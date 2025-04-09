/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 17:06:47 by scarlos-          #+#    #+#             */
/*   Updated: 2025/04/09 19:30:52 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <limits.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <string.h>
# include <stdbool.h>
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft/libft.h"

# define PATH_MAX        4096

extern int	g_exit_status;

typedef struct s_var
{
	char			*name;
	char			*value;
	int				exported;// 0=shell var, 1=env var
	struct s_var	*next;
}	t_var;

typedef struct s_shell
{
	t_var	*vars;
	char	**env;
// Add other shell state as needed
}	t_shell;

typedef struct s_parse
{
	char		**args;
	int			args_count;
	bool		in_quotes;
	char		quote_char;
	size_t		i;
	size_t		start;
	int			brace_count;
	const char	*cmd;
}				t_parse;

typedef struct CommandData
{
	char	**commands;// Array of commands
	char	***arguments;// Array of argument arrays
	char	*heredoc_delim;// New field for << delimiter
	char	*input_file;// Input redirection file
	char	*output_file;// Output redirection file
	int		append_output;// Flag for append mode (>>)
	int		num_commands;// Number of commands
	int		num_pipes;// Number of pipes
}	t_CommandData;
//pipes
void		handle_non_quoted_space(t_parse *state);
char		*clean_backslashes(const char *arg);
void		add_argument(t_parse *state);
void		handle_end_of_quoted_string(t_parse *state);
//token
void		initialize_command_data(t_CommandData *data);
void		is_pipe(t_CommandData **data, int *command_index, int *arg_index);
void		redirect_input(char **args, t_CommandData *data, int *i);
void		redirect_output(char **args, t_CommandData *data, int *i);
void		redirect_output_append(char **args, t_CommandData *data, int *i);

size_t		ft_strlen(const char *str);
void		ft_putchar_fd(char c, int fd);
void		ft_putstr_fd(char *s, int fd);
char		*ft_substr(const char *s, unsigned int start, size_t len);
char		**parse_command(const char *input);
char		**parse_command_line(char **input);
void		parse_input(char **args, int count, t_CommandData *data);
void		execute_commands(t_CommandData *data, char ***envp, t_var **vars);
char		*find_command_path(char *command, char *const envp[]);
bool		handle_and_continue_if_var_assignment(char *input, t_var **vars);
char		**copy_envp(char **envp);
void		free_command_data(t_CommandData *data);
bool		free_and_continue_if_empty(char *input);
void		free_envp(char **envp);
void		free_data_commands(char **commands, int num_commands);
void		free_data_arguments(char ***arguments, int num_commands);
//int		execute_builtin(char **args);
int			is_builtin(char *command, char **args, int *i, char **envp);
void		ft_echo(char **args);
int			ft_pwd(void);
int			ft_cd(char **args, int *i, char **envp);
void		ft_exit(char **args);
int			ft_unset(char **args, t_var **vars, char ***envp);
void		ft_env(char **envp);
//void	expand_arguments(char **args);
int			execute_builtin(char *command, char **args, int *i,
				char ***envp, t_var **vars, t_CommandData *data);
int			check_builtin(char *command);
char		*ft_strcpy(char *dest, const char *src);
char		*ft_strndup(const char *src, size_t n);
//vars
char		*expand_variables(const char *arg, t_var *vars, char **envp);
void		store_var(char *name, char *value, t_var **vars);
const char	*get_var_value(const char *name, t_var *vars);
void		free_all_vars(t_var **vars);
int			is_var_assignment(const char *input);
void		handle_var_assignment(const char *input, t_var **vars);
int			is_valid_var_name(const char *name);
int			ft_export(char **args, t_var **vars, char ***envp);
void		print_exported_env(char **envp);

#endif
