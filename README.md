# 🧊 Cub3D

*This project has been created as part of the 42 curriculum by [kemontei](https://profile.intra.42.fr/users/kemontei), [klino-an](https://profile.intra.42.fr/users/klino-an).*

[![42](https://img.shields.io/badge/42-Project-000000?style=flat-square&logo=42&logoColor=white)](https://42.fr)
[![Language](https://img.shields.io/badge/C-00599C?style=flat-square&logo=c&logoColor=white)](#-description)
[![MinilibX](https://img.shields.io/badge/MiniLibX-Linux-FCC624?style=flat-square&logo=linux&logoColor=black)](#%EF%B8%8F-instructions)
[![Norminette](https://img.shields.io/badge/42-Norminette-blue?style=flat-square)](#-description)
[![Platform](https://img.shields.io/badge/Platform-Linux-lightgrey?style=flat-square&logo=linux)](#%EF%B8%8F-instructions)

> A small first-person 3D engine written in C from scratch, powered by **ray-casting**, **MiniLibX**, and a healthy obsession with **Wolfenstein 3D**.

---

## 📖 Description

**Cub3D** is a graphical project of the 42 curriculum inspired by the legendary *Wolfenstein 3D* (1992, id Software).  
The goal is to build a **real-time ray-casting engine** in C, capable of rendering a pseudo-3D view of a maze described by a 2D textual map file (`.cub`).

You walk through a labyrinth whose four cardinal walls are **textured** with your own `.xpm` sprites, with customisable **floor** and **ceiling colors**, **animated doors** with sound effects, a **circular minimap** showing your point of view, and intuitive mouse + keyboard controls.

### 🧠 Core concepts

* **Ray-casting** using the **DDA algorithm** (Digital Differential Analyser) — one ray per screen column to find the closest wall.
* **Texture mapping** for the four cardinal directions (N, S, E, W) using the perpendicular wall distance to compute the texture column.
* **Camera-plane math** (`dir` + `plane` vectors) computing the player's FOV and perspective projection.
* **Event-driven loop** with MiniLibX hooks (`KeyPress`, `KeyRelease`, `MotionNotify`, `DestroyNotify`) driving a `mlx_loop_hook` frame update.
* **Map parser + flood-fill** to guarantee the maze is closed before the game starts.
* **Finite state machine** for the doors (CLOSED → OPENING → OPEN → CLOSING) with sound cues.

### ✨ Features

| | |
|---|---|
| 🧱 | **Textured N/S/E/W walls** loaded from your own `.xpm` files |
| 🌅 | **Custom floor & ceiling colors** parsed from the map |
| 🚪 | **Animated doors** (14 frames) with proximity-triggered open/close |
| 🔊 | **WAV sound effects** on door open / door close |
| 🗺️ | **Circular minimap** showing walls, doors, the player, and a real-time **FOV cone** |
| 🖱️ | **Mouse-look** toggle for horizontal rotation |
| ⌨️ | **Adjustable movement / rotation speed** with on-screen HUD digits |
| 🎨 | **Distance-based color gradient** — darker walls as they recede |
| 🐛 | **Debug mode** rendering the 2D top-down map and ray directions |
| ✅ | **Strict 42 parser**: flood-fill validation, RGB range check, texture & door integrity |

---

## 🎮 Controls

| Key / action | Effect |
|---|---|
| `W` `A` `S` `D` | Move forward / strafe left / back / strafe right |
| `←`  / `→` | Rotate left / right |
| `↑`  / `↓` | Increase / decrease rotation speed |
| `1`  / `2` | Decrease / increase movement speed |
| `Mouse` *(move)* | Look around (after `Space`) |
| `Space` | Toggle mouse-look on / off |
| `M` | Toggle the minimap on / off |
| `+` / `-` | Increase / decrease minimap radius (in tiles) |
| `]` / `[` | Zoom in / out the minimap tile size |
| `Esc` (or close window) | Quit the game |

A small on-screen **HUD** drawn with the bundled bitmap-font renderer displays the active **movement speed** and **rotation speed** in real time.

---

## 🗺️ Map format (`.cub` files)

A `.cub` map is composed of two parts: a header of texture / color directives, then the maze grid.

### Texture & color directives

| Identifier | Meaning | Example |
|:---:|---|---|
| `NO` | Path to the **North** wall texture (XPM) | `NO assets/brick.xpm` |
| `SO` | Path to the **South** wall texture | `SO assets/brick.xpm` |
| `WE` | Path to the **West** wall texture | `WE assets/brick.xpm` |
| `EA` | Path to the **East** wall texture | `EA assets/brick.xpm` |
| `F` `R,G,B` | **Floor** color (0–255) | `F 96,171,68` |
| `C` `R,G,B` | **Ceiling** color (0–255) | `C 88,143,255` |

### Grid characters

| Char | Meaning |
|:---:|---|
| `1`  | Wall |
| `0`  | Empty / walkable space |
| `N S E W` | Player start position and facing direction |
| `D`  | Door (animated; opens when the player gets close, closes when away) |
| ` ` *(space)* | Outside the playable area |

The parser runs a **flood-fill** from the player spawn to guarantee the map is closed (no leaks).

### Example (`maps/map1.cub`)

```text
NO assets/brick.xpm
SO assets/brick.xpm
WE assets/brick.xpm
EA assets/brick.xpm
C 88,143,255
F 96,171,68
11111111111111
10000000000001
10000010000001
100000N0000001
11111111111111
```

---

## 🛠️ Instructions

### Requirements

* **Linux** (Ubuntu / Debian family recommended — MiniLibX is bundled for Linux).
* `cc` / `gcc` and `make`.
* X11 development libraries:
  ```bash
  sudo apt install libx11-dev libxext-dev zlib1g-dev
  ```
* The Makefile clones `MiniLibX` from the official 42 repository on first build.

### Clone & build

```bash
git clone https://github.com/Kaiolinoan/Cub3D.git
cd Cub3D
make        # builds ./cub3d and ./libs/Libft/libft.a
```

> 💡 If `MiniLibX` is missing, run `make mlx` to (re)clone it, then `make` again.

### Run

```bash
./cub3d maps/map1.cub
```

The `maps/` folder ships with several playable maps: `map1.cub`, `map2.cub`, `map3.cub`, plus a small `test.cub`.

### Clean & rebuild

```bash
make clean   # remove object files only
make fclean  # remove objects + the cub3d binary
make re      # full rebuild from scratch
```

### Valgrind (memory-leak check)

```bash
make v       # rebuilds and runs Valgrind on maps/map1.cub
```

This is a convenience target that launches Valgrind with `--leak-check=full --track-origins=yes --track-fds=yes`.

---

## 🧩 Project layout

```
Cub3D/
├── Makefile
├── includes/          # cub3d.h + structs.h
├── srcs/
│   ├── main.c
│   ├── parsing/       # *.cub parser, flood-fill, RGB checks, elements
│   ├── game/          # raycasting, minimap, doors, movement, render
│   │   └── minimap/   # minimap (player, FOV, utils)
│   └── utils/         # libft-style helpers + HUD text renderer
├── maps/              # *.cub demo maps
├── assets/            # .xpm textures + .wav door sounds
├── libs/
│   ├── Libft/         # 42 libft
│   └── minilibx-linux/
└── markdowns/         # extra function-by-function documentation
```

---

## 📚 Resources

### 📖 Documentation & references

* [Lode's Computer Graphics Tutorial — Raycasting](https://lodev.org/cgtutor/raycasting.html)  
  The canonical reference for DDA ray-casting on a 2D grid: wall projection, texture mapping, camera plane.
* [Fran Byte — Ray-Casting Tutorial](https://42-fran-byte-f94097.gitlab.io/docs/cub3d/cub3d-approach-en/#/)  
  Classic walk-through of the technique used in Wolfenstein 3D.
* [MiniLibX documentation (42)](https://harm-smits.github.io/42docs/libs/minilibx.html)  
  Hooks, image buffers, window management.
  Reverse-engineering of the original id Software ray-caster.
* [Flood-fill algorithm (Wikipedia)](https://en.wikipedia.org/wiki/Flood_fill)  
  Used by the parser to ensure every map is closed.
* [42 subject page — Cub3D](https://projects.intra.42.fr/projects/cub3d)  
  Official Cub3D rules and expected behavior (linked from your intra).

### 🤖 AI usage

Generative AI was used as an **assistant** on specific, scoped tasks — not to generate the project end-to-end:

* **Math & concept explanation** — clarifying the DDA algorithm, the camera-plane vector math, perspective projection, and the door ray-casting logic.
* **Debugging** — diagnosing subtle bugs (texture-sampling artifacts, off-by-one errors in wall columns, minimap FOV alignment, leaks shown by Valgrind).
* **Refactoring suggestions** — splitting the minimap module into smaller files (`minimap_main.c`, `minimap_player.c`, `minimap_fov.c`, `minimap_fov_utils.c`, `utils.c`) and surfacing code smells.
* **Documentation generation** — producing the function-level Markdowns under `markdowns/` (`FUNCTIONS_*.md`, `cub3d_files_breakdown.md`, etc.) from the codebase.
* **Code review** — acting as a second pair of eyes on Norminette violations and `-Wall -Wextra -Werror` regressions.
* **Readme creation**

All architectural decisions, the implementation of the **ray-caster**, the **parser**, the **door state machine**, and the final code were written and verified manually.

---

## 👥 Authors

* [**kemontei**](https://profile.intra.42.fr/users/kemontei) — `@kemontei@student.42lisboa.com`
* [**klino-an**](https://profile.intra.42.fr/users/klino-an) — `@klino-an@student.42lisboa.com`

---

⭐ If this project inspired yours, feel free to fork it and experiment with your own textures, bigger maps, animated sprites, or a minimap renderer of your own!
