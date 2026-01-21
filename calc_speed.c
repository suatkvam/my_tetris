/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   calc_speed.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akivam <akivam@student.42istanbul.com.tr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 20:37:46 by akivam            #+#    #+#             */
/*   Updated: 2026/01/18 20:37:53 by akivam           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tetris.h"

int	calc_speed(int level, t_settings *settings)
{
	int	base;
	int	speed_level;

	speed_level = (level - 1) / settings->speed_level_step;
	base = settings->speed_base - speed_level;
	if (base < settings->speed_min)
		base = settings->speed_min;
	return (base);
}
