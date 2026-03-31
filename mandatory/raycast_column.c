/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast_column.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vinicius-moura <vinicius-moura@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 11:01:02 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/31 09:12:47 by vinicius-mo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	init_wall_data(t_game *game, int x, int wall[6])
{
	double	distance;
	int		line_height;
	int		raw_start;

	distance = game->rays[x].distance;
	if (distance <= 0.1)
		distance = 0.1;
	line_height = (int)(WINDOW_HEIGHT / distance);
	raw_start = -line_height / 2 + WINDOW_HEIGHT / 2;
	wall[0] = raw_start;
	if (wall[0] < 0)
		wall[0] = 0;
	wall[1] = line_height / 2 + WINDOW_HEIGHT / 2;
	if (wall[1] >= WINDOW_HEIGHT)
		wall[1] = WINDOW_HEIGHT - 1;
	wall[2] = 0;
	wall[3] = 0;
	wall[4] = line_height;
	wall[5] = raw_start;
	compute_wall_tex_coords(game, x, &wall[2], &wall[3]);
}

static int	sample_column_pixel(t_game *game, int y, int wall[6],
		double ray_angle)
{
	if (y < wall[0])
		return (get_floor_ceiling_color(game, y, 1, ray_angle));
	if (y <= wall[1])
		return (sample_wall_color(game, y, wall));
	return (get_floor_ceiling_color(game, y, 0, ray_angle));
}

// Renderiza uma coluna vertical (paredes + piso/teto)
void	render_column(t_game *game, int x)
{
	int		wall[6];
	int		y;
	double	ray_angle;

	ray_angle = game->player.angle - (FOV * DR / 2) + (x * (FOV * DR)
			/ NUM_RAYS);
	init_wall_data(game, x, wall);
	y = 0;
	while (y < WINDOW_HEIGHT)
	{
		draw_pixel(game, x, y, sample_column_pixel(game, y, wall, ray_angle));
		y++;
	}
}
