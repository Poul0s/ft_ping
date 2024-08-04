/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 15:09:51 by psalame           #+#    #+#             */
/*   Updated: 2024/08/04 13:27:18 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H
# include <stdbool.h>
# include <stdio.h>

enum flags {
	FLAG_HELP		=	1 << 0, // -?, --help
	FLAG_VERBOSE	=	1 << 1, // -v, --verbose
	
	// ...
	
	UNKNOWN_FLAG	=	1 << 2
};

int	get_flags(int ac, char **av);

#endif