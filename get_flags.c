/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_flags.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 15:58:52 by psalame           #+#    #+#             */
/*   Updated: 2025/11/06 18:33:42 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"
#include <stdio.h>

void	print_usage(char *ex_name) {
	printf("Try '%s --help' for more information.\n", ex_name);
}

int	get_flags(int ac, char **av, struct ping_opts *options)
{
	struct s_optres	opt;
	int	flags = 0;
	const struct s_option parameters[] = {
		{"verbose", 0, 'v'},
		{"help", 0, '?'},
		{"ttl", 1, 't'},
		{"count", 1, 'c'},
		{0, 0, 0}
	};

	while ((opt = ft_getopt_long(ac, av, "v?c:", parameters)).val != -1)
	{
		switch (opt.val)
		{
			case 'v':
				flags |= FLAG_VERBOSE;
				break;
			case '?':
				return FLAG_HELP;
			case 't':
				if (opt.arg) {
					char *endptr = 0;
					long val = strtol(opt.arg, &endptr, 10);
					if (endptr != NULL && *endptr != '\0') {
						fprintf(stderr, "%s: invalid value: (`%s' near `%s')\n", av[0], opt.arg, endptr);
						exit(1);
					} else if (val < 0 || val >= 256) { // todo end: check si possibilite de overflow dans le ping de inetutils
						fprintf(stderr, "%s: option value too big: `%s'\n", av[0], opt.arg);
						exit(1);
					} else if (val == 0) {
						fprintf(stderr, "%s: option value too small: `%s'\n", av[0], opt.arg);
						exit(1);
					}
					options->ttl = (unsigned char)val;
				} else {
					exit(EX_USAGE);
				}
				flags |= FLAG_TTL;
				break;
			case 'c':
				if (opt.arg) {
					char *endptr = 0;
					unsigned long val = strtoul(opt.arg, &endptr, 10);
					if (endptr != NULL && *endptr != '\0') {
						fprintf(stderr, "%s: invalid value: (`%s' near `%s')\n", av[0], opt.arg, endptr);
						exit(1);
					} else if (val > 0 && (endptr == NULL || *endptr == '\0')) {
						flags |= FLAG_COUNT;
						options->count = val;
					}
				} else
					exit(EX_USAGE);
				break;
			default:
				print_usage(av[0]);
				exit(EX_USAGE);
		}
	}
	return flags;
}
