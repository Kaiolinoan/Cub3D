# Minimap Line-by-Line Breakdown

## Constants / Config Values

The new codebase moves several hardcoded constants into runtime fields on `t_game` instead of `#define` macros. The ones that remain as macros:

```c
#define MINIMAP_MARGIN 12
```
- **What**: The minimap is drawn 12 pixels from the top-left corner of the game window.
- **Why**: Padding so the minimap doesn't touch the window edge.

```c
#define MINIMAP_PLAYER_COLOR 0x00FF4C4C
```
- **What**: Hex RGB color for the player dot.
- **Why**: Red dot at the center of the minimap — visually stands out.

```c
#define GREY  // (wall sentinel color)
```
- **What**: The color returned by `minimap_tile_color()` when the cell is a wall (`'1'`).
- **Why**: Used as a sentinel value inside `raycast_to_wall()` to detect wall hits — if the color returned equals `GREY`, the ray has hit a wall.

**Runtime fields (no longer macros):**

| Field | Type | Purpose |
|---|---|---|
| `game->minimap_tile_size` | `double` | Pixels per world tile on the minimap |
| `game->minimap_radius_tiles` | `double` | Visible radius in world-tile units |

From these, all pixel geometry is derived at runtime:
```
radius_px = minimap_radius_tiles * minimap_tile_size
diameter  = radius_px * 2
```

**Dynamic color fields** — floor/ceiling colors are now read from the map config and converted to pixel values at draw time via `rgb_to_color()`:

| Field | Meaning |
|---|---|
| `game->map->floor` | `t_rgb*` — floor color from map config |
| `game->map->ceiling` | `t_rgb*` — ceiling/void color from map config |

- **Why**: Colors are now data-driven from the `.cub` map file instead of being baked into the binary. The FOV overlay blends the floor color with white rather than using a fixed cyan (`MINIMAP_FOV_COLOR`).

---

## Function: `display_minimap(t_game *game)` (`utils.c`)

```c
void	display_minimap(t_game *game)
{
	if (game->minimap == false)
		game->minimap = true;
	else
		game->minimap = false;
}
```

**Variables:**
- `game->minimap`: Boolean flag in the game state.
  - **What it stores**: `true` if minimap should be drawn, `false` if hidden.
  - **What it does**: Toggles the minimap on/off (e.g. pressing M).
  - **Used by**: The render loop checks this flag before calling `render_minimap()`.

---

## Function: `minimap_map_height(t_map *map)` (`utils.c`)

```c
int	minimap_map_height(t_map *map)
{
	return ((int)array_len(map->grid));
}
```

**Variables:**
- `map->grid`: Array of strings, each string is one row of the map.
  - **What it stores**: The 2D map layout as an array of C strings.
  - **What it does**: Counts how many rows exist using `array_len()`.
- **Return value**: Integer = number of rows in the map.
- **Used for**: Bounds checking in `minimap_tile_color()` to know if a coordinate is out of bounds.

> **Change from old version**: No longer `static`. It is now called from `minimap_main.c`, so the `static` linkage was removed.

---

## Function: `rgb_to_color(t_rgb *rgb)` (`utils.c`)

```c
unsigned int	rgb_to_color(t_rgb *rgb)
{
	return ((rgb->r << 16) | (rgb->g << 8) | rgb->b);
}
```

**Purpose**: Converts a `t_rgb` struct (with separate `r`, `g`, `b` byte fields) into a packed `0x00RRGGBB` unsigned int color value.

**Variables:**
- `rgb->r, rgb->g, rgb->b`: Red, green, blue channels (0–255 each).
- **Return value**: A single `unsigned int` color suitable for `put_minimap_pixel()`.

**Used by**: `minimap_tile_color()` (to return floor/ceiling colors), `fov_line_pixel()` (to blend the floor color for the FOV overlay).

> **New function** — did not exist in the old version. Replaces the hardcoded `MINIMAP_FLOOR_COLOR`, `MINIMAP_WALL_COLOR`, and `MINIMAP_VOID_COLOR` macros by reading actual map config colors at runtime.

---

## Function: `minimap_tile_color(...)` (`utils.c`)

```c
unsigned int	minimap_tile_color(t_game *game, double world_x, double world_y)
{
	int		cell_x;      // Which column (X) of the map grid
	int		cell_y;      // Which row (Y) of the map grid
	char	cell;        // Character at map[cell_y][cell_x]
	int		map_height;  // Total number of rows in the map

	map_height = minimap_map_height(game->map);
	cell_x = (int)floor(world_x);  // Convert world X coord to map column
	cell_y = (int)floor(world_y);  // Convert world Y coord to map row

	// Check if cell_y is out of bounds (above or below map)
	if (cell_y < 0 || cell_y >= map_height || !game->map->grid[cell_y])
		return (rgb_to_color(game->map->ceiling));  // Out-of-bounds: ceiling color

	// Check if cell_x is out of bounds (left or right of map)
	if (cell_x < 0 || cell_x >= (int)ft_strlen(game->map->grid[cell_y]))
		return (rgb_to_color(game->map->ceiling));  // Out-of-bounds: ceiling color

	cell = game->map->grid[cell_y][cell_x];  // Get character at this position

	if (cell == '1')   // Wall
		return (GREY);
	if (cell == ' ')   // Void/empty space
		return (rgb_to_color(game->map->ceiling));
	return (rgb_to_color(game->map->floor));  // Walkable floor
}
```

**Purpose**: Given a world coordinate (X, Y), return what color that location should appear on the minimap.

**Variables explained:**
- `world_x, world_y`: Coordinates in world space (map tile units, e.g. `5.5` = 5.5 tiles from origin).
- `cell_x, cell_y`: Map grid indices calculated via `floor()`.
  - Example: `world_x=5.7` → `cell_x=5` (5th column)
  - Example: `world_y=3.2` → `cell_y=3` (3rd row)
- `map_height`: Total number of rows in the map.
- `cell`: The character stored in the map at `[cell_y][cell_x]`.
  - `'1'` = wall
  - `' '` = void/empty
  - Anything else (`'0'`, `'S'`, `'N'`, `'E'`, `'W'`) = floor

**Logic flow:**
1. Convert world coords to map cell indices.
2. Check if those indices are valid (not outside grid bounds).
3. Read the character at that cell.
4. Return the appropriate color based on the character.

**Color mapping:**

| Cell | Return value | Visual |
|---|---|---|
| `'1'` | `GREY` | Wall |
| `' '` | `rgb_to_color(ceiling)` | Void/empty |
| Out of bounds | `rgb_to_color(ceiling)` | Outside map |
| Other | `rgb_to_color(floor)` | Walkable floor |

> **Change from old version**: Walls now return `GREY` (used as a sentinel for ray collision), while void/out-of-bounds return the map's ceiling color instead of a fixed `MINIMAP_VOID_COLOR`. Floor returns the map's floor color instead of `MINIMAP_FLOOR_COLOR`. No longer `static` — called from `minimap_fov.c`.

---

## Function: `put_minimap_pixel(...)` (`utils.c`)

```c
void	put_minimap_pixel(t_game *game, int x, int y, unsigned int color)
{
	if (x < 0 || y < 0 || x >= game->win_w || y >= game->win_h)
		return ;  // Outside window — do nothing
	my_pixel_put(game, x, y, color);
}
```

**Variables:**
- `game->win_w`: Window width in pixels.
- `game->win_h`: Window height in pixels.
- `x, y`: Pixel coordinates to draw at (window space).
- `color`: RGB color value to write.

**Purpose**: Safe pixel drawing. Prevents writing outside the window bounds (segfault protection).

> **Change from old version**: The `t_img *img` parameter was removed. It now draws directly via `my_pixel_put(game, x, y, color)` instead of `my_pixel_put(img, x, y, color)`. The image buffer is accessed through the game struct internally.

---

## Function: `minimap_in_circle(...)` (`minimap_fov_utils.c`)

```c
bool	minimap_in_circle(int x, int y, t_ip center, int radius_px)
{
	int	center_offset_x_sq;  // Squared X distance from point to circle center
	int	center_offset_y_sq;  // Squared Y distance from point to circle center

	center_offset_x_sq = (x - center.x) * (x - center.x);
	center_offset_y_sq = (y - center.y) * (y - center.y);
	return (center_offset_x_sq + center_offset_y_sq <= radius_px * radius_px);
}
```

**Purpose**: Circle membership test. Returns `true` if the pixel at `(x, y)` falls inside the minimap circle.

**Variables:**
- `x, y`: Point coordinates to test (pixel position).
- `center`: A `t_ip` struct (integer point) holding the circle center `(center.x, center.y)`.
- `radius_px`: Radius of the circle in pixels.
- `center_offset_x_sq, center_offset_y_sq`: Squared offsets from center (avoids `sqrt`).

**Math**: Circle equation — $(x - c_x)^2 + (y - c_y)^2 \le r^2$.

> **Change from old version**: `center_x, center_y` as two separate `int` parameters were replaced by a single `t_ip center` struct. The two intermediate variables now store squared values directly (renamed from `center_offset_x/y` to `center_offset_x_sq/y_sq`).

---

## Function: `blend_color_with_white(...)` (`minimap_fov_utils.c`)

```c
unsigned int	blend_color_with_white(unsigned int base_color,
		double white_opacity)
{
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;

	r = (base_color >> 16) & 0xFF;
	g = (base_color >> 8) & 0xFF;
	b = base_color & 0xFF;
	r = (unsigned char)(r + (255 - r) * white_opacity);
	g = (unsigned char)(g + (255 - g) * white_opacity);
	b = (unsigned char)(b + (255 - b) * white_opacity);
	return ((r << 16) | (g << 8) | b);
}
```

**Purpose**: Linearly interpolates each RGB channel of `base_color` toward white by `white_opacity` (0.0 = unchanged, 1.0 = fully white). Used to tint the FOV cone.

**Variables:**
- `base_color`: The source color packed as `0x00RRGGBB`.
- `white_opacity`: How much to blend toward white. The FOV uses `0.2` (20% white).
- `r, g, b`: Individual channel values extracted via bitshifting.

**Formula per channel:**
```
channel' = channel + (255 - channel) * opacity
```
At `opacity=0.2`, a dark floor color becomes slightly lighter — giving the FOV a subtle brightened glow rather than a harsh fixed color.

> **New function** — replaces the fixed `MINIMAP_FOV_COLOR` (cyan). The FOV overlay color is now derived from the map's floor color blended with white.

---

## Function: `raycast_to_wall(...)` (`minimap_fov.c`)

```c
static double	raycast_to_wall(t_game *game, t_dp start, t_dp dir,
		double max_distance)
{
	double	ray_x;         // Current X position along the ray
	double	ray_y;         // Current Y position along the ray
	double	ray_step;      // How far to advance per iteration (world units)
	double	ray_distance;  // Total distance traveled so far

	ray_step = 0.05;       // Step 0.05 world units per iteration
	ray_distance = 0.0;
	ray_x = start.x;      // Start from player's world X
	ray_y = start.y;      // Start from player's world Y
	while (ray_distance < max_distance)
	{
		if (minimap_tile_color(game, ray_x, ray_y) == GREY)
			return (ray_distance);  // Hit a wall — return distance traveled
		ray_x += dir.x * ray_step;
		ray_y += dir.y * ray_step;
		ray_distance += ray_step;
	}
	return (max_distance);  // No wall found within range
}
```

**Purpose**: Cast a ray from `start` in direction `dir` and return the distance to the first wall hit, or `max_distance` if none found.

**Variables explained:**
- `start`: A `t_dp` struct (double-precision point) for the player's world position.
- `dir`: A normalized direction vector (`t_dp`) to cast along.
- `max_distance`: Maximum range to search in world tile units (`game->minimap_radius_tiles`).
- `ray_x, ray_y`: Current position as we march along the ray.
- `ray_step`: Precision of detection — `0.05` world units per step.
  - Smaller = more accurate but more iterations.
  - Larger = faster but may skip thin walls.
- `ray_distance`: Accumulates the total distance traveled.

**Algorithm:**
1. Start at the player's world position.
2. Loop up to `max_distance`:
   - If current cell returns `GREY`, a wall was hit — return distance.
   - Otherwise advance: `ray_x += dir.x * 0.05`, `ray_distance += 0.05`.
3. If loop ends, no wall found — return `max_distance`.

> **Change from old version**: Parameters changed from individual `double start_x, start_y, dir_x, dir_y` to `t_dp start, t_dp dir` structs. Wall detection now compares against `GREY` instead of `MINIMAP_WALL_COLOR`.

---

## Function: `fov_line_step(...)` (`minimap_fov_utils.c`)

```c
static void	fov_line_step(double *x, double *y, double sx, double sy)
{
	if (ft_abs(sx) > ft_abs(sy))
	{
		*x += sx / ft_abs(sx);   // Advance X by ±1, scale Y proportionally
		*y += sy / ft_abs(sx);
	}
	else
	{
		*x += sx / ft_abs(sy);   // Advance Y by ±1, scale X proportionally
		*y += sy / ft_abs(sy);
	}
}
```

**Purpose**: Advance one step along a line defined by slope `(sx, sy)`, always moving exactly 1 pixel along the dominant axis. This is a helper extracted from the FOV line drawing loop.

**Variables:**
- `x, y`: Pointers to the current floating-point position (modified in place).
- `sx, sy`: Total displacement of the line (`end - start`).

**Logic**: If the line is wider than it is tall (`|sx| > |sy|`), step 1 pixel in X and a fractional amount in Y. Otherwise, step 1 pixel in Y and a fractional amount in X. This ensures no gaps in the drawn line.

> **New function** — extracted from what was inline logic inside `draw_minimap_fov_line()` in the old version.

---

## Function: `fov_line_pixel(...)` (`minimap_fov_utils.c`)

```c
static void	fov_line_pixel(t_game *game, t_ip start, double lx, double ly)
{
	int	px;
	int	py;

	px = (int)(lx + 0.5);  // Round to nearest pixel
	py = (int)(ly + 0.5);
	if (minimap_in_circle(px, py, start,
			(int)(game->minimap_radius_tiles * game->minimap_tile_size)))
		put_minimap_pixel(game, px, py,
			blend_color_with_white(rgb_to_color(game->map->floor), 0.2));
}
```

**Purpose**: Draw one pixel of a FOV line, after circle-clipping and color-blending.

**Variables:**
- `start`: The minimap center `(t_ip)` — used as the circle center for clipping.
- `lx, ly`: Floating-point position along the line. Rounded to `px, py` for drawing.
- `radius_px`: Derived as `minimap_radius_tiles * minimap_tile_size`.

**What it does:**
1. Rounds floating-point position to the nearest integer pixel.
2. Circle-clips: only draws if `(px, py)` is inside the minimap circle.
3. Colors the pixel as the floor color blended 20% toward white.

> **New function** — extracted from the pixel loop inside the old `draw_minimap_fov_line()`. Also replaces the fixed `MINIMAP_FOV_COLOR` with a computed blended color.

---

## Function: `draw_minimap_fov_line(...)` (`minimap_fov_utils.c`)

```c
void	draw_minimap_fov_line(t_game *game, t_ip start, t_dp end)
{
	double	sx;     // Total X displacement
	double	sy;     // Total Y displacement
	double	lx;     // Current X position (floating point)
	double	ly;     // Current Y position (floating point)
	int		steps;  // Number of pixels to draw

	sx = end.x - start.x;
	sy = end.y - start.y;
	steps = ft_abs(sx);
	if (ft_abs(sy) > steps)
		steps = ft_abs(sy);   // Dominant axis determines step count
	if (steps <= 0)
		steps = 1;
	lx = start.x;
	ly = start.y;
	while (steps-- >= 0)
	{
		fov_line_pixel(game, start, lx, ly);
		fov_line_step(&lx, &ly, sx, sy);
	}
}
```

**Purpose**: Draw a line from `start` to `end` on the minimap, pixel by pixel, using the floor-blended FOV color and circle-clipping.

**Variables explained:**
- `start`: `t_ip` — the minimap center (integer pixel coords).
- `end`: `t_dp` — the ray's endpoint (double pixel coords, may be fractional).
- `sx, sy`: Total displacement from start to end.
- `steps`: Number of iterations = max of `|sx|` and `|sy|` (the dominant axis).
- `lx, ly`: Current floating-point position, initialized to `start`, advanced by `fov_line_step()` each iteration.

**Algorithm (DDA — Digital Differential Analyzer):**
1. Calculate total displacement.
2. Determine step count from the dominant axis.
3. Loop `steps` times:
   - Draw pixel at current position (via `fov_line_pixel`).
   - Advance by one step along the line (via `fov_line_step`).

> **Change from old version**: `start_x, start_y` as separate `int` params → `t_ip start`. `end_x, end_y` as separate `double` params → `t_dp end`. `img` parameter removed. Pixel drawing and color logic extracted into `fov_line_pixel()`. Step logic extracted into `fov_line_step()`. Fixed cyan color replaced by blended floor color.

---

## Function: `init_fov_dirs(...)` (`minimap_fov.c`)

```c
static void	init_fov_dirs(t_game *game, t_dp *left, t_dp *right)
{
	left->x = game->player.dir_x - game->player.plane_x;
	left->y = game->player.dir_y - game->player.plane_y;
	right->x = game->player.dir_x + game->player.plane_x;
	right->y = game->player.dir_y + game->player.plane_y;
}
```

**Purpose**: Compute the left and right edge directions of the player's FOV.

**Variables:**
- `game->player.dir_x, dir_y`: Unit vector pointing where the player is looking.
- `game->player.plane_x, plane_y`: Perpendicular vector representing FOV width (camera plane).
- `left`: Output — direction to the leftmost edge of the FOV (`dir - plane`).
- `right`: Output — direction to the rightmost edge of the FOV (`dir + plane`).

These are the exact same vectors used by the main ray-caster, so the minimap cone always matches what the player sees on screen.

> **New function** — extracted from what was inline initialization inside the old `draw_minimap_fov()`.

---

## Function: `get_ray_end(...)` (`minimap_fov.c`)

```c
static t_dp	get_ray_end(t_game *game, int cx, int cy, t_dp dir)
{
	double	dist;
	t_dp	player;
	t_dp	end;

	player = (t_dp){game->player.player_x, game->player.player_y};
	dist = raycast_to_wall(game, player, dir, game->minimap_radius_tiles);
	dist *= game->minimap_tile_size;  // Convert world tiles → screen pixels
	end.x = cx + dir.x * dist;
	end.y = cy + dir.y * dist;
	return (end);
}
```

**Purpose**: For a given normalized direction `dir`, find where that ray stops (at a wall or the minimap radius) and return the minimap screen pixel coordinate of that endpoint.

**Variables:**
- `cx, cy`: Minimap center in screen pixels.
- `dir`: Normalized direction vector for this ray (`t_dp`).
- `player`: Player world position packed as `t_dp` to pass to `raycast_to_wall()`.
- `dist`: Distance to wall in world tiles, then converted to pixels by multiplying by `minimap_tile_size`.
- `end`: Returned `t_dp` — the screen pixel endpoint of the ray.

**Formula:**
```
end.x = cx + dir.x * (wall_distance_tiles * minimap_tile_size)
end.y = cy + dir.y * (wall_distance_tiles * minimap_tile_size)
```

> **New function** — extracted from what was inline endpoint calculation inside the old `draw_minimap_fov()`. The old version also clamped `pixel_distance` to `radius_px`; here, clamping is implicit because `raycast_to_wall()` returns at most `minimap_radius_tiles`.

---

## Function: `draw_single_fov_ray(...)` (`minimap_fov.c`)

```c
static void	draw_single_fov_ray(t_game *game, int cx, int cy, t_fov_ray ray)
{
	double	ray_t;    // Interpolation parameter (0.0 = left, 1.0 = right)
	double	ray_len;  // Magnitude of interpolated direction vector
	t_dp	dir;      // Interpolated (then normalized) ray direction

	ray_t = (double)ray.idx / (double)ray.total;
	dir.x = ray.left.x + (ray.right.x - ray.left.x) * ray_t;
	dir.y = ray.left.y + (ray.right.y - ray.left.y) * ray_t;
	ray_len = sqrt(dir.x * dir.x + dir.y * dir.y);
	if (ray_len > 0)
	{
		dir = (t_dp){dir.x / ray_len, dir.y / ray_len};  // Normalize
		draw_minimap_fov_line(game, (t_ip){cx, cy},
			get_ray_end(game, cx, cy, dir));
	}
}
```

**Purpose**: Compute and draw one individual FOV ray, interpolated between the left and right FOV edges.

**Variables:**
- `ray.idx`: Current ray index (0 to `ray.total`).
- `ray.total`: Total number of rays.
- `ray.left, ray.right`: The FOV edge direction vectors (`t_dp`).
- `ray_t`: `idx / total` — goes from 0 to 1 across all rays.
- `dir`: Interpolated direction = `left + (right - left) * ray_t`. When `ray_t=0` this is the left edge; at `0.5` it's center; at `1.0` it's the right edge.
- `ray_len`: Magnitude of `dir` before normalization (used to avoid division by zero).

> **New function** — extracts what was the inner body of the `draw_minimap_fov()` loop in the old version.

---

## Struct: `t_fov_ray` (used in `draw_minimap_fov`)

The loop state is bundled into a `t_fov_ray` struct:

| Field | Type | Meaning |
|---|---|---|
| `left` | `t_dp` | Left edge direction of FOV |
| `right` | `t_dp` | Right edge direction of FOV |
| `idx` | `int` | Current ray index |
| `total` | `int` | Total number of rays |

> **New struct** — replaces the six separate local variables (`fov_left_x`, `fov_left_y`, `fov_right_x`, `fov_right_y`, `ray_index`, `rays`) that existed in the old flat `draw_minimap_fov()`.

---

## Function: `draw_minimap_fov(...)` (`minimap_fov.c`)

```c
void	draw_minimap_fov(t_game *game, int center_x, int center_y)
{
	t_fov_ray	ray;
	int			rays;

	init_fov_dirs(game, &ray.left, &ray.right);
	rays = (int)(game->minimap_radius_tiles * game->minimap_tile_size * 5);
	if (rays < 24)
		rays = 24;
	ray.total = rays;
	ray.idx = 0;
	while (ray.idx <= rays)
	{
		draw_single_fov_ray(game, center_x, center_y, ray);
		ray.idx++;
	}
}
```

**Purpose**: Draw the player's complete field of view as a filled cone on the minimap by iterating over all rays.

**Variables:**
- `center_x, center_y`: The minimap center in screen pixels.
- `ray`: A `t_fov_ray` struct holding the FOV edge directions and loop counters.
- `rays`: Total number of rays. Computed as `radius_tiles * tile_size * 5`, minimum 24.
  - More rays for larger minimaps to prevent visible gaps.

**Algorithm:**
1. Call `init_fov_dirs()` to populate `ray.left` and `ray.right`.
2. Determine how many rays to draw.
3. Loop `rays + 1` times, calling `draw_single_fov_ray()` each iteration.
4. Result: a filled cone that stops at walls, shown as a blended floor-color fan.

> **Change from old version**: The `img` parameter was removed. The heavy per-ray logic (interpolation, normalization, ray-cast, endpoint, line draw) is now delegated to `draw_single_fov_ray()` and `get_ray_end()`. Ray count formula unchanged but now uses runtime fields instead of macros.

---

## Function: `draw_minimap_player(...)` (`minimap_player.c`)

```c
void	draw_minimap_player(t_game *game, int center_x, int center_y)
{
	int		marker_offset_x;  // X offset from center (loop variable)
	int		marker_offset_y;  // Y offset from center (loop variable)
	double	radius;           // Radius of player dot in pixels
	double	radius_sq;        // radius^2 (pre-calculated for loop)

	radius = game->minimap_tile_size * 0.3;  // 30% of one tile
	radius_sq = radius * radius;
	marker_offset_y = -(int)radius;
	while (marker_offset_y <= (int)radius)
	{
		marker_offset_x = -(int)radius;
		while (marker_offset_x <= (int)radius)
		{
			if ((marker_offset_x * marker_offset_x
					+ marker_offset_y * marker_offset_y) <= radius_sq)
				put_minimap_pixel(game, center_x + marker_offset_x,
					center_y + marker_offset_y, MINIMAP_PLAYER_COLOR);
			marker_offset_x++;
		}
		marker_offset_y++;
	}
}
```

**Purpose**: Draw a small filled circle at the minimap center to represent the player.

**Variables explained:**
- `center_x, center_y`: Position of the player on the minimap (always the center of the rendered circle).
- `radius`: Player dot size — `minimap_tile_size * 0.3`. At tile size 14, this is 4.2 pixels.
- `radius_sq`: Pre-calculated `radius²` (avoids recalculating inside the nested loop).
- `marker_offset_x, marker_offset_y`: Offsets from center in pixels. Both loop from `-radius` to `+radius`, scanning a bounding square.

**Algorithm:**
1. Loop through a `2r × 2r` bounding box centered on the player position.
2. For each pixel, check if it's inside the circle: `offset_x² + offset_y² ≤ radius²`.
3. If inside, draw it `MINIMAP_PLAYER_COLOR` (red).
4. Result: a small filled red circle at the minimap center.

> **Change from old version**: `img` parameter removed. `radius` is no longer hardcoded to `5` — it is now `game->minimap_tile_size * 0.3` (runtime, scales with tile size). Loop bounds are now derived from that radius instead of hardcoded `5`/`-5`.

---

## Rendering Pipeline: `render_minimap(...)` (`minimap_main.c`)

```c
void	render_minimap(t_game *game)
{
	int	radius_px;   // Minimap circle radius in pixels
	int	diameter;    // Diameter = 2 * radius_px
	int	offset_x;    // X position of minimap top-left on screen
	int	offset_y;    // Y position of minimap top-left on screen

	radius_px = (int)(game->minimap_radius_tiles * game->minimap_tile_size);
	diameter = radius_px * 2;
	offset_x = MINIMAP_MARGIN;  // 12 pixels from left
	offset_y = MINIMAP_MARGIN;  // 12 pixels from top

	render_minimap_background(game, radius_px, diameter,
		(t_ip){offset_x, offset_y});           // PHASE 1: tile colors
	draw_minimap_fov(game, offset_x + radius_px, offset_y + radius_px);  // PHASE 2: FOV cone
	render_minimap_border(game, radius_px, diameter,
		(t_ip){offset_x, offset_y});           // PHASE 3: border ring
	draw_minimap_player(game, offset_x + radius_px, offset_y + radius_px); // PHASE 4: player dot
}
```

**Purpose**: The main function that renders the entire minimap. Called every frame.

**Key variables:**
- `radius_px`: `minimap_radius_tiles * minimap_tile_size` — circle radius in screen pixels.
- `diameter`: `radius_px * 2` — the square bounding area to iterate over.
- `offset_x, offset_y = 12`: Top-left screen position (fixed margin from corner).

**Render order:**

| Phase | Function | Draws |
|---|---|---|
| 1 | `render_minimap_background()` | Map tiles (floor / wall / void colors) |
| 2 | `draw_minimap_fov()` | FOV cone (blended floor color) |
| 3 | `render_minimap_border()` | Circle ring outline |
| 4 | `draw_minimap_player()` | Red player dot |

Each phase writes on top of the previous, so the player dot is never occluded.

> **Change from old version**: `img` parameter removed throughout. `radius_px`, `diameter`, and `offset` are now computed from runtime fields (`minimap_radius_tiles * minimap_tile_size`) instead of macros. The four phases are now separate static functions (`render_minimap_background`, `render_minimap_border`) instead of inline loops, and the same split applies to `draw_minimap_player` and `draw_minimap_fov`.

---

### Phase 1 — Background (`render_minimap_background`)

```c
static void	render_minimap_background(t_game *game, int radius_px,
		int diameter, t_ip offset)
```

Loops every pixel in the `diameter × diameter` bounding square. For each pixel:

1. Computes offset from minimap center: `poffx = px - radius_px`, `poffy = py - radius_px`.
2. Circle clips: skips pixel if `poffx² + poffy² > radius_px²`.
3. Maps offset to world coordinate:
   ```
   world_x = player_x + poffx / minimap_tile_size
   world_y = player_y + poffy / minimap_tile_size
   ```
4. Calls `minimap_tile_color()` and writes the color.

Result: a circular window into the map always centered on the player.

**Example trace** — player at world `(10, 10)`, `minimap_tile_size=14`, minimap center at screen `(82, 82)`:
- Center pixel `(px=70, py=70)`: `poffx=0`, `world_x=10` → draws tile at (10, 10).
- Left edge `(px=0, py=70)`: `poffx=-70`, `world_x=10-5=5` → draws tile 5 columns left of player.

### Phase 3 — Border (`render_minimap_border`)

```c
static void	render_minimap_border(t_game *game, int radius_px,
		int diameter, t_ip offset)
```

Same loop, but only draws pixels in the 2-pixel-wide border annulus:

```
(radius_px - 2)² ≤ dist² ≤ radius_px²
```

Color is fixed dark grey (`0x00555555`).

> **Change from old version**: Border was inline in `render_minimap()`. Now extracted to its own function. Border thickness changed from 5 pixels to 2 pixels. Border color changed from white (`MINIMAP_BORDER_COLOR = 0x00F5F5F5`) to dark grey (`0x00555555`).

---

## How It All Ties Together

1. **Player moves** → `game->player.player_x, player_y` update.
2. **Player looks** → `game->player.dir_x/y, plane_x/y` update (camera direction).
3. **Each frame**, the render loop calls `render_minimap()` (if `game->minimap == true`).
4. `render_minimap()` samples the map around the player, casts FOV rays, draws border, and places the player dot.
5. The image buffer is displayed at screen position `(MINIMAP_MARGIN, MINIMAP_MARGIN)` as a circular view.

---

## Summary of Variable Types

| Variable | Type | Range / Meaning |
|---|---|---|
| `world_x, world_y` | `double` | Map coordinates in tiles (e.g., `5.5` = 5.5 tiles from origin) |
| `game->player.player_x/y` | `double` | Player's current world position |
| `game->player.dir_x/y` | `double` | Normalized direction vector player is looking |
| `game->player.plane_x/y` | `double` | Perpendicular camera plane vector (defines FOV width) |
| `game->minimap_tile_size` | `double` | Screen pixels per world tile |
| `game->minimap_radius_tiles` | `double` | Minimap visible radius in world tiles |
| `radius_px` | `int` | `minimap_radius_tiles * minimap_tile_size` (circle radius in pixels) |
| `diameter` | `int` | `radius_px * 2` (bounding square to iterate) |
| `offset_x, offset_y` | `int` | Top-left minimap position on screen (always `MINIMAP_MARGIN`) |
| `poffx, poffy` | `double` | Pixel offset from minimap center |
| `pixel_x, pixel_y` | `int` | Loop counters over the bounding square |
| `cell_x, cell_y` | `int` | Map grid indices (column, row) |
| `cell` | `char` | Map cell character (`'0'`, `'1'`, `' '`, etc.) |
| `color` | `unsigned int` | Packed RGB color value (`0x00RRGGBB`) |
| `ray_t` | `double` | FOV interpolation parameter (`0.0` = left edge, `1.0` = right edge) |
| `ray_distance` | `double` | Distance traveled along a ray in world tile units |
| `dist` | `double` | Wall hit distance in tiles → converted to pixels for rendering |
| `t_dp` | struct | Double-precision 2D point / vector `{x, y}` |
| `t_ip` | struct | Integer 2D point `{x, y}` |
| `t_fov_ray` | struct | Bundles `left`, `right`, `idx`, `total` for the FOV ray loop |