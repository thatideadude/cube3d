/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmoura-d <vmoura-d@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 10:49:11 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/28 11:55:38 by vmoura-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d_mandatory.h"

static void	init_texture_prefs(char *prefs[4])
{
	prefs[0] = "NO ";
	prefs[1] = "SO ";
	prefs[2] = "WE ";
	prefs[3] = "EA ";
}

// Trata diretiva de piso 'F' (cor ou textura)
static int	handle_floor_directive(char *line, t_game *game)
{
	char	*content;

	if (ft_strncmp(line, "F ", 2) != 0)
		return (0);
	content = line + 2;
	while (*content == ' ')
		content++;
	if (ft_strstr(content, ".xpm"))
	{
		game->texture_paths[4] = ft_strdup(content);
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
static int	handle_ceiling_directive(char *line, t_game *game)
{
	char	*content;

	if (ft_strncmp(line, "C ", 2) != 0)
		return (0);
	content = line + 2;
	while (*content == ' ')
		content++;
	if (ft_strstr(content, ".xpm"))
	{
		game->texture_paths[5] = ft_strdup(content);
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
int	handle_texture_directive(char *line, t_game *game)
{
	char		*prefs[4];
	char		*content;
	int			k;

	init_texture_prefs(prefs);
	k = 0;
	while (k < 4)
	{
		if (ft_strncmp(line, prefs[k], 3) == 0)
		{
			content = line + 3;
			while (*content == ' ')
				content++;
			game->texture_paths[k] = ft_strdup(content);
			return (1);
		}
		k++;
	}
	if (handle_floor_directive(line, game))
		return (1);
	if (handle_ceiling_directive(line, game))
		return (1);
	return (0);
}
