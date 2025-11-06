/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getopt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 14:28:00 by psalame           #+#    #+#             */
/*   Updated: 2025/11/06 18:10:14 by psalame          ###   ########.fr       */
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
		char * eq_pos = strchr(opt, '=');
		if (eq_pos != NULL)
			*eq_pos = '\0';
		if (strcmp(opt, lopts->name) == 0) {
			if (eq_pos != NULL)
				*eq_pos = '=';
			return lopts;
		}
		if (eq_pos != NULL)
			*eq_pos = '=';
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
		case 4:
			fprintf(stderr, "%s: option '%s' doesn't allow an argument\n", exec_name, (char *)value);
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
	static int	next_arg = 2;
	if (i < ac && j == 1)
		next_arg = sort_next_argument(ac, av, i);

	if (i < ac && is_arg_option(av[i]))
	{
		optind = i + 1;
		if (j == 1 && av[i][1] == '-' && lopts != NULL)
		{
			// todo has_arg with '=value'
			const t_option * opt = find_opt_long(av[i] + 2, lopts);
			i++;
			j = 1;
			if (opt == NULL)
			{
				print_err(av[0], 1, (intptr_t) av[i - 1]);
				return (t_optres){0, NULL};
			}
			else
			{
				char * eq_pos = strchr(av[i - 1] + 2, '=');

				if (eq_pos != NULL && !opt->has_arg)
				{
					*eq_pos = '\0';
					print_err(av[0], 4, (intptr_t) av[i - 1]);
					return (t_optres){0, NULL};
				} 
				else if (opt->has_arg && eq_pos == NULL && av[next_arg] == NULL)
				{
					print_err(av[0], 3, (intptr_t) av[i - 1]);
					return (t_optres){0, NULL}; // maybe include the opt->val in struct for future usage
				}
				else
				{
					char * arg = NULL;
					if (opt->has_arg)
					{
						if (eq_pos != NULL)
							arg = eq_pos + 1;
						else
						{
							move_argument_at_pos(av, next_arg, i);
							optind = i + 1;
							arg = av[i++];
						}
					}
					return (t_optres){opt->val, arg};
				}
			}
		}
		else
		{
			char c = av[i][j++];
			int curr_i = i;
			int curr_j = j;
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
			else if (*(opt + 1) == ':' && !av[curr_i][curr_j] && next_arg >= ac)
			{
				print_err(av[0], 2, c);
				return (t_optres){0, NULL}; // maybe include the c in struct for future usage
			}
			else
			{
				char	*arg;
				if (*(opt + 1) == ':') {
					if (av[curr_i][curr_j]) {
						arg = av[curr_i] + curr_j;
						i = optind;
						j = 1;
					}
					else
					{
						move_argument_at_pos(av, next_arg, curr_i + 1);
						arg = av[curr_i + 1];
						next_arg++;
						optind++;
	
						if (!av[curr_i][curr_j])
						{
							i = optind;
							j = 1;
						}
					}
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

