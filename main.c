/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 15:10:14 by psalame           #+#    #+#             */
/*   Updated: 2025/11/05 14:59:57 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

int	main(int ac, char **arguments)
{
	int		flags = get_flags(ac, arguments);
	printf("flags: %d\ncurrent argument (%d): %s\n", flags, optind, arguments[optind]);

	return (0);
}