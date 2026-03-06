#include "cub3d_bonus.h"

// Valida argumentos e faz parse do mapa (moved from main_bonus)
int validate_and_parse(int argc, char **argv, t_game *game)
{
    if (argc != 2)
        return (0);
    int len = strlen(argv[1]);
    if (len < 4 || strcmp(argv[1] + len - 4, ".cub") != 0)
        return (0);
    memset(game, 0, sizeof(t_game));
    if (!parse_map_file(argv[1], game))
        return (0);
    return (1);
}

// Trata uma linha de diretiva de textura (NO,SO,WE,EA,DO,D,F,C)
static int handle_texture_directive(char *line, t_game *game)
{
    if (!line || !*line) return 0;
    if (!strncmp(line, "NO ", 3) || !strncmp(line, "SO ", 3) || !strncmp(line, "WE ", 3) || !strncmp(line, "EA ", 3)) {
        char *c = line + 3; while (*c == ' ') c++; char *e = c + strlen(c) - 1; while (e > c && (*e == ' ' || *e == '\t' || *e == '\r' || *e == '\n')) *e-- = '\0';
        if (!strncmp(line, "NO ", 3)) game->texture_paths[0] = strdup(c);
        else if (!strncmp(line, "SO ", 3)) game->texture_paths[1] = strdup(c);
        else if (!strncmp(line, "WE ", 3)) game->texture_paths[2] = strdup(c);
        else game->texture_paths[3] = strdup(c);
        return 1;
    }
    if (!strncmp(line, "DO ", 3) || !strncmp(line, "D ", 2)) {
        char *c = (!strncmp(line, "DO ", 3)) ? line + 3 : line + 2; while (*c == ' ') c++; char *e = c + strlen(c) - 1; while (e > c && (*e == ' ' || *e == '\t' || *e == '\r' || *e == '\n')) *e-- = '\0';
        game->texture_paths[6] = strdup(c); return 1;
    }
    if (!strncmp(line, "F ", 2)) {
        char *c = line + 2; while (*c == ' ') c++; if (strstr(c, ".xpm")) { game->texture_paths[4] = strdup(c); game->use_floor_texture = 1; } else { if (!parse_color(c, &game->floor_color)) return 0; game->use_floor_texture = 0; } return 1;
    }
    if (!strncmp(line, "C ", 2)) {
        char *c = line + 2; while (*c == ' ') c++; if (strstr(c, ".xpm")) { game->texture_paths[5] = strdup(c); game->use_ceiling_texture = 1; } else { if (!parse_color(c, &game->ceiling_color)) return 0; game->use_ceiling_texture = 0; } return 1;
    }
    return 0;
}

// Processa linhas do ficheiro, separando diretivas do mapa
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

// Copia mapa temporário para game->map e conta jogadores/portas
static int copy_map_and_count(char temp_map[50][1024], int map_lines, t_game *game, int *player_count)
{
    int i = 0, j, len; *player_count = 0;
    while (i < map_lines) {
        game->map[i] = strdup(temp_map[i]); len = strlen(temp_map[i]);
        if (len > game->map_width) game->map_width = len;
        j = 0;
        while (j < len) {
            char ch = temp_map[i][j];
            if (ch == 'N' || ch == 'S' || ch == 'E' || ch == 'W') (*player_count)++;
            if (ch == 'D' && game->door_count < 64) {
                game->doors[game->door_count].x = j;
                game->doors[game->door_count].y = i;
                game->doors[game->door_count].active = 0;
                game->door_count++;
            }
            j++;
        }
        i++;
    }
    return (1);
}

// Parsea o ficheiro .cub para map, texturas e portas (bonus)
int parse_map_file(char *filename, t_game *game)
{
    FILE *file;
    char temp_map[50][1024];
    int map_lines = 0;

    file = fopen(filename, "r");
    if (!file)
        return (0);
    for (int i = 0; i < 7; i++) game->texture_paths[i] = NULL;
    game->use_floor_texture = 0; game->use_ceiling_texture = 0; game->use_door_texture = 0;
    if (!process_file_lines(file, temp_map, &map_lines, game)) { fclose(file); return (0); }
    fclose(file);
    game->map = malloc(sizeof(char*) * (map_lines + 1)); game->map_height = map_lines; game->map_width = 0; game->door_count = 0;
    int player_count = 0;
    if (!copy_map_and_count(temp_map, map_lines, game, &player_count)) { free_map(game->map); return (0); }
    game->map[map_lines] = NULL;
    if (player_count != 1) { free_map(game->map); return (0); }
    if (!validate_map(game)) { free_map(game->map); return (0); }
    return (1);
}
