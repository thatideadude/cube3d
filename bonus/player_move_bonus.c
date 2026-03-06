#include "cub3d_bonus.h"

// Move o jogador com base nas teclas pressionadas (WASD + sprint + setas)
void move_player(t_game *game)
{
    double base_speed = 0.08, rot_speed = 0.05;
    double sprint = (game->keys.shift) ? SPRINT_MULT : 1.0;
    double move_speed = base_speed * sprint;
    double nx, ny;
    if (game->keys.left_arrow)
    {
        game->player.angle -= rot_speed;
        if (game->player.angle < 0) game->player.angle += 2 * PI;
        game->player.dx = cos(game->player.angle);
        game->player.dy = sin(game->player.angle);
    }
    if (game->keys.right_arrow)
    {
        game->player.angle += rot_speed;
        if (game->player.angle >= 2 * PI) game->player.angle -= 2 * PI;
        game->player.dx = cos(game->player.angle);
        game->player.dy = sin(game->player.angle);
    }
    if (game->keys.w) { nx = game->player.x + game->player.dx * move_speed; ny = game->player.y + game->player.dy * move_speed; if (is_wall(game, (int)floor(nx), (int)floor(game->player.y)) == 0) game->player.x = nx; if (is_wall(game, (int)floor(game->player.x), (int)floor(ny)) == 0) game->player.y = ny; }
    if (game->keys.s) { nx = game->player.x - game->player.dx * move_speed; ny = game->player.y - game->player.dy * move_speed; if (is_wall(game, (int)floor(nx), (int)floor(game->player.y)) == 0) game->player.x = nx; if (is_wall(game, (int)floor(game->player.x), (int)floor(ny)) == 0) game->player.y = ny; }
    if (game->keys.a) { nx = game->player.x + game->player.dy * move_speed; ny = game->player.y - game->player.dx * move_speed; if (is_wall(game, (int)floor(nx), (int)floor(game->player.y)) == 0) game->player.x = nx; if (is_wall(game, (int)floor(game->player.x), (int)floor(ny)) == 0) game->player.y = ny; }
    if (game->keys.d) { nx = game->player.x - game->player.dy * move_speed; ny = game->player.y + game->player.dx * move_speed; if (is_wall(game, (int)floor(nx), (int)floor(game->player.y)) == 0) game->player.x = nx; if (is_wall(game, (int)floor(game->player.x), (int)floor(ny)) == 0) game->player.y = ny; }
    resolve_player_collision(game);
}
