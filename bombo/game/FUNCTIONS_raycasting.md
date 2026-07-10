# raycasting.c — Function Breakdown

## Function: `out_of_bounds(t_game *game, t_ray *ray)`

```c
bool	out_of_bounds(t_game *game, t_ray *ray)
{
	if (ray->map_y < 0 || ray->map_y >= game->win_h || ray->map_x < 0)
		return (true);
	if (ray->map_x >= (int)ft_strlen(game->map->grid[ray->map_y]))
		return (true);
	return (false);
}
```

**Purpose**: Check if a ray's current grid cell is outside the map bounds.

**Parameters:**
- `game`: Game struct containing map grid and window height (used as grid height).
- `ray`: Ray struct with current `map_x` and `map_y` grid coordinates.

**Return Value:**
- `true` if the cell is out of bounds, `false` otherwise.

**Logic:**
1. **Y bounds** — Check if `map_y` is outside `[0, win_h)` range:
   - `map_y < 0` → above map.
   - `map_y >= win_h` → below map.
   
2. **X bounds (negative)** — Check if `map_x < 0` (left of map).

3. **X bounds (positive)** — Check if `map_x >= strlen(grid[map_y])` (right of map row).
   - Different rows can have different lengths (ragged arrays).

4. Return `false` if all checks pass (in bounds).

**Used By:**
- `cast_ray()` — Prevents accessing invalid grid cells during DDA traversal.

---

## Function: `cast_ray(t_game *game, t_ray *ray, char tile)`

```c
void	cast_ray(t_game *game, t_ray *ray, char tile)
{
	while (!ray->hit)
	{
		if (ray->side_dist_x < ray->side_dist_y)
		{
			ray->side_dist_x += ray->delta_dist_x;
			ray->map_x += ray->step_x;
			ray->side = 0;
		}
		else
		{
			ray->side_dist_y += ray->delta_dist_y;
			ray->map_y += ray->step_y;
			ray->side = 1;
		}
		if (out_of_bounds(game, ray))
			return ;
		tile = game->map->grid[ray->map_y][ray->map_x];
		if (tile == '1')
			ray->hit = 1;
		else if (tile == 'D' && ray_hits_door(game, ray))
		{
			ray->hit = 1;
			ray->is_door = true;
		}
	}
}
```

**Purpose**: Perform DDA (Digital Differential Analyzer) grid traversal from player to wall/door hit.

**Parameters:**
- `game`: Game struct with map grid.
- `ray`: Ray struct with initialized DDA parameters and direction.
- `tile`: Unused parameter (probably leftover from refactoring).

**Return Value:** None (void). Updates `ray->map_x/y` and `ray->hit`.

**Variables:**
- `ray->side_dist_x/y`: Perpendicular distance to next vertical/horizontal grid line.
- `ray->delta_dist_x/y`: Distance to cross one grid unit.
- `ray->step_x/y`: Direction to step (±1).
- `ray->side`: Which grid line was crossed last (0 = vertical X, 1 = horizontal Y).
- `tile`: Character at current grid cell.

**DDA Algorithm (Grid Traversal):**

1. **Loop while not hit** — Continue until a wall or door blocks the ray.

2. **Advance along shorter distance** — Compare distances to next grid lines:
   - If `side_dist_x < side_dist_y`:
     - Ray will cross a vertical grid line first.
     - Increment `side_dist_x` by `delta_dist_x` (distance to next vertical line).
     - Move to next cell: `map_x += step_x` (±1).
     - Mark as vertical hit: `side = 0`.
   - Else (X distance is farther):
     - Ray will cross a horizontal grid line first.
     - Increment `side_dist_y` by `delta_dist_y`.
     - Move to next cell: `map_y += step_y` (±1).
     - Mark as horizontal hit: `side = 1`.

3. **Bounds check** — If new cell is out of bounds, stop (return).

4. **Read grid cell** — Get character at `grid[map_y][map_x]`:
   - `'1'` = solid wall → set `ray->hit = 1` and exit loop.
   - `'D'` = door → check if door blocks ray via `ray_hits_door()`:
     - If door is closed or partially open and blocks ray: `hit = 1, is_door = true`.
     - If door is fully open: continue traversing.
   - Any other cell (`'0'`, `' '`, player direction): continue loop.

5. Return when hit or out of bounds.

**Example Ray Path:**
```
Start: (2.3, 3.7)  Player in cell (2, 3)
Step 1: side_dist_x=0.7, side_dist_y=0.3 → move Y first → (2, 4)
Step 2: side_dist_x=1.7, side_dist_y=1.3 → move Y first → (2, 5)
Step 3: side_dist_x=2.7, side_dist_y=2.3 → move X first → (3, 5)
Step 4: Read grid[5][3] → '1' → HIT!
```

---

## Function: `check_raydir_x(t_player *player, t_ray *ray)`

```c
void	check_raydir_x(t_player *player, t_ray *ray)
{
	if (ray->ray_dir_x == 0)
		ray->delta_dist_x = 1e30;
	else
		ray->delta_dist_x = ft_abs(1.0 / ray->ray_dir_x);
	if (ray->ray_dir_x < 0)
	{
		ray->step_x = -1;
		ray->side_dist_x = (player->player_x - ray->map_x)
			* ray->delta_dist_x;
	}
	else
	{
		ray->step_x = 1;
		ray->side_dist_x = (ray->map_x + 1.0 - player->player_x)
			* ray->delta_dist_x;
	}
}
```

**Purpose**: Initialize X-axis DDA parameters (step direction and initial distance to next vertical grid line).

**Parameters:**
- `player`: Player struct with player position.
- `ray`: Ray struct with ray direction; will be populated with X DDA parameters.

**Return Value:** None (void).

**Logic:**

1. **Compute delta distance** — Distance to traverse one grid unit in X:
   - If ray direction X is 0 (perfectly vertical ray): `delta_dist_x = 1e30` (essentially infinity).
   - Otherwise: `delta_dist_x = |1.0 / ray_dir_x|` (reciprocal of direction).

2. **Determine step direction** — Which way to move along X:
   - If `ray_dir_x < 0`: Ray is going left → `step_x = -1`.
   - Else: Ray is going right → `step_x = 1`.

3. **Compute initial side distance** — Distance from player to first vertical grid line:
   - If stepping left (negative):
     - `side_dist_x = (player_x - floor(player_x)) * delta_dist_x`
     - Distance to the left grid line (fractional part).
   - Else (stepping right):
     - `side_dist_x = (floor(player_x) + 1 - player_x) * delta_dist_x`
     - Distance to the right grid line.

**Example:**
```
Player at (5.3, 10.5), ray_dir_x = -1.0 (going left)
delta_dist_x = |1.0 / -1.0| = 1.0
step_x = -1
side_dist_x = (5.3 - 5.0) * 1.0 = 0.3  (distance to grid line at x=5)

Next iteration of cast_ray():
side_dist_x = 0.3 + 1.0 = 1.3, map_x = 5 - 1 = 4
```

---

## Function: `check_raydir_y(t_player *player, t_ray *ray)`

```c
void	check_raydir_y(t_player *player, t_ray *ray)
{
	if (ray->ray_dir_y == 0)
		ray->delta_dist_y = 1e30;
	else
		ray->delta_dist_y = ft_abs(1.0 / ray->ray_dir_y);
	if (ray->ray_dir_y < 0)
	{
		ray->step_y = -1;
		ray->side_dist_y = (player->player_y - ray->map_y)
			* ray->delta_dist_y;
	}
	else
	{
		ray->step_y = 1;
		ray->side_dist_y = (ray->map_y + 1.0 - player->player_y)
			* ray->delta_dist_y;
	}
}
```

**Purpose**: Initialize Y-axis DDA parameters (step direction and initial distance to next horizontal grid line).

**Parameters:**
- `player`: Player struct with player position.
- `ray`: Ray struct with ray direction; will be populated with Y DDA parameters.

**Return Value:** None (void).

**Logic:** Identical to `check_raydir_x()` but for the Y axis.

1. **Compute delta distance**:
   - If `ray_dir_y == 0`: `delta_dist_y = 1e30`.
   - Else: `delta_dist_y = |1.0 / ray_dir_y|`.

2. **Step direction**:
   - If `ray_dir_y < 0`: `step_y = -1` (going up).
   - Else: `step_y = 1` (going down).

3. **Initial side distance**:
   - If stepping up: distance to upper grid line (fractional part of Y).
   - Else: distance to lower grid line (1 - fractional part).
