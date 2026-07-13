# Player.c Breakdown — Input Handling & Movement

This file contains all player input handling, movement, and camera rotation logic.

---

## File Overview

The player system manages:
1. **Input handling** — Capturing keyboard events (W/A/S/D for movement, arrows for rotation)
2. **Movement logic** — Moving player forward/backward, strafing left/right
3. **Camera rotation** — Rotating the viewing direction and FOV plane
4. **Player initialization** — Setting up player position and viewing angle based on map starting point

---

## Function: `player_moving(int keycode, void *param)`

```c
int	player_moving(int keycode, void *param)
{
	t_game	*game;

	game = param;
	if (keycode == XK_w)
		game->player.up = true;
	else if (keycode == XK_s)
		game->player.down = true;
	else if (keycode == XK_a)
		game->player.left = true;
	else if (keycode == XK_d)
		game->player.right = true;
	else if (keycode == XK_Left)
		game->player.rotate_l = true;
	else if (keycode == XK_Right)
		game->player.rotate_r = true;
	else if (keycode == XK_Escape)
		finish_game(game);
	else if (keycode == XK_m)
		display_minimap(game);
	return (0);
}
```

**Purpose**: Called when a key is pressed. Sets movement/rotation flags to `true`.

**Variables explained:**
- `keycode`: The X11 key code representing which key was pressed.
- `param`: Pointer to the game state (cast to `t_game *`).
- `game->player.up/down/left/right`: Boolean flags tracking active movement directions.
- `game->player.rotate_l/rotate_r`: Boolean flags for left/right rotation.

**Key codes:**
- `XK_w` = 'W' key (move forward in viewing direction)
- `XK_s` = 'S' key (move backward)
- `XK_a` = 'A' key (strafe left)
- `XK_d` = 'D' key (strafe right)
- `XK_Left` = Left arrow (rotate counter-clockwise)
- `XK_Right` = Right arrow (rotate clockwise)
- `XK_Escape` = Exit game
- `XK_m` = Toggle minimap display

**Algorithm:**
1. Receive key press event.
2. Match key code to player action.
3. Set corresponding flag to `true`.
4. Return 0 (success).

**Used by**: MLX hook system (called every time a key is pressed).

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

**Purpose**: Called when a key is released. Sets movement/rotation flags to `false`.

**Variables explained:**
- Same as `player_moving()`, but sets flags to `false` instead of `true`.

**Algorithm:**
1. Receive key release event.
2. Match key code to player action.
3. Set corresponding flag to `false`.
4. Return 0 (success).

**Important design note:** The flags persist between frames. This allows **smooth continuous movement**:
- Press 'W' → `up = true` → continues until key release
- Release 'W' → `up = false` → movement stops
- Multiple keys can be pressed simultaneously (e.g., W+D for forward-right diagonal movement)

**Used by**: MLX hook system (called every time a key is released).

---

## Function: `rotate(t_player *player, double rot)`

```c
void	rotate(t_player *player, double rot)
{
	double	old_dir_x;
	double	old_dir_y;
	double	old_plane_x;
	double	old_plane_y;

	old_dir_x = player->dir_x;
	old_dir_y = player->dir_y;
	old_plane_x = player->plane_x;
	old_plane_y = player->plane_y;
	player->dir_x = old_dir_x * cos(rot) - old_dir_y * sin(rot);
	player->dir_y = old_dir_x * sin(rot) + old_dir_y * cos(rot);
	player->plane_x = old_plane_x * cos(rot) - old_plane_y * sin(rot);
	player->plane_y = old_plane_x * sin(rot) + old_plane_y * cos(rot);
}
```

**Purpose**: Rotate both the viewing direction and FOV plane by angle `rot` (in radians).

**Variables explained:**
- `rot`: Rotation angle in **radians** (positive = counter-clockwise, negative = clockwise).
- `old_dir_x/y`: Backup of current direction before rotation.
- `old_plane_x/y`: Backup of current FOV plane before rotation.

**Algorithm (2D Rotation Matrix):**

The rotation uses the standard 2D rotation formula:
```
new_x = old_x * cos(angle) - old_y * sin(angle)
new_y = old_x * sin(angle) + old_y * cos(angle)
```

This rotates both vectors by the same angle, maintaining their perpendicular relationship.

**Example:**
- Player facing north: `dir = (0, -1)`, `plane = (0.66, 0)`
- Rotate 90° counter-clockwise (`rot = π/2`):
  - `dir_x = 0 * cos(π/2) - (-1) * sin(π/2) = 0 * 0 - (-1) * 1 = 1`
  - `dir_y = 0 * sin(π/2) + (-1) * cos(π/2) = 0 * 1 + (-1) * 0 = 0`
  - Result: `dir = (1, 0)` (now facing east) ✓

**Used by**: `move_player()` when rotation flags are set.

---

## Function: `move_player(t_player *player)`

```c
void	move_player(t_player *player)
{
	double	speed;
	double	rot_speed;

	speed = 0.1;
	rot_speed = 0.03;
	if (player->up)
	{
		player->player_x += player->dir_x * speed;
		player->player_y += player->dir_y * speed;
	}
	if (player->down)
	{
		player->player_x -= player->dir_x * speed;
		player->player_y -= player->dir_y * speed;
	}
	if (player->left)
	{
		player->player_x -= player->plane_x * speed;
		player->player_y -= player->plane_y * speed;
	}
	if (player->right)
	{
		player->player_x += player->plane_x * speed;
		player->player_y += player->plane_y * speed;
	}
	if (player->rotate_l)
		rotate(player, -rot_speed);
	if (player->rotate_r)
		rotate(player, rot_speed);
}
```

**Purpose**: Update player position and rotation based on active movement flags. Called every frame.

**Variables explained:**
- `speed = 0.1`: Movement speed in world units per frame (tiles/frame).
- `rot_speed = 0.03`: Rotation speed in radians per frame (≈1.7° per frame).
- `player->up/down/left/right`: Movement flags set by `player_moving()` and cleared by `player_idle()`.
- `player->rotate_l/r`: Rotation flags.

**Movement calculations:**

**Forward/Backward** (W/S keys):
```c
if (player->up)
{
    player->player_x += player->dir_x * speed;  // Move in viewing direction
    player->player_y += player->dir_y * speed;
}
```
- Multiplies the unit direction vector by speed to move the player.
- Example: If `dir = (0.7, -0.7)` and `speed = 0.1`, move by `(0.07, -0.07)` tiles.

**Left/Right Strafing** (A/D keys):
```c
if (player->left)
{
    player->player_x -= player->plane_x * speed;  // Move perpendicular to viewing direction
    player->player_y -= player->plane_y * speed;
}
```
- Multiplies the perpendicular plane vector by speed.
- Plane is always perpendicular to direction, so this moves left/right relative to camera.

**Rotation** (Arrow keys):
```c
if (player->rotate_l)
    rotate(player, -rot_speed);  // Rotate counter-clockwise
if (player->rotate_r)
    rotate(player, rot_speed);   // Rotate clockwise
```
- Calls `rotate()` function with positive/negative angle.

**Important:** This function does **NOT** check for collisions (wall avoidance). That's typically handled elsewhere.

**Algorithm:**
1. Check each movement flag.
2. If flag is set, update `player_x/y` by moving along appropriate vector.
3. Check each rotation flag.
4. If flag is set, call `rotate()` with appropriate angle sign.

**Used by**: `render()` function (called every frame before raycasting).

---

## Function: `initialize_player(t_game *game, t_player *player)`

```c
void	initialize_player(t_game *game, t_player *player)
{
	player->player_x = game->map->start_x + 0.5;
	player->player_y = game->map->start_y + 0.5;
	if (player->starting_direction == NORTH)
	{
		player->dir_x = 0;
		player->dir_y = -1;
		player->plane_x = 0.66;
		player->plane_y = 0;
	}
	if (player->starting_direction == SOUTH)
	{
		player->dir_x = 0;
		player->dir_y = 1;
		player->plane_x = -0.66;
		player->plane_y = 0;
	}
	if (player->starting_direction == EAST)
	{
		player->dir_x = 1;
		player->dir_y = 0;
		player->plane_x = 0;
		player->plane_y = 0.66;
	}
	if (player->starting_direction == WEST)
	{
		player->dir_x = -1;
		player->dir_y = 0;
		player->plane_x = 0;
		player->plane_y = -0.66;
	}
}
```

**Purpose**: Set up player position, viewing direction, and FOV plane based on map spawn point and starting direction.

**Variables explained:**
- `game->map->start_x/y`: Map coordinates of spawn point (from parsing the .cub file).
- `player->player_x/y`: Player's world position, initialized to center of spawn tile (+0.5 offset).
- `player->starting_direction`: Enum value (NORTH, SOUTH, EAST, WEST) from map file.
- `player->dir_x/y`: Normalized direction vector pointing where player looks.
- `player->plane_x/y`: Perpendicular vector representing FOV width (always 0.66 magnitude).

**Direction setup (one of four cases):**

| Direction | `dir` (normalized) | `plane` (perpendicular) | Visual |
|-----------|-------------------|------------------------|--------|
| NORTH     | (0, -1)           | (0.66, 0)              | Looking up, FOV to right |
| SOUTH     | (0, 1)            | (-0.66, 0)             | Looking down, FOV to left |
| EAST      | (1, 0)            | (0, 0.66)              | Looking right, FOV up |
| WEST      | (-1, 0)           | (0, -0.66)             | Looking left, FOV down |

**Key observations:**

1. **Direction is always unit length** (magnitude = 1).
   - `NORTH`: `√(0² + (-1)²) = 1` ✓
   - `EAST`: `√(1² + 0²) = 1` ✓

2. **Plane is always perpendicular to direction** (dot product = 0).
   - `NORTH` and its plane: `(0, -1) · (0.66, 0) = 0 + 0 = 0` ✓
   - `EAST` and its plane: `(1, 0) · (0, 0.66) = 0 + 0 = 0` ✓

3. **Plane magnitude = 0.66** (controls FOV width).
   - Larger values = wider FOV
   - See [OFFSET_EXPLANATION.md](OFFSET_EXPLANATION.md) for FOV adjustments

4. **Player starts at center of spawn tile** (+0.5 offset).
   - Example: If spawn is at tile (5, 3), player starts at world (5.5, 3.5)

**Algorithm:**
1. Get spawn coordinates from map (saved during map parsing).
2. Center player in that tile (+0.5 offset).
3. Match `starting_direction` enum.
4. Set corresponding `dir` and `plane` vectors.

**Used by**: Game initialization (called once at startup).

---

## Player Structure (Expected t_player)

Based on function usage, the player structure should have:

```c
typedef struct s_player
{
	// Position and Direction
	double	player_x;           // Player X coordinate (world tiles)
	double	player_y;           // Player Y coordinate (world tiles)
	double	dir_x;              // Viewing direction X (normalized)
	double	dir_y;              // Viewing direction Y (normalized)
	double	plane_x;            // FOV plane X (normalized, perpendicular to dir)
	double	plane_y;            // FOV plane Y (normalized, perpendicular to dir)
	
	// Input/State Flags
	bool	up;                 // W key pressed
	bool	down;               // S key pressed
	bool	left;               // A key pressed
	bool	right;              // D key pressed
	bool	rotate_l;           // Left arrow pressed
	bool	rotate_r;           // Right arrow pressed
	
	// Initialization
	int		starting_direction; // NORTH, SOUTH, EAST, or WEST
}	t_player;
```

---

## Input & Movement Flow

```
Event Loop (runs every frame)
│
├─ Key Press → player_moving() → sets movement flag to true
│  (flag stays true until key release)
│
├─ Key Release → player_idle() → sets movement flag to false
│
├─ render() calls move_player()
│  │
│  └─ For each active flag:
│     ├─ if (up) → player_x/y += dir * speed
│     ├─ if (down) → player_x/y -= dir * speed
│     ├─ if (left) → player_x/y -= plane * speed
│     ├─ if (right) → player_x/y += plane * speed
│     ├─ if (rotate_l) → rotate(dir & plane, -angle)
│     └─ if (rotate_r) → rotate(dir & plane, +angle)
│
└─ Raycasting renders view based on new player position
```

---

## Key Concepts

### Smooth Movement
- Flags persist between frames, allowing continuous motion while key is held
- Player can press multiple keys simultaneously for diagonal movement
- This creates fluid, responsive controls

### Vector-Based Movement
- Movement happens along normalized direction vectors
- Forward/backward uses `dir` vector
- Strafing uses perpendicular `plane` vector
- Distance traveled = vector magnitude × speed

### Rotation Mechanics
- **2D rotation matrix** used (standard trigonometric approach)
- Both `dir` and `plane` rotated simultaneously
- They remain perpendicular after rotation (mathematical property)
- Rotation angle in radians: `0.03 rad ≈ 1.7°`

### Spawn & Starting Direction
- Player starts at center of spawn tile from map file
- Direction set based on map's start marker (N/S/E/W)
- FOV determined by plane magnitude (0.66 creates ~66° FOV)

---

## Performance Considerations

- **No collision detection**: Movement doesn't check for walls. Collision handling is in the raycasting/rendering phase.
- **Flag-based system**: Efficient continuous movement without polling keyboard state constantly.
- **Matrix rotation**: Uses trigonometric functions (may want to precompute for very high frame rates).
