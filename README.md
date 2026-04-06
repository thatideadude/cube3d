# cub3D - My First Raycaster

## 📌 About the Project

**cub3D** is a 2D-to-3D rendering engine built in **C**. The goal is to create a dynamic "3D" view of a maze from a 1st-person perspective. Instead of using actual 3D objects or modern GPUs, this project uses the **Raycasting** algorithm to calculate wall distances and render them on a 2D screen using the **MiniLibX** library.

> "Raycasting is a rendering technique to create a 3D perspective in a 2D map."

## 🛠️ Features

* **Smooth 1st Person Movement**: Use `W`, `A`, `S`, and `D` to move and walk through the maze.
* **Rotation**: Use the `Left` and `Right` arrow keys to look around.
* **Textured Walls**: Different textures for North, South, East, and West faces of the walls.
* **Floor & Ceiling Colors**: Customizable colors parsed from the configuration file.
* **Map Parsing**: Validates `.cub` files, ensuring maps are closed and start positions are valid.
* **Window Management**: Clean exit on `ESC` or clicking the window's close button.

## 📁 Configuration File (.cub)

The program takes a scene description file as an argument. The file must follow these rules:

* **Textures**: `NO`, `SO`, `WE`, `EA` followed by the path to an `.xpm` file.
* **Colors**: `F` (Floor) and `C` (Ceiling) followed by RGB values (e.g., `220,100,0`).
* **Map Characters**:
* `1`: Wall
* `0`: Empty space
* `N`, `S`, `E`, `W`: Player starting position and orientation.


* **Constraints**: The map must be surrounded by walls.

Example:

```text
NO ./path_to_north_texture.xpm
SO ./path_to_south_texture.xpm
WE ./path_to_west_texture.xpm
EA ./path_to_east_texture.xpm

F 220,100,0
C 225,30,0

  111111
  100001
  101101
  100S01
  111111

```

## 🚀 Getting Started

### Prerequisites

* A Unix-based system (macOS or Linux).
* `gcc` or `clang` compiler.
* `make`.
* **MiniLibX** library dependencies (X11, etc.).

### Installation

1. Clone the repository:
```bash
git clone https://github.com/thatideadude/cub3d.git
cd cub3d

```


2. Compile the project:
```bash
make

```



### Running the Game

Run the executable with a valid map:

```bash
./cub3D maps/map.cub

```

## 🕹️ Controls

| Key | Action |
| --- | --- |
| **W** | Move Forward |
| **S** | Move Backward |
| **A** | Strafe Left |
| **D** | Strafe Right |
| **←** | Rotate Left |
| **→** | Rotate Right |
| **ESC** | Close Game |

## 📐 Technical Highlights: Raycasting

The engine works by casting a "ray" for every vertical strip of the screen.

1. **DDA Algorithm**: Used to find the distance between the player and the first wall hit by the ray.
2. **Fisheye Correction**: Adjusts the distance based on the camera angle to prevent the "bowl" distortion.
3. **Wall Height**: Calculated inversely proportional to the distance (farther = shorter).
4. **Texture Mapping**: Determining exactly which pixel of the texture corresponds to the hit point on the wall.

## 📜 License

This project was developed as part of the 42 curriculum. Please refer to the school's guidelines regarding code sharing and peer-to-peer learning.

Of course\! Here is the English version of that section and the conclusion of your README, maintaining a professional and technical tone.

-----

## 🤖 Use of AI

This project was developed with the assistance of Artificial Intelligence, specially in explaining the deeper mathematics of raycasting as a **specialized tutor in computer graphics and linear algebra**.

AI played a crucial role in two main areas:

### 1\. Demystifying Raycasting Mathematics

The concepts behind **cub3D** involve trigonometric and vector calculations that can be daunting at first. AI assisted in gaining a deep understanding of:

  * **DDA (Digital Differential Analyzer)**: Step-by-step explanations of how the algorithm traverses the map grid efficiently without skipping walls.
  * **Camera Vectors & Projection Planes**: Assistance in mathematically visualizing how to transform a direction vector into a Field of View (FOV).
  * **Collision Calculation**: Formulas to determine the perpendicular distance to avoid the "fisheye" lens distortion.

### 2\. Learning Rendering Techniques

To transform raw data into pixels, AI was used to study:

  * **Texture Buffering**: How to map $(x, y)$ coordinates from a `.xpm` texture onto a specific vertical strip of pixels on the screen.
  * **Performance Optimization**: Strategies to minimize MiniLibX overhead by using image buffers to render frames entirely before pushing them to the window.
  * **Logical Debugging**: Identifying memory leaks and edge cases during the parsing of `.cub` files.

> **Note:** All code was written and reviewed manually. AI explanations were used to ensure every line of mathematical logic was fully understood, respecting the **Norminette** rules and academic integrity.

-----

## 💡 Lessons Learned

  * **Memory Management**: Deep dive into dynamic allocation and preventing segmentation faults in C.
  * **Applied Trigonometry**: Practical application of sine and cosine functions for real-time movement and rotation.
  * **Data Structures**: Organizing complex structs to manage game states, images, and map data simultaneously.
  * **Graphics Pipeline**: Understanding the flow from raw data parsing to the final pixel display.

## 👥 Author

  * **Vinicius Moura** - [GitHub](https://github.com/vinimoura99)
  * **Marcelo Carvalho** - [GitHub](https://github.com/thatideadude)

-----




