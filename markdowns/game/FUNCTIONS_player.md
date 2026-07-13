# player.c — Function Breakdown

## Function: `player_moving(int keycode, void *param)`

```c
int	player_moving(int keycode, void *param)
{
	t_game	*game;

	game = param;
	if (keycode == XK_Escape)
		finish_game(game);
	handle_keys(keycode, &game->player);
	handle_minimap_keys(keycode, game);
	handle_speed_keys(keycode, game);
	return (0);
}
```

**Purpose**: X11 key press event handler. Called whenever any key is pressed.

**Parameters:**
- `keycode`: X11 key code (e.g., `XK_w`, `XK_Escape`).
- `param`: Game struct passed as context.

**Return Value:**
- `0` (required by MLX hook protocol).

**Logic:**

1. **Cast param to game** — Extract game pointer from void param.

2. **Handle Escape** — If ESC pressed, exit game immediately via `finish_game()`.

3. **Handle movement keys** — Call `handle_keys()` to update movement flags (WASD, arrows).

4. **Handle minimap keys** — Call `handle_minimap_keys()` to toggle/resize minimap (M, +/-, [/]).

5. **Handle speed keys** — Call `handle_speed_keys()` to adjust movement/rotation speed (1/2, Up/Down).

6. Return `0` to indicate key was processed.

---

## Function: `player_idle(int keycode, void *param)`

```c
int	player_idle(int keycode, void *param)
{
	t_game	*game;

	game = param;
	if (keycode == XK_w)
		game->player.up = false;
	else if (keycode == XK_s)
		game->player.down = false;
	else if (keycode == XK_a)
		game->player.left = false;
	else if (keycode == XK_d)
		game->player.right = false;
	else if (keycode == XK_Left)
		game->player.rotate_l = false;
	else if (keycode == XK_Right)
		game->player.rotate_r = false;
	return (0);
}
```

**Purpose**: X11 key release event handler. Called whenever any key is released.

**Parameters:**
- `keycode`: X11 key code.
- `param`: Game struct passed as context.

**Return Value:**
- `0` (required by MLX hook protocol).

**Logic:**

For each movement key, clear its corresponding player flag:
- **W** → `up = false`.
- **S** → `down = false`.
- **A** → `left = false`.
- **D** → `right = false`.
- **Left arrow** → `rotate_l = false`.
- **Right arrow** → `rotate_r = false`.

Speed and minimap keys don't have flags—they directly modify `game->movement_speed`, `game->rotation_speed`, and `game->minimap_radius_tiles` on press.

**Key State Pattern:**
- **Press** (via `player_moving()`): Set flag to `true`.
- **Release** (via `player_idle()`): Set flag to `false`.
- **Each frame** (via `move_player()` in render loop): Check flags and apply movement.

This allows smooth, continuous movement for held keys.

---

## Function: `set_player_dir_values(t_player *player, double dx, double dy)` (static)

```c
static void	set_player_dir_values(t_player *player, double dx, double dy)
{
	player->dir_x = dx;
	player->dir_y = dy;
}
```

**Purpose**: Set player's direction vector (where player is looking).

**Parameters:**
- `player`: Player struct to update.
- `dx`, `dy`: Direction components (typically -1, 0, or 1 for cardinal directions).

**Return Value:** None (void).

---

## Function: `set_player_plane_values(t_player *player, double px, double py)` (static)

```c
static void	set_player_plane_values(t_player *player, double px, double py)
{
	player->plane_x = px;
	player->plane_y = py;
}
```

**Purpose**: Set player's camera plane vector (perpendicular to direction, controls FOV).

**Parameters:**
- `player`: Player struct to update.
- `px`, `py`: Plane components (typically ±fov where fov ≈ 0.66).

**Return Value:** None (void).

---

## Function: `initialize_player(t_game *game, t_player *player)`

```c
void	initialize_player(t_game *game, t_player *player)
{
	player->player_x = game->map->start_x;
	player->player_y = game->map->start_y;
	if (player->starting_direction == NORTH)
	{
		set_player_dir_values(player, 0, -1);
		set_player_plane_values(player, game->fov, 0);
	}
	else if (player->starting_direction == SOUTH)
	{
		set_player_dir_values(player, 0, 1);
		set_player_plane_values(player, -game->fov, 0);
	}
	else if (player->starting_direction == EAST)
	{
		set_player_dir_values(player, 1, 0);
		set_player_plane_values(player, 0, game->fov);
	}
	else if (player->starting_direction == WEST)
	{
		set_player_dir_values(player, -1, 0);
		set_player_plane_values(player, 0, -game->fov);
	}
}
```

**Purpose**: Initialize player position and direction vectors based on map starting position and orientation.

**Parameters:**
- `game`: Game struct with map starting position/direction and FOV setting.
- `player`: Player struct to populate.

**Return Value:** None (void).

**Initialization Steps:**

1. **Set player position** — Place player at starting coordinates from map:
   - `player_x = map->start_x`
   - `player_y = map->start_y`
   - These are determined by the player starting position character (N/S/E/W) in the map.

2. **Set direction vectors based on starting direction**:

| Direction | Dir Vector | Plane Vector |
|-----------|-----------|--------------|
| **NORTH** | (0, -1) | (fov, 0) |
| **SOUTH** | (0, 1) | (-fov, 0) |
| **EAST** | (1, 0) | (0, fov) |
| **WEST** | (-1, 0) | (0, -fov) |

The plane vector is perpendicular to the direction and controls the camera's FOV (field of view).

**Example:**
```
Map starting position: (5, 10) with 'N' (North)
Result: player at (5, 10) looking up (dir=(0,-1)), camera plane to the right (plane=(0.66,0))
```

**Called By:**
- `mlx_main()` in `mlx.c` after window creation and image initialization.

**Input Dependency:**
- Requires `game->map->start_x/y` (set by `get_starting_info()` during parsing).
- Requires `player->starting_direction` (set by `get_starting_dir()` during parsing).
- Requires `game->fov` (set to 0.66 in `initialize_game()`).
