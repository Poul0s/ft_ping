/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getopt_Int.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 14:35:28 by psalame           #+#    #+#             */
/*   Updated: 2024/08/04 19:37:22 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_GETOPT_INT_H
# define FT_GETOPT_INT_H
# include <stdbool.h>
# define NULL (void *) 0

bool	is_arg_option(char * arg);
void	sort_next_argument(int ac, char ** av, int cursor);

#endif