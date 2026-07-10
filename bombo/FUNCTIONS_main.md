# main.c — Function Breakdown

## Function: `main(int argc, char **argv)`

```c
int	main(int argc, char **argv)
{
	t_game	*game;

	if (argc != 2)
		return (print_error("Invalid argument"), 1);
	game = initialize_game(argv[1]);
	if (!game)
		return (1);
	if (!check_if_map_is_valid(argv[1], game))
		return (clear_game(game), 1);
	// game->debug = true;
	mlx_main(game);
	clear_game(game);
	return (0);
}
```

**Purpose**: Program entry point. Validates command-line arguments, initializes the game state from a `.cub` map file, validates the map, runs the main game loop, and cleans up resources.

**Parameters:**
- `argc`: Argument count. Must be exactly 2 (program name + map file).
- `argv`: Argument vector. `argv[1]` should be the path to a `.cub` map file.

**Return Value:**
- `0` on successful exit.
- `1` on error (invalid arguments, initialization failure, or invalid map).

**Variables:**
- `game`: Pointer to the heap-allocated `t_game` struct containing all game state (map, player, sprites, textures, window, etc.).

**Line-by-line Logic:**

1. `if (argc != 2)` — Check that exactly one map file argument is provided. If not, print error and exit.

2. `game = initialize_game(argv[1])` — Parse the map file (`.cub`) and allocate/populate the `t_game` struct:
   - Reads map configuration (textures, colors, layout).
   - Initializes default game settings (minimap size, movement speed, FOV).
   - Sets up the map grid and player starting position.
   - Returns `NULL` on failure (errors printed inside `initialize_game`).

3. `if (!game) return (1);` — Early exit if initialization failed.

4. `if (!check_if_map_is_valid(...))` — Validate the parsed map:
   - Checks for exactly one player starting position (N/S/E/W).
   - Validates map characters (`0`, `1`, `D`, spaces).
   - Validates door placements.
   - Runs flood-fill to ensure map is completely surrounded by walls.
   - Returns `false` on validation failure.

5. `// game->debug = true;` — Commented debug toggle. When uncommented, enables debug mode (top-down 2D view instead of raycaster).

6. `mlx_main(game)` — Initialize MLX (graphics library):
   - Creates window and image buffers.
   - Sets up keyboard/mouse hooks.
   - Starts the render loop (blocks until window closes or `finish_game()` calls `exit()`).

7. `clear_game(game)` — Free all allocated memory (maps, images, sprites, window, MLX context).
   - **Note**: This line is typically dead code because `mlx_loop()` in `mlx_main` blocks and calls `exit()` via `finish_game()`, so `clear_game()` here never executes. Real cleanup is in `finish_game()` in `mlx.c`.

8. `return (0)` — Normal exit code (unreachable in practice).

**Call Chain:**
```
main()
  ├─ print_error()         [error reporting]
  ├─ initialize_game()     [parsing, allocation]
  ├─ check_if_map_is_valid() [validation]
  │   ├─ check_extension()
  │   ├─ check_maze_chars()
  │   ├─ check_doors()
  │   ├─ get_starting_info()
  │   └─ flood_fill_prep()
  ├─ mlx_main()            [graphics & main loop]
  └─ clear_game()          [cleanup]
```

**Key Notes:**
- The map file (`.cub`) contains 6 elements: NO, SO, WE, EA (texture paths), F (floor color), C (ceiling color), followed by the map layout.
- Player spawn position is determined by finding N/S/E/W characters in the map grid.
- If `game->debug` is enabled, the game runs in a top-down 2D visualization mode instead of 3D raycasting.
