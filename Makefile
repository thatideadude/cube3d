# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::   #
#    Makefile                                           :+:      :+:    :+:   #
#                                                     +:+ +:+         +:+     #
#    By: vmoura <vmoura@student.42.fr>              +#+  +:+       +#+        #
#                                                 +#+#+#+#+#+   +#+           #
#    Created: 2026/02/28 00:00:00 by vmoura            #+#    #+#             #
#    Updated: 2026/02/28 00:00:00 by vmoura           ###   ########.fr       #
#                                                                              #
# **************************************************************************** #

# Consolidated Makefile for Cub3D - Handles both mandatory and bonus versions

# Project names
NAME = cub3d


# Directories
MANDATORY_DIR = mandatory
MLX_DIR = mlx
TEXTURES_DIR = textures
MAPS_DIR = maps

# Compilation
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -I$(MLX_DIR)
MLXFLAGS = -L$(MLX_DIR) -lmlx -lXext -lX11 -lm -lz -o3

# Source files
MANDATORY_SRCS = $(MANDATORY_DIR)/main.c \
	$(MANDATORY_DIR)/parse.c \
	$(MANDATORY_DIR)/parse_utils.c \
	$(MANDATORY_DIR)/parse_read.c \
	$(MANDATORY_DIR)/parse_finalize.c \
	$(MANDATORY_DIR)/parse_validate.c \
	$(MANDATORY_DIR)/ft_utils_a.c \
	$(MANDATORY_DIR)/ft_utils_b.c \
	$(MANDATORY_DIR)/textures.c \
	$(MANDATORY_DIR)/player.c \
	$(MANDATORY_DIR)/player_move.c \
	$(MANDATORY_DIR)/player_collision.c \
	$(MANDATORY_DIR)/raycast.c \
	$(MANDATORY_DIR)/raycast_column.c \
	$(MANDATORY_DIR)/raycast_utils.c \
	$(MANDATORY_DIR)/input.c \
	$(MANDATORY_DIR)/cleanup.c \
	$(MANDATORY_DIR)/floor.c
# Collect all bonus source files automatically


# Object files
MANDATORY_OBJS = $(MANDATORY_SRCS:.c=.o)


# MLX Library
MLX = $(MLX_DIR)/libmlx.a

# When building bonus, only link a small set of mandatory helper objects
MANDATORY_HELPERS_OBJS = mandatory/parse.o mandatory/textures.o mandatory/player.o

# Default target - builds mandatory version only
all: $(NAME)

# Build mandatory version (only essential raycasting features)
$(NAME): $(MLX) $(MANDATORY_OBJS)
	@echo "Building mandatory version..."
	@$(CC) $(MANDATORY_OBJS) $(MLXFLAGS) -o $(NAME)
	@echo "Mandatory version built successfully as $(NAME)"

# Force helper for always-rebuild targets
.PHONY: FORCE

# Compile object files
%.o: %.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Build MLX library
$(MLX):
	@echo "Building MLX library..."
	@make -C $(MLX_DIR) > /dev/null 2>&1 || true

# Clean object files
clean:
	@echo "Cleaning object files..."
	@rm -f $(MANDATORY_OBJS) $(BONUS_OBJS) || true
	@find . -name "*.o" -print -delete || true
	@make -C $(MLX_DIR) clean > /dev/null 2>&1 || true

# Full clean including executables
fclean: clean
	@echo "Full clean..."
	@rm -f $(NAME) $(NAME_BONUS)

# Rebuild everything
re: fclean all

# Rebuild bonus
re_bonus: fclean bonus

# Run mandatory version with default map
run: $(NAME)
	@echo "Running mandatory version..."
	./$(NAME) $(MAPS_DIR)/valid.cub


# Test mandatory version
test: $(NAME)
	@echo "Testing mandatory version..."
	./$(NAME) $(MAPS_DIR)/valid.cub

# Verify project structure
check:
	@echo "Checking project structure..."
	@test -d $(MLX_DIR) || (echo "Error: MLX directory not found" && exit 1)
	@test -d $(TEXTURES_DIR) || (echo "Error: Textures directory not found" && exit 1)
	@test -d $(MAPS_DIR) || (echo "Error: Maps directory not found" && exit 1)
	@test -f $(MANDATORY_SRCS) || (echo "Error: Mandatory source file not found" && exit 1)
	@echo "Project structure OK!"

# Install dependencies
install:
	@echo "Installing dependencies..."
	sudo apt-get update
	sudo apt-get install gcc make xorg libxext-dev libbsd-dev

# Show help
help:
	@echo "Cub3D - 42 School Project"
	@echo ""
	@echo "Available targets:"
	@echo "  all, make        - Build mandatory version (basic raycasting)"
	@echo "  bonus            - Build bonus version (all features)"
	@echo "  clean            - Clean object files"
	@echo "  fclean           - Full clean (objects + executables)"
	@echo "  re               - Rebuild mandatory"
	@echo "  re_bonus         - Rebuild bonus version"
	@echo "  run              - Run mandatory version"
	@echo "  run_bonus        - Run bonus version"
	@echo "  test             - Test mandatory version"
	@echo "  test_bonus       - Test bonus version"
	@echo "  check            - Verify project structure"
	@echo "  install          - Install basic dependencies"
	@echo "  install_bonus    - Install bonus dependencies (SDL2)"
	@echo "  help             - Show this help"
	@echo ""
	@echo "Mandatory features:"
	@echo "  - Raycasting engine"
	@echo "  - Wall textures (N, S, E, W)"
	@echo "  - Floor and ceiling colors/textures"
	@echo "  - WASD movement + arrow key rotation"
	@echo "  - .cub file parsing and validation"
	@echo ""
	@echo "Bonus features:"
	@echo "  - All mandatory features plus:"
	@echo "  - Mouse controls"
	@echo "  - Minimap"
	@echo "  - Weapons and shooting"
	@echo "  - Enemies with 2-shot health"
	@echo "  - Z-buffer depth sorting"
	@echo "  - Enhanced HUD"

.PHONY: all bonus clean fclean re re_bonus run run_bonus test test_bonus \
        check install install_bonus help FORCE