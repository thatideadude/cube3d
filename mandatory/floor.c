#include "cub3d_mandatory.h"

// Retorna a cor do pixel da textura nas coordenadas mundiais (amostragem UV)
static int sample_texture(t_texture *tex, double world_x, double world_y)
{
    double fx = world_x - floor(world_x);
    double fy = world_y - floor(world_y);
    fx = fmod(fx + 1.0, 1.0);
    fy = fmod(fy + 1.0, 1.0);
    int tex_x = (int)(fx * tex->width);
    int tex_y = (int)(fy * tex->height);
    if (tex_x < 0)
        tex_x = 0;
    if (tex_x >= tex->width)
        tex_x = tex->width - 1;
    if (tex_y < 0)
        tex_y = 0;
    if (tex_y >= tex->height)
        tex_y = tex->height - 1;
    return get_texture_color(tex, tex_x, tex_y);
}

// Calcula e retorna a cor do teto para o pixel de tela fornecido;
// amostra textura de teto quando ativada
int get_ceiling_sample(t_game *game, int screen_x, int screen_y, double ray_angle)
{
    (void)screen_x;
    double proj_plane = (WINDOW_WIDTH / 2.0) / tan((FOV * DR) / 2.0);
    double horizon = (WINDOW_HEIGHT / 2.0) - game->player.pitch;
    double rel_y = (double)screen_y - horizon;
    if (rel_y >= 0)
        return 0x000000;
    double player_eye = 0.5;
    double ray_dx = cos(ray_angle);
    double ray_dy = sin(ray_angle);
    double row_dist = (player_eye * proj_plane) / (-rel_y);
    double world_x = game->player.x + ray_dx * row_dist;
    double world_y = game->player.y + ray_dy * row_dist;
    if (game->use_ceiling_texture)
        return sample_texture(&game->ceiling_texture, world_x, world_y);
    return (game->ceiling_color.r << 16) | (game->ceiling_color.g << 8) | game->ceiling_color.b;
}

// Calcula e retorna a cor do piso para o pixel de tela fornecido;
// amostra textura de piso quando ativada
int get_floor_sample(t_game *game, int screen_x, int screen_y, double ray_angle)
{
    (void)screen_x;
    double proj_plane = (WINDOW_WIDTH / 2.0) / tan((FOV * DR) / 2.0);
    double horizon = (WINDOW_HEIGHT / 2.0) - game->player.pitch;
    double rel_y = (double)screen_y - horizon;
    if (rel_y <= 0)
        return 0x000000;
    double player_eye = 0.5;
    double ray_dx = cos(ray_angle);
    double ray_dy = sin(ray_angle);
    double row_dist = (player_eye * proj_plane) / rel_y;
    double world_x = game->player.x + ray_dx * row_dist;
    double world_y = game->player.y + ray_dy * row_dist;
    if (game->use_floor_texture)
        return sample_texture(&game->floor_texture, world_x, world_y);
    return (game->floor_color.r << 16) | (game->floor_color.g << 8) | game->floor_color.b;
}

// Retorna a cor do piso ou do teto com base em is_ceiling;
// ignora wall_distance (parâmetro presente para compatibilidade)
int get_floor_ceiling_color(t_game *game,
                            int screen_x,
                            int screen_y,
                            int is_ceiling,
                            double ray_angle,
                            double wall_distance)
{
    (void)wall_distance;
    if (is_ceiling)
        return get_ceiling_sample(game, screen_x, screen_y, ray_angle);
    return get_floor_sample(game, screen_x, screen_y, ray_angle);
}
