/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_flags.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 15:58:52 by psalame           #+#    #+#             */
/*   Updated: 2025/11/06 19:23:34 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"
#include <stdio.h>
#include <unistd.h>

void	print_usage(char *ex_name) {
	printf("Try '%s --help' for more information.\n", ex_name);
}

long	parse_long_opt(char *ex_name, char *optarg, unsigned long max, bool allow_zero) {
	char *endptr = 0;
	long val = strtol(optarg, &endptr, 10);
	if (endptr != NULL && *endptr != '\0') {
		fprintf(stderr, "%s: invalid value: (`%s' near `%s')\n", ex_name, optarg, endptr);
		exit(1);
	} else if (val < 0 || (unsigned long)val > max) { // todo end: check si possibilite de overflow dans le ping de inetutils
		fprintf(stderr, "%s: option value too big: `%s'\n", ex_name, optarg);
		exit(1);
	} else if (!allow_zero && val == 0) {
		fprintf(stderr, "%s: option value too small: `%s'\n", ex_name, optarg);
		exit(1);
	}
	return val;
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
		{"flood", 0, 'f'},
		{"interval", 1, 'i'},
		{"size", 1, 's'},
		{0, 0, 0}
	};

	while ((opt = ft_getopt_long(ac, av, "v?fc:i:s:", parameters)).val != -1)
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
					options->ttl = (unsigned char)parse_long_opt(av[0], opt.arg, 255, false);
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

			case 'f':
				if (geteuid() == 0) {
					options->interval_ms = 10; // 10 ms minimum for flood ping
				} else {
					fprintf(stderr, "%s: flood ping requires root privileges\n", av[0]);
					exit(EX_NOPERM);
				}
				flags |= FLAG_FLOOD;
				break;
			case 'i':
				if (opt.arg) {
					char *endptr = 0;
					double interval = strtod(opt.arg, &endptr);
					if (endptr != NULL && *endptr != '\0') {
						fprintf(stderr, "%s: invalid value: (`%s' near `%s')\n", av[0], opt.arg, endptr);
						exit(1);
					} else {
						options->interval_ms = (unsigned long)(interval * 1000);
						if (geteuid() != 0 && options->interval_ms < USER_MIN_INTERVAL_MS) {
							fprintf(stderr, "%s: option value too small: `%s'\n", av[0], opt.arg);
							exit(1);
						}
						flags |= FLAG_INTERVAL;
					}
				} else {
					print_usage(av[0]);
					exit(EX_USAGE);
				}
				break;
			case 's':
				if (opt.arg) {
					options->size = (unsigned char)parse_long_opt(av[0], opt.arg, MAX_PACKET_SIZE, true);
				} else {
					exit(EX_USAGE);
				}
				flags |= FLAG_SIZE;
				break;
			default:
				print_usage(av[0]);
				exit(EX_USAGE);
		}
	}
	return flags;
}
