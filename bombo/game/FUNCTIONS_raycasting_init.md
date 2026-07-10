# raycasting_init.c — Function Breakdown

## Function: `init_ray(t_game *game, t_player *player, t_ray *ray, int x)` (static)

```c
static void	init_ray(t_game *game, t_player *player, t_ray *ray, int x)
{
	ray->camera_x = 2 * x / (double)game->win_w - 1;
	ray->ray_dir_x = player->dir_x + player->plane_x * ray->camera_x;
	ray->ray_dir_y = player->dir_y + player->plane_y * ray->camera_x;
	ray->map_x = (int)player->player_x;
	ray->map_y = (int)player->player_y;
	ray->hit = false;
	ray->is_door = false;
	check_raydir_x(player, ray);
	check_raydir_y(player, ray);
}
```

**Purpose**: Initialize a single ray struct for column `x` with DDA (Digital Differential Analyzer) raycasting parameters.

**Parameters:**
- `game`: Game struct (contains window width).
- `player`: Player struct (contains position and direction).
- `ray`: Ray struct to initialize.
- `x`: Screen column index (0 to `win_w - 1`).

**Return Value:** None (void).

**Variables:**
- `ray->camera_x`: Normalized x-position on the camera plane (-1.0 to 1.0).
- `ray->ray_dir_x/y`: Ray direction in world space.
- `ray->map_x/y`: Current grid cell being tested.
- `ray->hit`: Hit flag (initially false).
- `ray->is_door`: Door hit flag (initially false).

**Initialization Steps:**

1. **Compute camera plane coordinate** — Map screen column to camera plane:
   - `camera_x = 2 * x / win_w - 1` produces value in `[-1, 1]` range.
   - `-1` = left edge, `0` = center, `1` = right edge.

2. **Compute ray direction** — Combine player direction and camera plane offset:
   - Base direction: `player->dir_x/y` (where player is looking).
   - Plane offset: `player->plane_x/y * camera_x` (perpendicular spread).
   - Result is the ray direction for this column.

3. **Initialize grid cell** — Start DDA from player's current cell:
   - `ray->map_x = (int)player->player_x` (floor to integer).
   - `ray->map_y = (int)player->player_y`.

4. **Clear hit flags** — `hit = false`, `is_door = false`.

5. **Initialize DDA parameters** — Call `check_raydir_x()` and `check_raydir_y()`:
   - Compute step direction (±1) based on ray direction sign.
   - Compute initial side distances to next grid lines.
   - Compute delta distances (distance to cross one grid unit).

**Ray Direction Vectors:**
- Player direction (`dir_x`, `dir_y`) — always unit length (normalized).
- Camera plane (`plane_x`, `plane_y`) — perpendicular to direction, controls FOV.
- Example: Player looking NORTH, plane points EAST.
  - `dir = (0, -1)`, `plane = (0.66, 0)`.
  - Left column: `ray_dir = (0, -1) + (0.66, 0) * (-1) = (-0.66, -1)`.
  - Center column: `ray_dir = (0, -1) + (0, 0) = (0, -1)`.
  - Right column: `ray_dir = (0, -1) + (0.66, 0) * (1) = (0.66, -1)`.

---

## Function: `raycasting(t_game *game)`

```c
int	raycasting(t_game *game)
{
	t_ray	ray;
	int		x;

	x = 0;
	while (x < game->win_w)
	{
		init_ray(game, &game->player, &ray, x);
		cast_ray(game, &ray, 0);
		calculate_wall(game, &ray);
		draw_textured_line(game, &ray, x, 0);
		x++;
	}
	return (0);
}
```

**Purpose**: Main raycasting loop. Cast one ray per screen column and draw the resulting wall slice.

**Parameters:**
- `game`: Game struct with all game state and graphics context.

**Return Value:**
- Always `0` (required by MLX loop callback protocol).

**Variables:**
- `ray`: Ray struct for the current column.
- `x`: Current screen column (0 to `win_w - 1`).

**Per-Column Pipeline:**

For each screen column `x`:

1. **Initialize ray** — `init_ray()` sets up DDA parameters for column `x`.

2. **Cast ray** — `cast_ray()` performs DDA line-drawing algorithm:
   - Walks grid cell by cell from player position.
   - Stops when a wall (`'1'`) or door (`'D'`) is hit.
   - Returns with `ray->hit = true` and `ray->map_x/y` pointing to hit cell.

3. **Calculate wall geometry** — `calculate_wall()`:
   - Computes perpendicular distance to wall (to avoid fish-eye).
   - Computes wall slice height in pixels.
   - Computes draw range (top and bottom pixels on screen).

4. **Draw wall slice** — `draw_textured_line()`:
   - Samples texture based on wall hit coordinates.
   - Applies shading (distance-based fog, side darkening).
   - Writes pixels to backbuffer from `draw_start` to `draw_end`.

**Loop Count:**
- Iterates exactly `game->win_w` times (e.g., 1920 rays for 1920px width).
- Total ray calculations: O(width × grid_traversal).

**Call Chain:**
- Called from `render()` in `render.c`.
- Each step calls functions from `raycasting.c`, `raycasting2.c`, `raycasting_draw.c`.

**Performance:**
- Raycasting is the most expensive part of each frame.
- Grid traversal per ray is typically 20-50 cells on average maze.
- Total time: width × average_traversal × per-pixel operations.
