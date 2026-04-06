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
	$(MANDATORY_DIR)/floor.c \
	$(MANDATORY_DIR)/utils.c \

# Object files
MANDATORY_OBJS = $(MANDATORY_SRCS:.c=.o)

# MLX Library
$(MLX_DIR)/libmlx.a:
		if [ ! -d "$(MLX_DIR)" ]; then \
			git clone https://github.com/42Paris/minilibx-linux.git $(MLX_DIR); \
		fi; \
		$(MAKE) -C $(MLX_DIR); \


MANDATORY_HELPERS_OBJS = mandatory/parse.o mandatory/textures.o mandatory/player.o

# Default target - builds mandatory version only
all: $(MLX) $(NAME)


# Build mandatory version (only essential raycasting features)
$(NAME):$(MLX) $(MANDATORY_OBJS)
	@echo "Building mandatory version..."
	@$(CC) $(MANDATORY_OBJS) $(MLXFLAGS) -o $(NAME)
	@echo "Mandatory version built successfully as $(NAME)"

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
	@rm -f $(MANDATORY_OBJS) || true
	@find . -name "*.o" -print -delete || true
	@make -C $(MLX_DIR) clean > /dev/null 2>&1 || true

# Full clean including executables
fclean: clean
	@echo "Full clean..."
	@rm -f $(NAME)
	@rm -rf mlx
	

# Rebuild everything
re: fclean all
