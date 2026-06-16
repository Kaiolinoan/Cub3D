# Minimap Line-by-Line Breakdown

## Constants (Macros at Top)

```c
#define MINIMAP_TILE_SIZE 14
```
- **What**: Each map tile (1 world unit) is drawn as 14×14 pixels on the minimap.
- **Why**: Scaling factor to convert from world coordinates to screen pixels. Larger = more zoomed in.

```c
#define MINIMAP_RADIUS_TILES 5.0
```
- **What**: The minimap circle shows 5 tiles in radius around the player.
- **Why**: Determines the FOV size. With `MINIMAP_TILE_SIZE=14`, this equals 70 pixels radius.

```c
#define MINIMAP_MARGIN 12
```
- **What**: The minimap is drawn 12 pixels from the top-left corner of the game window.
- **Why**: Padding so the minimap doesn't touch the window edge.

```c
#define MINIMAP_FLOOR_COLOR 0x00EDF4FC
#define MINIMAP_WALL_COLOR 0x00001F8C
#define MINIMAP_VOID_COLOR 0x00181818
#define MINIMAP_BORDER_COLOR 0x00F5F5F5
#define MINIMAP_PLAYER_COLOR 0x00FF4C4C
#define MINIMAP_FOV_COLOR 0x0000E5FF
```
- **What**: Hex RGB color codes (0x00RRGGBB format).
- **Why**: Visual appearance on screen:
  - FLOOR_COLOR = light cyan (walkable areas)
  - WALL_COLOR = dark blue (walls/obstacles)
  - VOID_COLOR = dark gray (empty space outside map)
  - BORDER_COLOR = white (circle outline)
  - PLAYER_COLOR = red (player dot at center)
  - FOV_COLOR = cyan (field of view wedge)

---

## Function: `display_minimap(t_game *game)` (Line 18-23)

```c
void    display_minimap(t_game *game)
{
    if (game->minimap == false)
        game->minimap = true;
    else
        game->minimap = false;
}
```

**Variables:**
- `game->minimap`: Boolean flag in the game state.
  - **What it stores**: true if minimap should be drawn, false if hidden.
  - **What it does**: This function toggles it on/off (like pressing M to show/hide minimap).
  - **Used by**: The render loop checks this flag before calling `render_minimap()`.

---

## Function: `minimap_map_height(t_map *map)` (Line 25-28)

```c
static int	minimap_map_height(t_map *map)
{
	return ((int)array_len(map->grid));
}
```

**Variables:**
- `map->grid`: Array of strings (each string is one row of the map).
  - **What it stores**: The 2D map layout as an array of C strings.
  - **What it does**: We count how many rows exist using `array_len()`.
- **Return value**: Integer = number of rows in the map.
- **Used for**: Bounds checking in `minimap_tile_color()` to know if a coordinate is out of bounds.

---

## Function: `minimap_tile_color(...)` (Line 30-49)

```c
static unsigned int	minimap_tile_color(t_game *game, double world_x,
	double world_y)
{
	int		cell_x;      // Stores which column (X) of the map grid
	int		cell_y;      // Stores which row (Y) of the map grid
	char	cell;         // Stores the character at map[cell_y][cell_x]
	int		map_height;  // Stores total number of rows in the map

	map_height = minimap_map_height(game->map);
	cell_x = (int)floor(world_x);  // Convert world X coord to map column
	cell_y = (int)floor(world_y);  // Convert world Y coord to map row
	
	// Check if cell_y is out of bounds (above or below map)
	if (cell_y < 0 || cell_y >= map_height || !game->map->grid[cell_y])
		return (MINIMAP_VOID_COLOR);  // Return dark gray for out-of-bounds
	
	// Check if cell_x is out of bounds (left or right of map)
	if (cell_x < 0 || cell_x >= (int)ft_strlen(game->map->grid[cell_y]))
		return (MINIMAP_VOID_COLOR);  // Return dark gray for out-of-bounds
	
	cell = game->map->grid[cell_y][cell_x];  // Get the character at this position
	
	if (cell == '1')          // If it's a wall
		return (MINIMAP_WALL_COLOR);      // Return dark blue
	if (cell == ' ')          // If it's empty space/void
		return (MINIMAP_VOID_COLOR);      // Return dark gray
	return (MINIMAP_FLOOR_COLOR);         // Otherwise it's floor (return light cyan)
}
```

**Purpose**: Given a world coordinate (X, Y), determine what color that location should be on the minimap.

**Variables explained:**
- `world_x, world_y`: Input parameters. Coordinates in world space (in map tile units, e.g., 5.5 means 5.5 tiles from origin).
- `cell_x, cell_y`: The map grid indices (row/column) that correspond to the world position. Calculated using `floor()` which rounds down.
  - Example: world_x=5.7 → cell_x=5 (5th column)
  - Example: world_y=3.2 → cell_y=3 (3rd row)
- `map_height`: Total number of rows in the map (e.g., if map has 15 rows, this is 15).
- `cell`: The character stored in the map at position [cell_y][cell_x].
  - `'1'` = wall
  - `' '` = void/empty
  - Other characters (e.g., `'0'`, `'S'`, `'N'`, `'E'`, `'W'`) = floor

**Logic flow:**
1. Convert world coords to map cell indices.
2. Check if those indices are valid (not outside grid bounds).
3. Read the character at that cell.
4. Return the appropriate color based on what the character is.

**Used by**: `render_minimap()` to determine the color of each pixel on the minimap.

---

## Function: `put_minimap_pixel(...)` (Line 51-56)

```c
static void	put_minimap_pixel(t_game *game, t_img *img, int x, int y,
	unsigned int color)
{
	if (x < 0 || y < 0 || x >= game->win_w || y >= game->win_h)
		return ;  // If pixel is outside the window, do nothing (early exit)
	my_pixel_put(img, x, y, color);  // Otherwise, write the color to the image
}
```

**Variables:**
- `game->win_w`: Window width in pixels (e.g., 1920).
- `game->win_h`: Window height in pixels (e.g., 1080).
- `x, y`: Pixel coordinates to draw at (in window space).
- `color`: The RGB color value to write.
- `img`: The image buffer to draw into (the frame being rendered).

**Purpose**: Safe pixel drawing. Prevents writing outside the window bounds (segfault protection).

**Used by**: Every function that needs to draw a pixel (`draw_minimap_line`, `draw_minimap_marker`, `render_minimap`).

---

## Function: `minimap_in_circle(...)` (Line 58-67)

```c
static bool	minimap_in_circle(int x, int y, int center_x, int center_y,
	int radius_px)
{
	int	center_offset_x;  // X distance from point to circle center
	int	center_offset_y;  // Y distance from point to circle center

	center_offset_x = x - center_x;        // How far is point X from center X?
	center_offset_y = y - center_y;        // How far is point Y from center Y?
	return (center_offset_x * center_offset_x + center_offset_y * center_offset_y <= radius_px * radius_px);  
	// Pythagorean theorem: if distance^2 <= radius^2, point is inside circle
}
```

**Purpose**: Circle membership test. Used to check if a pixel is inside the minimap circle.

**Variables:**
- `x, y`: Point coordinates to test (pixel position).
- `center_x, center_y`: Center of the circle (e.g., where player is on minimap).
- `radius_px`: Radius of circle in pixels.
- `center_offset_x, center_offset_y`: Offsets from center.

**Math**: Uses the circle equation: $(x - center_x)^2 + (y - center_y)^2 \le r^2$.

**Used by**: `draw_minimap_line()` to clip lines to the circle, and `render_minimap()` to decide which pixels to sample.

---

## Function: `raycast_to_wall(...)` (NEW - Wall Detection)

```c
static double	raycast_to_wall(t_game *game, double start_x, double start_y,
	double dir_x, double dir_y, double max_distance)
{
	double	ray_x;         // Current position along the ray
	double	ray_y;
	double	ray_step;      // How far to step per iteration (world units)
	double	ray_distance;  // Distance traveled so far

	ray_step = 0.05;       // Step 0.05 world units at a time (precision)
	ray_distance = 0.0;
	ray_x = start_x;       // Start from player's world position
	ray_y = start_y;
	while (ray_distance < max_distance)
	{
		// Check if current position is a wall
		if (minimap_tile_color(game, ray_x, ray_y) == MINIMAP_WALL_COLOR)
			return (ray_distance);  // Found wall, return distance traveled
		
		// Step along the ray direction
		ray_x += dir_x * ray_step;
		ray_y += dir_y * ray_step;
		ray_distance += ray_step;
	}
	return (max_distance);  // No wall found within max_distance
}
```

**Purpose**: Cast a ray from the player in a given direction and find where it hits a wall. This enables the FOV wedge to stop at walls instead of passing through them.

**Variables explained:**
- `start_x, start_y`: Player's world position (starting point of ray).
- `dir_x, dir_y`: Normalized direction vector to cast along (unit length, e.g., [0.707, -0.707]).
- `max_distance`: Maximum distance to check (in world tiles, usually `MINIMAP_RADIUS_TILES`).
- `ray_x, ray_y`: Current position as we step along the ray.
- `ray_step`: How far to move per iteration (0.05 world tiles). 
  - Smaller values = more accurate detection but slower.
  - Larger values = faster but may miss thin walls.
- `ray_distance`: Total distance traveled along the ray so far.

**Algorithm:**
1. Start at player position.
2. Loop up to max_distance:
   - Check if current position is a wall using `minimap_tile_color()`.
   - If wall found, return the distance traveled so far.
   - Otherwise, step forward by (normalized_direction × step_size).
3. If loop completes without hitting a wall, return max_distance (ray reached max range).

**Used by**: `draw_minimap_fov()` to determine where each FOV ray stops.

---

## Function: `draw_minimap_fov_line(...)` (formerly `draw_minimap_line`)

```c
static void	draw_minimap_fov_line(t_game *game, t_img *img, int start_x,
	int start_y, double end_x, double end_y)
{
	double	line_step_x;  // How much X changes per iteration
	double	line_step_y;  // How much Y changes per iteration
	double	line_x;       // Current X position as we step along the line
	double	line_y;       // Current Y position as we step along the line
	int		steps;        // Number of steps to draw (length of line)
	int		line_index;   // Loop counter (current step)
	int		pixel_x;      // Rounded integer X coordinate
	int		pixel_y;      // Rounded integer Y coordinate
	int		radius_px;    // Radius of the minimap circle in pixels

	// Calculate how much to move each axis
	line_step_x = end_x - start_x;  // Total X distance
	line_step_y = end_y - start_y;  // Total Y distance
	
	// Calculate number of steps needed (use the longer axis)
	// This ensures the line is smooth without gaps
	if (ft_abs(line_step_x) > ft_abs(line_step_y))
		steps = ft_abs(line_step_x);
	else
		steps = ft_abs(line_step_y);
	if (steps <= 0)
		steps = 1;  // Ensure at least 1 step
	
	line_x = start_x;  // Start position
	line_y = start_y;
	
	radius_px = (int)(MINIMAP_RADIUS_TILES * MINIMAP_TILE_SIZE);  // Calculate circle radius
	
	line_index = 0;
	while (line_index <= steps)
	{
		// Round current position to nearest integer pixel
		pixel_x = (int)(line_x + 0.5);
		pixel_y = (int)(line_y + 0.5);
		
		// Only draw if pixel is inside the minimap circle
		if (minimap_in_circle(pixel_x, pixel_y, start_x, start_y, radius_px))
			put_minimap_pixel(game, img, pixel_x, pixel_y, MINIMAP_FOV_COLOR);
		
		// Move one step toward the end point
		line_x += line_step_x / steps;
		line_y += line_step_y / steps;
		line_index++;
	}
}
```

**Purpose**: Draw a line from (start_x, start_y) to (end_x, end_y) using cyan color, but only the part inside the minimap circle.

**Variables explained:**
- `start_x, start_y`: Starting point (always minimap center when drawing FOV).
- `end_x, end_y`: Ending point (on the circle edge or at wall, representing FOV direction).
- `line_step_x, line_step_y`: Total displacement vectors. Example: if start=(100,100) and end=(150,80), then line_step_x=50, line_step_y=-20.
- `steps`: How many iterations needed. Takes the longer axis to avoid gaps. Example: if line_step_x=50 and line_step_y=-20, steps=50.
- `line_x, line_y`: Floating-point tracking as we step along the line.
- `line_index`: Loop counter, increments from 0 to steps.
- `pixel_x, pixel_y`: Rounded-to-integer pixel coordinates. The `+ 0.5` before casting ensures proper rounding.
- `radius_px`: Minimap circle radius (=70 pixels if MINIMAP_TILE_SIZE=14 and MINIMAP_RADIUS_TILES=5.0).
- `steps`: How many iterations needed. Takes the longer axis to avoid gaps. Example: if step_x=50 and step_y=-20, steps=50.
- `line_x, line_y`: Floating-point tracking as we step along the line.
- `line_index`: Loop counter, increments from 0 to steps.
- `pixel_x, pixel_y`: Rounded-to-integer pixel coordinates. The `+ 0.5` before casting ensures proper rounding.
- `radius_px`: Minimap circle radius (=70 pixels if MINIMAP_TILE_SIZE=14 and MINIMAP_RADIUS_TILES=5.0).

**Algorithm (DDA - Digital Differential Analyzer):**
1. Calculate total displacement.
2. Determine step count (max of axis distances).
3. Loop from 0 to steps:
   - Round current position to pixel.
   - If pixel is in circle, draw it cyan.
   - Increment current by (step/steps) to move along the line.

**Used by**: `draw_minimap_fov()` to draw the FOV wedge rays.

---

## Function: `draw_minimap_fov(...)` (Wall-Aware FOV)

```c
static void	draw_minimap_fov(t_game *game, t_img *img, int center_x,
	int center_y)
{
	double	fov_left_x;         // X component of left FOV edge direction
	double	fov_left_y;         // Y component of left FOV edge direction
	double	fov_right_x;        // X component of right FOV edge direction
	double	fov_right_y;        // Y component of right FOV edge direction
	double	ray_dir_x;          // Interpolated ray direction X (between left and right)
	double	ray_dir_y;          // Interpolated ray direction Y (between left and right)
	double	ray_dir_len;        // Length (magnitude) of the ray direction vector
	double	ray_t;              // Interpolation parameter (0.0 to 1.0)
	int		rays;               // Number of rays to draw
	int		ray_index;          // Loop counter
	double	radius_px;          // Radius of minimap circle in pixels
	double	radius_world;       // Radius of minimap circle in world tiles
	double	distance_to_wall;   // Distance from player to wall (world units)
	double	pixel_distance;     // Distance converted to minimap pixels
	double	endpoint_x;         // Endpoint of ray (after hitting wall)
	double	endpoint_y;

	radius_px = MINIMAP_RADIUS_TILES * MINIMAP_TILE_SIZE;
	radius_world = MINIMAP_RADIUS_TILES;
	
	// Get player's viewing direction
	fov_left_x = game->player.dir_x - game->player.plane_x;
	fov_left_y = game->player.dir_y - game->player.plane_y;
	
	// Get right edge of FOV
	fov_right_x = game->player.dir_x + game->player.plane_x;
	fov_right_y = game->player.dir_y + game->player.plane_y;
	
	rays = (int)(radius_px * 5);
	if (rays < 24)
		rays = 24;
	
	ray_index = 0;
	while (ray_index <= rays)
	{
		ray_t = (double)ray_index / (double)rays;
		ray_dir_x = fov_left_x + (fov_right_x - fov_left_x) * ray_t;
		ray_dir_y = fov_left_y + (fov_right_y - fov_left_y) * ray_t;
		ray_dir_len = sqrt(ray_dir_x * ray_dir_x + ray_dir_y * ray_dir_y);
		if (ray_dir_len > 0)
		{
			// NEW: Raycast to find wall distance
			distance_to_wall = raycast_to_wall(game, game->player.player_x,
				game->player.player_y, ray_dir_x / ray_dir_len, ray_dir_y / ray_dir_len,
				radius_world);
			
			// Convert world distance to pixel distance
			pixel_distance = distance_to_wall * MINIMAP_TILE_SIZE;
			if (pixel_distance > radius_px)
				pixel_distance = radius_px;
			
			// Calculate endpoint where ray stops (at wall or circle edge)
			endpoint_x = center_x + (ray_dir_x / ray_dir_len) * pixel_distance;
			endpoint_y = center_y + (ray_dir_y / ray_dir_len) * pixel_distance;
			
			draw_minimap_fov_line(game, img, center_x, center_y, endpoint_x,
				endpoint_y);
		}
		ray_index++;
	}
}
```

**Purpose**: Draw the player's field of view as a filled wedge on the minimap. The wedge now stops at walls, showing realistic sight lines.

**Variables explained:**

- `game->player.dir_x, game->player.dir_y`: Unit vector pointing where the player is looking (direction).
  - Example: player looking north might be (0, -1).
  - Example: player looking northeast might be (0.707, -0.707).

- `game->player.plane_x, game->player.plane_y`: Vector perpendicular to direction, representing FOV width.
  - Same length as `dir`.
  - Points to the left/right of the player.
  - Example: if FOV is 60°, plane might be half that perpendicular direction.

- `fov_left_x, fov_left_y`: The direction to the **left edge** of the FOV.
  - Formula: `left = dir - plane` (subtracting the plane vector rotates FOV left).

- `fov_right_x, fov_right_y`: The direction to the **right edge** of the FOV.
  - Formula: `right = dir + plane` (adding the plane vector rotates FOV right).

- `ray_dir_x, ray_dir_y`: An **interpolated direction** between left and right edges.
  - When `ray_t=0`: `ray_dir = left` (leftmost ray).
  - When `ray_t=0.5`: `ray_dir = (left + right) / 2` (center ray).
  - When `ray_t=1`: `ray_dir = right` (rightmost ray).

- `ray_dir_len`: Length (magnitude) of the ray direction vector.
  - Used to **normalize** the vector: `(ray_dir_x / ray_dir_len, ray_dir_y / ray_dir_len)` gives a unit direction.

- `ray_t`: Interpolation parameter, goes from 0 to 1.
  - `ray_t = ray_index / rays`, so if rays=350 and ray_index ranges 0–350, ray_t goes 0 → 1 in steps of 1/350.

- `radius_world`: Minimap radius in world tile units (5.0 tiles).

- `distance_to_wall`: **NEW** — Distance from player to wall in world tiles, returned by `raycast_to_wall()`.
  - If no wall is hit within radius_world, this equals radius_world.

- `pixel_distance`: **NEW** — distance_to_wall converted to minimap pixels for rendering.
  - Formula: `pixel_distance = distance_to_wall * MINIMAP_TILE_SIZE`.
  - Capped at radius_px so it never exceeds the circle boundary.

- `endpoint_x, endpoint_y`: **NEW** — The end point of the ray on the minimap, now determined by wall distance instead of always extending to the circle edge.

**Algorithm:**
1. Calculate left and right FOV edge directions from player's `dir` and `plane`.
2. Decide how many rays to draw (~350 by default, `radius_px * 5`).
3. Loop through all rays:
   - Interpolate `ray_t` from 0 to 1.
   - Calculate `ray_dir = fov_left + (fov_right - fov_left) * ray_t` (a direction between left and right).
   - Normalize `ray_dir` to unit length.
   - **NEW**: Call `raycast_to_wall()` to find the distance to the nearest wall along this ray.
   - **NEW**: Convert distance to pixels and cap at circle radius.
   - Calculate endpoint at that distance instead of always at circle edge.
   - Draw a line from center to that endpoint (cyan color).
4. Result: a filled cyan wedge that respects walls—rays stop when they hit obstacles.

**Used by**: `render_minimap()` to overlay the FOV on the minimap.

---

## Function: `draw_minimap_player(...)` (formerly `draw_minimap_marker`)

```c
static void	draw_minimap_player(t_game *game, t_img *img, int center_x,
	int center_y)
{
	int	marker_offset_x;     // X offset from the center (used for drawing circle)
	int	marker_offset_y;     // Y offset from the center (used for drawing circle)
	double	radius;           // Radius of the player marker circle (5 pixels)
	double	radius_sq;        // Radius squared (for circle test)

	radius = 5;                              // Player dot is 5 pixels in radius
	radius_sq = radius * radius;            // Pre-calculate radius^2 = 25
	
	marker_offset_y = -5;  // Start 5 pixels above center
	while (marker_offset_y <= 5)  // Loop from -5 to +5 (height of 11 pixels)
	{
		marker_offset_x = -5;  // Start 5 pixels to the left of center
		while (marker_offset_x <= 5)  // Loop from -5 to +5 (width of 11 pixels)
		{
			// Check if this pixel is inside the circle
			if ((marker_offset_x * marker_offset_x + marker_offset_y * marker_offset_y) <= radius_sq)
				put_minimap_pixel(game, img, center_x + marker_offset_x, center_y + marker_offset_y,
					MINIMAP_PLAYER_COLOR);  // Draw red pixel
			marker_offset_x++;
		}
		marker_offset_y++;
	}
}
```

**Purpose**: Draw a small filled red circle at the minimap center to represent the player.

**Variables explained:**
- `center_x, center_y`: Position of the player on the minimap (always at the center of the rendered circle).
- `radius`: Size of the player marker = 5 pixels.
- `radius_sq`: Pre-calculated `radius^2 = 25` (optimization to avoid recalculating in loop).
- `marker_offset_x, marker_offset_y`: Offsets from center in pixels.
  - `marker_offset_y` loops from -5 to +5 (vertical span).
  - `marker_offset_x` loops from -5 to +5 (horizontal span).
  - This creates an 11×11 bounding box around center.

**Algorithm:**
1. Loop through an 11×11 square centered on the player position.
2. For each pixel, check if it's inside a circle of radius 5: `marker_offset_x^2 + marker_offset_y^2 <= 25`.
3. If inside, draw it red (MINIMAP_PLAYER_COLOR).
4. Result: a small red filled circle.

**Used by**: `render_minimap()` to draw the player indicator.

---

## Function: `render_minimap(...)` — **MAIN RENDERER**

```c
void	render_minimap(t_game *game, t_img *img)
{
	int		radius_px;           // Minimap circle radius in pixels
	int		diameter;            // Diameter of minimap = 2 * radius
	int		offset_x;            // X position of minimap top-left corner on screen
	int		offset_y;            // Y position of minimap top-left corner on screen
	int		pixel_x;             // Current screen X pixel (within minimap square)
	int		pixel_y;             // Current screen Y pixel (within minimap square)
	double	pixel_offset_x;      // Pixel offset from circle center (X)
	double	pixel_offset_y;      // Pixel offset from circle center (Y)
	double	world_x;             // World coordinate (X) this pixel maps to
	double	world_y;             // World coordinate (Y) this pixel maps to
	double	player_world_x;      // Player's world X position
	double	player_world_y;      // Player's world Y position

	// Calculate dimensions
	radius_px = (int)(MINIMAP_RADIUS_TILES * MINIMAP_TILE_SIZE);  // 70 pixels
	diameter = radius_px * 2;  // 140 pixels (width and height of square)
	
	// Position of minimap on screen
	offset_x = MINIMAP_MARGIN;  // 12 pixels from left
	offset_y = MINIMAP_MARGIN;  // 12 pixels from top
	
	// Player position in world space
	player_world_x = game->player.player_x;  // Current player X (in tiles)
	player_world_y = game->player.player_y;  // Current player Y (in tiles)
	
	// ===== PHASE 1: Draw the map tiles (floor/wall/void) =====
	pixel_y = 0;
	while (pixel_y < diameter)  // For each row of the minimap square
	{
		pixel_x = 0;
		while (pixel_x < diameter)  // For each column of the minimap square
		{
			// Calculate pixel offset from circle center
			pixel_offset_x = pixel_x - radius_px;  // If pixel_x=70, this is 0 (center); pixel_x=0 gives -70 (left edge)
			pixel_offset_y = pixel_y - radius_px;
			
			// Check if this pixel is inside the minimap circle
			if (pixel_offset_x * pixel_offset_x + pixel_offset_y * pixel_offset_y <= (double)(radius_px * radius_px))
			{
				// Convert pixel offset to world coordinates
				// Each pixel is 1/MINIMAP_TILE_SIZE of a world unit
				world_x = player_world_x + (pixel_offset_x / (double)MINIMAP_TILE_SIZE);
				world_y = player_world_y + (pixel_offset_y / (double)MINIMAP_TILE_SIZE);
				
				// Get the color for this world position and draw it
				put_minimap_pixel(game, img, offset_x + pixel_x, offset_y + pixel_y,
					minimap_tile_color(game, world_x, world_y));
			}
			pixel_x++;
		}
		pixel_y++;
	}
	
	// ===== PHASE 2: Draw the FOV wedge =====
	draw_minimap_fov(game, img, offset_x + radius_px, offset_y + radius_px);
	
	// ===== PHASE 3: Draw the border ring =====
	pixel_y = 0;
	while (pixel_y < diameter)
	{
		pixel_x = 0;
		while (pixel_x < diameter)
		{
			pixel_offset_x = pixel_x - radius_px;
			pixel_offset_y = pixel_y - radius_px;
			
			// Check if pixel is in the border ring
			// Inner edge: distance >= (radius - 5)
			// Outer edge: distance <= radius
			// This creates a 5-pixel-thick white ring
			if (pixel_offset_x * pixel_offset_x + pixel_offset_y * pixel_offset_y <= (double)(radius_px * radius_px)
				&& pixel_offset_x * pixel_offset_x + pixel_offset_y * pixel_offset_y >= (double)((radius_px - 5)
					* (radius_px - 1)))
				put_minimap_pixel(game, img, offset_x + pixel_x, offset_y + pixel_y,
					MINIMAP_BORDER_COLOR);  // White
			pixel_x++;
		}
		pixel_y++;
	}
	
	// ===== PHASE 4: Draw the player marker =====
	draw_minimap_player(game, img, offset_x + radius_px, offset_y + radius_px);
}
```

**Purpose**: The main function that renders the entire minimap. Called every frame.

**Overview by phase:**

**PHASE 1 - Map Sampling (lines with first while loops):**
- Loops through a 140×140 pixel square.
- For each pixel, calculates its offset from the circle center: `pixel_offset_x = pixel_x - 70`, `pixel_offset_y = pixel_y - 70`.
- Only processes pixels inside the circle (distance ≤ radius).
- Converts pixel offset to world coordinates: `world_x = player_world_x + pixel_offset_x / 14`.
- Queries the map color at that world position via `minimap_tile_color()`.
- Draws the pixel at screen position `(offset_x + pixel_x, offset_y + pixel_y)`.
- Result: the minimap shows a circular view of the map centered on the player.

**Example trace:**
- Player at world position (10, 10).
- Minimap center at screen (82, 82) [12 + 70].
- If `pixel_x=70, pixel_y=70` (center pixel):
  - `pixel_offset_x = 0, pixel_offset_y = 0`
  - `world_x = 10 + 0/14 = 10, world_y = 10`
  - Check map at (10, 10), draw that color at screen (82, 82).
- If `pixel_x=0, pixel_y=70` (left edge):
  - `pixel_offset_x = -70, pixel_offset_y = 0`
  - `world_x = 10 - 5 = 5, world_y = 10`
  - Check map 5 tiles to the left of player.

**PHASE 2 - FOV Overlay:**
- Calls `draw_minimap_fov()` to draw the cyan FOV wedge on top.

**PHASE 3 - Border Ring:**
- Similar double loop through the square.
- For each pixel, checks if it's in the border:
  - Inner boundary: `pixel_offset_x^2 + pixel_offset_y^2 >= (r-5)^2` (not too close to center)
  - Outer boundary: `pixel_offset_x^2 + pixel_offset_y^2 <= r^2` (within circle)
  - This forms a white ring 5 pixels thick.

**PHASE 4 - Player Marker:**
- Calls `draw_minimap_player()` to draw a small red dot at the center.

**Key variables:**
- `radius_px = 70`: Minimap circle radius.
- `diameter = 140`: Square to loop through.
- `offset_x, offset_y = 12`: Top-left position on screen (margin from corner).
- `player_world_x/y`: Player's current position (changes every frame as player moves).
- `pixel_x, pixel_y`: Loop counters iterating over the square.
- `pixel_offset_x, pixel_offset_y`: Pixel offsets from circle center.
- `world_x/y`: The world tile coordinate that the pixel represents.

**Final render order:**
1. Map tiles (floor/wall/void colors).
2. FOV wedge (cyan lines).
3. Border ring (white circle outline).
4. Player marker (red dot).

---

## How it all ties together in the game

1. **Player moves** → `game->player.player_x, player_y` update.
2. **Player looks** → `game->player.dir_x/y, plane_x/y` update (camera direction).
3. **Each frame**, rendering code calls `render_minimap()`.
4. `render_minimap()` samples the map around the player, draws the FOV wedge, and renders everything to the image buffer.
5. **Image buffer is displayed** on screen at position (12, 12) as a circular view.

---

## Summary of Variable Types

| Variable | Type | Range/Meaning |
|----------|------|---------------|
| `world_x/y` | `double` | Map coordinates in tiles (e.g., 5.5 = 5.5 tiles from origin) |
| `player_world_x/y` | `double` | Player's current position |
| `game->player.player_x/y` | `double` | Player position (same as player_world_x/y) |
| `game->player.dir_x/y` | `double` | Normalized direction vector player is looking |
| `game->player.plane_x/y` | `double` | Normalized perpendicular vector (FOV width) |
| `pixel_offset_x/y` | `double` or `int` | Offset from minimap center in pixels |
| `pixel_x/y` | `int` | Screen pixel coordinates within the 140×140 square |
| `offset_x/y` | `int` | Top-left position of minimap on screen |
| `radius_px` | `int` | 70 pixels (circle radius) |
| `cell_x/y` | `int` | Map grid indices (row/column) |
| `cell` | `char` | Map cell character ('0', '1', ' ', etc.) |
| `color` | `unsigned int` | RGB hex color value |

