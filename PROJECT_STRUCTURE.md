# Cub3D - Project Structure

This project is organized according to 42 School requirements, separating mandatory and bonus features.

## Directory Structure

```
Cub3d/
├── Makefile                    # Master makefile for both versions
├── PROJECT_STRUCTURE.md        # This documentation
├── README.md                   # Main project documentation
├── mandatory/                  # Mandatory version (essential features only)
│   ├── cub3d_mandatory.c      # Core raycasting implementation
│   ├── cub3d_mandatory.h      # Header with mandatory structures
│   └── Makefile               # Makefile for mandatory version
├── bonus/                      # Bonus version (all features)
│   ├── cub3d_bonus.c          # Full-featured implementation
│   ├── cub3d_bonus.h          # Header with all structures
│   └── Makefile               # Makefile for bonus version
├── maps/                       # Shared map files
│   ├── valid.cub              # Valid test map
│   └── valid2.cub             # Another valid test map
├── textures/                   # Shared texture files
│   ├── north.xpm              # North wall texture
│   ├── south.xpm              # South wall texture
│   ├── east.xpm               # East wall texture
│   ├── west.xpm               # West wall texture
│   ├── enemy.xpm              # Enemy texture (bonus only)
│   ├── weapon_*.xpm           # Weapon textures (bonus only)
│   └── ...
├── mlx/                        # MLX graphics library
└── archive/                    # Archived development files
    └── old_development/        # Old test and development files
```

## Build Commands

### Mandatory Version (Basic Requirements)
```bash
make                    # Build mandatory version
make mandatory          # Same as above
make test               # Build and test with default map
make run                # Build and run with default map
```

### Bonus Version (Extended Features)
```bash
make bonus              # Build bonus version
make test_bonus         # Build and test bonus version
make run_bonus          # Build and run bonus version
```

### Both Versions
```bash
make both               # Build both versions
make clean              # Clean object files
make fclean             # Clean everything including executables
make re                 # Rebuild mandatory
make re_bonus           # Rebuild bonus
```

### Dependencies
```bash
make install            # Install basic dependencies (MLX, X11)
make install_bonus      # Install bonus dependencies (SDL2)
```

## Features

### Mandatory Features ✅
- **Raycasting Engine**: 3D rendering using raycasting algorithm
- **Wall Textures**: Support for N, S, E, W wall textures
- **Floor/Ceiling Colors**: Configurable floor and ceiling colors
- **Movement**: WASD movement with collision detection
- **Rotation**: Arrow key rotation controls
- **Map Parsing**: .cub file parsing and validation
- **Map Validation**: Ensures map is surrounded by walls and has single player

### Bonus Features ✅
- **All mandatory features plus:**
- **Mouse Controls**: Mouse look and movement
- **Minimap**: Real-time minimap display
- **Weapons**: Weapon system with shooting mechanics
- **Enemies**: Enemy entities with AI
- **Audio**: Sound effects using SDL2

## Executables

- `cub3d`: Mandatory version (lightweight, basic features)
- `cub3d_bonus`: Bonus version (full-featured)

## Dependencies

### Mandatory Version
- MLX (MiniLibX)
- X11 libraries
- Math library
- Standard C libraries

### Bonus Version
- All mandatory dependencies plus:
- SDL2 (audio support)
- SDL2_mixer (sound mixing)

## Usage

```bash
# Run mandatory version
./cub3d maps/valid.cub

# Run bonus version  
./cub3d_bonus maps/valid.cub
```

## Controls

### Mandatory Controls
- **W**: Move forward
- **S**: Move backward
- **A**: Strafe left
- **D**: Strafe right
- **←**: Rotate left
- **→**: Rotate right
- **ESC**: Exit

### Bonus Controls (Additional)
- **Mouse**: Look around
- **Space**: Shoot (when weapon available)
- **Tab**: Toggle minimap
- All mandatory controls

## Map Format

The .cub files define the game world using a specific format:
- `NO`, `SO`, `WE`, `EA`: Texture paths for walls
- `F`, `C`: Floor and ceiling colors (RGB)
- Map grid with walls (1), empty space (0), and player start (N/S/E/W)

## Compilation Flags

### Mandatory
- `-Wall -Wextra -Werror`: Strict compilation
- `-lmlx -lXext -lX11 -lm -lz`: MLX and math libraries

### Bonus
- All mandatory flags plus:
- `-lSDL2 -lSDL2_mixer`: SDL2 audio libraries

---

This structure ensures compliance with 42 School evaluation requirements while maintaining clean code organization and easy testing of both mandatory and bonus features.
