#include "cub3d_bonus.h"

// Fecha a janela e encerra o jogo
int close_hook(t_game *game)
{
    mlx_destroy_window(game->mlx_ptr, game->win_ptr);
    exit(0);
    return (0);
}

// Atualização por frame chamada pelo loop (move, portas, render)
// (movida para update_bonus.c para reduzir número de funções neste arquivo)

// Valida argumentos e faz parse do mapa
static int validate_and_parse(int argc, char **argv, t_game *game)
{
    if (argc != 2)
    {
        printf("Uso: %s <arquivo.cub>\n", argv[0]);
        return (0);
    }
    int len = strlen(argv[1]);
    if (len < 4 || strcmp(argv[1] + len - 4, ".cub") != 0)
    {
        printf("Erro: O arquivo deve ter extensão .cub\n");
        return (0);
    }
    memset(game, 0, sizeof(t_game));
    if (!parse_map_file(argv[1], game))
        return (0);
    return (1);
}

// Inicializa recursos gráficos e texturas
static int setup_graphics_and_resources(t_game *game)
{
    init_player(game);
    game->mlx_ptr = mlx_init();
    if (!game->mlx_ptr)
        return (0);
    game->win_ptr = mlx_new_window(game->mlx_ptr, WINDOW_WIDTH, WINDOW_HEIGHT, "Cub3D - Bonus");
    if (!game->win_ptr)
        return (0);
    game->img_ptr = mlx_new_image(game->mlx_ptr, WINDOW_WIDTH, WINDOW_HEIGHT);
    game->img_data = mlx_get_data_addr(game->img_ptr, &game->bits_per_pixel, &game->size_line, &game->endian);
    if (!load_textures(game))
        return (0);
    return (1);
}

// Configura hooks do MLX e inicia o loop principal
static void setup_hooks_and_loop(t_game *game)
{
    mlx_mouse_hide(game->mlx_ptr, game->win_ptr);
    mlx_mouse_move(game->mlx_ptr, game->win_ptr, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    render_frame(game);
    mlx_hook(game->win_ptr, 2, 1L<<0, key_press, game);
    mlx_hook(game->win_ptr, 3, 1L<<1, key_release, game);
    mlx_hook(game->win_ptr, 6, 1L<<6, mouse_move, game);
    mlx_hook(game->win_ptr, 17, 1L<<17, close_hook, game);
    mlx_loop_hook(game->mlx_ptr, update_game, game);
    mlx_loop(game->mlx_ptr);
}

// Entrada principal para a versão bonus (substitui o monolito original)
int main(int argc, char **argv)
{
    t_game game;

    if (!validate_and_parse(argc, argv, &game))
        return (1);
    if (!setup_graphics_and_resources(&game))
    {
        free_map(game.map);
        return (1);
    }
    setup_hooks_and_loop(&game);
    free_map(game.map);
    return (0);
}
