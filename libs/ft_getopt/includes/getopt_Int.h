/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getopt_Int.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 14:35:28 by psalame           #+#    #+#             */
/*   Updated: 2025/11/05 21:35:04 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_GETOPT_INT_H
# define FT_GETOPT_INT_H
# include <stdbool.h>
# define NULL (void *) 0

bool	is_arg_option(char * arg);
void	move_argument_at_pos(char ** av, int src_i, int dest_i);
int		sort_next_argument(int ac, char ** av, int cursor);

#endif