#include "cub3d_bonus.h"

// Atualiza o estado das portas (fecha atrás do jogador quando apropriado)
void update_doors(t_game *game)
{
    for (int i = 0; i < game->door_count; i++)
    {
        if (game->doors[i].active)
        {
            double cx = (double)game->doors[i].x + 0.5;
            double cy = (double)game->doors[i].y + 0.5;
            double vx = game->player.x - cx;
            double vy = game->player.y - cy;
            double dot = vx * (double)game->doors[i].ox + vy * (double)game->doors[i].oy;
            double dist2 = vx * vx + vy * vy;
            if (dot > 0.25 && dist2 > 0.36)
            {
                game->doors[i].active = 0;
            }
        }
    }
}

// Prepara o minimapa: calcula dimensões e desenha fundo + células
static void prepare_minimap(t_game *game, int *bx, int *by, int *box_w, int *box_h, int *px, int *py, double *scale)
{
    double shrink = 0.55;
    int max_w = (int)(MINIMAP_MAX_W * shrink), max_h = (int)(MINIMAP_MAX_H * shrink);
    int map_w = game->map_width, map_h = game->map_height;
    if (map_w <= 0 || map_h <= 0)
        return;
    double sx = (double)max_w / (double)map_w, sy = (double)max_h / (double)map_h;
    *scale = (sx < sy) ? sx : sy; if (*scale < 1.0) *scale = 1.0;
    int draw_w = (int)(map_w * *scale), draw_h = (int)(map_h * *scale);
    int dest_x = MINIMAP_PADDING, dest_y = MINIMAP_PADDING;
    *box_w = draw_w + 6; *box_h = draw_h + 6;
    *bx = dest_x - 3; *by = dest_y - 3;
    int bg_color = 0x0F1418, inner_shadow = 0x0A0C0E;
    minimap_draw_background(game, *bx, *by, *box_w, *box_h, bg_color, inner_shadow);
    minimap_draw_cells(game, dest_x, dest_y, *scale, draw_w, draw_h);
    *px = dest_x + (int)(game->player.x * *scale);
    *py = dest_y + (int)(game->player.y * *scale);
}

// Wrapper curto para desenhar o minimapa; delega cálculos para helper
void draw_minimap(t_game *game)
{
    int bx, by, box_w, box_h, px, py;
    double scale;
    prepare_minimap(game, &bx, &by, &box_w, &box_h, &px, &py, &scale);
    if (game->map_width <= 0 || game->map_height <= 0)
        return;
    minimap_draw_direction_and_tip(game, game->player.angle, px, py, box_w - 6, box_h - 6, scale);
    minimap_draw_marker_and_border(game, bx, by, box_w, box_h, px, py, scale);
}
