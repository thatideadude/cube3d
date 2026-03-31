/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_read.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vinicius-moura <vinicius-moura@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 11:53:00 by vmoura-d          #+#    #+#             */
/*   Updated: 2026/03/31 09:12:15 by vinicius-mo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	append_map_line(char temp_map[50][1024], int *map_lines, char *line)
{
	int	i;

	if (*map_lines >= 50)
		return (0);
	i = 0;
	while (line[i] && i < 1023)
	{
		temp_map[*map_lines][i] = line[i];
		i++;
	}
	temp_map[*map_lines][i] = '\0';
	(*map_lines)++;
	return (1);
}

static int	process_line(char *line, char temp_map[50][1024], int state[2],
		t_game *game)
{
	if (line[0] == '\0')
		return (1);
	if (!state[0])
	{
		if (handle_texture_directive(line, game))
			return (1);
		if (!ft_strchr(line, '1') && !ft_strchr(line, '0'))
			return (1);
		state[0] = 1;
	}
	return (append_map_line(temp_map, &state[1], line));
}

static int	process_content(char *content, char temp_map[50][1024],
		int *out_map_lines, t_game *game)
{
	char	line[1024];
	int		state[2];
	int		i;
	int		j;

	state[0] = 0;
	state[1] = 0;
	i = 0;
	while (1)
	{
		j = 0;
		while (content[i] && content[i] != '\n' && j < 1023)
			line[j++] = content[i++];
		line[j] = '\0';
		if (!process_line(line, temp_map, state, game))
			return (0);
		if (!content[i])
			break ;
		i++;
	}
	*out_map_lines = state[1];
	return (1);
}

static int	read_file_content(char *filename, char *content, int size)
{
	int	fd;
	int	bytes;
	int	total;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return (0);
	total = 0;
	bytes = 1;
	while (bytes > 0 && total < size - 1)
	{
		bytes = read(fd, content + total, size - total - 1);
		if (bytes < 0)
		{
			close(fd);
			return (0);
		}
		total += bytes;
	}
	content[total] = '\0';
	close(fd);
	return (bytes == 0);
}

int	parse_file_to_temp_map(char *filename, char temp_map[50][1024],
		int *out_map_lines, t_game *game)
{
	char	content[65536];

	if (!read_file_content(filename, content, 65536))
		return (0);
	return (process_content(content, temp_map, out_map_lines, game));
}
