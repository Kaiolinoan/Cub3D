# mlx.c — Function Breakdown

## Function: `finish_game(void *param)`

```c
int	finish_game(void *param)
{
	t_game	*game;

	game = param;
	clear_game(game);
	exit(EXIT_SUCCESS);
	return (0);
}
```

**Purpose**: Clean shutdown handler. Frees all resources and exits the program.

**Parameters:**
- `param`: Game struct passed as context from MLX hooks.

**Return Value:**
- `0` (unreachable since `exit()` terminates process).

**Logic:**

1. **Cast param to game** — Extract game pointer.

2. **Free all resources** — Call `clear_game()` to deallocate:
   - Graphics buffers and images.
   - Textures and sprite paths.
   - Map data and player state.
   - MLX window and display context.

3. **Exit process** — `exit(EXIT_SUCCESS)` terminates with status 0.

**Called By:**
- ESC key handler in `player_moving()`.
- Window close button (via MLX `DestroyNotify` hook).

---

## Function: `mouse_move(int x, int y, void *param)`

```c
int	mouse_move(int x, int y, void *param)
{
	t_game	*game;
	int		delta_x;
	int		center_x;
	int		center_y;

	game = param;
	center_x = game->win_w / 2;
	center_y = game->win_h / 2;
	if ((x == center_x && y == center_y) || (game->player.mouse_flag == false))
		return (0);
	delta_x = x - center_x;
	rotate(&game->player, delta_x * game->rotation_speed * 0.01);
	mlx_mouse_move(game->mlx, game->win, center_x, center_y);
	mlx_mouse_hide(game->mlx, game->win);
	return (0);
}
```

**Purpose**: Mouse movement event handler. Rotates player view based on mouse position.

**Parameters:**
- `x`, `y`: Current mouse coordinates on screen.
- `param`: Game struct passed as context.

**Return Value:**
- `0` (required by MLX hook protocol).

**Logic:**

1. **Calculate screen center** — `center_x = win_w / 2`, `center_y = win_h / 2`.

2. **Early exit conditions**:
   - If mouse is at screen center → no rotation needed.
   - If `mouse_flag == false` → mouse control disabled (toggle with Spacebar).

3. **Compute delta** — `delta_x = x - center_x` (horizontal distance from center).

4. **Rotate player** — Rotate by `delta_x * rotation_speed * 0.01`:
   - Scaled down by 0.01 to make rotation less sensitive than keyboard.
   - Positive delta (mouse right) → rotate right.
   - Negative delta (mouse left) → rotate left.

5. **Center mouse** — Call `mlx_mouse_move()` to warp mouse back to screen center.

6. **Hide mouse** — Call `mlx_mouse_hide()` to hide cursor for immersion.

**Mouse Control Toggle:**
- Spacebar toggles `game->player.mouse_flag`.
- When disabled, mouse movement is ignored.

---

## Function: `mlx_main(t_game *game)`

```c
void	mlx_main(t_game *game)
{
	game->mlx = mlx_init();
	if (!game->mlx)
		return (print_error(MLX), clear_game(game));
	if (game->debug)
	{
		game->win_w = ft_strlen(*game->map->grid) * 64;
		game->win_h = array_len(game->map->grid) * 64;
	}
	else
		mlx_get_screen_size(game->mlx, &game->win_w, &game->win_h);
	game->win = mlx_new_window(game->mlx, game->win_w, game->win_h, "Cub3D");
	if (!game->win)
		return (print_error(MLX_WIN), clear_game(game));
	if (!initialize_images(game, &game->sprites))
		return ;
	initialize_player(game, &game->player);
	init_door_audio();
	mlx_hook(game->win, KeyPress, KeyPressMask, player_moving, game);
	mlx_hook(game->win, KeyRelease, KeyReleaseMask, player_idle, game);
	mlx_hook(game->win, 6, PointerMotionMask, mouse_move, game);
	mlx_hook(game->win, DestroyNotify, StructureNotifyMask, finish_game, game);
	mlx_loop_hook(game->mlx, &render, game);
	mlx_loop(game->mlx);
}
```

**Purpose**: Master initialization function for graphics, window, input hooks, and main render loop.

**Parameters:**
- `game`: Game struct with all game state.

**Return Value:** None (void). Blocks until window closes (via `mlx_loop()`).

**Initialization Steps:**

1. **Initialize MLX** — `mlx_init()` initializes X11/graphics context.

2. **Determine window size**:
   - If debug mode: Calculate from map dimensions `(map_width * 64, map_height * 64)`.
   - Else: Get screen resolution via `mlx_get_screen_size()`.

3. **Create window** — `mlx_new_window()` creates a window with title "Cub3D".

4. **Initialize graphics** — `initialize_images()`:
   - Creates backbuffer image.
   - Loads all wall textures (NO, SO, WE, EA).
   - Loads debug squares.
   - Loads door animation frames.

5. **Initialize player** — `initialize_player()` sets position and direction.

6. **Initialize audio** — `init_door_audio()` sets up sound system for door SFX.

7. **Register input hooks**:
   - **KeyPress** → `player_moving()` (set movement flags).
   - **KeyRelease** → `player_idle()` (clear movement flags).
   - **PointerMotion** (mouse event 6) → `mouse_move()` (handle mouse look).
   - **DestroyNotify** (window close) → `finish_game()` (cleanup and exit).

8. **Register render loop** — `mlx_loop_hook()` registers `render()` to be called every frame.

9. **Start main loop** — `mlx_loop()` blocks and runs event/render loop until window closes.

**Program Flow:**
```
mlx_loop() (blocking)
  ├─ (keyboard/mouse events) → call registered hooks
  ├─ (every frame) → call render()
  │   ├─ process movement/rotation
  │   ├─ raycasting/drawing
  │   └─ display frame
  └─ (on window close) → finish_game() → exit()
```

**Error Handling:**
- If MLX init or window creation fails, cleanup via `clear_game()` and return.
- If image initialization fails, return silently (error logged in sub-functions).
