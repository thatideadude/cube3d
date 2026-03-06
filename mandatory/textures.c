#include "cub3d_mandatory.h"

// Carrega as 4 texturas de parede (NO, SO, WE, EA)
int load_wall_textures(t_game *game)
{
    for (int i = 0; i < 4; i++)
    {
        if (!game->texture_paths[i])
            return (0);
        game->textures[i].img_ptr = mlx_xpm_file_to_image(game->mlx_ptr,
                                                         game->texture_paths[i],
                                                         &game->textures[i].width,
                                                         &game->textures[i].height);
        if (!game->textures[i].img_ptr)
            return (0);
        game->textures[i].data = mlx_get_data_addr(game->textures[i].img_ptr,
                                                   &game->textures[i].bpp,
                                                   &game->textures[i].size_line,
                                                   &game->textures[i].endian);
    }
    return (1);
}

// Carrega a textura de piso, se especificada
int load_floor_texture(t_game *game)
{
    if (game->use_floor_texture && game->texture_paths[4])
    {
        game->floor_texture.img_ptr = mlx_xpm_file_to_image(game->mlx_ptr,
                                                            game->texture_paths[4],
                                                            &game->floor_texture.width,
                                                            &game->floor_texture.height);
        if (!game->floor_texture.img_ptr)
            return (0);
        game->floor_texture.data = mlx_get_data_addr(game->floor_texture.img_ptr,
                                                     &game->floor_texture.bpp,
                                                     &game->floor_texture.size_line,
                                                     &game->floor_texture.endian);
    }
    return (1);
}

// Carrega a textura de teto, se especificada
int load_ceiling_texture(t_game *game)
{
    if (game->use_ceiling_texture && game->texture_paths[5])
    {
        game->ceiling_texture.img_ptr = mlx_xpm_file_to_image(game->mlx_ptr,
                                                              game->texture_paths[5],
                                                              &game->ceiling_texture.width,
                                                              &game->ceiling_texture.height);
        if (!game->ceiling_texture.img_ptr)
            return (0);
        game->ceiling_texture.data = mlx_get_data_addr(game->ceiling_texture.img_ptr,
                                                       &game->ceiling_texture.bpp,
                                                       &game->ceiling_texture.size_line,
                                                       &game->ceiling_texture.endian);
    }
    return (1);
}

// Carrega todas as texturas necessárias
int load_textures(t_game *game)
{
    if (!load_wall_textures(game))
        return (0);
    if (!load_floor_texture(game))
        return (0);
    if (!load_ceiling_texture(game))
        return (0);
    return (1);
}

// Retorna a cor do pixel (x,y) na textura passada
int get_texture_color(t_texture *texture, int x, int y)
{
    if (x < 0 || x >= texture->width || y < 0 || y >= texture->height)
        return (0x000000);
    int pixel = (y * texture->size_line) + (x * (texture->bpp / 8));
    int b = texture->data[pixel] & 0xFF;
    int g = (texture->data[pixel + 1] & 0xFF) << 8;
    int r = (texture->data[pixel + 2] & 0xFF) << 16;
    return (r | g | b);
}

