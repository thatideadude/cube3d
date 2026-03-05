#include "cub3d_bonus.h"

// Most functions adapted from mandatory version, with mouse look support

#define COLLISION_RADIUS 0.50
#define MIN_RAY_DISTANCE 0.50
#define SPRINT_MULT 1.8
// Mouse sensitivity macros are defined in bonus/cub3d_bonus.h (MOUSE_SENS, MOUSE_VSENS)

// Resolve player collisions against nearby wall cells using circle-AABB push-out
void resolve_player_collision(t_game *game)
{
    double r = COLLISION_RADIUS;
    int px_cell = (int)floor(game->player.x);
    int py_cell = (int)floor(game->player.y);

    for (int wy = py_cell - 1; wy <= py_cell + 1; wy++)
    {
        for (int wx = px_cell - 1; wx <= px_cell + 1; wx++)
        {
            if (is_wall(game, wx, wy))
            {
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
                    dx = game->player.x - (wx + 0.5);
                    dy = game->player.y - (wy + 0.5);
                    dist2 = dx * dx + dy * dy;
                    if (dist2 == 0.0) { dx = 0.01; dy = 0.01; dist2 = dx * dx + dy * dy; }
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

int load_textures(t_game *game)
{
    printf("Carregando texturas (bonus)...\n");
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
    }
    if (game->use_floor_texture && game->texture_paths[4])
    {
        game->floor_texture.img_ptr = mlx_xpm_file_to_image(game->mlx_ptr,
            game->texture_paths[4], &game->floor_texture.width, &game->floor_texture.height);
        if (!game->floor_texture.img_ptr) return (0);
        game->floor_texture.data = mlx_get_data_addr(game->floor_texture.img_ptr,
            &game->floor_texture.bpp, &game->floor_texture.size_line, &game->floor_texture.endian);
    }
    if (game->use_ceiling_texture && game->texture_paths[5])
    {
        game->ceiling_texture.img_ptr = mlx_xpm_file_to_image(game->mlx_ptr,
            game->texture_paths[5], &game->ceiling_texture.width, &game->ceiling_texture.height);
        if (!game->ceiling_texture.img_ptr) return (0);
        game->ceiling_texture.data = mlx_get_data_addr(game->ceiling_texture.img_ptr,
            &game->ceiling_texture.bpp, &game->ceiling_texture.size_line, &game->ceiling_texture.endian);
    }
    // door texture (optional)
    game->use_door_texture = 0;
    if (game->texture_paths[6])
    {
        // try to load door texture if provided
        game->door_texture.img_ptr = mlx_xpm_file_to_image(game->mlx_ptr,
            game->texture_paths[6], &game->door_texture.width, &game->door_texture.height);
        if (game->door_texture.img_ptr)
        {
            game->door_texture.data = mlx_get_data_addr(game->door_texture.img_ptr,
                &game->door_texture.bpp, &game->door_texture.size_line, &game->door_texture.endian);
            game->use_door_texture = 1;
            printf("Door texture loaded: %s\n", game->texture_paths[6]);
        }
        else
        {
            printf("Aviso: não foi possível carregar door texture: %s\n", game->texture_paths[6]);
            game->use_door_texture = 0;
        }
    }
    return (1);
}

// helper: write a pixel into the image buffer (bounds-checked)
static void minimap_put_pixel(t_game *game, int x, int y, int color)
{
    if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT) return;
    int dp = (y * game->size_line) + (x * (game->bits_per_pixel / 8));
    game->img_data[dp] = color & 0xFF;
    game->img_data[dp + 1] = (color >> 8) & 0xFF;
    game->img_data[dp + 2] = (color >> 16) & 0xFF;
}

// Bresenham line for minimap overlay
static void minimap_draw_line(t_game *game, int x0, int y0, int x1, int y1, int color)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    while (1)
    {
        minimap_put_pixel(game, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// filled circle (small radius) for player marker
static void minimap_fill_circle(t_game *game, int cx, int cy, int r, int color)
{
    for (int dy = -r; dy <= r; dy++)
    {
        for (int dx = -r; dx <= r; dx++)
        {
            if (dx * dx + dy * dy <= r * r)
                minimap_put_pixel(game, cx + dx, cy + dy, color);
        }
    }
}

// Procedural minimap: draw a smaller, cleaner minimap with direction indicator
void draw_minimap(t_game *game)
{
    int padding = MINIMAP_PADDING;
    /* shrink the configured minimap to make it more compact on screen */
    double shrink = 0.55; // reduce overall minimap size (tweakable)
    int max_w = (int)(MINIMAP_MAX_W * shrink);
    int max_h = (int)(MINIMAP_MAX_H * shrink);
    int map_w = game->map_width;
    int map_h = game->map_height;
    if (map_w <= 0 || map_h <= 0) return;

    double sx = (double)max_w / (double)map_w;
    double sy = (double)max_h / (double)map_h;
    double scale = (sx < sy) ? sx : sy;
    if (scale < 1.0) scale = 1.0; // ensure at least one pixel per cell for readability

    int draw_w = (int)(map_w * scale);
    int draw_h = (int)(map_h * scale);
    int dest_x = padding;
    int dest_y = padding;

    // draw a subtle rounded background box
    int box_w = draw_w + 6; int box_h = draw_h + 6;
    int bx = dest_x - 3; int by = dest_y - 3;
    // cleaner background and subtle inner shadow
    int bg_color = 0x0F1418; // very dark slate
    int inner_shadow = 0x0A0C0E; // slightly darker for inner rim
    for (int yy = 0; yy < box_h; yy++)
    {
        for (int xx = 0; xx < box_w; xx++)
        {
            minimap_put_pixel(game, bx + xx, by + yy, bg_color);
        }
    }
    // 1px inner shadow to give separation from game view
    for (int i = 1; i < box_w - 1; i++) { minimap_put_pixel(game, bx + i, by + 1, inner_shadow); minimap_put_pixel(game, bx + i, by + box_h - 2, inner_shadow); }
    for (int i = 1; i < box_h - 1; i++) { minimap_put_pixel(game, bx + 1, by + i, inner_shadow); minimap_put_pixel(game, bx + box_w - 2, by + i, inner_shadow); }

    // draw map cells (walls darker, floor lighter)
    for (int my = 0; my < map_h; my++)
    {
        for (int mx = 0; mx < map_w; mx++)
        {
            char cell = (mx >= (int)strlen(game->map[my])) ? ' ' : game->map[my][mx];
            // clearer colors: darker walls, neutral floor
            int color;
            if (cell == '1') color = 0x15171A;
            else if (cell == 'D')
            {
                // find door entry and choose color based on open/closed state
                int found = 0;
                for (int di = 0; di < game->door_count; di++)
                {
                    if (game->doors[di].x == mx && game->doors[di].y == my)
                    {
                        found = 1;
                        color = game->doors[di].active ? 0xA67C52 : 0x5C3A21; // open : closed
                        break;
                    }
                }
                if (!found) color = 0x5C3A21;
            }
            else color = 0xC7D0D2;
            // draw cells with 1px spacing to reduce visual clutter
            int sx_px = dest_x + (int)(mx * scale);
            int sy_px = dest_y + (int)(my * scale);
            int cell_w = (int)floor(scale);
            int cell_h = (int)floor(scale);
            if (cell_w <= 0)
                cell_w = 1;
            if (cell_h <= 0)
                cell_h = 1;
            for (int cy = 0; cy < cell_h; cy++)
            {
                for (int cx = 0; cx < cell_w; cx++)
                {
                    int tx = sx_px + cx;
                    int ty = sy_px + cy;
                    minimap_put_pixel(game, tx, ty, color);
                }
            }
        }
    }

    // draw facing direction line (short, thin) first so marker is not obscured
    double ang = game->player.angle;
    int px = dest_x + (int)(game->player.x * scale);
    int py = dest_y + (int)(game->player.y * scale);
    int len = (int)(fmax(draw_w, draw_h) * 0.26);
    if (len < 3) len = 3;
    int steps = len * 2;
    for (int i = 1; i <= steps; i++)
    {
        double t = (double)i / (double)steps * (double)len;
        int tx = px + (int)round(cos(ang) * t);
        int ty = py + (int)round(sin(ang) * t);
        int col = (i > steps - (steps/6)) ? 0xA00000 : 0xD04040;
        minimap_put_pixel(game, tx, ty, col);
    }
    // arrow tip (very small)
    int tipx = px + (int)round(cos(ang) * (double)len);
    int tipy = py + (int)round(sin(ang) * (double)len);
    double perp_x = -sin(ang);
    double perp_y = cos(ang);
    int halfw = (int)fmax(1.0, scale * 0.10);
    int ax = tipx; int ay = tipy;
    int bx2 = tipx + (int)round(perp_x * halfw);
    int by2 = tipy + (int)round(perp_y * halfw);
    int cx2 = tipx - (int)round(perp_x * halfw);
    int cy2 = tipy - (int)round(perp_y * halfw);
    minimap_draw_line(game, ax, ay, bx2, by2, 0x800000);
    minimap_draw_line(game, ax, ay, cx2, cy2, 0x800000);
    minimap_draw_line(game, bx2, by2, cx2, cy2, 0x800000);

    // draw player marker (larger, high-contrast): black border, golden fill, white center
    int marker_r = (int)fmax(2.0, scale * 0.5);
    minimap_fill_circle(game, px, py, marker_r + 1, 0x000000); // border
    minimap_fill_circle(game, px, py, marker_r, 0xFFD700); // gold fill
    if (marker_r > 1)
        minimap_fill_circle(game, px, py, 1, 0xFFFFFF); // small white core
    else
        minimap_put_pixel(game, px, py, 0xFFFFFF);

    // draw border
    for (int i = 0; i < box_w; i++)
    {
        minimap_put_pixel(game, bx + i, by, 0x000000);
        minimap_put_pixel(game, bx + i, by + box_h - 1, 0x000000);
    }
    for (int i = 0; i < box_h; i++)
    {
        minimap_put_pixel(game, bx, by + i, 0x000000);
        minimap_put_pixel(game, bx + box_w - 1, by + i, 0x000000);
    }
}

int get_texture_color(t_texture *texture, int x, int y)
{
    if (x < 0 || x >= texture->width || y < 0 || y >= texture->height)
        return (0x000000);
    int pixel = (y * texture->size_line) + (x * (texture->bpp / 8));
    int b = texture->data[pixel] & 0xFF;
    int g = (texture->data[pixel + 1] & 0xFF) << 8;
    int r = texture->data[pixel + 2] & 0xFF;
    return (r << 16) | g | b;
}

int get_floor_ceiling_color(t_game *game, int screen_x, int screen_y, int is_ceiling, double ray_angle, double wall_distance)
{
    (void)screen_x; (void)wall_distance;
    /* Standard floorcast:
       row_distance = (player_eye_height * proj_plane) / rel_y
       world = player_pos + ray_dir * row_distance
       This anchors floor texels to world space so they rotate correctly with the camera. */
    double proj_plane = (WINDOW_WIDTH / 2.0) / tan((FOV * DR) / 2.0);
    double horizon = (WINDOW_HEIGHT / 2.0) - game->player.pitch;
    double rel_y = (double)screen_y - horizon;
    if (fabs(rel_y) < 1e-6) return 0x000000;

    double player_eye = 0.5; // player eye height above floor in world units (tweakable)
    double ray_dx = cos(ray_angle);
    double ray_dy = sin(ray_angle);

    if (is_ceiling)
    {
        if (rel_y >= 0) return 0x000000;
        double row_dist = (player_eye * proj_plane) / (-rel_y);
        double world_x = game->player.x + ray_dx * row_dist;
        double world_y = game->player.y + ray_dy * row_dist;

        if (game->use_ceiling_texture)
        {
            t_texture *tex = &game->ceiling_texture;
            double fx = world_x - floor(world_x);
            double fy = world_y - floor(world_y);
            fx = fmod(fx + 1.0, 1.0);
            fy = fmod(fy + 1.0, 1.0);
            int tex_x = (int)(fx * tex->width);
            int tex_y = (int)(fy * tex->height);
            if (tex_x < 0) tex_x = 0;
            if (tex_x >= tex->width) tex_x = tex->width - 1;
            if (tex_y < 0) tex_y = 0;
            if (tex_y >= tex->height) tex_y = tex->height - 1;
            return get_texture_color(tex, tex_x, tex_y);
        }
        return (game->ceiling_color.r << 16) | (game->ceiling_color.g << 8) | game->ceiling_color.b;
    }
    else
    {
        if (rel_y <= 0) return 0x000000;
        double row_dist = (player_eye * proj_plane) / rel_y;
        double world_x = game->player.x + ray_dx * row_dist;
        double world_y = game->player.y + ray_dy * row_dist;

        if (game->use_floor_texture)
        {
            t_texture *tex = &game->floor_texture;
            double fx = world_x - floor(world_x);
            double fy = world_y - floor(world_y);
            fx = fmod(fx + 1.0, 1.0);
            fy = fmod(fy + 1.0, 1.0);
            int tex_x = (int)(fx * tex->width);
            int tex_y = (int)(fy * tex->height);
            if (tex_x < 0) tex_x = 0;
            if (tex_x >= tex->width) tex_x = tex->width - 1;
            if (tex_y < 0) tex_y = 0;
            if (tex_y >= tex->height) tex_y = tex->height - 1;
            return get_texture_color(tex, tex_x, tex_y);
        }
        return (game->floor_color.r << 16) | (game->floor_color.g << 8) | game->floor_color.b;
    }
}

void free_map(char **map)
{
    int i = 0;
    if (!map) return;
    while (map[i]) { free(map[i]); i++; }
    free(map);
}

void init_player(t_game *game)
{
    for (int y = 0; y < game->map_height; y++)
    {
        for (int x = 0; x < (int)strlen(game->map[y]); x++)
        {
            if (game->map[y][x] == 'N' || game->map[y][x] == 'S' || game->map[y][x] == 'E' || game->map[y][x] == 'W')
            {
                game->player.x = x + 0.5;
                game->player.y = y + 0.5;
                if (game->map[y][x] == 'N') game->player.angle = 3 * PI / 2;
                else if (game->map[y][x] == 'S') game->player.angle = PI / 2;
                else if (game->map[y][x] == 'E') game->player.angle = 0;
                else if (game->map[y][x] == 'W') game->player.angle = PI;
                game->player.dx = cos(game->player.angle);
                game->player.dy = sin(game->player.angle);
                game->player.pitch = 0.0; // vertical look offset in pixels
                game->map[y][x] = '0';
                return;
            }
        }
    }
    game->player.x = 1.5; game->player.y = 1.5; game->player.angle = 0;
    game->player.dx = cos(game->player.angle); game->player.dy = sin(game->player.angle);
    game->player.pitch = 0.0;
}

int is_wall(t_game *game, int x, int y)
{
    if (x < 0 || x >= game->map_width || y < 0 || y >= game->map_height) return (1);
    if (y >= game->map_height || x >= (int)strlen(game->map[y])) return (1);
    char c = game->map[y][x];
    if (c == '1') return 1;
    if (c == 'D')
    {
        // find door entry and treat as wall if closed, passable if active(open)
        for (int i = 0; i < game->door_count; i++)
        {
            if (game->doors[i].x == x && game->doors[i].y == y)
                return (game->doors[i].active ? 0 : 1);
        }
        return 1; // unknown door treated as wall
    }
    return 0;
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
                // previous integer hit X not needed after switching to floor-based detection
                /* compute exact map cell hit using floored ray position to handle both axes */
                int mapx = (int)floor(ray_x);
                int mapy = (int)floor(ray_y);
                game->rays[i].map_x = mapx;
                game->rays[i].map_y = mapy;
                if (mapy >= 0 && mapy < game->map_height && mapx >= 0 && mapx < (int)strlen(game->map[mapy]))
                    game->rays[i].is_door = (game->map[mapy][mapx] == 'D');
                else
                    game->rays[i].is_door = 0;

                if ((int)floor(ray_x) != (int)floor(prev_x)) // vertical wall hit
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

// Attempt to open a door directly in front of the player (within ~0.8 units)
int open_door_in_front(t_game *game)
{
    double look_dist = 0.8;
    int fx = (int)floor(game->player.x + game->player.dx * look_dist);
    int fy = (int)floor(game->player.y + game->player.dy * look_dist);
    if (fx < 0 || fx >= game->map_width || fy < 0 || fy >= game->map_height) return 0;
    if (game->map[fy][fx] != 'D') return 0;
    for (int i = 0; i < game->door_count; i++)
    {
        if (game->doors[i].x == fx && game->doors[i].y == fy)
        {
            game->doors[i].active = 1; // open
            // store open direction from player to door (normalize to -1/0/1)
            int ox = (int)round(game->player.dx);
            int oy = (int)round(game->player.dy);
            if (ox == 0 && oy == 0)
            {
                if (fabs(game->player.dx) > fabs(game->player.dy)) ox = (game->player.dx > 0) ? 1 : -1;
                else oy = (game->player.dy > 0) ? 1 : -1;
            }
            game->doors[i].ox = ox; game->doors[i].oy = oy;
            return 1;
        }
    }
    return 0;
}

// Update doors: close any open door after player has passed through (behind them)
void update_doors(t_game *game)
{
    for (int i = 0; i < game->door_count; i++)
    {
        if (game->doors[i].active)
        {
            double cx = (double)game->doors[i].x + 0.5;
            double cy = (double)game->doors[i].y + 0.5;
            double vx = game->player.x - cx;
            double vy = game->player.y - cy;
            double dot = vx * (double)game->doors[i].ox + vy * (double)game->doors[i].oy;
            double dist2 = vx * vx + vy * vy;
            /* Close door when player has moved beyond the door center in the opening direction.
               Use a more permissive threshold to avoid missing closure due to rounding. */
            if (dot > 0.25 && dist2 > 0.36) // dot positive and distance > 0.6 units
            {
                game->doors[i].active = 0; // close behind
            }
        }
    }
}

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

void render_3d(t_game *game)
{
    memset(game->img_data, 0, WINDOW_WIDTH * WINDOW_HEIGHT * (game->bits_per_pixel / 8));
    for (int x = 0; x < NUM_RAYS; x++)
    {
        double distance = game->rays[x].distance; if (distance <= 0.1) distance = 0.1;
        int line_height = (int)(WINDOW_HEIGHT / distance);
        int draw_start = -line_height / 2 + WINDOW_HEIGHT / 2 - (int)game->player.pitch; // apply pitch
        if (draw_start < 0) draw_start = 0;
        int draw_end = line_height / 2 + WINDOW_HEIGHT / 2 - (int)game->player.pitch; // apply pitch
        if (draw_end >= WINDOW_HEIGHT) draw_end = WINDOW_HEIGHT - 1;
        t_texture *tex_ptr = NULL;
        int tex_x = 0;
        double wx = game->rays[x].wall_x;
        if (wx < 0) wx += 1.0;
        if (wx >= 1.0) wx = fmod(wx, 1.0);
        if (game->rays[x].is_door && game->use_door_texture)
        {
            tex_ptr = &game->door_texture;
        }
        else
        {
            int texture_index = 0;
            double ray_angle = game->player.angle - (FOV * DR / 2) + (x * (FOV * DR) / NUM_RAYS);
            if (game->rays[x].hit_vertical)
                texture_index = (cos(ray_angle) > 0) ? 3 : 2; // East : West
            else
                texture_index = (sin(ray_angle) > 0) ? 1 : 0; // South : North
            tex_ptr = &game->textures[texture_index];
        }
        if (!tex_ptr) tex_ptr = &game->textures[0];
        int tex_width = tex_ptr->width;
        int tex_height = tex_ptr->height;
        tex_x = (int)(wx * tex_width);
        if (tex_x < 0) tex_x = 0;
        if (tex_x >= tex_width) tex_x = tex_width - 1;
        double ray_angle = game->player.angle - (FOV * DR / 2) + (x * (FOV * DR) / NUM_RAYS);
        for (int y = 0; y < WINDOW_HEIGHT; y++)
        {
            int color;
            if (y < draw_start)
            {
                color = get_floor_ceiling_color(game, x, y, 1, ray_angle, distance);
            }
            else if (y >= draw_start && y <= draw_end)
            {
                int wall_h = draw_end - draw_start;
                if (wall_h <= 0) wall_h = 1;
                int tex_y = ((y - draw_start) * tex_height) / wall_h;
                if (tex_y < 0) tex_y = 0;
                if (tex_y >= tex_height) tex_y = tex_height - 1;
                color = get_texture_color(tex_ptr, tex_x, tex_y);
                double brightness = 1.0 - (distance / 20.0);
                if (brightness < 0.3) brightness = 0.3;
                if (brightness > 1.0) brightness = 1.0;
                if (game->rays[x].hit_vertical) brightness *= 0.7;
                int r = ((color >> 16) & 0xFF) * brightness;
                int g = ((color >> 8) & 0xFF) * brightness;
                int b = (color & 0xFF) * brightness;
                color = (r << 16) | (g << 8) | b;
            }
            else
            {
                color = get_floor_ceiling_color(game, x, y, 0, ray_angle, distance);
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
    // overlay minimap
    draw_minimap(game);
    mlx_put_image_to_window(game->mlx_ptr, game->win_ptr, game->img_ptr, 0, 0);
}

int validate_map(t_game *game)
{
    for (int y = 0; y < game->map_height; y++)
    {
        for (int x = 0; x < (int)strlen(game->map[y]); x++)
        {
            char cell = game->map[y][x];
            if (cell == '0' || cell == 'N' || cell == 'S' || cell == 'E' || cell == 'W')
            {
                if (y == 0 || y == game->map_height - 1 || x == 0 || x >= (int)strlen(game->map[y]) - 1)
                    return (0);
                if (y > 0 && (game->map[y-1][x] == ' ' || x >= (int)strlen(game->map[y-1]))) return (0);
                if (y < game->map_height - 1 && (game->map[y+1][x] == ' ' || x >= (int)strlen(game->map[y+1]))) return (0);
            }
        }
    }
    return (1);
}

int parse_color(char *color_str, t_color *color)
{
    char *token; char *str_copy = strdup(color_str);
    while (*str_copy == ' ') str_copy++;
    token = strtok(str_copy, ","); if (!token) return (0); color->r = atoi(token); if (color->r < 0 || color->r > 255) return (0);
    token = strtok(NULL, ","); if (!token) return (0); color->g = atoi(token); if (color->g < 0 || color->g > 255) return (0);
    token = strtok(NULL, ","); if (!token) return (0); color->b = atoi(token); if (color->b < 0 || color->b > 255) return (0);
    free(str_copy); return (1);
}

int parse_map_file(char *filename, t_game *game)
{
    FILE *file; char line[1024]; int map_started = 0; int map_lines = 0; char temp_map[50][1024]; int player_count = 0;
    file = fopen(filename, "r");
    if (!file) return (0);
    for (int i = 0; i < 7; i++)
        game->texture_paths[i] = NULL;
    game->use_floor_texture = 0;
    game->use_ceiling_texture = 0;
    game->use_door_texture = 0;
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == '\0' || line[0] == '\n') continue;
        if (!map_started)
        {
            if (strncmp(line, "NO ", 3) == 0)
            {
                char *content = line + 3; while (*content == ' ') content++;
                char *end = content + strlen(content) - 1; while (end > content && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) { *end = '\0'; end--; }
                game->texture_paths[0] = strdup(content);
            }
            else if (strncmp(line, "SO ", 3) == 0)
            {
                char *content = line + 3; while (*content == ' ') content++;
                char *end = content + strlen(content) - 1; while (end > content && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) { *end = '\0'; end--; }
                game->texture_paths[1] = strdup(content);
            }
            else if (strncmp(line, "WE ", 3) == 0)
            {
                char *content = line + 3; while (*content == ' ') content++;
                char *end = content + strlen(content) - 1; while (end > content && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) { *end = '\0'; end--; }
                game->texture_paths[2] = strdup(content);
            }
            else if (strncmp(line, "EA ", 3) == 0)
            {
                char *content = line + 3; while (*content == ' ') content++;
                char *end = content + strlen(content) - 1; while (end > content && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) { *end = '\0'; end--; }
                game->texture_paths[3] = strdup(content);
            }
            /* Accept both 'DO ' and legacy 'D ' directives for door texture path */
            else if (strncmp(line, "DO ", 3) == 0 || strncmp(line, "D ", 2) == 0)
            {
                char *content = (strncmp(line, "DO ", 3) == 0) ? line + 3 : line + 2;
                while (*content == ' ') content++;
                char *end = content + strlen(content) - 1; while (end > content && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) { *end = '\0'; end--; }
                game->texture_paths[6] = strdup(content);
            }
            else if (strncmp(line, "F ", 2) == 0)
            {
                char *content = line + 2; while (*content == ' ') content++;
                if (strstr(content, ".xpm")) { game->texture_paths[4] = strdup(content); game->use_floor_texture = 1; }
                else { if (!parse_color(content, &game->floor_color)) { fclose(file); return (0); } game->use_floor_texture = 0; }
            }
            else if (strncmp(line, "C ", 2) == 0)
            {
                char *content = line + 2; while (*content == ' ') content++;
                if (strstr(content, ".xpm")) { game->texture_paths[5] = strdup(content); game->use_ceiling_texture = 1; }
                else { if (!parse_color(content, &game->ceiling_color)) { fclose(file); return (0); } game->use_ceiling_texture = 0; }
            }
            else if (strchr(line, '1') || strchr(line, '0')) { map_started = 1; strcpy(temp_map[map_lines], line); map_lines++; }
        }
        else { if (map_lines < 50) { strcpy(temp_map[map_lines], line); map_lines++; } }
    }
    fclose(file);
    for (int i = 0; i < 4; i++) if (!game->texture_paths[i]) return (0);
    game->map = malloc(sizeof(char*) * (map_lines + 1)); game->map_height = map_lines; game->map_width = 0;
    game->door_count = 0;
    for (int i = 0; i < map_lines; i++) {
        game->map[i] = strdup(temp_map[i]);
        int len = strlen(temp_map[i]);
        if (len > game->map_width) game->map_width = len;
        for (int j = 0; j < len; j++) {
            char ch = temp_map[i][j];
            if (ch == 'N' || ch == 'S' || ch == 'E' || ch == 'W') player_count++;
            if (ch == 'D') {
                if (game->door_count < 64) {
                    game->doors[game->door_count].x = j;
                    game->doors[game->door_count].y = i;
                    game->doors[game->door_count].active = 0; // closed by default
                    game->door_count++;
                }
            }
        }
    }
    game->map[map_lines] = NULL;
    if (player_count != 1) { free_map(game->map); return (0); }
    if (!validate_map(game)) { free_map(game->map); return (0); }
    return (1);
}

void move_player(t_game *game)
{
    double base_speed = 0.08;
    double sprint = (game->keys.shift) ? SPRINT_MULT : 1.0;
    double move_speed = base_speed * sprint; double new_x, new_y;
    // Rotation by mouse only; remove arrow-keys rotation in bonus
    // W/S
    if (game->keys.w)
    {
        new_x = game->player.x + game->player.dx * move_speed; new_y = game->player.y + game->player.dy * move_speed;
        if (can_move(game, new_x, game->player.y)) game->player.x = new_x;
        if (can_move(game, game->player.x, new_y)) game->player.y = new_y;
    }
    if (game->keys.s)
    {
        new_x = game->player.x - game->player.dx * move_speed; new_y = game->player.y - game->player.dy * move_speed;
        if (can_move(game, new_x, game->player.y)) game->player.x = new_x;
        if (can_move(game, game->player.x, new_y)) game->player.y = new_y;
    }
    // A/D strafe
    if (game->keys.a)
    {
        new_x = game->player.x + game->player.dy * move_speed; new_y = game->player.y - game->player.dx * move_speed;
        if (can_move(game, new_x, game->player.y)) game->player.x = new_x;
        if (can_move(game, game->player.x, new_y)) game->player.y = new_y;
    }
    if (game->keys.d)
    {
        new_x = game->player.x - game->player.dy * move_speed; new_y = game->player.y + game->player.dx * move_speed;
        if (can_move(game, new_x, game->player.y)) game->player.x = new_x;
        if (can_move(game, game->player.x, new_y)) game->player.y = new_y;
    }

    // resolve collision to enforce invisible wall
    resolve_player_collision(game);
}

int key_press(int keycode, t_game *game)
{
    if (keycode == 65307) { mlx_destroy_window(game->mlx_ptr, game->win_ptr); exit(0); }
    else if (keycode == 119) game->keys.w = 1; // W
    else if (keycode == 115) game->keys.s = 1; // S
    else if (keycode == 97) game->keys.a = 1; // A
    else if (keycode == 100) game->keys.d = 1; // D
    else if (keycode == 65505 || keycode == 65506 || keycode == 301) game->keys.shift = 1; // Shift (X11 left/right / fallback)
    else if (keycode == 111) { open_door_in_front(game); } // 'o' to open door in front
    return (0);
}

int key_release(int keycode, t_game *game)
{
    if (keycode == 119) game->keys.w = 0;
    else if (keycode == 115) game->keys.s = 0;
    else if (keycode == 97) game->keys.a = 0;
    else if (keycode == 100) game->keys.d = 0;
    else if (keycode == 65505 || keycode == 65506 || keycode == 301) game->keys.shift = 0;
    return (0);
}

// Mouse motion handler: x,y are pointer coordinates relative to window
int mouse_move(int x, int y, t_game *game)
{
    int cx = WINDOW_WIDTH / 2;
    int cy = WINDOW_HEIGHT / 2;
    int dx = x - cx;
    int dy = y - cy;
    double sens = MOUSE_SENS; // horizontal sensitivity
    double vsens = MOUSE_VSENS;  // vertical sensitivity (pixels)
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
        // clamp pitch to reasonable range (look up/down)
        double limit = WINDOW_HEIGHT / 2 - 10;
        if (game->player.pitch > limit) game->player.pitch = limit;
        if (game->player.pitch < -limit) game->player.pitch = -limit;
    }
    // recenter cursor to keep receiving deltas
    mlx_mouse_move(game->mlx_ptr, game->win_ptr, cx, cy);
    return (0);
}

int update_game(t_game *game)
{
    move_player(game);
    update_doors(game);
    render_frame(game);
    return (0);
}

int close_hook(t_game *game)
{
    mlx_destroy_window(game->mlx_ptr, game->win_ptr);
    exit(0);
}

int main(int argc, char **argv)
{
    t_game game;
    if (argc != 2) { printf("Uso: %s <arquivo.cub>\n", argv[0]); return (1); }
    int len = strlen(argv[1]); if (len < 4 || strcmp(argv[1] + len - 4, ".cub") != 0) { printf("Erro: O arquivo deve ter extensão .cub\n"); return (1); }
    memset(&game, 0, sizeof(t_game));
    if (!parse_map_file(argv[1], &game)) { printf("Erro ao carregar o mapa\n"); return (1); }
    init_player(&game);
    game.mlx_ptr = mlx_init(); if (!game.mlx_ptr) { free_map(game.map); return (1); }
    game.win_ptr = mlx_new_window(game.mlx_ptr, WINDOW_WIDTH, WINDOW_HEIGHT, "Cub3D - Bonus"); if (!game.win_ptr) { free_map(game.map); return (1); }
    game.img_ptr = mlx_new_image(game.mlx_ptr, WINDOW_WIDTH, WINDOW_HEIGHT);
    game.img_data = mlx_get_data_addr(game.img_ptr, &game.bits_per_pixel, &game.size_line, &game.endian);
    if (!load_textures(&game)) { printf("Erro ao carregar texturas\n"); free_map(game.map); return (1); }
    // procedural minimap — nothing else to load
    // hide cursor and center it
    mlx_mouse_hide(game.mlx_ptr, game.win_ptr);
    mlx_mouse_move(game.mlx_ptr, game.win_ptr, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
    printf("=== CUB3D BONUS ===\nControls: WASD to move, mouse to look (horiz & vert), ESC to exit\n");
    render_frame(&game);
    // hooks: KeyPress(2), KeyRelease(3), MotionNotify(6), DestroyNotify(17)
    mlx_hook(game.win_ptr, 2, 1L<<0, key_press, &game);
    mlx_hook(game.win_ptr, 3, 1L<<1, key_release, &game);
    mlx_hook(game.win_ptr, 6, 1L<<6, mouse_move, &game);
    mlx_hook(game.win_ptr, 17, 1L<<17, close_hook, &game);
    mlx_loop_hook(game.mlx_ptr, update_game, &game);
    mlx_loop(game.mlx_ptr);
    free_map(game.map);
    return (0);
}
