/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmoura-d <vmoura-d@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 11:01:02 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/28 11:55:38 by vmoura-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_mandatory.h"

// Desenha um pixel na imagem principal com a cor dada
void	draw_pixel(t_game *game, int x, int y, int color)
{
	int	pixel;

	if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT)
	{
		pixel = (y * game->size_line) + (x * (game->bits_per_pixel / 8));
		game->img_data[pixel] = color & 0xFF;
		game->img_data[pixel + 1] = (color >> 8) & 0xFF;
		game->img_data[pixel + 2] = (color >> 16) & 0xFF;
	}
}

// Lança todos os raios do jogador para preencher game->rays
void	cast_rays(t_game *game)
{
	double	ray_angle;
	double	angle_step;
	int		i;

	ray_angle = game->player.angle - (FOV * DR / 2);
	angle_step = (FOV * DR) / NUM_RAYS;
	i = 0;
	while (i < NUM_RAYS)
	{
		trace_ray(game, i, ray_angle);
		ray_angle += angle_step;
		i++;
	}
}

// Renderiza a cena 3D usando os raios já calculados
void	render_3d(t_game *game)
{
	int	x;

	ft_memset(game->img_data, 0, WINDOW_WIDTH * WINDOW_HEIGHT
		* (game->bits_per_pixel / 8));
	x = 0;
	while (x < NUM_RAYS)
	{
		render_column(game, x);
		x++;
	}
}

// Função chamada a cada frame para atualizar colisões, lançar raios e desenhar
void	render_frame(t_game *game)
{
	resolve_player_collision(game);
	cast_rays(game);
	render_3d(game);
	mlx_put_image_to_window(game->mlx_ptr, game->win_ptr, game->img_ptr, 0, 0);
}
