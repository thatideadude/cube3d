#include "cub3d_mandatory.h"

// Desenha um pixel na imagem principal com a cor dada
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

// Lança todos os raios do jogador para preencher game->rays
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

// Renderiza a cena 3D usando os raios já calculados
void render_3d(t_game *game)
{
    memset(game->img_data, 0, WINDOW_WIDTH * WINDOW_HEIGHT * (game->bits_per_pixel / 8));

    for (int x = 0; x < NUM_RAYS; x++)
    {
        double distance = game->rays[x].distance;
        if (distance <= 0.1)
            distance = 0.1;
        int line_height = (int)(WINDOW_HEIGHT / distance);
        int draw_start = -line_height / 2 + WINDOW_HEIGHT / 2;
        if (draw_start < 0)
            draw_start = 0;
        int draw_end = line_height / 2 + WINDOW_HEIGHT / 2;
        if (draw_end >= WINDOW_HEIGHT)
            draw_end = WINDOW_HEIGHT - 1;

        int texture_index = 0;
        int tex_x = 0;

        compute_wall_tex_coords(game, x, &texture_index, &tex_x);
        render_column(game, x, draw_start, draw_end, texture_index, tex_x, distance);
    }
}

// Renderiza uma coluna vertical (paredes + piso/teto)
void render_column(t_game *game,
                   int x,
                   int draw_start,
                   int draw_end,
                   int texture_index,
                   int tex_x,
                   double distance)
{
    for (int y = 0; y < WINDOW_HEIGHT; y++)
    {
        int color;

        if (y < draw_start)
            color = get_floor_ceiling_color(game, x, y, 1, game->player.angle - (FOV * DR / 2) + (x * (FOV * DR) / NUM_RAYS), distance);
        else if (y >= draw_start && y <= draw_end)
            color = sample_wall_color(game, y, draw_start, draw_end, tex_x, texture_index);
        else
            color = get_floor_ceiling_color(game, x, y, 0, game->player.angle - (FOV * DR / 2) + (x * (FOV * DR) / NUM_RAYS), distance);

        draw_pixel(game, x, y, color);
    }
}

// Função chamada a cada frame para atualizar colisões, lançar raios e desenhar
void render_frame(t_game *game)
{
    resolve_player_collision(game);
    cast_rays(game);
    render_3d(game);
    mlx_put_image_to_window(game->mlx_ptr, game->win_ptr, game->img_ptr, 0, 0);
}
