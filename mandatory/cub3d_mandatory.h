/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d_mandatory.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmoura-d <vmoura-d@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 11:07:38 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/28 11:55:38 by vmoura-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_MANDATORY_H
# define CUB3D_MANDATORY_H

# include "../mlx/mlx.h"
# include <fcntl.h>
# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

# define WINDOW_WIDTH 1024
# define WINDOW_HEIGHT 768
# define TILE_SIZE 64

// Constantes para raycasting
# define FOV 60
# define NUM_RAYS WINDOW_WIDTH
# define PI 3.14159265359
# define DR 0.0174533

/* Shared tunables moved to header so split modules can use them */
# define COLLISION_RADIUS 0.50
# define MIN_RAY_DISTANCE 0.50
# define SPRINT_MULT 1.8

typedef struct s_player
{
	double		x;
	double		y;
	double		angle;
	double		dx;
	double		dy;
	double		pitch;
}				t_player;

typedef struct s_ray
{
	double		distance;
	int			hit_vertical;
	double		wall_x;
	int			map_x;
	int			map_y;
	int			is_door;
}				t_ray;

typedef struct s_texture
{
	void		*img_ptr;
	char		*data;
	int			width;
	int			height;
	int			bpp;
	int			size_line;
	int			endian;
}				t_texture;

typedef struct s_keys
{
	int			w;
	int			s;
	int			a;
	int			d;
	int			left_arrow;
	int			right_arrow;
	int			shift;
}				t_keys;

typedef struct s_color
{
	int			r;
	int			g;
	int			b;
}				t_color;

typedef struct s_game
{
	void		*mlx_ptr;
	void		*win_ptr;
	void		*img_ptr;
	char		*img_data;
	int			bits_per_pixel;
	int			size_line;
	int			endian;

	char		**map;
	int			map_width;
	int			map_height;

	t_player	player;
	t_ray		rays[NUM_RAYS];
	t_texture	textures[4];
	t_texture	floor_texture;
	t_texture	ceiling_texture;
	char		*texture_paths[6];

	t_color		floor_color;
	t_color		ceiling_color;
	int			use_floor_texture;
	int			use_ceiling_texture;
	t_keys		keys;
}				t_game;

// Funções principais
int				parse_map_file(char *filename, t_game *game);
int				parse_file_to_temp_map(char *filename, char temp_map[50][1024],
					int *map_lines, t_game *game);
int				parse_color(char *color_str, t_color *color);
int				validate_map(t_game *game);
void			free_map(char **map);
void			init_player(t_game *game);
int				load_textures(t_game *game);
int				get_texture_color(t_texture *texture, int x, int y);
void			cast_rays(t_game *game);
void			render_3d(t_game *game);
void			draw_pixel(t_game *game, int x, int y, int color);
int				key_press(int keycode, t_game *game);
int				key_release(int keycode, t_game *game);
int				update_game(t_game *game);
void			move_player(t_game *game);
void			render_frame(t_game *game);
int				is_wall(t_game *game, int x, int y);
int				close_hook(t_game *game);
void			cleanup_game(t_game *game);

/* Additional prototypes required after splitting files */
int				get_floor_ceiling_color(t_game *game, int y, int is_ceiling,
					double ray_angle);
void			resolve_player_collision(t_game *game);

/* New helper prototypes added to split large functions */
int				handle_texture_directive(char *line, t_game *game);
int				append_map_line(char temp_map[50][1024], int *map_lines,
					char *line);
int				finalize_map(char temp_map[50][1024], int map_lines,
					t_game *game);
void			compute_wall_tex_coords(t_game *game, int x, int *texture_index,
					int *tex_x);
int				sample_wall_color(t_game *game, int y, int wall[6]);

double			trace_ray(t_game *game, int i, double ray_angle);
void			render_column(t_game *game, int x);

int				load_wall_textures(t_game *game);
int				load_floor_ceiling_textures(t_game *game);

int				get_ceiling_sample(t_game *game, int screen_x, int screen_y,
					double ray_angle);
int				get_floor_sample(t_game *game, int screen_x, int screen_y,
					double ray_angle);

int				ft_strlen(char *str);
int				ft_strncmp(char *s1, char *s2, int n);
int				ft_strcmp(char *s1, char *s2);
void			*ft_memset(void *ptr, int value, int len);
char			*ft_strchr(char *str, int c);
char			*ft_strstr(char *str, char *to_find);
char			*ft_strdup(char *src);
char			*ft_strcpy(char *dst, char *src);
int				ft_atoi(char *str);

#endif
