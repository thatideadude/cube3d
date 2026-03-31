/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floor.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vinicius-moura <vinicius-moura@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 10:39:30 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/31 09:11:45 by vinicius-mo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// Retorna a cor do pixel da textura nas coordenadas mundiais (amostragem UV)
static int	sample_texture(t_texture *tex, double world_x, double world_y)
{
	double	fx;
	double	fy;
	int		tex_x;
	int		tex_y;

	fx = world_x - floor(world_x);
	fy = world_y - floor(world_y);
	fx = fmod(fx + 1.0, 1.0);
	fy = fmod(fy + 1.0, 1.0);
	tex_x = (int)(fx * tex->width);
	tex_y = (int)(fy * tex->height);
	if (tex_x < 0)
		tex_x = 0;
	if (tex_x >= tex->width)
		tex_x = tex->width - 1;
	if (tex_y < 0)
		tex_y = 0;
	if (tex_y >= tex->height)
		tex_y = tex->height - 1;
	return (get_texture_color(tex, tex_x, tex_y));
}

// Calcula amostra para piso/teto com base na direção do raio
static int	get_surface_sample(t_game *game, int screen_y,
		double ray_angle, int is_ceiling)
{
	double	horizon;
	double	rel_y;
	double	row_dist;
	double	world_x;
	double	world_y;

	horizon = (WINDOW_HEIGHT / 2.0) - game->player.pitch;
	rel_y = (double)screen_y - horizon;
	if ((is_ceiling && rel_y >= 0) || (!is_ceiling && rel_y <= 0))
		return (0x000000);
	row_dist = (0.5 * ((WINDOW_WIDTH / 2.0) / tan((FOV * DR) / 2.0)))
		/ fabs(rel_y);
	world_x = game->player.x + cos(ray_angle) * row_dist;
	world_y = game->player.y + sin(ray_angle) * row_dist;
	if (is_ceiling && game->use_ceiling_texture)
		return (sample_texture(&game->ceiling_texture, world_x, world_y));
	if (!is_ceiling && game->use_floor_texture)
		return (sample_texture(&game->floor_texture, world_x, world_y));
	if (is_ceiling)
		return ((game->ceiling_color.r << 16)
			| (game->ceiling_color.g << 8) | game->ceiling_color.b);
	return ((game->floor_color.r << 16)
		| (game->floor_color.g << 8) | game->floor_color.b);
}

// Calcula e retorna a cor do teto para o pixel de tela fornecido;
// amostra textura de teto quando ativada
int	get_ceiling_sample(t_game *game, int screen_x, int screen_y,
		double ray_angle)
{
	(void)screen_x;
	return (get_surface_sample(game, screen_y, ray_angle, 1));
}

// Calcula e retorna a cor do piso para o pixel de tela fornecido;
// amostra textura de piso quando ativada
int	get_floor_sample(t_game *game, int screen_x, int screen_y, double ray_angle)
{
	(void)screen_x;
	return (get_surface_sample(game, screen_y, ray_angle, 0));
}

// Retorna a cor do piso ou do teto com base em is_ceiling;
// escolhe entre amostra de piso e de teto
int	get_floor_ceiling_color(t_game *game, int screen_y,
		int is_ceiling, double ray_angle)
{
	if (is_ceiling)
		return (get_ceiling_sample(game, 0, screen_y, ray_angle));
	return (get_floor_sample(game, 0, screen_y, ray_angle));
}
