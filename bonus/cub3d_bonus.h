#ifndef CUB3D_BONUS_H
# define CUB3D_BONUS_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <string.h>
# include <math.h>
# include "../mlx/mlx.h"

# define WINDOW_WIDTH 1024
# define WINDOW_HEIGHT 768
# define TILE_SIZE 64

// Constantes para raycasting
# define FOV 60
# define NUM_RAYS WINDOW_WIDTH
# define PI 3.14159265359
# define DR 0.0174533

// Bonus-specific tunables
// Mouse sensitivity tuning (lower values = slower look)
// Adjust these to change how fast the view moves with mouse input
# define MOUSE_SENS 0.0006   /* horizontal sensitivity (radians per pixel) */
# define MOUSE_VSENS 0.06    /* vertical sensitivity (pixels per pixel) */

// Minimap tunables (display size and padding)
# define MINIMAP_MAX_W 200
# define MINIMAP_MAX_H 200
# define MINIMAP_PADDING 10

typedef struct s_player {
    double x;
    double y;
    double angle;
    double dx;
    double dy;
    double pitch; // olhar para cima/baixo em pixels (bonus)
} t_player;

typedef struct s_ray {
    double distance;
    int hit_vertical;
    double wall_x;
    int map_x;
    int map_y;
    int is_door;
} t_ray;

typedef struct s_texture {
    void *img_ptr;
    char *data;
    int width;
    int height;
    int bpp;
    int size_line;
    int endian;
} t_texture;

typedef struct s_keys {
    int w;
    int s;
    int a;
    int d;
    int left_arrow;
    int right_arrow;
    int shift;
} t_keys;

typedef struct s_color {
    int r;
    int g;
    int b;
} t_color;

typedef struct s_game {
    void *mlx_ptr;
    void *win_ptr;
    void *img_ptr;
    char *img_data;
    int bits_per_pixel;
    int size_line;
    int endian;
    
    char **map;
    int map_width;
    int map_height;
    
    t_player player;
    t_ray rays[NUM_RAYS];
    t_texture textures[4]; // 0=North, 1=South, 2=West, 3=East
    t_texture door_texture; // texture for doors (bonus)
    t_texture floor_texture;   // Textura do chão
    t_texture ceiling_texture; // Textura do teto
    char *texture_paths[7];    // 4 paredes + chão + teto + porta
    int use_door_texture;

    // Manter cores como fallback se não houver texturas
    t_color floor_color;
    t_color ceiling_color;
    int use_floor_texture;     // Flag para usar textura ou cor
    int use_ceiling_texture;   // Flag para usar textura ou cor
    
    t_keys keys;
    // Door state tracking (simple list)
    struct s_door {
        int x;
        int y;
        int ox; // direction vector from door towards the side the player opened from
        int oy;
        int active;
    } doors[64];
    int door_count;
} t_game;

// Funções principais
int parse_map_file(char *filename, t_game *game);
void free_map(char **map);
void init_player(t_game *game);
void cast_rays(t_game *game);
void render_3d(t_game *game);
void draw_pixel(t_game *game, int x, int y, int color);
int load_textures(t_game *game);
int get_texture_color(t_texture *texture, int x, int y);
int key_press(int keycode, t_game *game);
int key_release(int keycode, t_game *game);
int update_game(t_game *game);
void move_player(t_game *game);
void render_frame(t_game *game);
int is_wall(t_game *game, int x, int y);
int close_hook(t_game *game);
int parse_color(char *color_str, t_color *color);
int parse_texture_path(char *path_str, char **texture_path);
int load_floor_ceiling_textures(t_game *game);
int get_floor_ceiling_color(t_game *game, int x, int y, int is_ceiling, double ray_angle, double distance);
int validate_map(t_game *game);

/* Door functions (bonus) */
int open_door_in_front(t_game *game);
void update_doors(t_game *game);

/* Minimap functions (bonus) */
void draw_minimap(t_game *game);

#endif
