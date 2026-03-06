#include "cub3d_bonus.h"

// Carrega um XPM em uma estrutura de textura; retorna 1 em sucesso
static int load_xpm(t_game *game, const char *path, t_texture *tex)
{
    tex->img_ptr = mlx_xpm_file_to_image(game->mlx_ptr, (char *)path,
        &tex->width, &tex->height);
    if (!tex->img_ptr)
        return (0);
    tex->data = mlx_get_data_addr(tex->img_ptr, &tex->bpp,
        &tex->size_line, &tex->endian);
    return (1);
}

// Carrega as quatro texturas básicas de parede; falha em erro
static int load_basic_textures(t_game *game)
{
    for (int i = 0; i < 4; i++)
    {
        if (!game->texture_paths[i])
        {
            printf("Erro: Caminho da textura %d não definido\n", i);
            return (0);
        }
        if (!load_xpm(game, game->texture_paths[i], &game->textures[i]))
        {
            printf("Erro ao carregar textura: %s\n", game->texture_paths[i]);
            return (0);
        }
    }
    return (1);
}

// Tenta carregar piso/teto quando habilitado; retorna 0 em falha
static int load_optional_floor_ceiling(t_game *game)
{
    if (game->use_floor_texture && game->texture_paths[4])
    {
        if (!load_xpm(game, game->texture_paths[4], &game->floor_texture))
            return (0);
    }
    if (game->use_ceiling_texture && game->texture_paths[5])
    {
        if (!load_xpm(game, game->texture_paths[5], &game->ceiling_texture))
            return (0);
    }
    return (1);
}

// Tenta carregar a textura da porta e ajusta a flag use_door_texture
static void try_load_door_texture(t_game *game)
{
    game->use_door_texture = 0;
    if (!game->texture_paths[6])
        return;
    if (load_xpm(game, game->texture_paths[6], &game->door_texture))
    {
        game->use_door_texture = 1;
        printf("Door texture loaded: %s\n", game->texture_paths[6]);
    }
    else
    {
        printf("Aviso: não foi possível carregar door texture: %s\n", game->texture_paths[6]);
        game->use_door_texture = 0;
    }
}

// Retorna a cor do pixel (x,y) na textura passada
int get_texture_color(t_texture *texture, int x, int y)
{
    if (!texture || !texture->data) return (0x000000);
    if (x < 0 || x >= texture->width || y < 0 || y >= texture->height)
        return (0x000000);
    int pixel = (y * texture->size_line) + (x * (texture->bpp / 8));
    int b = texture->data[pixel] & 0xFF;
    int g = (texture->data[pixel + 1] & 0xFF) << 8;
    int r = (texture->data[pixel + 2] & 0xFF) << 16;
    return (r | g | b);
}

// Carrega texturas (paredes, piso, teto e porta opcional)
int load_textures(t_game *game)
{
    printf("Carregando texturas (bonus)...\n");
    if (!load_basic_textures(game))
        return (0);
    if (!load_optional_floor_ceiling(game))
        return (0);
    try_load_door_texture(game);
    return (1);
}
