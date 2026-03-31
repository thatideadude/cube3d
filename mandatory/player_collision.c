/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player_collision.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vinicius-moura <vinicius-moura@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 10:55:37 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/31 09:42:45 by vinicius-mo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	clamp_to_wall_cell(t_game *game, int wx, int wy,
		double closest[2])
{
	closest[0] = game->player.x;
	closest[1] = game->player.y;
	if (closest[0] < (double)wx)
		closest[0] = (double)wx;
	if (closest[0] > (double)(wx + 1))
		closest[0] = (double)(wx + 1);
	if (closest[1] < (double)wy)
		closest[1] = (double)wy;
	if (closest[1] > (double)(wy + 1))
		closest[1] = (double)(wy + 1);
}

static void	fix_zero_distance(t_game *game, int wx, int wy,
		double values[3])
{
	if (values[2] != 0.0)
		return ;
	values[0] = game->player.x - (wx + 0.5);
	values[1] = game->player.y - (wy + 0.5);
	values[2] = values[0] * values[0] + values[1] * values[1];
	if (values[2] == 0.0)
	{
		values[0] = 0.01;
		values[1] = 0.01;
		values[2] = values[0] * values[0] + values[1] * values[1];
	}
}

static void	apply_push_if_needed(t_game *game, double dx,
		double dy, double dist2)
{
	double	r;
	double	dist;
	double	push;

	r = COLLISION_RADIUS;
	dist = sqrt(dist2);
	if (dist >= r)
		return ;
	push = r - dist;
	game->player.x += (dx / dist) * push;
	game->player.y += (dy / dist) * push;
}

static void	push_player_from_cell(t_game *game, int wx, int wy)
{
	double	closest[2];
	double	values[3];

	clamp_to_wall_cell(game, wx, wy, closest);
	values[0] = game->player.x - closest[0];
	values[1] = game->player.y - closest[1];
	values[2] = values[0] * values[0] + values[1] * values[1];
	fix_zero_distance(game, wx, wy, values);
	apply_push_if_needed(game, values[0], values[1], values[2]);
}

// Resolve colisões do jogador empurrando-o para fora de paredes
void	resolve_player_collision(t_game *game)
{
	int	px_cell;
	int	py_cell;
	int	wy;
	int	wx;

	px_cell = (int)floor(game->player.x);
	py_cell = (int)floor(game->player.y);
	wy = py_cell - 1;
	while (wy <= py_cell + 1)
	{
		wx = px_cell - 1;
		while (wx <= px_cell + 1)
		{
			if (is_wall(game, wx, wy))
				push_player_from_cell(game, wx, wy);
			wx++;
		}
		wy++;
	}
}
