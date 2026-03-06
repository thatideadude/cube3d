#include "cub3d_mandatory.h"

// Trata evento de fechar a janela
int	close_hook(t_game *game)
{
	mlx_destroy_window(game->mlx_ptr, game->win_ptr);
	exit(0);
	return (0);
}

// Valida argumentos e faz parse inicial do mapa
static int	validate_and_parse(int argc, char **argv, t_game *game)
{
	int	len;

	if (argc != 2)
	{
		printf("Uso: %s <arquivo.cub>\n", argv[0]);
		return (0);
	}
	len = strlen(argv[1]);
	if (len < 4 || strcmp(argv[1] + len - 4, ".cub") != 0)
	{
		printf("Erro: ext .cub\n");
		return (0);
	}
	memset(game, 0, sizeof(t_game));
	if (!parse_map_file(argv[1], game))
	{
		printf("Erro ao carregar o mapa\n");
		return (0);
	}
	return (1);
}

// Inicializa a janela, imagens e texturas necessárias
static int	setup_graphics_and_resources(t_game *game)
{
	init_player(game);
	game->mlx_ptr = mlx_init();
	if (!game->mlx_ptr)
		return (0);
	game->win_ptr = mlx_new_window(game->mlx_ptr,
		WINDOW_WIDTH, WINDOW_HEIGHT, "Cub3D - Mandatory");
	if (!game->win_ptr)
		return (0);
	game->img_ptr = mlx_new_image(game->mlx_ptr,
		WINDOW_WIDTH, WINDOW_HEIGHT);
	game->img_data = mlx_get_data_addr(game->img_ptr,
		&game->bits_per_pixel, &game->size_line, &game->endian);
	if (!load_textures(game))
		return (0);
	return (1);
}

// Configura hooks do MLX e inicia o loop principal
static void	setup_hooks_and_loop(t_game *game)
{
	render_frame(game);
	mlx_hook(game->win_ptr, 2, 1L << 0, key_press, game);
	mlx_hook(game->win_ptr, 3, 1L << 1, key_release, game);
	mlx_hook(game->win_ptr, 17, 1L << 17, close_hook, game);
	mlx_loop_hook(game->mlx_ptr, update_game, game);
	mlx_loop(game->mlx_ptr);
}

// Ponto de entrada do programa
int	main(int argc, char **argv)
{
	t_game	game;

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
