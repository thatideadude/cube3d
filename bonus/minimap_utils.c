#include "cub3d_bonus.h"

// Desenha um pixel no buffer do minimapa (usa o mesmo buffer da imagem principal)
void minimap_put_pixel(t_game *game, int x, int y, int color)
{
    if (x < 0 || y < 0 || x >= WINDOW_WIDTH || y >= WINDOW_HEIGHT) return;
    int bpp = game->bits_per_pixel / 8;
    int i = (y * game->size_line) + (x * bpp);
    game->img_data[i + 0] = color & 0xFF;
    game->img_data[i + 1] = (color >> 8) & 0xFF;
    game->img_data[i + 2] = (color >> 16) & 0xFF;
}

// Desenha uma linha simples Bresenham curta para o minimapa
void minimap_draw_line(t_game *game, int x0, int y0, int x1, int y1, int color)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    while (1)
    {
        minimap_put_pixel(game, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// Preenche um círculo simples no minimapa
void minimap_fill_circle(t_game *game, int cx, int cy, int r, int color)
{
    for (int y = -r; y <= r; y++)
    {
        int dx = (int)sqrt((double)(r * r - y * y));
        for (int x = -dx; x <= dx; x++)
            minimap_put_pixel(game, cx + x, cy + y, color);
    }
}

// Desenha as células do mapa no minimapa (cada célula como um quadrado)
void minimap_draw_cells(t_game *game, int dest_x, int dest_y, double scale, int draw_w, int draw_h)
{
    (void)draw_w; (void)draw_h;
    int cell_w = (int)scale, cell_h = (int)scale;
    for (int my = 0; my < game->map_height; my++)
    {
        for (int mx = 0; mx < game->map_width; mx++)
        {
            int screen_x = dest_x + mx * cell_w;
            int screen_y = dest_y + my * cell_h;
            int color = (game->map[my][mx] == '1') ? 0xCCCCCC : 0x222222;
            for (int yy = 0; yy < cell_h; yy++)
                for (int xx = 0; xx < cell_w; xx++)
                    minimap_put_pixel(game, screen_x + xx, screen_y + yy, color);
        }
    }
}
