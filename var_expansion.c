#include "minishell.h"

static size_t handle_exit_status(char *dest, int fill, size_t *i, size_t *j)
{
	char *status;
	size_t len;

	status = ft_itoa(g_exit_status);
	len = ft_strlen(status);
	if (fill && dest)
		ft_strcpy(dest, status);
	free(status);
	if (j)
		*j += len;
	*i += 2;
	return (len);
}

static char	*get_var_value_helper(const char *name, t_var *vars, char **envp)
{
	t_var	*var;
	int		k;

	var = vars;
	k = 0;
	while (var)
	{
		if (ft_strcmp(var->name, name) == 0)
			return (var->value);
		var = var->next;
	}
	while (envp && envp[k])
	{
		if (ft_strncmp(envp[k], name, ft_strlen(name)) == 0
			&& envp[k][ft_strlen(name)] == '=')
			return (envp[k] + ft_strlen(name) + 1);
		k++;
	}
	return ("");
}

static size_t	get_var_len(const char *str, size_t i, t_var *vars, char **envp)
{
	size_t	var_len;
	char	*var_name;
	char	*var_value;

	var_len = 0;
	while (isalnum(str[i + 1 + var_len]) || str[i + 1 + var_len] == '_')
		var_len++;
	var_name = ft_strndup(&str[i + 1], var_len);
	if (!var_name)
		return (0);
	var_value = get_var_value_helper(var_name, vars, envp);
	var_len = ft_strlen(var_value);
	free(var_name);
	return (var_len);
}

static size_t calc_expanded_size(const char *arg, t_var *vars, char **envp,
								char quote_type)
{
	size_t size;
	size_t i;
	size_t var_len;

	size = 0;
	i = 0;
	if (quote_type == '\'')
		return (ft_strlen(arg) + 1);
	while (arg[i])
	{
		if (arg[i] == '$' && arg[i + 1] == '?')
			size += handle_exit_status(NULL, 0, &i, NULL);
		else if (arg[i] == '$' && (isalpha(arg[i + 1]) || arg[i + 1] == '_'))
		{
			var_len = get_var_len(arg, i, vars, envp);
			size += var_len;
			while (isalnum(arg[i + 1]) || arg[i + 1] == '_')
				i++;
			i++;
		}
		else
		{
			size++;
			i++;
		}
	}
	return (size + 1);
}

static void fill_expanded(char *dest, const char *src, t_var *vars, char **envp, char quote_type)
{
    size_t i;
    size_t j;
    size_t var_len;
    char *key;
    char *val;

	i = 0;
	j = 0;
	key = NULL;
    if (!dest || !src)
        return;
    if (quote_type == '\'')
    {
        i = 1;
        while (src[i] && src[i] != '\'')
            dest[j++] = src[i++];
    }
    else if (quote_type == '"')
    {
        i = 1;
        while (src[i] && src[i] != '"')
        {
            if (src[i] == '$' && (ft_isalpha(src[i + 1]) || src[i + 1] == '_' || src[i + 1] == '?'))
            {
                if (src[i + 1] == '?')
                {
                    handle_exit_status(&dest[j], 1, &i, &j);
                }
                else
                {
                    var_len = 0;
                    while (ft_isalnum(src[i + 1 + var_len]) || src[i + 1 + var_len] == '_')
                        var_len++;
                    key = ft_strndup(&src[i + 1], var_len);
                    if (!key)
                        return;
                    val = get_var_value_helper(key, vars, envp);
                    if (val)
                    {
                        ft_strcpy(&dest[j], val);
                        j += ft_strlen(val);
                    }
                    free(key);
                    key = NULL;
                    i += var_len + 1;
                }
            }
            else
                dest[j++] = src[i++];
        }
    }
    else
    {
        while (src[i])
        {
            if (src[i] == '$' && (ft_isalpha(src[i + 1]) || src[i + 1] == '_' || src[i + 1] == '?'))
            {
                if (src[i + 1] == '?')
                {
                    handle_exit_status(&dest[j], 1, &i, &j);
                }
                else
                {
                    var_len = 0;
                    while (ft_isalnum(src[i + 1 + var_len]) || src[i + 1 + var_len] == '_')
                        var_len++;
                    key = ft_strndup(&src[i + 1], var_len);
                    if (!key)
                        return;
                    val = get_var_value_helper(key, vars, envp);
                    if (val)
                    {
                        ft_strcpy(&dest[j], val);
                        j += ft_strlen(val);
                    }
                    free(key);
                    key = NULL;
                    i += var_len + 1;
                }
            }
            else
                dest[j++] = src[i++];
        }
    }
    dest[j] = '\0';
}

char	*expand_variables(const char *arg, t_var *vars, char **envp, char quote_type)
{
	size_t total_size;
	char *expanded;

	if (!arg)
		return (NULL);
	total_size = calc_expanded_size(arg, vars, envp, quote_type);
	expanded = malloc(total_size);
	if (!expanded)
		return (NULL);
	fill_expanded(expanded, arg, vars, envp, quote_type);
	return (expanded);
}

/* char *expand_variables(char *arg, t_var *vars, char **envp)
{
	size_t total_size = 0;
	int i = 0;
	printf("Expanding: %s\n", arg);

	// Pass 1: Calculate size
	while (arg[i])
	{
		if (arg[i] == '$' && (isalpha(arg[i + 1]) || arg[i + 1] == '_'))
		{
			size_t var_len = 0;
			char *var_start = &arg[i + 1];
			while (isalnum(var_start[var_len]) || var_start[var_len] == '_')
				var_len++;
			char *var_name = strndup(var_start, var_len);
			if (!var_name) return (NULL);
			char *var_value = (char *)get_var_value(var_name, vars);
			if (!var_value && envp)
			{
				for (int k = 0; envp[k]; k++)
				{
					if (strncmp(envp[k], var_name, var_len) == 0
						&& envp[k][var_len] == '=')
					{
						var_value = envp[k] + var_len + 1;
						break ;
					}
				}
			}
			if (!var_value)
				var_value = "";
			printf("Var: %s, Value: '%s'\n", var_name, var_value);
			total_size += strlen(var_value);
			i += var_len + 1;
			free(var_name);
		}
		else
		{
			total_size++;
			i++;
		}
	}
	total_size++;  // Null terminator

	// Pass 2: Build string
	char *new_arg = malloc(total_size);
	if (!new_arg)
		return (NULL);
	i = 0;
	int j = 0;CommandData
	{
		if (arg[i] == '$' && (isalpha(arg[i + 1]) || arg[i + 1] == '_'))
		{
			size_t var_len = 0;
			char *var_start = &arg[i + 1];
			while (isalnum(var_start[var_len]) || var_start[var_len] == '_')
				var_len++;
			char *var_name = strndup(var_start, var_len);
			char *var_value = (char *)get_var_value(var_name, vars);
			if (!var_value && envp)
			{
				for (int k = 0; envp[k]; k++)
				{
					if (strncmp(envp[k], var_name, var_len) == 0
						&& envp[k][var_len] == '=')
					{
						var_value = envp[k] + var_len + 1;
						break ;
					}
				}
			}
			if (!var_value)
				var_value = "";
			strcpy(&new_arg[j], var_value);
			j += strlen(var_value);
			i += var_len + 1;
			free(var_name);
		}
		else
		{
			new_arg[j++] = arg[i++];
		}
	}
	new_arg[j] = '\0';
	printf("Expanded to: '%s', Size: %zu\n", new_arg, total_size);
	return (new_arg);
} */

/* char *expand_variables(char *arg)
{
	char *new_arg;
	int i = 0, j = 0;
	char *var_value;
	int new_size = strlen(arg) + 1;
	char *tmp;
	char *var_start;
	int var_len;
	char *var_name;

	new_arg = malloc(new_size);
	if (!new_arg)
		return (NULL);
	while (arg[i])
	{
		if (arg[i] == '$')
		{
			if (arg[i + 1] == '?')
			{
				var_value = ft_itoa(g_exit_status);
				if (!var_value)
				{
					free(new_arg);
					return (NULL);
				}
				new_size += ft_strlen(var_value) - 2;
				tmp = realloc(new_arg, new_size);
				if (!tmp)
				{
					free(var_value);
					free(new_arg);
					return (NULL);
				}
				new_arg = tmp;
				ft_strcpy(&new_arg[j], var_value);
				j += strlen(var_value);
				i += 2;
				free(var_value);
				continue ;
			}
			else if (isalpha(arg[i + 1]) || arg[i + 1] == '_')
			{
				var_start = &arg[i + 1];
				var_len = 0;
				while (isalnum(var_start[var_len]) || var_start[var_len] == '_')
					var_len++;
				var_name = ft_strndup(var_start, var_len);
				if (!var_name)
				{
					free(new_arg);
					return (NULL);
				}
				// Check g_vars first
				var_value = (char *)get_var_value(var_name);
				if (!var_value)
					var_value = getenv(var_name);  // Fallback to environment
				if (!var_value)
					var_value = "";
				new_size += ft_strlen(var_value) - (var_len + 1);
				tmp = realloc(new_arg, new_size);
				if (!tmp)
				{
					free(new_arg);
					free(var_name);
					return (NULL);
				}
				new_arg = tmp;
				ft_strcpy(&new_arg[j], var_value);
				j += strlen(var_value);
				i += var_len + 1;
				free(var_name);
				continue ;
			}
		}
		new_arg[j++] = arg[i++];
	}
	new_arg[j] = '\0';
	return (new_arg);
} */

/* void expand_arguments(char **args)
{
	int i = 0;
	char *expanded;

	while (args[i])
	{
		expanded = expand_variables(args[i]);
		if (expanded)
		{
			free(args[i]);  // Free old argument
			args[i] = expanded; // Replace with expanded value
		}
		i++;
	}
} */
