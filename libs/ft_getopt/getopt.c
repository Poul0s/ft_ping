/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getopt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 14:28:00 by psalame           #+#    #+#             */
/*   Updated: 2024/08/04 20:12:11 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "getopt.h"
#include "getopt_Int.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

static const t_option * find_opt_long(char * opt, const struct s_option * lopts)
{
	while (lopts->name != NULL)
	{
		if (strcmp(opt, lopts->name) == 0)
			return lopts;
		lopts++;
	}
	return NULL;
}

static void	print_err(char *exec_name, int err_nb, intptr_t value)
{
	switch (err_nb)
	{
		case 0:
			fprintf(stderr, "%s: invalid option -- '%c'\n", exec_name, (char) value);
			break;
		case 1:
			fprintf(stderr, "%s: unrecognized option '%s'\n", exec_name, (char *)value);
			break;
		case 2:
			fprintf(stderr, "%s: option '%c' requires an argument\n", exec_name, (char)value);
			break;
		case 3:
			fprintf(stderr, "%s: option '%s' requires an argument\n", exec_name, (char *)value);
			break;
	}
}

t_optres	ft_getopt(int ac, char ** av, const char * opts)
{
	return ft_getopt_long(ac, av, opts, (void *) 0);
}

t_optres	ft_getopt_long(int ac, char ** av, const char * opts, const struct s_option * lopts)
{
	static int	i = 1;
	static int	j = 1;
	if (i < ac && j == 1)
		sort_next_argument(ac, av, i);

	if (i < ac && is_arg_option(av[i]))
	{
		if (j == 1 && av[i][1] == '-' && lopts != NULL)
		{
			const t_option * opt = find_opt_long(av[i] + 2, lopts);
			i++;
			j = 1;
			if (opt == NULL)
			{
				print_err(av[0], 1, (intptr_t) av[i - 1]);
				return (t_optres){0, NULL, i};
			}
			else if (opt->has_arg && av[i] == NULL)
			{
				print_err(av[0], 3, (intptr_t) av[i - 1]);
				return (t_optres){0, NULL, i}; // maybe include the opt->val in struct for future usage
			}
			else
				return (t_optres){opt->val, opt->has_arg ? av[i++] : NULL, i};
		}
		else
		{
			char c = av[i][j++];
			if (!av[i][j])
			{
				i++;
				j = 1;
			}

			char * opt = strchr(opts, c);
			if (!opt)
			{
				print_err(av[0], 0, c);
				return (t_optres){0, NULL, i}; // maybe include the c in struct for future usage
			}
			else if (*(opt + 1) == ':' && i >= ac)
			{
				print_err(av[0], 2, c);
				return (t_optres){0, NULL, i}; // maybe include the c in struct for future usage
			}
			else
			{
				char	*arg;
				if (*(opt + 1) == ':')
				{
					if (j == 1)
						arg = av[i];
					else
					{
						arg = av[i] + j;
						j = 1;
					}
					i++;
				}
				else
					arg = NULL;
				return (t_optres){c, arg, i};
			}
		}
	}
	else
		return (t_optres){-1, NULL, i};
}

