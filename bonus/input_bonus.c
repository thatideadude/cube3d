#include "cub3d_bonus.h"

// Trata evento de pressionar tecla no modo bonus
int key_press(int keycode, t_game *game)
{
    if (keycode == 65307) { mlx_destroy_window(game->mlx_ptr, game->win_ptr); exit(0); }
    else if (keycode == 119) game->keys.w = 1;
    else if (keycode == 115) game->keys.s = 1;
    else if (keycode == 97) game->keys.a = 1;
    else if (keycode == 100) game->keys.d = 1;
    else if (keycode == 65505 || keycode == 65506 || keycode == 301) game->keys.shift = 1;
    else if (keycode == 111) { open_door_in_front(game); }
    return (0);
}

// Trata evento de soltar tecla no modo bonus
int key_release(int keycode, t_game *game)
{
    if (keycode == 119) game->keys.w = 0;
    else if (keycode == 115) game->keys.s = 0;
    else if (keycode == 97) game->keys.a = 0;
    else if (keycode == 100) game->keys.d = 0;
    else if (keycode == 65505 || keycode == 65506 || keycode == 301) game->keys.shift = 0;
    return (0);
}

// Mouse move handler para olhar horizontal e vertical com recentro do cursor
int mouse_move(int x, int y, t_game *game)
{
    int cx = WINDOW_WIDTH / 2;
    int cy = WINDOW_HEIGHT / 2;
    int dx = x - cx; int dy = y - cy;
    double sens = MOUSE_SENS; double vsens = MOUSE_VSENS;
    if (dx != 0)
    {
        game->player.angle += dx * sens;
        if (game->player.angle < 0) game->player.angle += 2 * PI;
        if (game->player.angle >= 2 * PI) game->player.angle -= 2 * PI;
        game->player.dx = cos(game->player.angle);
        game->player.dy = sin(game->player.angle);
    }
    if (dy != 0)
    {
        game->player.pitch += dy * vsens;
        double limit = WINDOW_HEIGHT / 2 - 10;
        if (game->player.pitch > limit) game->player.pitch = limit;
        if (game->player.pitch < -limit) game->player.pitch = -limit;
    }
    mlx_mouse_move(game->mlx_ptr, game->win_ptr, cx, cy);
    return (0);
}
