/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_finalize.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmoura-d <vmoura-d@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 10:49:11 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/28 11:55:39 by vmoura-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_mandatory.h"

// Copia o mapa temporário para game->map e conta jogadores
static int	copy_map_and_count(char temp_map[50][1024], int map_lines,
		t_game *game, int *player_count)
{
	int		i;
	int		j;
	int		len;
	char	c;

	i = 0;
	*player_count = 0;
	while (i < map_lines)
	{
		game->map[i] = ft_strdup(temp_map[i]);
		len = ft_strlen(temp_map[i]);
		if (len > game->map_width)
			game->map_width = len;
		j = 0;
		while (j < len)
		{
			c = temp_map[i][j];
			if (c == 'N' || c == 'S' || c == 'E' || c == 'W')
				(*player_count)++;
			j++;
		}
		i++;
	}
	return (1);
}

// Finaliza o mapa alocando game->map e validando jogador
int	finalize_map(char temp_map[50][1024], int map_lines, t_game *game)
{
	int	player_count;

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
