#include "cub3d_mandatory.h"

// Calcula índice de textura e coordenada X na textura para a coluna x
void compute_wall_tex_coords(t_game *game,
                             int x,
                             int *texture_index,
                             int *tex_x)
{
    double ray_angle = game->player.angle - (FOV * DR / 2) + (x * (FOV * DR) / NUM_RAYS);
    if (game->rays[x].hit_vertical)
    {
        if (cos(ray_angle) > 0)
            *texture_index = 3;
        else
            *texture_index = 2;
    }
    else
    {
        if (sin(ray_angle) > 0)
            *texture_index = 1;
        else
            *texture_index = 0;
    }
    double wx = game->rays[x].wall_x;
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
int sample_wall_color(t_game *game,
                      int y,
                      int draw_start,
                      int draw_end,
                      int tex_x,
                      int texture_index)
{
    int tex_h = game->textures[texture_index].height;
    int wall_h = draw_end - draw_start;
    if (wall_h <= 0)
        wall_h = 1;
    int tex_y = ((y - draw_start) * tex_h) / wall_h;
    if (tex_y < 0)
        tex_y = 0;
    if (tex_y >= tex_h)
        tex_y = tex_h - 1;
    return get_texture_color(&game->textures[texture_index], tex_x, tex_y);
}

// Define os dados do raio quando ele bate em uma parede
static int set_ray_hit(t_game *game, int i, double ray_x, double prev_x, double ray_y)
{
    int mapx = (int)floor(ray_x);
    int mapy = (int)floor(ray_y);

    game->rays[i].map_x = mapx;
    game->rays[i].map_y = mapy;
    if (mapy >= 0 && mapy < game->map_height && mapx >= 0 && mapx < (int)strlen(game->map[mapy]))
        game->rays[i].is_door = (game->map[mapy][mapx] == 'D');
    else
        game->rays[i].is_door = 0;
    if ((int)floor(ray_x) != (int)floor(prev_x))
    {
        game->rays[i].hit_vertical = 1;
        game->rays[i].wall_x = ray_y - floor(ray_y);
    }
    else
    {
        game->rays[i].hit_vertical = 0;
        game->rays[i].wall_x = ray_x - floor(ray_x);
    }
    return (1);
}

// Avança o raio em passos até encontrar parede, retorna distância percorrida
static double step_until_hit(t_game *game,
                            int i,
                            double ray_x,
                            double ray_y,
                            double ray_dx,
                            double ray_dy,
                            double step_size)
{
    double distance = 0.0;

    while (distance < 30)
    {
        double prev_x = ray_x;

        ray_x += ray_dx * step_size;
        ray_y += ray_dy * step_size;
        distance += step_size;

        if (is_wall(game, (int)ray_x, (int)ray_y))
        {
            set_ray_hit(game, i, ray_x, prev_x, ray_y);
            break;
        }
    }
    return (distance);
}

// Lança um raio e atualiza game->rays[i] com distância e hit info
double trace_ray(t_game *game, int i, double ray_angle)
{
    double ray_x = game->player.x;
    double ray_y = game->player.y;
    double ray_dx = cos(ray_angle);
    double ray_dy = sin(ray_angle);
    double step_size = 0.002;

    game->rays[i].hit_vertical = 0;
    game->rays[i].wall_x = 0;
    game->rays[i].distance = 50;

    double distance = step_until_hit(game, i, ray_x, ray_y, ray_dx, ray_dy, step_size);

    distance = distance * cos(ray_angle - game->player.angle);
    if (distance < MIN_RAY_DISTANCE)
        distance = MIN_RAY_DISTANCE;
    game->rays[i].distance = distance;
    return (distance);
}
