#include "cub3d_bonus.h"

// Lança todos os raios chamando trace_ray para cada coluna
void cast_rays(t_game *game)
{
    double ray_angle = game->player.angle - (FOV * DR / 2);
    double angle_step = (FOV * DR) / NUM_RAYS;

    for (int i = 0; i < NUM_RAYS; i++)
    {
        trace_ray(game, i, ray_angle);
        ray_angle += angle_step;
    }
}

// Abre porta à frente do jogador se houver uma dentro do alcance
int open_door_in_front(t_game *game)
{
    double look_dist = 0.8;
    int fx = (int)floor(game->player.x + game->player.dx * look_dist);
    int fy = (int)floor(game->player.y + game->player.dy * look_dist);
    if (fx < 0 || fx >= game->map_width || fy < 0 || fy >= game->map_height)
        return 0;
    if (game->map[fy][fx] != 'D') return 0;
    for (int i = 0; i < game->door_count; i++)
    {
        if (game->doors[i].x == fx && game->doors[i].y == fy)
        {
            game->doors[i].active = 1;
            int ox = (int)round(game->player.dx);
            int oy = (int)round(game->player.dy);
            if (ox == 0 && oy == 0)
            {
                if (fabs(game->player.dx) > fabs(game->player.dy)) ox = (game->player.dx > 0) ? 1 : -1; else oy = (game->player.dy > 0) ? 1 : -1;
            }
            game->doors[i].ox = ox; game->doors[i].oy = oy;
            return 1;
        }
    }
    return 0;
}
