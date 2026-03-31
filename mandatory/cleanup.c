/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vinicius-moura <vinicius-moura@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 11:41:34 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/31 09:23:30 by vinicius-mo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	destroy_loaded_textures(t_game *game)
{
	int	i;

	i = 0;
	while (i < 4)
	{
		if (game->textures[i].img_ptr)
			mlx_destroy_image(game->mlx_ptr, game->textures[i].img_ptr);
		i++;
	}
	if (game->floor_texture.img_ptr)
		mlx_destroy_image(game->mlx_ptr, game->floor_texture.img_ptr);
	if (game->ceiling_texture.img_ptr)
		mlx_destroy_image(game->mlx_ptr, game->ceiling_texture.img_ptr);
}

void	free_texture_paths(t_game *game)
{
	int	i;

	i = 0;
	while (i < 6)
	{
		if (game->texture_paths[i])
			free(game->texture_paths[i]);
		game->texture_paths[i] = NULL;
		i++;
	}
}

void	cleanup_game(t_game *game)
{
	if (game->mlx_ptr)
		destroy_loaded_textures(game);
	if (game->mlx_ptr && game->img_ptr)
		mlx_destroy_image(game->mlx_ptr, game->img_ptr);
	if (game->mlx_ptr && game->win_ptr)
		mlx_destroy_window(game->mlx_ptr, game->win_ptr);
	if (game->mlx_ptr)
	{
		mlx_destroy_display(game->mlx_ptr);
		free(game->mlx_ptr);
	}
	free_map(game->map);
	game->map = NULL;
	free_texture_paths(game);
}
