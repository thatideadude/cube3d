/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmoura-d <vmoura-d@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 10:51:54 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/28 11:55:40 by vmoura-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_mandatory.h"

static int	parse_component(char **str, int *value)
{
	int	result;

	while (**str == ' ')
		(*str)++;
	if (**str < '0' || **str > '9')
		return (0);
	result = 0;
	while (**str >= '0' && **str <= '9')
	{
		result = (result * 10) + (**str - '0');
		(*str)++;
	}
	while (**str == ' ')
		(*str)++;
	*value = result;
	return (1);
}

// Converte string de cor 'R,G,B' para t_color
int	parse_color(char *color_str, t_color *color)
{
	char	*ptr;

	ptr = color_str;
	if (!parse_component(&ptr, &color->r) || *ptr != ',')
		return (0);
	ptr++;
	if (!parse_component(&ptr, &color->g) || *ptr != ',')
		return (0);
	ptr++;
	if (!parse_component(&ptr, &color->b))
		return (0);
	if (*ptr != '\0')
		return (0);
	if (color->r < 0 || color->r > 255)
		return (0);
	if (color->g < 0 || color->g > 255)
		return (0);
	if (color->b < 0 || color->b > 255)
		return (0);
	return (1);
}

// Liberta o array de strings que representa o mapa
void	free_map(char **map)
{
	int	i;

	i = 0;
	if (!map)
		return ;
	while (map[i])
	{
		free(map[i]);
		i++;
	}
	free(map);
}

// Verifica se as coordenadas (x,y) referem-se a uma parede
int	is_wall(t_game *game, int x, int y)
{
	if (x < 0 || x >= game->map_width || y < 0 || y >= game->map_height)
		return (1);
	if (y >= game->map_height || x >= ft_strlen(game->map[y]))
		return (1);
	return (game->map[y][x] == '1');
}

// Parseia o ficheiro .cub montando as texturas e o mapa em memória
int	parse_map_file(char *filename, t_game *game)
{
	char	temp_map[50][1024];
	int		map_lines;
	int		i;

	i = 0;
	while (i < 6)
	{
		game->texture_paths[i] = NULL;
		i++;
	}
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
