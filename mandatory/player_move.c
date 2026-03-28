/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player_move.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmoura-d <vmoura-d@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 10:55:37 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/28 11:00:25 by vmoura-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_mandatory.h"
#include <sys/time.h>

static void	update_player_direction(t_game *game, double rot_speed)
{
	if (game->keys.left_arrow)
		game->player.angle -= rot_speed;
	if (game->keys.right_arrow)
		game->player.angle += rot_speed;
	if (game->player.angle < 0)
		game->player.angle += 2 * PI;
	if (game->player.angle >= 2 * PI)
		game->player.angle -= 2 * PI;
	game->player.dx = cos(game->player.angle);
	game->player.dy = sin(game->player.angle);
}

static int	can_move(t_game *game, double x, double y)
{
	double	r;
	int		ix;
	int		iy;

	r = 0.2;
	ix = (int)floor(x - r);
	iy = (int)floor(y - r);
	if (is_wall(game, ix, iy))
		return (0);
	ix = (int)floor(x + r);
	iy = (int)floor(y - r);
	if (is_wall(game, ix, iy))
		return (0);
	ix = (int)floor(x - r);
	iy = (int)floor(y + r);
	if (is_wall(game, ix, iy))
		return (0);
	ix = (int)floor(x + r);
	iy = (int)floor(y + r);
	if (is_wall(game, ix, iy))
		return (0);
	return (1);
}

static void	apply_move(t_game *game, double sx, double sy)
{
	double	nx;
	double	ny;

	nx = game->player.x + sx;
	ny = game->player.y + sy;
	if (can_move(game, nx, game->player.y))
		game->player.x = nx;
	if (can_move(game, game->player.x, ny))
		game->player.y = ny;
}

static void	apply_key_moves(t_game *game, double move_speed)
{
	if (game->keys.w)
		apply_move(game, game->player.dx * move_speed,
			game->player.dy * move_speed);
	if (game->keys.s)
		apply_move(game, -game->player.dx * move_speed,
			-game->player.dy * move_speed);
	if (game->keys.a)
		apply_move(game, game->player.dy * move_speed,
			-game->player.dx * move_speed);
	if (game->keys.d)
		apply_move(game, -game->player.dy * move_speed,
			game->player.dx * move_speed);
}

// Move o jogador com base nas teclas pressionadas (inclui sprint e rotação)
void	move_player(t_game *game)
{
	static long		last_us;
	struct timeval	tv;
	long			now_us;
	double			speed[4];

	gettimeofday(&tv, NULL);
	now_us = (tv.tv_sec * 1000000L) + tv.tv_usec;
	if (last_us == 0)
		last_us = now_us;
	speed[3] = (now_us - last_us) / 1000000.0;
	if (speed[3] > 0.05)
		speed[3] = 0.05;
	last_us = now_us;
	speed[0] = 3.5 * speed[3];
	speed[1] = 2.2 * speed[3];
	speed[2] = 1.0;
	if (game->keys.shift)
		speed[2] = SPRINT_MULT;
	speed[0] *= speed[2];
	speed[1] *= speed[2];
	if (game->keys.left_arrow || game->keys.right_arrow)
		update_player_direction(game, speed[1]);
	apply_key_moves(game, speed[0]);
	resolve_player_collision(game);
}
