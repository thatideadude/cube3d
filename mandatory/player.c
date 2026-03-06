#include "cub3d_mandatory.h"

// Inicializa a posição e direção do jogador a partir do mapa
void init_player(t_game *game)
{
    for (int y = 0; y < game->map_height; y++)
    {
        for (int x = 0; x < (int)strlen(game->map[y]); x++)
        {
            if (game->map[y][x] == 'N' || game->map[y][x] == 'S' || game->map[y][x] == 'E' || game->map[y][x] == 'W')
            {
                game->player.x = x + 0.5; game->player.y = y + 0.5;
                if (game->map[y][x] == 'N') game->player.angle = 3 * PI / 2;
                else if (game->map[y][x] == 'S') game->player.angle = PI / 2;
                else if (game->map[y][x] == 'E') game->player.angle = 0;
                else if (game->map[y][x] == 'W') game->player.angle = PI;
                game->player.dx = cos(game->player.angle); game->player.dy = sin(game->player.angle);
                game->map[y][x] = '0'; return;
            }
        }
    }
    game->player.x = 1.5; game->player.y = 1.5; game->player.angle = 0; game->player.dx = cos(game->player.angle); game->player.dy = sin(game->player.angle);
}

// Verifica se o jogador pode mover-se para as coordenadas (x,y) sem bater
int can_move(t_game *game, double x, double y)
{
    double r = 0.2; int ix, iy;
    ix = (int)floor(x - r); iy = (int)floor(y - r); if (is_wall(game, ix, iy)) return 0;
    ix = (int)floor(x + r); iy = (int)floor(y - r); if (is_wall(game, ix, iy)) return 0;
    ix = (int)floor(x - r); iy = (int)floor(y + r); if (is_wall(game, ix, iy)) return 0;
    ix = (int)floor(x + r); iy = (int)floor(y + r); if (is_wall(game, ix, iy)) return 0;
    return 1;
}

// Resolve colisões do jogador empurrando-o para fora de paredes
void resolve_player_collision(t_game *game)
{
    double r = COLLISION_RADIUS; int px_cell = (int)floor(game->player.x); int py_cell = (int)floor(game->player.y);
    for (int wy = py_cell - 1; wy <= py_cell + 1; wy++)
    {
        for (int wx = px_cell - 1; wx <= px_cell + 1; wx++)
        {
            if (is_wall(game, wx, wy))
            {
                double closest_x = game->player.x; double closest_y = game->player.y;
                if (closest_x < (double)wx) closest_x = (double)wx;
                if (closest_x > (double)(wx + 1)) closest_x = (double)(wx + 1);
                if (closest_y < (double)wy) closest_y = (double)wy;
                if (closest_y > (double)(wy + 1)) closest_y = (double)(wy + 1);
                double dx = game->player.x - closest_x; double dy = game->player.y - closest_y; double dist2 = dx * dx + dy * dy;
                if (dist2 == 0.0) { dx = game->player.x - (wx + 0.5); dy = game->player.y - (wy + 0.5); dist2 = dx * dx + dy * dy; if (dist2 == 0.0) { dx = 0.01; dy = 0.01; dist2 = dx * dx + dy * dy; } }
                double dist = sqrt(dist2);
                if (dist < r) { double push = r - dist; game->player.x += (dx / dist) * push; game->player.y += (dy / dist) * push; }
            }
        }
    }
}

// Move o jogador com base nas teclas pressionadas (inclui sprint e rotação)
void move_player(t_game *game)
{
    double base_speed = 0.08, rot_speed = 0.05;
    double sprint = (game->keys.shift) ? SPRINT_MULT : 1.0;
    double move_speed = base_speed * sprint;
    double nx, ny;
    if (game->keys.left_arrow) { game->player.angle -= rot_speed; if (game->player.angle < 0) game->player.angle += 2 * PI; game->player.dx = cos(game->player.angle); game->player.dy = sin(game->player.angle); }
    if (game->keys.right_arrow) { game->player.angle += rot_speed; if (game->player.angle >= 2 * PI) game->player.angle -= 2 * PI; game->player.dx = cos(game->player.angle); game->player.dy = sin(game->player.angle); }
    if (game->keys.w) { nx = game->player.x + game->player.dx * move_speed; ny = game->player.y + game->player.dy * move_speed; if (can_move(game, nx, game->player.y)) game->player.x = nx; if (can_move(game, game->player.x, ny)) game->player.y = ny; }
    if (game->keys.s) { nx = game->player.x - game->player.dx * move_speed; ny = game->player.y - game->player.dy * move_speed; if (can_move(game, nx, game->player.y)) game->player.x = nx; if (can_move(game, game->player.x, ny)) game->player.y = ny; }
    if (game->keys.a) { nx = game->player.x + game->player.dy * move_speed; ny = game->player.y - game->player.dx * move_speed; if (can_move(game, nx, game->player.y)) game->player.x = nx; if (can_move(game, game->player.x, ny)) game->player.y = ny; }
    if (game->keys.d) { nx = game->player.x - game->player.dy * move_speed; ny = game->player.y + game->player.dx * move_speed; if (can_move(game, nx, game->player.y)) game->player.x = nx; if (can_move(game, game->player.x, ny)) game->player.y = ny; }
    resolve_player_collision(game);
}
