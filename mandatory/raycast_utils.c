/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vinicius-moura <vinicius-moura@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 11:03:26 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/31 09:12:53 by vinicius-mo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// Calcula índice de textura e coordenada X na textura para a coluna x
void	compute_wall_tex_coords(t_game *game, int x, int *texture_index,
		int *tex_x)
{
	double	ray_angle;
	double	wx;

	ray_angle = game->player.angle - (FOV * DR / 2) + (x * (FOV * DR)
			/ NUM_RAYS);
	if (game->rays[x].hit_vertical && cos(ray_angle) > 0)
		*texture_index = 3;
	else if (game->rays[x].hit_vertical)
		*texture_index = 2;
	else if (sin(ray_angle) > 0)
		*texture_index = 1;
	else
		*texture_index = 0;
	wx = game->rays[x].wall_x;
	if (wx < 0)
		wx += 1.0;
	if (wx >= 1.0)
		wx = fmod(wx, 1.0);
	*tex_x = (int)(wx * game->textures[*texture_index].width);
	if (*tex_x < 0)
		*tex_x = 0;
	if (*tex_x >= game->textures[*texture_index].width)
		*tex_x = game->textures[*texture_index].width - 1;
}

// Amostra a cor do pixel de parede vertical com mapeamento de textura
int	sample_wall_color(t_game *game, int y, int wall[6])
{
	int	tex_h;
	int	wall_h;
	int	tex_y;

	tex_h = game->textures[wall[2]].height;
	wall_h = wall[4];
	if (wall_h <= 0)
		wall_h = 1;
	tex_y = ((y - wall[5]) * tex_h) / wall_h;
	if (tex_y < 0)
		tex_y = 0;
	if (tex_y >= tex_h)
		tex_y = tex_h - 1;
	return (get_texture_color(&game->textures[wall[2]], wall[3], tex_y));
}

// Define os dados do raio quando ele bate em uma parede
static int	set_ray_hit(t_game *game, int i, double pos[3])
{
	int	mapx;
	int	mapy;

	mapx = (int)floor(pos[0]);
	mapy = (int)floor(pos[2]);
	game->rays[i].map_x = mapx;
	game->rays[i].map_y = mapy;
	if (mapy >= 0 && mapy < game->map_height && mapx >= 0
		&& mapx < ft_strlen(game->map[mapy]))
		game->rays[i].is_door = (game->map[mapy][mapx] == 'D');
	else
		game->rays[i].is_door = 0;
	if ((int)floor(pos[0]) != (int)floor(pos[1]))
	{
		game->rays[i].hit_vertical = 1;
		game->rays[i].wall_x = pos[2] - floor(pos[2]);
	}
	else
	{
		game->rays[i].hit_vertical = 0;
		game->rays[i].wall_x = pos[0] - floor(pos[0]);
	}
	return (1);
}

// Avança o raio em passos até encontrar parede, retorna distância percorrida
static double	step_until_hit(t_game *game, int i, double state[6])
{
	double	pos[3];
	double	prev_x;

	state[5] = 0.0;
	while (state[5] < 30)
	{
		prev_x = state[0];
		state[0] += state[2] * state[4];
		state[1] += state[3] * state[4];
		state[5] += state[4];
		if (is_wall(game, (int)state[0], (int)state[1]))
		{
			pos[0] = state[0];
			pos[1] = prev_x;
			pos[2] = state[1];
			set_ray_hit(game, i, pos);
			break ;
		}
	}
	return (state[5]);
}

// Lança um raio e atualiza game->rays[i] com distância e hit info
double	trace_ray(t_game *game, int i, double ray_angle)
{
	double	state[6];
	double	distance;

	state[0] = game->player.x;
	state[1] = game->player.y;
	state[2] = cos(ray_angle);
	state[3] = sin(ray_angle);
	state[4] = 0.01;
	game->rays[i].hit_vertical = 0;
	game->rays[i].wall_x = 0;
	game->rays[i].distance = 50;
	distance = step_until_hit(game, i, state);
	distance = distance * cos(ray_angle - game->player.angle);
	if (distance < MIN_RAY_DISTANCE)
		distance = MIN_RAY_DISTANCE;
	game->rays[i].distance = distance;
	return (distance);
}
