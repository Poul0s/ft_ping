/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_flags.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 15:58:52 by psalame           #+#    #+#             */
/*   Updated: 2024/08/01 17:59:55 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"
#include <string.h>

static int	get_flag(char *flagArg, bool charFlag)
{
	int	res = 0;
	if (charFlag)
	{
		char	flags[] = {'?', 'v', 0};
		while (flags[res])
			if (flags[res] == *flagArg)
				break;
			else
				res++;
	}
	else
	{
		char	*flags[] = {"help", "verbose", NULL};
		while (flags[res])
			if (strcmp(flags[res], flagArg) == 0)
				break;
			else
				res++;

	}
	res = 1 << res;
	if (res == UNKNOWN_FLAG)
	{
		if (charFlag)
			dprintf(2, "%s: invalid option -- '%c'\n", "ft_ping", *flagArg); // todo set exec name
		else
			dprintf(2, "%s: unrecognized option '--%s'\n", "ft_ping", flagArg); // todo set exec name
	}
	return (res);
}

static inline bool	unique_flag(int flags)
{
	return (flags & (UNKNOWN_FLAG | FLAG_HELP));
}

int	get_flags(char ***arguments)
{
	int	flags = 0;

	while (**arguments && ***arguments == '-' && (**arguments)[1] && !unique_flag(flags))
	{
		char	*flagArg = *((*arguments)++) + 1;
		if (*flagArg == '-')
		{
			if (*++flagArg)
				flags |= get_flag(flagArg, false);
		}
		else
		{
			while (*flagArg && !unique_flag(flags))
				flags |= get_flag(flagArg++, true);
		}
	}
	return flags;
}
