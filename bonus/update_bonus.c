#include "cub3d_bonus.h"

// Atualização por frame chamada pelo loop (move, portas, render)
int update_game(t_game *game)
{
    move_player(game);
    update_doors(game);
    render_frame(game);
    return (0);
}
