#include "cub3d_bonus.h"

// Desenha um pixel na imagem principal
void draw_pixel(t_game *game, int x, int y, int color)
{
    if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT)
    {
        int pixel = (y * game->size_line) + (x * (game->bits_per_pixel / 8));
        game->img_data[pixel] = color & 0xFF;
        game->img_data[pixel + 1] = (color >> 8) & 0xFF;
        game->img_data[pixel + 2] = (color >> 16) & 0xFF;
    }
}

// Calcula a cor de um pixel da coluna (piso, teto ou parede) e retorna o valor RGB
static int sample_column_color(t_game *game, int x, int y, int draw_start, int draw_end,
    t_texture *tex_ptr, int tex_x, int tex_height, double ray_angle, double distance, int hit_vertical)
{
    if (y < draw_start)
        return (get_floor_ceiling_color(game, x, y, 1, ray_angle, distance));
    if (y > draw_end)
        return (get_floor_ceiling_color(game, x, y, 0, ray_angle, distance));

    int wall_h = draw_end - draw_start;
    if (wall_h <= 0) wall_h = 1;
    int tex_y = ((y - draw_start) * tex_height) / wall_h;
    if (tex_y < 0) tex_y = 0;
    if (tex_y >= tex_height) tex_y = tex_height - 1;
    int color = get_texture_color(tex_ptr, tex_x, tex_y);

    double brightness = 1.0 - (distance / 20.0);
    if (brightness < 0.3) brightness = 0.3;
    if (brightness > 1.0) brightness = 1.0;
    if (hit_vertical) brightness *= 0.7;
    int r = ((color >> 16) & 0xFF) * brightness;
    int g = ((color >> 8) & 0xFF) * brightness;
    int b = (color & 0xFF) * brightness;
    return ((r << 16) | (g << 8) | b);
}

// Renderiza uma coluna vertical inteira (paredes + piso/teto)
static void render_column(t_game *game, int x)
{
    double distance = game->rays[x].distance; if (distance <= 0.1) distance = 0.1;
    int line_height = (int)(WINDOW_HEIGHT / distance);
    int draw_start = -line_height / 2 + WINDOW_HEIGHT / 2 - (int)game->player.pitch; if (draw_start < 0) draw_start = 0;
    int draw_end = line_height / 2 + WINDOW_HEIGHT / 2 - (int)game->player.pitch; if (draw_end >= WINDOW_HEIGHT) draw_end = WINDOW_HEIGHT - 1;

    double wx = game->rays[x].wall_x; if (wx < 0) wx += 1.0; if (wx >= 1.0) wx = fmod(wx, 1.0);
    t_texture *tex_ptr = NULL;
    if (game->rays[x].is_door && game->use_door_texture)
        tex_ptr = &game->door_texture;
    else
    {
        int texture_index = 0; double ray_a = game->player.angle - (FOV * DR / 2) + (x * (FOV * DR) / NUM_RAYS);
        if (game->rays[x].hit_vertical) texture_index = (cos(ray_a) > 0) ? 3 : 2; else texture_index = (sin(ray_a) > 0) ? 1 : 0;
        tex_ptr = &game->textures[texture_index];
    }
    if (!tex_ptr) tex_ptr = &game->textures[0];

    int tex_x = (int)(wx * tex_ptr->width); if (tex_x < 0) tex_x = 0; if (tex_x >= tex_ptr->width) tex_x = tex_ptr->width - 1;
    double ray_angle = game->player.angle - (FOV * DR / 2) + (x * (FOV * DR) / NUM_RAYS);
    for (int y = 0; y < WINDOW_HEIGHT; y++)
        draw_pixel(game, x, y, sample_column_color(game, x, y, draw_start, draw_end,
            tex_ptr, tex_x, tex_ptr->height, ray_angle, distance, game->rays[x].hit_vertical));
}

// Renderiza a cena 3D chamando render_column para cada coluna
void render_3d(t_game *game)
{
    memset(game->img_data, 0, WINDOW_WIDTH * WINDOW_HEIGHT * (game->bits_per_pixel / 8));
    for (int x = 0; x < NUM_RAYS; x++)
    {
        render_column(game, x);
    }
}

// Renderiza o frame completo (colisões, raios, 3D e minimapa)
void render_frame(t_game *game)
{
    resolve_player_collision(game);
    cast_rays(game);
    render_3d(game);
    draw_minimap(game);
    mlx_put_image_to_window(game->mlx_ptr, game->win_ptr, game->img_ptr, 0, 0);
}
