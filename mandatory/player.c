/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmoura-d <vmoura-d@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 10:55:37 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/28 11:55:41 by vmoura-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_mandatory.h"

static void	set_player_angle(t_game *game, char dir)
{
	if (dir == 'N')
		game->player.angle = 3 * PI / 2;
	else if (dir == 'S')
		game->player.angle = PI / 2;
	else if (dir == 'E')
		game->player.angle = 0;
	else if (dir == 'W')
		game->player.angle = PI;
}

static int	set_spawn_if_found(t_game *game, int x, int y)
{
	char	cell;

	cell = game->map[y][x];
	if (cell != 'N' && cell != 'S' && cell != 'E' && cell != 'W')
		return (0);
	game->player.x = x + 0.5;
	game->player.y = y + 0.5;
	set_player_angle(game, cell);
	game->player.dx = cos(game->player.angle);
	game->player.dy = sin(game->player.angle);
	game->map[y][x] = '0';
	return (1);
}

// Inicializa a posição e direção do jogador a partir do mapa
void	init_player(t_game *game)
{
	int	y;
	int	x;

	y = 0;
	while (y < game->map_height)
	{
		x = 0;
		while (x < ft_strlen(game->map[y]))
		{
			if (set_spawn_if_found(game, x, y))
				return ;
			x++;
		}
		y++;
	}
	game->player.x = 1.5;
	game->player.y = 1.5;
	game->player.angle = 0;
	game->player.dx = cos(game->player.angle);
	game->player.dy = sin(game->player.angle);
}
