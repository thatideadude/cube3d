/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_validate.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vinicius-moura <vinicius-moura@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 10:51:54 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/31 09:12:26 by vinicius-mo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static int	is_invalid_walkable(t_game *game, int x, int y, char cell)
{
	if (cell != '0' && cell != 'N' && cell != 'S' && cell != 'E' && cell != 'W')
		return (0);
	if (y == 0 || y == game->map_height - 1 || x == 0
		|| x >= ft_strlen(game->map[y]) - 1)
		return (1);
	if (y > 0 && (x >= ft_strlen(game->map[y - 1])
			|| game->map[y - 1][x] == ' '))
		return (1);
	if (y < game->map_height - 1 && (x >= ft_strlen(game->map[y + 1])
			|| game->map[y + 1][x] == ' '))
		return (1);
	return (0);
}

// Valida a coerência do mapa (bordas e espaços adjacentes)
int	validate_map(t_game *game)
{
	int		y;
	int		x;
	char	cell;

	y = 0;
	while (y < game->map_height)
	{
		x = 0;
		while (x < ft_strlen(game->map[y]))
		{
			cell = game->map[y][x];
			if (is_invalid_walkable(game, x, y, cell))
				return (0);
			x++;
		}
		y++;
	}
	return (1);
}
