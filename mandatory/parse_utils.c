#include "cub3d_mandatory.h"

// Trata diretiva de piso 'F' (cor ou textura)
static int handle_floor_directive(char *line, t_game *game)
{
    char *content;

    if (strncmp(line, "F ", 2) != 0)
        return (0);
    content = line + 2;
    while (*content == ' ')
        content++;
    if (strstr(content, ".xpm"))
    {
        game->texture_paths[4] = strdup(content);
        game->use_floor_texture = 1;
    }
    else
    {
        if (!parse_color(content, &game->floor_color))
            return (0);
        game->use_floor_texture = 0;
    }
    return (1);
}

// Trata diretiva de teto 'C' (cor ou textura)
static int handle_ceiling_directive(char *line, t_game *game)
{
    char *content;

    if (strncmp(line, "C ", 2) != 0)
        return (0);
    content = line + 2;
    while (*content == ' ')
        content++;
    if (strstr(content, ".xpm"))
    {
        game->texture_paths[5] = strdup(content);
        game->use_ceiling_texture = 1;
    }
    else
    {
        if (!parse_color(content, &game->ceiling_color))
            return (0);
        game->use_ceiling_texture = 0;
    }
    return (1);
}

// Trata diretivas de textura (NO, SO, WE, EA, F, C)
int handle_texture_directive(char *line, t_game *game)
{
    const char *prefs[4] = {"NO ", "SO ", "WE ", "EA "};

    for (int k = 0; k < 4; k++)
    {
        if (strncmp(line, prefs[k], 3) == 0)
        {
            char *content = line + 3;

            while (*content == ' ')
                content++;
            game->texture_paths[k] = strdup(content);
            return (1);
        }
    }
    if (handle_floor_directive(line, game))
        return (1);
    if (handle_ceiling_directive(line, game))
        return (1);
    return (0);
}

// Processa as linhas do ficheiro, separando diretivas e mapa
static int process_file_lines(FILE *file, char temp_map[50][1024], int *map_lines, t_game *game)
{
    char line[1024];
    int map_started = 0;

    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == '\0')
            continue;
        if (!map_started)
        {
            if (handle_texture_directive(line, game))
                continue;
            if (!strchr(line, '1') && !strchr(line, '0'))
                continue;
            map_started = 1;
        }
        if (*map_lines >= 50)
            return (0);
        strcpy(temp_map[*map_lines], line);
        (*map_lines)++;
    }
    return (1);
}

// Lê o ficheiro para um mapa temporário (array de strings)
int parse_file_to_temp_map(char *filename, char temp_map[50][1024], int *out_map_lines, t_game *game)
{
    FILE *file;
    int map_lines = 0;

    file = fopen(filename, "r");
    if (!file)
        return (0);
    if (!process_file_lines(file, temp_map, &map_lines, game))
    {
        fclose(file);
        return (0);
    }
    fclose(file);
    *out_map_lines = map_lines;
    return (1);
}

// Copia o mapa temporário para game->map e conta jogadores
static int copy_map_and_count(char temp_map[50][1024], int map_lines, t_game *game, int *player_count)
{
    int i;
    int j;
    int len;

    i = 0;
    *player_count = 0;
    while (i < map_lines)
    {
        game->map[i] = strdup(temp_map[i]);
        len = strlen(temp_map[i]);
        if (len > game->map_width)
            game->map_width = len;
        j = 0;
        while (j < len)
        {
            char c = temp_map[i][j];
            if (c == 'N' || c == 'S' || c == 'E' || c == 'W')
                (*player_count)++;
            j++;
        }
        i++;
    }
    return (1);
}

// Finaliza o mapa alocando game->map e validando jogador
int finalize_map(char temp_map[50][1024], int map_lines, t_game *game)
{
    int player_count;

    game->map = malloc(sizeof(char *) * (map_lines + 1));
    if (!game->map)
        return (0);
    game->map_height = map_lines;
    game->map_width = 0;
    if (!copy_map_and_count(temp_map, map_lines, game, &player_count))
    {
        free_map(game->map);
        return (0);
    }
    game->map[map_lines] = NULL;
    if (player_count != 1)
    {
        free_map(game->map);
        return (0);
    }
    return (1);
}
