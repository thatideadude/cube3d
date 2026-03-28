/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmoura-d <vmoura-d@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 10:37:56 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/28 11:46:40 by vmoura-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_mandatory.h"

// Trata tecla pressionada e define flags em game->keys
int	key_press(int keycode, t_game *game)
{
	if (keycode == 65307)
		close_hook(game);
	else if (keycode == 119)
		game->keys.w = 1;
	else if (keycode == 115)
		game->keys.s = 1;
	else if (keycode == 97)
		game->keys.a = 1;
	else if (keycode == 100)
		game->keys.d = 1;
	else if (keycode == 65505 || keycode == 65506 || keycode == 301)
		game->keys.shift = 1;
	else if (keycode == 65361)
		game->keys.left_arrow = 1;
	else if (keycode == 65363)
		game->keys.right_arrow = 1;
	return (0);
}

// Trata tecla liberada e limpa flags em game->keys
int	key_release(int keycode, t_game *game)
{
	if (keycode == 119)
		game->keys.w = 0;
	else if (keycode == 115)
		game->keys.s = 0;
	else if (keycode == 97)
		game->keys.a = 0;
	else if (keycode == 100)
		game->keys.d = 0;
	else if (keycode == 65505 || keycode == 65506 || keycode == 301)
		game->keys.shift = 0;
	else if (keycode == 65361)
		game->keys.left_arrow = 0;
	else if (keycode == 65363)
		game->keys.right_arrow = 0;
	return (0);
}

// Atualiza o estado do jogo a cada frame do loop MLX
int	update_game(t_game *game)
{
	move_player(game);
	render_frame(game);
	return (0);
}
