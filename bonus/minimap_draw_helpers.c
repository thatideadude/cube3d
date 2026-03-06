#include "cub3d_bonus.h"

// Desenha o fundo do minimapa com uma borda externa
void minimap_draw_background(t_game *game, int bx, int by, int bw, int bh, int bg_color, int inner_shadow)
{
    for (int y = by; y < by + bh; y++)
        for (int x = bx; x < bx + bw; x++)
            minimap_put_pixel(game, x, y, bg_color);
    // sombra interna simples
    for (int y = by + 2; y < by + bh - 2; y++)
        for (int x = bx + 2; x < bx + bw - 2; x++)
            minimap_put_pixel(game, x, y, inner_shadow);
}

// Desenha a direção do jogador e uma pequena ponta para indicar frente
void minimap_draw_direction_and_tip(t_game *game, double angle, int px, int py, int map_w, int map_h, double scale)
{
    (void)map_w; (void)map_h;
    int len = (int)(6 * scale);
    int tx = px + (int)(cos(angle) * len);
    int ty = py + (int)(sin(angle) * len);
    minimap_draw_line(game, px, py, tx, ty, 0xFF0000);
    minimap_fill_circle(game, tx, ty, (int)(2 * scale), 0xFFAAAA);
}

// Desenha o marcador do jogador e a borda do minimapa
void minimap_draw_marker_and_border(t_game *game, int bx, int by, int bw, int bh, int px, int py, double scale)
{
    minimap_fill_circle(game, px, py, (int)(3 * scale), 0x00FF00);
    // borda externa
    for (int x = bx; x < bx + bw; x++)
    {
        minimap_put_pixel(game, x, by, 0x000000);
        minimap_put_pixel(game, x, by + bh - 1, 0x000000);
    }
    for (int y = by; y < by + bh; y++)
    {
        minimap_put_pixel(game, bx, y, 0x000000);
        minimap_put_pixel(game, bx + bw - 1, y, 0x000000);
    }
}
