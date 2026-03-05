#include "cub3d_mandatory.h"

#define COLLISION_RADIUS 0.50
#define MIN_RAY_DISTANCE 0.50
#define SPRINT_MULT 1.8

// Função para carregar texturas
int load_textures(t_game *game)
{
    printf("Carregando texturas...\n");
    
    // Carregar texturas das paredes
    for (int i = 0; i < 4; i++)
    {
        if (!game->texture_paths[i])
        {
            printf("Erro: Caminho da textura %d não definido\n", i);
            return (0);
        }
        
        game->textures[i].img_ptr = mlx_xpm_file_to_image(game->mlx_ptr, 
            game->texture_paths[i], &game->textures[i].width, &game->textures[i].height);
        
        if (!game->textures[i].img_ptr)
        {
            printf("Erro ao carregar textura: %s\n", game->texture_paths[i]);
            return (0);
        }
        
        game->textures[i].data = mlx_get_data_addr(game->textures[i].img_ptr,
            &game->textures[i].bpp, &game->textures[i].size_line, &game->textures[i].endian);
        
        printf("Textura carregada: %s (%dx%d)\n", game->texture_paths[i], 
               game->textures[i].width, game->textures[i].height);
    }
    
    // Carregar textura do chão se especificada
    if (game->use_floor_texture && game->texture_paths[4])
    {
        game->floor_texture.img_ptr = mlx_xpm_file_to_image(game->mlx_ptr,
            game->texture_paths[4], &game->floor_texture.width, &game->floor_texture.height);
            
        if (!game->floor_texture.img_ptr)
        {
            printf("Erro ao carregar textura do chão: %s\n", game->texture_paths[4]);
            return (0);
        }
        
        game->floor_texture.data = mlx_get_data_addr(game->floor_texture.img_ptr,
            &game->floor_texture.bpp, &game->floor_texture.size_line, &game->floor_texture.endian);
            
        printf("Textura do chão carregada: %s (%dx%d)\n", game->texture_paths[4],
               game->floor_texture.width, game->floor_texture.height);
    }
    
    // Carregar textura do teto se especificada
    if (game->use_ceiling_texture && game->texture_paths[5])
    {
        game->ceiling_texture.img_ptr = mlx_xpm_file_to_image(game->mlx_ptr,
            game->texture_paths[5], &game->ceiling_texture.width, &game->ceiling_texture.height);
            
        if (!game->ceiling_texture.img_ptr)
        {
            printf("Erro ao carregar textura do teto: %s\n", game->texture_paths[5]);
            return (0);
        }
        
        game->ceiling_texture.data = mlx_get_data_addr(game->ceiling_texture.img_ptr,
            &game->ceiling_texture.bpp, &game->ceiling_texture.size_line, &game->ceiling_texture.endian);
            
        printf("Textura do teto carregada: %s (%dx%d)\n", game->texture_paths[5],
               game->ceiling_texture.width, game->ceiling_texture.height);
    }
    
    return (1);
}

// Função para obter cor de uma textura
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

// Função para obter cor do chão ou teto (textura ou cor sólida)
int get_floor_ceiling_color(t_game *game, int screen_x, int screen_y, int is_ceiling, double ray_angle, double wall_distance)
{
    (void)screen_x; // Evitar warning, não usado neste algoritmo
    (void)wall_distance; // Evitar warning, não usado neste algoritmo
    
    if (is_ceiling)
    {
        if (game->use_ceiling_texture)
        {
            // Algoritmo de floor casting para teto
            double y_offset = screen_y - WINDOW_HEIGHT / 2.0;
            if (y_offset >= 0) return 0x000000; // Teto só na parte superior
            
            // Distância até o teto baseada na perspectiva
            double ceiling_distance = (WINDOW_HEIGHT / 2.0) / (-y_offset);
            
            // Calcular posição no mundo
            double world_x = game->player.x + cos(ray_angle) * ceiling_distance;
            double world_y = game->player.y + sin(ray_angle) * ceiling_distance;
            
            // Mapear para coordenadas da textura (repetir textura)
            int tex_x = (int)(fabs(world_x) * game->ceiling_texture.width) % game->ceiling_texture.width;
            int tex_y = (int)(fabs(world_y) * game->ceiling_texture.height) % game->ceiling_texture.height;
            
            return get_texture_color(&game->ceiling_texture, tex_x, tex_y);
        }
        else
        {
            return (game->ceiling_color.r << 16) | 
                   (game->ceiling_color.g << 8) | 
                    game->ceiling_color.b;
        }
    }
    else // floor
    {
        if (game->use_floor_texture)
        {
            // Algoritmo de floor casting para chão
            double y_offset = screen_y - WINDOW_HEIGHT / 2.0;
            if (y_offset <= 0) return 0x000000; // Chão só na parte inferior
            
            // Distância até o chão baseada na perspectiva
            double floor_distance = (WINDOW_HEIGHT / 2.0) / y_offset;
            
            // Calcular posição no mundo
            double world_x = game->player.x + cos(ray_angle) * floor_distance;
            double world_y = game->player.y + sin(ray_angle) * floor_distance;
            
            // Mapear para coordenadas da textura (repetir textura)
            int tex_x = (int)(fabs(world_x) * game->floor_texture.width) % game->floor_texture.width;
            int tex_y = (int)(fabs(world_y) * game->floor_texture.height) % game->floor_texture.height;
            
            return get_texture_color(&game->floor_texture, tex_x, tex_y);
        }
        else
        {
            return (game->floor_color.r << 16) | 
                   (game->floor_color.g << 8) | 
                    game->floor_color.b;
        }
    }
}

// Função para liberar memória do mapa
void free_map(char **map)
{
    int i = 0;
    
    if (!map)
        return;
    while (map[i])
    {
        free(map[i]);
        i++;
    }
    free(map);
}

// Inicializar posição do jogador
void init_player(t_game *game)
{
    for (int y = 0; y < game->map_height; y++)
    {
        for (int x = 0; x < (int)strlen(game->map[y]); x++)
        {
            if (game->map[y][x] == 'N' || game->map[y][x] == 'S' || 
                game->map[y][x] == 'E' || game->map[y][x] == 'W')
            {
                game->player.x = x + 0.5;
                game->player.y = y + 0.5;
                
                if (game->map[y][x] == 'N')
                    game->player.angle = 3 * PI / 2;
                else if (game->map[y][x] == 'S')
                    game->player.angle = PI / 2;
                else if (game->map[y][x] == 'E')
                    game->player.angle = 0;
                else if (game->map[y][x] == 'W')
                    game->player.angle = PI;
                
                game->player.dx = cos(game->player.angle);
                game->player.dy = sin(game->player.angle);
                game->map[y][x] = '0'; // Substituir por espaço vazio
                
                printf("Jogador encontrado em (%.1f, %.1f)\n", 
                       game->player.x, game->player.y);
                return;
            }
        }
    }
    
    // Se não encontrar jogador, colocar posição padrão
    game->player.x = 1.5;
    game->player.y = 1.5;
    game->player.angle = 0;
    game->player.dx = cos(game->player.angle);
    game->player.dy = sin(game->player.angle);
}

// Verificar se é parede
int is_wall(t_game *game, int x, int y)
{
    if (x < 0 || x >= game->map_width || y < 0 || y >= game->map_height)
        return (1);
    if (y >= game->map_height || x >= (int)strlen(game->map[y]))
        return (1);
    return (game->map[y][x] == '1');
}

// Check whether player can move to (x,y) keeping a small radius from walls
int can_move(t_game *game, double x, double y)
{
    double r = 0.2; // collision radius
    int ix, iy;

    ix = (int)floor(x - r);
    iy = (int)floor(y - r);
    if (is_wall(game, ix, iy)) return 0;

    ix = (int)floor(x + r);
    iy = (int)floor(y - r);
    if (is_wall(game, ix, iy)) return 0;

    ix = (int)floor(x - r);
    iy = (int)floor(y + r);
    if (is_wall(game, ix, iy)) return 0;

    ix = (int)floor(x + r);
    iy = (int)floor(y + r);
    if (is_wall(game, ix, iy)) return 0;

    return 1;
}

// Resolve player collisions against nearby wall cells using circle-AABB push-out
void resolve_player_collision(t_game *game)
{
    double r = COLLISION_RADIUS;
    int px_cell = (int)floor(game->player.x);
    int py_cell = (int)floor(game->player.y);

    // check neighborhood -1..+1 in both axes (expand if needed)
    for (int wy = py_cell - 1; wy <= py_cell + 1; wy++)
    {
        for (int wx = px_cell - 1; wx <= px_cell + 1; wx++)
        {
            if (is_wall(game, wx, wy))
            {
                // AABB of the wall cell [wx, wx+1] x [wy, wy+1]
                double closest_x = game->player.x;
                double closest_y = game->player.y;
                if (closest_x < (double)wx) closest_x = (double)wx;
                if (closest_x > (double)(wx + 1)) closest_x = (double)(wx + 1);
                if (closest_y < (double)wy) closest_y = (double)wy;
                if (closest_y > (double)(wy + 1)) closest_y = (double)(wy + 1);

                double dx = game->player.x - closest_x;
                double dy = game->player.y - closest_y;
                double dist2 = dx * dx + dy * dy;
                if (dist2 == 0.0)
                {
                    // player center exactly on corner; nudge out
                    dx = game->player.x - (wx + 0.5);
                    dy = game->player.y - (wy + 0.5);
                    dist2 = dx * dx + dy * dy;
                    if (dist2 == 0.0)
                    {
                        // arbitrary nudge
                        dx = 0.01; dy = 0.01; dist2 = dx * dx + dy * dy;
                    }
                }
                double dist = sqrt(dist2);
                if (dist < r)
                {
                    double push = r - dist;
                    game->player.x += (dx / dist) * push;
                    game->player.y += (dy / dist) * push;
                }
            }
        }
    }
}

// Raycasting (step-based) - reverted from DDA to previous working implementation
void cast_rays(t_game *game)
{
    double ray_angle = game->player.angle - (FOV * DR / 2);
    double angle_step = (FOV * DR) / NUM_RAYS;

    for (int i = 0; i < NUM_RAYS; i++)
    {
        double distance = 0;
        double ray_x = game->player.x;
        double ray_y = game->player.y;
        double ray_dx = cos(ray_angle);
        double ray_dy = sin(ray_angle);
        double step_size = 0.002; // fine-grained step for precision

        game->rays[i].hit_vertical = 0;
        game->rays[i].wall_x = 0;
        game->rays[i].distance = 50;

        while (distance < 30)
        {
            double prev_x = ray_x;

            ray_x += ray_dx * step_size;
            ray_y += ray_dy * step_size;
            distance += step_size;

            if (is_wall(game, (int)ray_x, (int)ray_y))
            {
                int hit_x = (int)ray_x;
                int prev_hit_x = (int)prev_x;

                if (hit_x != prev_hit_x) // vertical wall hit
                {
                    game->rays[i].hit_vertical = 1;
                    game->rays[i].wall_x = ray_y - floor(ray_y);
                }
                else // horizontal wall hit
                {
                    game->rays[i].hit_vertical = 0;
                    game->rays[i].wall_x = ray_x - floor(ray_x);
                }
                break;
            }
        }

        // fisheye correction
        distance = distance * cos(ray_angle - game->player.angle);
        if (distance < MIN_RAY_DISTANCE)
            distance = MIN_RAY_DISTANCE;

        game->rays[i].distance = distance;
        ray_angle += angle_step;
    }
}

// Desenhar pixel
void draw_pixel(t_game *game, int x, int y, int color)
{
    if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT)
    {
        int pixel = (y * game->size_line) + (x * (game->bits_per_pixel / 8));
        game->img_data[pixel] = color & 0xFF;
        game->img_data[pixel + 1] = (color >> 8) & 0xFF;
        game->img_data[pixel + 2] = (color >> 16) & 0xFF;
    }
}

// Renderização 3D
void render_3d(t_game *game)
{
    // Limpar a tela
    memset(game->img_data, 0, WINDOW_WIDTH * WINDOW_HEIGHT * (game->bits_per_pixel / 8));
    
    for (int x = 0; x < NUM_RAYS; x++)
    {
        double distance = game->rays[x].distance;
        if (distance <= 0.1) distance = 0.1;
        
        // Calcular altura da parede
        int line_height = (int)(WINDOW_HEIGHT / distance);
        
        // Calcular início e fim da parede
        int draw_start = -line_height / 2 + WINDOW_HEIGHT / 2;
        if (draw_start < 0) draw_start = 0;
        
        int draw_end = line_height / 2 + WINDOW_HEIGHT / 2;
        if (draw_end >= WINDOW_HEIGHT) draw_end = WINDOW_HEIGHT - 1;
        
        // Determinar textura baseada na direção da parede
        int texture_index = 0;
        if (game->rays[x].hit_vertical)
        {
            double ray_angle = game->player.angle - (FOV * DR / 2) + (x * (FOV * DR) / NUM_RAYS);
            texture_index = (cos(ray_angle) > 0) ? 3 : 2; // East : West
        }
        else
        {
            double ray_angle = game->player.angle - (FOV * DR / 2) + (x * (FOV * DR) / NUM_RAYS);
            texture_index = (sin(ray_angle) > 0) ? 1 : 0; // South : North
        }
        
        // Coordenada X da textura
        double wx = game->rays[x].wall_x;
        if (wx < 0) wx += 1.0;
        if (wx >= 1.0) wx = fmod(wx, 1.0);
        int tex_x = (int)(wx * game->textures[texture_index].width);
        if (tex_x < 0) tex_x = 0;
        if (tex_x >= game->textures[texture_index].width) 
            tex_x = game->textures[texture_index].width - 1;
        
        // Calcular ângulo do raio para chão/teto
        double ray_angle = game->player.angle - (FOV * DR / 2) + (x * (FOV * DR) / NUM_RAYS);
        
        // Renderizar coluna vertical
        for (int y = 0; y < WINDOW_HEIGHT; y++)
        {
            int color;
            
            if (y < draw_start) // TETO
            {
                color = get_floor_ceiling_color(game, x, y, 1, ray_angle, distance); // 1 = ceiling
            }
            else if (y >= draw_start && y <= draw_end) // PAREDE
            {
                // ...existing wall rendering code...
                int wall_h = draw_end - draw_start;
                if (wall_h <= 0) wall_h = 1;
                int tex_y = ((y - draw_start) * game->textures[texture_index].height) / wall_h;
                if (tex_y < 0) tex_y = 0;
                if (tex_y >= game->textures[texture_index].height) 
                    tex_y = game->textures[texture_index].height - 1;
                
                color = get_texture_color(&game->textures[texture_index], tex_x, tex_y);
                
                // Aplicar sombreamento baseado na distância
                double brightness = 1.0 - (distance / 20.0);
                if (brightness < 0.3) brightness = 0.3;
                if (brightness > 1.0) brightness = 1.0;
                
                // Escurecer paredes verticais para contraste
                if (game->rays[x].hit_vertical)
                    brightness *= 0.7;
                
                int r = ((color >> 16) & 0xFF) * brightness;
                int g = ((color >> 8) & 0xFF) * brightness;
                int b = (color & 0xFF) * brightness;
                color = (r << 16) | (g << 8) | b;
            }
            else // CHÃO
            {
                color = get_floor_ceiling_color(game, x, y, 0, ray_angle, distance); // 0 = floor
            }
            
            draw_pixel(game, x, y, color);
        }
    }
}

void render_frame(t_game *game)
{
    // ensure player is pushed out of walls before raycasting to avoid near-wall artefacts
    resolve_player_collision(game);
    cast_rays(game);
    render_3d(game);
    mlx_put_image_to_window(game->mlx_ptr, game->win_ptr, game->img_ptr, 0, 0);
}

// Validar mapa
int validate_map(t_game *game)
{
    // Verificar se o mapa está cercado por paredes
    for (int y = 0; y < game->map_height; y++)
    {
        for (int x = 0; x < (int)strlen(game->map[y]); x++)
        {
            char cell = game->map[y][x];
            
            // Se for espaço vazio ou jogador, verificar se está nas bordas
            if (cell == '0' || cell == 'N' || cell == 'S' || cell == 'E' || cell == 'W')
            {
                // Verificar bordas do mapa
                if (y == 0 || y == game->map_height - 1 || 
                    x == 0 || x >= (int)strlen(game->map[y]) - 1)
                {
                    printf("Erro: Mapa não está cercado por paredes\n");
                    return (0);
                }
                
                // Verificar células adjacentes
                if (y > 0 && (game->map[y-1][x] == ' ' || x >= (int)strlen(game->map[y-1])))
                {
                    printf("Erro: Mapa não está cercado por paredes (cima)\n");
                    return (0);
                }
                if (y < game->map_height - 1 && (game->map[y+1][x] == ' ' || x >= (int)strlen(game->map[y+1])))
                {
                    printf("Erro: Mapa não está cercado por paredes (baixo)\n");
                    return (0);
                }
            }
        }
    }
    
    return (1);
}

// Parsear cor RGB
int parse_color(char *color_str, t_color *color)
{
    char *token;
    char *str_copy = strdup(color_str);
    
    // Remover espaços no início
    while (*str_copy == ' ')
        str_copy++;
    
    token = strtok(str_copy, ",");
    if (!token) return (0);
    color->r = atoi(token);
    if (color->r < 0 || color->r > 255) return (0);
    
    token = strtok(NULL, ",");
    if (!token) return (0);
    color->g = atoi(token);
    if (color->g < 0 || color->g > 255) return (0);
    
    token = strtok(NULL, ",");
    if (!token) return (0);
    color->b = atoi(token);
    if (color->b < 0 || color->b > 255) return (0);
    
    free(str_copy);
    return (1);
}

// Parsear arquivo de mapa
int parse_map_file(char *filename, t_game *game)
{
    FILE *file;
    char line[1024];
    int map_started = 0;
    int map_lines = 0;
    char temp_map[50][1024];
    int player_count = 0;
    
    printf("Abrindo arquivo: %s\n", filename);
    
    file = fopen(filename, "r");
    if (!file)
    {
        printf("Erro: Não foi possível abrir o arquivo %s\n", filename);
        return (0);
    }
    
    // Inicializar paths das texturas
    for (int i = 0; i < 6; i++)
        game->texture_paths[i] = NULL;
    game->use_floor_texture = 0;
    game->use_ceiling_texture = 0;
    
    while (fgets(line, sizeof(line), file))
    {
        // Remover quebra de linha
        line[strcspn(line, "\n")] = 0;
        
        // Pular linhas vazias
        if (line[0] == '\0' || line[0] == '\n')
            continue;
        
        if (!map_started)
        {
            // Parse de texturas e cores
            if (strncmp(line, "NO ", 3) == 0)
                game->texture_paths[0] = strdup(line + 3);
            else if (strncmp(line, "SO ", 3) == 0)
                game->texture_paths[1] = strdup(line + 3);
            else if (strncmp(line, "WE ", 3) == 0)
                game->texture_paths[2] = strdup(line + 3);
            else if (strncmp(line, "EA ", 3) == 0)
                game->texture_paths[3] = strdup(line + 3);
            else if (strncmp(line, "F ", 2) == 0)
            {
                // Verificar se é textura ou cor
                char *content = line + 2;
                while (*content == ' ') content++; // Pular espaços
                
                if (strstr(content, ".xpm"))
                {
                    // É uma textura
                    game->texture_paths[4] = strdup(content);
                    game->use_floor_texture = 1;
                    printf("Textura do chão: %s\n", content);
                }
                else
                {
                    // É uma cor
                    if (!parse_color(content, &game->floor_color))
                    {
                        printf("Erro: Cor do chão inválida\n");
                        fclose(file);
                        return (0);
                    }
                    game->use_floor_texture = 0;
                    printf("Cor do chão: %d,%d,%d\n", game->floor_color.r, 
                           game->floor_color.g, game->floor_color.b);
                }
            }
            else if (strncmp(line, "C ", 2) == 0)
            {
                // Verificar se é textura ou cor
                char *content = line + 2;
                while (*content == ' ') content++; // Pular espaços
                
                if (strstr(content, ".xpm"))
                {
                    // É uma textura
                    game->texture_paths[5] = strdup(content);
                    game->use_ceiling_texture = 1;
                    printf("Textura do teto: %s\n", content);
                }
                else
                {
                    // É uma cor
                    if (!parse_color(content, &game->ceiling_color))
                    {
                        printf("Erro: Cor do teto inválida\n");
                        fclose(file);
                        return (0);
                    }
                    game->use_ceiling_texture = 0;
                    printf("Cor do teto: %d,%d,%d\n", game->ceiling_color.r, 
                           game->ceiling_color.g, game->ceiling_color.b);
                }
            }
            else if (strchr(line, '1') || strchr(line, '0'))
            {
                map_started = 1;
                strcpy(temp_map[map_lines], line);
                map_lines++;
            }
        }
        else
        {
            // Continuar lendo o mapa
            if (map_lines < 50)
            {
                strcpy(temp_map[map_lines], line);
                map_lines++;
            }
        }
    }
    
    fclose(file);
    
    // Verificar se todas as texturas foram definidas
    for (int i = 0; i < 4; i++)
    {
        if (!game->texture_paths[i])
        {
            printf("Erro: Textura %d não definida\n", i);
            return (0);
        }
    }
    
    // Alocar e copiar o mapa
    game->map = malloc(sizeof(char*) * (map_lines + 1));
    game->map_height = map_lines;
    game->map_width = 0;
    
    for (int i = 0; i < map_lines; i++)
    {
        game->map[i] = strdup(temp_map[i]);
        int len = strlen(temp_map[i]);
        if (len > game->map_width)
            game->map_width = len;
        
        // Contar jogadores
        for (int j = 0; j < len; j++)
        {
            if (temp_map[i][j] == 'N' || temp_map[i][j] == 'S' || 
                temp_map[i][j] == 'E' || temp_map[i][j] == 'W')
                player_count++;
        }
    }
    game->map[map_lines] = NULL;
    
    // Validar número de jogadores
    if (player_count != 1)
    {
        printf("Erro: Deve haver exatamente um jogador no mapa (encontrados: %d)\n", player_count);
        free_map(game->map);
        return (0);
    }
    
    // Validar mapa
    if (!validate_map(game))
    {
        free_map(game->map);
        return (0);
    }
    
    printf("Mapa carregado: %d x %d\n", game->map_width, game->map_height);
    return (1);
}

// Movimento do jogador
void move_player(t_game *game)
{
    double base_speed = 0.08;
    double rot_speed = 0.05;
    double sprint = (game->keys.shift) ? SPRINT_MULT : 1.0;
    double move_speed = base_speed * sprint;
    double new_x, new_y;
    
    // Rotação com setas esquerda/direita
    if (game->keys.left_arrow)
    {
        game->player.angle -= rot_speed;
        if (game->player.angle < 0)
            game->player.angle += 2 * PI;
        game->player.dx = cos(game->player.angle);
        game->player.dy = sin(game->player.angle);
    }
    
    if (game->keys.right_arrow)
    {
        game->player.angle += rot_speed;
        if (game->player.angle >= 2 * PI)
            game->player.angle -= 2 * PI;
        game->player.dx = cos(game->player.angle);
        game->player.dy = sin(game->player.angle);
    }
    
    // Movimento W/S (frente/trás)
    if (game->keys.w)
    {
        new_x = game->player.x + game->player.dx * move_speed;
        new_y = game->player.y + game->player.dy * move_speed;
        
        if (can_move(game, new_x, game->player.y))
            game->player.x = new_x;
        if (can_move(game, game->player.x, new_y))
            game->player.y = new_y;
    }
    
    if (game->keys.s)
    {
        new_x = game->player.x - game->player.dx * move_speed;
        new_y = game->player.y - game->player.dy * move_speed;
        
        if (can_move(game, new_x, game->player.y))
            game->player.x = new_x;
        if (can_move(game, game->player.x, new_y))
            game->player.y = new_y;
    }
    
    // Movimento A/D (strafe)
    if (game->keys.a)
    {
        new_x = game->player.x + game->player.dy * move_speed; // Perpendicular
        new_y = game->player.y - game->player.dx * move_speed;
        
        if (can_move(game, new_x, game->player.y))
            game->player.x = new_x;
        if (can_move(game, game->player.x, new_y))
            game->player.y = new_y;
    }
    
    if (game->keys.d)
    {
        new_x = game->player.x - game->player.dy * move_speed; // Perpendicular
        new_y = game->player.y + game->player.dx * move_speed;
        
        if (can_move(game, new_x, game->player.y))
            game->player.x = new_x;
        if (can_move(game, game->player.x, new_y))
            game->player.y = new_y;
    }

    // after movement attempts, resolve collisions to enforce invisible wall
    resolve_player_collision(game);
}

// Eventos de teclas
int key_press(int keycode, t_game *game)
{
    if (keycode == 65307) // ESC
    {
        mlx_destroy_window(game->mlx_ptr, game->win_ptr);
        exit(0);
    }
    else if (keycode == 119) game->keys.w = 1;           // W
    else if (keycode == 115) game->keys.s = 1;           // S
    else if (keycode == 97) game->keys.a = 1;            // A
    else if (keycode == 100) game->keys.d = 1;           // D
    else if (keycode == 65505 || keycode == 65506 || keycode == 301) game->keys.shift = 1; // Shift
    else if (keycode == 65361) game->keys.left_arrow = 1;  // Seta esquerda
    else if (keycode == 65363) game->keys.right_arrow = 1; // Seta direita

    return (0);
}

int key_release(int keycode, t_game *game)
{
    if (keycode == 119) game->keys.w = 0;           // W
    else if (keycode == 115) game->keys.s = 0;      // S
    else if (keycode == 97) game->keys.a = 0;       // A
    else if (keycode == 100) game->keys.d = 0;      // D
    else if (keycode == 65505 || keycode == 65506 || keycode == 301) game->keys.shift = 0; // Shift
    else if (keycode == 65361) game->keys.left_arrow = 0;  // Seta esquerda
    else if (keycode == 65363) game->keys.right_arrow = 0; // Seta direita

    return (0);
}

// Game loop
int update_game(t_game *game)
{
    move_player(game);
    render_frame(game);
    return (0);
}

// Fechar janela
int close_hook(t_game *game)
{
    mlx_destroy_window(game->mlx_ptr, game->win_ptr);
    exit(0);
}

int main(int argc, char **argv)
{
    t_game game;
    
    if (argc != 2)
    {
        printf("Uso: %s <arquivo.cub>\n", argv[0]);
        return (1);
    }
    
    // Verificar extensão .cub
    int len = strlen(argv[1]);
    if (len < 4 || strcmp(argv[1] + len - 4, ".cub") != 0)
    {
        printf("Erro: O arquivo deve ter extensão .cub\n");
        return (1);
    }
    
    // Inicializar estrutura do jogo
    memset(&game, 0, sizeof(t_game));
    
    // Parsear arquivo
    if (!parse_map_file(argv[1], &game))
    {
        printf("Erro ao carregar o mapa\n");
        return (1);
    }
    
    // Inicializar jogador
    init_player(&game);
    
    // Inicializar MLX
    game.mlx_ptr = mlx_init();
    if (!game.mlx_ptr)
    {
        printf("Erro ao inicializar MLX\n");
        free_map(game.map);
        return (1);
    }
    
    // Criar janela
    game.win_ptr = mlx_new_window(game.mlx_ptr, WINDOW_WIDTH, WINDOW_HEIGHT, "Cub3D - Mandatory");
    if (!game.win_ptr)
    {
        printf("Erro ao criar janela\n");
        free_map(game.map);
        return (1);
    }
    
    // Criar imagem
    game.img_ptr = mlx_new_image(game.mlx_ptr, WINDOW_WIDTH, WINDOW_HEIGHT);
    game.img_data = mlx_get_data_addr(game.img_ptr, &game.bits_per_pixel, &game.size_line, &game.endian);
    
    // Carregar texturas
    if (!load_textures(&game))
    {
        printf("Erro ao carregar texturas\n");
        free_map(game.map);
        return (1);
    }
    
    printf("=== CUB3D MANDATORY ===\n");
    printf("Controles:\n");
    printf("  W/S         - Mover para frente/trás\n");
    printf("  A/D         - Strafe esquerda/direita\n");
    printf("  ←/→         - Girar esquerda/direita\n");
    printf("  ESC         - Sair\n");
    
    // Renderizar primeiro frame
    render_frame(&game);
    
    // Configurar hooks
    mlx_hook(game.win_ptr, 2, 1L<<0, key_press, &game);    // KeyPress
    mlx_hook(game.win_ptr, 3, 1L<<1, key_release, &game);  // KeyRelease
    mlx_hook(game.win_ptr, 17, 1L<<17, close_hook, &game); // Fechar janela
    mlx_loop_hook(game.mlx_ptr, update_game, &game);       // Game loop
    
    // Loop principal
    mlx_loop(game.mlx_ptr);
    
    free_map(game.map);
    return (0);
}
