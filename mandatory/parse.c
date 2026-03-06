#include "cub3d_mandatory.h"

// Converte string de cor 'R,G,B' para t_color
int parse_color(char *color_str, t_color *color)
{
    char *token;
    char *str_copy = strdup(color_str);

    while (*str_copy == ' ')
        str_copy++;
    token = strtok(str_copy, ",");
    if (!token)
        return (0);
    color->r = atoi(token);
    if (color->r < 0 || color->r > 255)
        return (0);
    token = strtok(NULL, ",");
    if (!token)
        return (0);
    color->g = atoi(token);
    if (color->g < 0 || color->g > 255)
        return (0);
    token = strtok(NULL, ",");
    if (!token)
        return (0);
    color->b = atoi(token);
    if (color->b < 0 || color->b > 255)
        return (0);
    free(str_copy);
    return (1);
}

// Liberta o array de strings que representa o mapa
void free_map(char **map)
{
    int i;

    i = 0;
    if (!map)
        return;
    while (map[i])
    {
        free(map[i]);
        i++;
    }
    free(map);
}

// Valida a coerência do mapa (bordas e espaços adjacentes)
int validate_map(t_game *game)
{
    for (int y = 0; y < game->map_height; y++)
    {
        for (int x = 0; x < (int)strlen(game->map[y]); x++)
        {
            char cell;

            cell = game->map[y][x];
            if (cell == '0' || cell == 'N' || cell == 'S' || cell == 'E' || cell == 'W')
            {
                if (y == 0 || y == game->map_height - 1 || x == 0 || x >= (int)strlen(game->map[y]) - 1)
                    return (0);
                if (y > 0 && (game->map[y - 1][x] == ' ' || x >= (int)strlen(game->map[y - 1])))
                    return (0);
                if (y < game->map_height - 1 && (game->map[y + 1][x] == ' ' || x >= (int)strlen(game->map[y + 1])))
                    return (0);
            }
        }
    }
    return (1);
}

// Verifica se as coordenadas (x,y) referem-se a uma parede
int is_wall(t_game *game, int x, int y)
{
    if (x < 0 || x >= game->map_width || y < 0 || y >= game->map_height)
        return (1);
    if (y >= game->map_height || x >= (int)strlen(game->map[y]))
        return (1);
    return (game->map[y][x] == '1');
}

// Parseia o ficheiro .cub montando as texturas e o mapa em memória
int parse_map_file(char *filename, t_game *game)
{
    char temp_map[50][1024];
    int map_lines;

    for (int i = 0; i < 6; i++)
        game->texture_paths[i] = NULL;
    game->use_floor_texture = 0;
    game->use_ceiling_texture = 0;
    if (!parse_file_to_temp_map(filename, temp_map, &map_lines, game))
        return (0);
    if (!finalize_map(temp_map, map_lines, game))
        return (0);
    if (!validate_map(game))
    {
        free_map(game->map);
        return (0);
    }
    return (1);
}
