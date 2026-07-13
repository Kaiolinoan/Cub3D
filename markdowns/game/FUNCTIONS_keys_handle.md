# keys_handle.c — Function Breakdown

## Function: `handle_keys(int keycode, t_player *player)`

```c
void	handle_keys(int keycode, t_player *player)
{
	if (keycode == XK_w)
		player->up = true;
	else if (keycode == XK_s)
		player->down = true;
	else if (keycode == XK_a)
		player->left = true;
	else if (keycode == XK_d)
		player->right = true;
	else if (keycode == XK_Left)
		player->rotate_l = true;
	else if (keycode == XK_Right)
		player->rotate_r = true;
	if (keycode == XK_space)
	{
		if (player->mouse_flag == false)
			player->mouse_flag = true;
		else
			player->mouse_flag = false;
	}
}
```

**Purpose**: Handle movement and mouse control toggle key presses.

**Parameters:**
- `keycode`: X11 key code.
- `player`: Player struct with movement flags.

**Return Value:** None (void).

**Logic:**

1. **Movement keys** — Set corresponding movement flag to `true`:
   - **W** → `up = true` (move forward).
   - **S** → `down = true` (move backward).
   - **A** → `left = true` (strafe left).
   - **D** → `right = true` (strafe right).
   - **Left arrow** → `rotate_l = true` (turn left).
   - **Right arrow** → `rotate_r = true` (turn right).

2. **Mouse toggle** (Spacebar):
   - Toggle `mouse_flag` between `true` and `false`.
   - When true: Mouse movement controls rotation.
   - When false: Mouse movement is ignored.

**Movement Input Pattern:**
- On press: Set flag to `true` (via `handle_keys()`).
- On release: Set flag to `false` (via `player_idle()`).
- Every frame: Check flags and apply movement (via `move_player()`).

---

## Function: `handle_minimap_size(int keycode, t_game *game)` (static)

```c
static void	handle_minimap_size(int keycode, t_game *game)
{
	if (keycode == XK_plus || keycode == XK_equal)
		game->minimap_radius_tiles += 0.5;
	else if (keycode == XK_minus)
	{
		if (game->minimap_radius_tiles > 1.0)
			game->minimap_radius_tiles -= 0.5;
	}
}
```

**Purpose**: Adjust minimap visible radius (zoom out/in).

**Parameters:**
- `keycode`: X11 key code.
- `game`: Game struct with minimap settings.

**Return Value:** None (void).

**Logic:**

- **+ (Plus/Equal)** — Increase visible radius by 0.5 tiles (zoom out).
- **- (Minus)** — Decrease visible radius by 0.5 tiles (zoom in), but not below 1.0.

**Effect:**
- Larger radius = can see more of map on minimap.
- Smaller radius = detailed view of area near player.

---

## Function: `handle_minimap_zoom(int keycode, t_game *game)` (static)

```c
static void	handle_minimap_zoom(int keycode, t_game *game)
{
	double	radius_px;

	if (keycode == XK_bracketright)
	{
		if (game->minimap_tile_size < 40)
		{
			radius_px = game->minimap_radius_tiles * game->minimap_tile_size;
			game->minimap_tile_size += 2;
			game->minimap_radius_tiles = radius_px / game->minimap_tile_size;
		}
	}
	else if (keycode == XK_bracketleft)
	{
		if (game->minimap_tile_size > 4)
		{
			radius_px = game->minimap_radius_tiles * game->minimap_tile_size;
			game->minimap_tile_size -= 2;
			game->minimap_radius_tiles = radius_px / game->minimap_tile_size;
		}
	}
}
```

**Purpose**: Adjust minimap pixel scale (magnify/shrink display).

**Parameters:**
- `keycode`: X11 key code.
- `game`: Game struct with minimap settings.

**Return Value:** None (void).

**Logic:**

- **] (Right Bracket)** — Increase pixels-per-tile by 2 (magnify).
  - Capped at 40 pixels per tile.
  - Recalculate `radius_tiles` to maintain constant visible area (radius_px = constant).

- **[ (Left Bracket)** — Decrease pixels-per-tile by 2 (shrink).
  - Minimum 4 pixels per tile.
  - Recalculate radius to maintain constant visible area.

**Relationship:**
- `radius_pixels = radius_tiles * tile_size`
- These keys maintain constant radius in pixels while scaling tile size.
- Net effect: maintains visible map area while changing magnification.

---

## Function: `handle_minimap_keys(int keycode, t_game *game)`

```c
void	handle_minimap_keys(int keycode, t_game *game)
{
	if (keycode == XK_m)
		display_minimap(game);
	handle_minimap_size(keycode, game);
	handle_minimap_zoom(keycode, game);
}
```

**Purpose**: Handle all minimap-related key presses.

**Parameters:**
- `keycode`: X11 key code.
- `game`: Game struct.

**Return Value:** None (void).

**Logic:**

1. **M key** — Toggle minimap visibility (call `display_minimap()`).

2. **+/- keys** — Adjust visible radius (call `handle_minimap_size()`).

3. **[/] keys** — Adjust pixel scale (call `handle_minimap_zoom()`).

---

## Function: `handle_speed_keys(int keycode, t_game *game)`

```c
void	handle_speed_keys(int keycode, t_game *game)
{
	if (keycode == XK_1)
	{
		if (game->movement_speed > 0.01)
			game->movement_speed -= 0.01;
	}
	else if (keycode == XK_2)
	{
		if (game->movement_speed < 0.7)
			game->movement_speed += 0.01;
	}
	else if (keycode == XK_Down)
	{
		if (game->rotation_speed > 0.005)
			game->rotation_speed -= 0.005;
	}
	else if (keycode == XK_Up)
		game->rotation_speed += 0.005;
}
```

**Purpose**: Adjust player movement and rotation speed.

**Parameters:**
- `keycode`: X11 key code.
- `game`: Game struct with speed settings.

**Return Value:** None (void).

**Logic:**

- **1 (One)** — Decrease movement speed by 0.01 (minimum 0.01).
- **2 (Two)** — Increase movement speed by 0.01 (maximum 0.7).
- **Down Arrow** — Decrease rotation speed by 0.005 (minimum 0.005).
- **Up Arrow** — Increase rotation speed by 0.005 (no maximum).

**Default Speeds:**
- Movement: 0.07
- Rotation: 0.05

**Displayed on Screen:**
- HUD text in top-right corner shows current speeds (displayed by `display_speeds()`).
