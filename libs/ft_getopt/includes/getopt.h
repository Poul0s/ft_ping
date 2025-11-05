/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getopt.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psalame <psalame@student.42angouleme.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 14:13:16 by psalame           #+#    #+#             */
/*   Updated: 2024/08/04 17:37:50 by psalame          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_GETOPT_H
# define FT_GETOPT_H 1

extern int optind;

typedef struct s_option
{
	char	*name;
	_Bool	has_arg;
	int		val;
}	t_option;

typedef struct s_optres
{
	int		val;
	char	*arg;
}	t_optres;

t_optres	ft_getopt(int argc, char ** argv, const char * options);
t_optres	ft_getopt_long(int argc, char ** argv, const char * options, const struct s_option * long_options);

#endif