# movement.c — Function Breakdown

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

**Purpose**: Rotate the player's viewing direction and camera plane by a given angle using 2D rotation matrix.

**Parameters:**
- `player`: Player struct with direction and plane vectors.
- `rot`: Rotation angle in radians. Positive = counterclockwise, negative = clockwise.

**Return Value:** None (void).

**Logic:**

Apply 2D rotation matrix to both direction and plane vectors:
```
[ cos(θ)  -sin(θ) ] [ x ]
[ sin(θ)   cos(θ) ] [ y ]
```

1. **Save old vectors** — Store original `dir_x/y` and `plane_x/y`.

2. **Rotate direction vector** — Apply rotation matrix:
   - `new_dir_x = old_dir_x * cos(rot) - old_dir_y * sin(rot)`.
   - `new_dir_y = old_dir_x * sin(rot) + old_dir_y * cos(rot)`.

3. **Rotate plane vector** — Same rotation applied to camera plane:
   - `new_plane_x = old_plane_x * cos(rot) - old_plane_y * sin(rot)`.
   - `new_plane_y = old_plane_x * sin(rot) + old_plane_y * cos(rot)`.

**Result:** Both vectors rotate while remaining perpendicular to each other and maintaining unit length.

**Example:**
```
Player looking NORTH: dir = (0, -1), plane = (0.66, 0)
Rotate 90° counterclockwise (rot = π/2, cos=0, sin=1):
  new_dir = (0*0 - (-1)*1, 0*1 + (-1)*0) = (1, 0) → looking EAST
  new_plane = (0.66*0 - 0*1, 0.66*1 + 0*0) = (0, 0.66) → plane points NORTH
```

---

## Function: `update_player_x(t_game *game, double dir, double speed, bool sum)` (static)

```c
static void	update_player_x(t_game *game, double dir, double speed, bool sum)
{
	double	new_x;
	double	padding;
	double	collision_x;

	padding = 0.2;
	if (sum)
	{
		new_x = game->player.player_x + dir * speed;
		collision_x = new_x + dir * padding;
	}
	else
	{
		new_x = game->player.player_x - dir * speed;
		collision_x = new_x - dir * padding;
	}
	if (game->map->grid[(int)game->player.player_y][(int)collision_x] == '1')
		return ;
	if (game->map->grid[(int)collision_x][(int)game->player.player_y] == 'D'
	&& is_door_blocking(game, (int)collision_x, (int)game->player.player_y))
		return ;
	game->player.player_x = new_x;
}
```

**Purpose**: Update player X position with collision detection and door blocking.

**Parameters:**
- `game`: Game struct with player and map.
- `dir`: Direction component (typically `player->dir_x` or `player->plane_x`).
- `speed`: Movement speed multiplier.
- `sum`: `true` to add movement, `false` to subtract (reverse direction).

**Return Value:** None (void).

**Variables:**
- `new_x`: Candidate new X position.
- `padding`: Collision buffer (0.2 units) to prevent clipping into walls.
- `collision_x`: Actual point to test for collision (ahead of player).

**Logic:**

1. **Calculate new position**:
   - If `sum`: `new_x = player_x + dir * speed` (forward/rightward).
   - Else: `new_x = player_x - dir * speed` (backward/leftward).

2. **Calculate collision point** — Extend new position by padding in the same direction:
   - If `sum`: `collision_x = new_x + dir * padding`.
   - Else: `collision_x = new_x - dir * padding`.

3. **Collision check 1 — Wall** — Test if collision point hits a wall:
   - Read `grid[player_y][(int)collision_x]`.
   - If it's `'1'`, return early (no movement).

4. **Collision check 2 — Door** — Test if collision point hits a closed door:
   - Read `grid[(int)collision_x][player_x]`.
   - If it's `'D'` and `is_door_blocking()` returns true, return early.

5. **Apply movement** — If both checks pass, update `player_x = new_x`.

**Collision Padding:**
- Prevents player from getting too close to walls (0.2 units = 1/5 grid cell).
- Allows smooth movement along walls without getting stuck.

---

## Function: `update_player_y(t_game *game, double dir, double speed, bool sum)` (static)

```c
static void	update_player_y(t_game *game, double dir, double speed, bool sum)
{
	double	new_y;
	double	padding;
	double	collision_y;

	padding = 0.35;
	if (sum)
	{
		new_y = game->player.player_y + dir * speed;
		collision_y = new_y + dir * padding;
	}
	else
	{
		new_y = game->player.player_y - dir * speed;
		collision_y = new_y - dir * padding;
	}
	if (game->map->grid[(int)collision_y][(int)game->player.player_x] == '1')
		return ;
	if (game->map->grid[(int)collision_y][(int)game->player.player_x] == 'D'
	&& is_door_blocking(game, (int)game->player.player_x, (int)collision_y))
		return ;
	game->player.player_y = new_y;
}
```

**Purpose**: Update player Y position with collision detection and door blocking.

**Logic:** Identical to `update_player_x()` but for Y-axis with padding = 0.35 (larger buffer to prevent wall clipping).

---

## Function: `check_direction_to_move(t_game *game, t_player *player)` (static)

```c
static void	check_direction_to_move(t_game *game, t_player *player)
{
	double	speed;

	speed = game->movement_speed;
	if (player->up)
	{
		update_player_x(game, player->dir_x, speed, true);
		update_player_y(game, player->dir_y, speed, true);
	}
	if (player->down)
	{
		update_player_x(game, player->dir_x, speed, false);
		update_player_y(game, player->dir_y, speed, false);
	}
	if (player->left)
	{
		update_player_x(game, player->plane_x, speed, false);
		update_player_y(game, player->plane_y, speed, false);
	}
	if (player->right)
	{
		update_player_x(game, player->plane_x, speed, true);
		update_player_y(game, player->plane_y, speed, true);
	}
}
```

**Purpose**: Check which movement keys are pressed and update player position accordingly.

**Parameters:**
- `game`: Game struct with player and map.
- `player`: Player struct with movement flags and direction/plane vectors.

**Return Value:** None (void).

**Logic:**

For each movement direction flag:
- **Up** (W key) — Move forward along `dir` vector: `+dir_x`, `+dir_y`.
- **Down** (S key) — Move backward: `-dir_x`, `-dir_y`.
- **Left** (A key) — Strafe left: `-plane_x`, `-plane_y`.
- **Right** (D key) — Strafe right: `+plane_x`, `+plane_y`.

Each movement is applied with collision detection via `update_player_x()` and `update_player_y()`.

**Independent Axis Movement:**
- X and Y are updated independently.
- Allows moving diagonally (W+D simultaneously).
- If collision in one axis, movement in other axis may still apply.

---

## Function: `move_player(t_game *game, t_player *player)`

```c
void	move_player(t_game *game, t_player *player)
{
	check_direction_to_move(game, player);
	if (player->rotate_l)
		rotate(player, -game->rotation_speed);
	if (player->rotate_r)
		rotate(player, game->rotation_speed);
}
```

**Purpose**: Master player movement function. Combines translation and rotation based on key states.

**Parameters:**
- `game`: Game struct with movement and rotation speeds.
- `player`: Player struct with input flags and position/direction.

**Return Value:** None (void).

**Logic:**

1. **Translate player** — Call `check_direction_to_move()` to handle WASD movement.

2. **Rotate left** (Left arrow) — Rotate counterclockwise by `rotation_speed`.

3. **Rotate right** (Right arrow) — Rotate clockwise by `-rotation_speed`.

**Called Every Frame:**
- Part of main render loop in `render()`.
- Allows smooth, frame-rate-independent movement (speed scaled by game settings).

**Input State:**
- Movement flags (`up`, `down`, `left`, `right`, `rotate_l`, `rotate_r`) are set by keyboard hooks.
- Persist until key release (pressed in `player_moving()`, released in `player_idle()`).
