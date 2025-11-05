/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getopt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 14:28:00 by psalame           #+#    #+#             */
/*   Updated: 2025/11/05 15:02:22 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "getopt.h"
#include "getopt_Int.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

int optind = 1;

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
		optind = i + 1;
		if (j == 1 && av[i][1] == '-' && lopts != NULL)
		{
			const t_option * opt = find_opt_long(av[i] + 2, lopts);
			i++;
			j = 1;
			if (opt == NULL)
			{
				print_err(av[0], 1, (intptr_t) av[i - 1]);
				return (t_optres){0, NULL};
			}
			else if (opt->has_arg && av[i] == NULL)
			{
				fprintf(stderr, "WARNING: has_arg functionnality not finish. It may and it will produce unexpected result if used.\n"); // see todo in sort_arg_options.c
				print_err(av[0], 3, (intptr_t) av[i - 1]);
				return (t_optres){0, NULL}; // maybe include the opt->val in struct for future usage
			}
			else {
				if (opt->has_arg) {
					fprintf(stderr, "WARNING: has_arg functionnality not finish. It may and it will produce unexpected result if used.\n"); // see todo in sort_arg_options.c
					optind = i + 1;
				}
				return (t_optres){opt->val, opt->has_arg ? av[i++] : NULL};
			}
		}
		else
		{
			char c = av[i][j++];
			if (!av[i][j])
			{
				i = optind;
				j = 1;
			}

			char * opt = strchr(opts, c);
			if (!opt)
			{
				print_err(av[0], 0, c);
				return (t_optres){0, NULL}; // maybe include the c in struct for future usage
			}
			else if (*(opt + 1) == ':' && optind >= ac)
			{
				fprintf(stderr, "WARNING: has_arg functionnality not finish. It may and it will produce unexpected result if used.\n"); // see todo in sort_arg_options.c
				print_err(av[0], 2, c);
				return (t_optres){0, NULL}; // maybe include the c in struct for future usage
			}
			else
			{
				char	*arg;
				if (*(opt + 1) == ':') {
					fprintf(stderr, "WARNING: has_arg functionnality not finish. It may and it will produce unexpected result if used.\n"); // see todo in sort_arg_options.c
					arg = av[optind++];
				}
				else
					arg = NULL;
				return (t_optres){c, arg};
			}
		}
	}
	else
		return (t_optres){-1, NULL};
}

