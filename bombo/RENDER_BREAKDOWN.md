# Render.c Breakdown — Raycasting & 3D Rendering

This file contains the core raycasting algorithm that creates the 3D first-person view, plus utility rendering functions.

---

## File Overview

The rendering system:
1. **Raycasting** — Projects rays from player toward each screen column, finds wall intersections
2. **Wall height calculation** — Determines how tall walls appear based on distance
3. **Texture drawing** — Applies sprites/textures to wall segments
4. **Main render loop** — Orchestrates background painting, movement, raycasting, and minimap

---

## Function: `my_pixel_put(t_img *img, int x, int y, int color)`

```c
void	my_pixel_put(t_img *img, int x, int y, int color)
{
	char	*dst;
	int		offset;

	offset = (y * img->line_length + x * (img->bits_per_pixel / 8));
	dst = img->addr + offset;
	*(unsigned int *)dst = color;
}
```

**Purpose**: Draw a single pixel at screen coordinates (x, y) with a given color.

**Variables explained:**
- `img`: Image buffer structure containing pixel data and metadata.
- `x, y`: Screen coordinates of the pixel to draw.
- `color`: RGB color value (unsigned int, typically 0xRRGGBB).
- `offset`: Byte offset in the image buffer where this pixel's data is stored.
- `line_length`: Bytes per row in image (used for 2D→1D coordinate conversion).
- `bits_per_pixel`: Bits per pixel (typically 32 for RGBA, so divide by 8 for bytes).
- `dst`: Pointer to the pixel location in memory.

**Algorithm:**
1. Calculate byte offset: `row_index * bytes_per_row + column_index * bytes_per_pixel`
2. Get pointer to that memory location.
3. Write color value (32-bit unsigned int) at that address.

**Why the formula works:**
- Image buffer is a 1D array in memory, but represents a 2D grid
- Each row takes up `line_length` bytes
- Each pixel takes `bits_per_pixel / 8` bytes
- To access pixel at (x, y): skip y rows + x columns within that row

**Used by**: All drawing functions (textures, raycasting, minimap).

---

## Function: `draw_texture(t_img *img, t_img sprite, float x, float y, int size)` (Static)

```c
static void	draw_texture(t_img *img, t_img sprite, float x, float y, int size)
{
	int				tex_y;
	int				tex_x;
	int				offset;
	unsigned int	color;

	tex_y = 0;
	while (tex_y < size)
	{
		tex_x = 0;
		while (tex_x < size)
		{
			offset = (tex_y * sprite.line_length) + (tex_x * sprite.bits_per_pixel / 8);
			color = *(unsigned int *)(sprite.addr + offset);
			my_pixel_put(img, tex_x + x, tex_y + y, color);
			tex_x++;
		}
		tex_y++;
	}
}
```

**Purpose**: Copy a texture/sprite from source image to screen at position (x, y).

**Variables explained:**
- `img`: Destination image (screen buffer).
- `sprite`: Source image (texture to copy).
- `x, y`: Top-left position on screen to draw the texture.
- `size`: Width and height of the texture in pixels (square texture).
- `tex_x, tex_y`: Loop counters iterating through texture pixels.
- `offset`: Byte offset in the sprite image buffer.
- `color`: Pixel value read from sprite.

**Algorithm:**
1. Loop through each row of the texture (`tex_y` from 0 to size).
2. For each row, loop through each column (`tex_x` from 0 to size).
3. Calculate offset in sprite buffer.
4. Read pixel color from sprite buffer.
5. Draw that pixel to screen at position (x + tex_x, y + tex_y).

**Used by**: Debug rendering (render_wall, print_rays) and potentially texture application.

---

## Function: `paint_background(t_game *game, t_img *img)` (Static)

```c
static void	paint_background(t_game *game, t_img *img)
{
	int	y;
	int	x;

	y = 0;
	while (y < game->win_h)
	{
		x = 0;
		while (x < game->win_w)
			my_pixel_put(img, x++, y, GREY);
		y++;
	}
}
```

**Purpose**: Fill the entire screen with a grey background color before raycasting.

**Variables explained:**
- `game->win_h, win_w`: Window height and width in pixels.
- `y, x`: Loop counters for rows and columns.

**Algorithm:**
1. Loop through every row (y = 0 to win_h).
2. For each row, loop through every column (x = 0 to win_w).
3. Set each pixel to grey color.

**Why this step?** Ensures the entire screen is cleared before drawing new frame.

**Used by**: `render()` (called at start of each frame).

---

## Function: `raycasting(t_game *game, t_player *player, t_img *img)`

This is the **core 3D rendering algorithm**. It implements **DDA-based raycasting** to create the first-person perspective.

```c
int	raycasting(t_game *game, t_player *player, t_img *img) 
{
	double	cameraX;        // Normalized X position on camera plane (-1 to 1)
	double	rayDirX;        // Ray direction X component
	double	rayDirY;        // Ray direction Y component
	double	sideDistX;      // Distance to next X grid line
	double	sideDistY;      // Distance to next Y grid line
	double	deltaDistX;     // Distance between grid lines on X axis
	double	deltaDistY;     // Distance between grid lines on Y axis
	double	perpWallDist;   // Perpendicular distance to wall
	int		stepX;          // Direction to step on X axis (-1 or 1)
	int		stepY;          // Direction to step on Y axis (-1 or 1)
	int		map_x;          // Current map grid X coordinate
	int		map_y;          // Current map grid Y coordinate
	int		x;              // Current screen column
	bool	hit;            // Has ray hit a wall?
	int		side;           // Which side was hit? (0 = X, 1 = Y)
	int		lineHeight;     // Height of wall on screen
	int		DrawStart;      // Top pixel of wall on screen
	int		DrawEnd;        // Bottom pixel of wall on screen
	int		y;              // Current screen row (for drawing)

	x = 0;
	while (x < game->win_w)
	{
		// ... (detailed breakdown below)
		x++;
	}
	return (0);
}
```

### Raycasting Algorithm Overview

The algorithm works column-by-column across the screen:

1. **For each screen column** (x from 0 to win_w):
   - Cast a ray from player toward that column
   - Find where ray hits a wall
   - Calculate wall distance and height
   - Draw a vertical line of wall pixels

### Phase 1: Create Ray Direction

```c
cameraX = 2 * x / (double)game->win_w - 1;  // Range: -1 to 1
rayDirX = player->dir_x + player->plane_x * cameraX;
rayDirY = player->dir_y + player->plane_y * cameraX;
```

**Purpose**: Create a ray direction for this screen column.

**Explanation:**
- `cameraX`: Normalized position on the camera plane, from -1 (left edge) to +1 (right edge).
  - `x = 0` → `cameraX = -1` (leftmost ray)
  - `x = win_w/2` → `cameraX = 0` (center ray)
  - `x = win_w - 1` → `cameraX ≈ 1` (rightmost ray)

- Ray direction = center direction + offset along plane:
  ```
  rayDir = dir + plane * cameraX
  ```
  - When `cameraX = 0`: ray goes straight ahead (dir)
  - When `cameraX = -1`: ray goes toward left edge of FOV
  - When `cameraX = 1`: ray goes toward right edge of FOV

**Example:**
- Player facing east: `dir = (1, 0)`, `plane = (0, 0.66)`
- For leftmost screen column (`x = 0`, `cameraX = -1`):
  - `rayDirX = 1 + 0 * (-1) = 1`
  - `rayDirY = 0 + 0.66 * (-1) = -0.66`
  - Result: ray pointing up-right at an angle

### Phase 2: Initialize Grid Stepping

```c
map_x = (int) player->player_x;
map_y = (int) player->player_y;
hit = false;

// Calculate distance between grid line intersections
if (rayDirX == 0)
	deltaDistX = 1e30;  // Ray is horizontal, never hits X line
else
	deltaDistX = ft_abs(1.0 / rayDirX);

if (rayDirY == 0)
	deltaDistY = 1e30;  // Ray is vertical, never hits Y line
else
	deltaDistY = ft_abs(1.0 / rayDirY);
```

**Purpose**: Prepare for DDA (Digital Differential Analyzer) raycasting by calculating grid traversal values.

**Variables explained:**
- `map_x, map_y`: Current grid cell being tested (starts at player's cell).
- `deltaDistX, deltaDistY`: Distance along ray between consecutive grid line crossings.
  - If ray is horizontal (`rayDirX = 0`): never crosses X lines, set to infinity
  - If ray is vertical (`rayDirY = 0`): never crosses Y lines, set to infinity

**Why `1.0 / rayDir`?**
- Grid lines are 1 unit apart
- Distance = units / speed
- Speed along ray = `|rayDir|`
- So distance = 1 / |rayDir|

**Example:**
- Ray direction: `(1, 0)` (straight east, horizontal)
- `deltaDistX = 1 / 1 = 1` (crosses X line every 1 unit of travel)
- `deltaDistY = 1 / 0 = ∞` (never crosses Y line)

### Phase 3: Calculate Initial Side Distances

```c
if (rayDirX < 0)
{
	stepX = -1;  // Ray going left
	sideDistX = (player->player_x - map_x) * deltaDistX;
}
else
{
	stepX = 1;   // Ray going right
	sideDistX = (map_x + 1.0 - player->player_x) * deltaDistX;
}

if (rayDirY < 0)
{
	stepY = -1;  // Ray going up
	sideDistY = (player->player_y - map_y) * deltaDistY;
}
else
{
	stepY = 1;   // Ray going down
	sideDistY = (map_y + 1.0 - player->player_y) * deltaDistY;
}
```

**Purpose**: Calculate distance from player to the first grid line in each direction.

**`sideDistX` calculation example (rayDirX < 0, going left):**
- Player at world position (5.7, 10.3)
- `map_x = 5` (grid cell)
- Distance to next X grid line to the left = distance from player to left edge of current cell
- Left edge is at x = 5.0
- Distance = (5.7 - 5.0) * deltaDistX (normalized to ray)

**`sideDistX` calculation example (rayDirX > 0, going right):**
- Distance to next X grid line to the right = distance from player to right edge
- Right edge is at x = 6.0
- Distance = (6.0 - 5.7) * deltaDistX

**Why multiply by deltaDistX?** Convert grid distances to ray distances.

### Phase 4: DDA Loop (Find Wall)

```c
while (!hit)
{
	if (sideDistX < sideDistY)
	{
		sideDistX += deltaDistX;
		map_x += stepX;
		side = 0;  // Hit vertical wall (side X)
	}
	else
	{
		sideDistY += deltaDistY;
		map_y += stepY;
		side = 1;  // Hit horizontal wall (side Y)
	}
	if (game->map->grid[map_y][map_x] == '1')
		hit = 1;
}
```

**Purpose**: Step through grid cells until wall is found using DDA algorithm.

**Algorithm:**
1. Compare which grid line is closer (X or Y)
2. Step to that grid line
3. Check if new cell contains wall ('1')
4. If wall found, break; otherwise repeat

**Why DDA?** Efficient grid traversal without floating-point errors. Only steps along actual grid lines.

**Example trace (ray from (5.7, 10.3) going right-up):**
```
Initial: map_x=5, map_y=10, sideDistX=0.3*deltaX, sideDistY=0.7*deltaY
         (assuming deltaX < deltaY, so sideDistX < sideDistY)

Step 1: sideDistX < sideDistY
        → sideDistX += deltaDistX
        → map_x = 6
        → Check grid[10][6]: is it '1'? No, continue

Step 2: sideDistX < sideDistY (still)
        → sideDistX += deltaDistX
        → map_x = 7
        → Check grid[10][7]: is it '1'? YES!
        → hit = true, break
```

### Phase 5: Calculate Perpendicular Distance

```c
if (side == 0)
	perpWallDist = sideDistX - deltaDistX;
else
	perpWallDist = sideDistY - deltaDistY;

if (perpWallDist == 0)
	perpWallDist = 0.0001;  // Avoid division by zero
```

**Purpose**: Get perpendicular distance from player to wall (fixes fish-eye distortion).

**Why perpendicular?** Direct ray distance would show objects curved (fish-eye effect). Perpendicular distance corrects this.

**Why subtract deltaDistX/Y?** We went one step too far, so subtract one step to get to the wall.

### Phase 6: Calculate Wall Height & Screen Position

```c
lineHeight = (int) (game->win_h / perpWallDist);
DrawStart = -lineHeight / 2 + game->win_h / 2;
if (DrawStart < 0)
	DrawStart = 0;
DrawEnd = lineHeight / 2 + game->win_h / 2;
if (DrawEnd >= game->win_h)
	DrawEnd = game->win_h - 1;
```

**Purpose**: Determine where wall appears on screen.

**`lineHeight` calculation:**
- Closer walls appear taller: `height = win_h / distance`
- Farther walls appear shorter
- Example: If wall is 1 unit away and win_h=600, height=600 (fills screen)
- If wall is 2 units away, height=300 (half screen)

**`DrawStart` calculation:**
- Center the wall vertically on screen
- `lineHeight / 2` = half height above/below center
- `-lineHeight / 2 + win_h / 2` = top of wall
- Clamped to [0, win_h]

**`DrawEnd` calculation:**
- `-lineHeight / 2 + win_h / 2 + lineHeight` = bottom of wall
- Simplified to `lineHeight / 2 + win_h / 2`

### Phase 7: Draw Wall Column

```c
y = DrawStart;
while (y < DrawEnd)
{
	my_pixel_put(img, x, y, RED);
	y++;
}
```

**Purpose**: Draw a vertical line representing the wall at screen column x.

**Variables explained:**
- `x`: Screen column (main loop variable from 1 to win_w)
- `y`: Current row within the wall
- `RED`: Wall color (hardcoded for debugging)

**Algorithm:**
1. Loop from DrawStart to DrawEnd
2. Draw each pixel in column x at color RED
3. This creates a vertical stripe of the wall

**Note:** In production, this would sample textures based on which side was hit (`side` variable).

---

## Function: `print_rays(t_player *player, t_img *img)` (Static, Debug Only)

```c
static void print_rays(t_player *player, t_img *img)
{
	int		px;
	int		py;
	double	ray_x;
	double	ray_y;
	double	ray_x2;
	double	ray_y2;
	int		j;
	int		i;

	px = player->player_x * PX;
	py = player->player_y * PX;
	ray_x = player->dir_x - player->plane_x;   // Left FOV edge
	ray_y = player->dir_y - player->plane_y;
	ray_x2 = player->dir_x + player->plane_x;  // Right FOV edge
	ray_y2 = player->dir_y + player->plane_y;
	
	j = 0;
	while (j < 100)
	{
		my_pixel_put(img, px + ray_x * j,
			py + ray_y * j, RED);
		j++;
	}
	
	i = 0;
	while (i < 100)
	{
		my_pixel_put(img, px + ray_x2 * i,
			py + ray_y2 * i, RED);
		i++;
	}
}
```

**Purpose**: Debug visualization of FOV edges on screen.

**What it does:**
- Draws two red lines from player position
- Left line: from player along left FOV edge
- Right line: from player along right FOV edge
- Creates a "cone" showing field of view

**Variables explained:**
- `px, py`: Player position in screen coordinates (multiplied by PX constant)
- `ray_x/y`: Direction of left FOV edge
- `ray_x2/y2`: Direction of right FOV edge
- `j, i`: Loop counters for drawing line length

**Used by**: Debug render mode only (controlled by `debug` flag).

---

## Function: `render(t_game *game)`

```c
int	render(t_game *game)
{
	bool	debug;

	debug = false;
	if (debug)
	{
		// DEBUG MODE: Draw map, walls, player, rays, minimap
		paint_background(game, &game->buffer.img);
		render_wall(game, &game->buffer.img);
		move_player(&game->player);
		draw_texture(&game->buffer.img, game->sprites.south.img, 
			game->player.player_x * PX, game->player.player_y * PX, 20);
		print_rays(&game->player, &game->buffer.img);
		if (game->minimap)
			render_minimap(game, &game->buffer.img);
		mlx_put_image_to_window(game->mlx, game->win, game->buffer.img, 0, 0);
	}
	else
	{
		// PRODUCTION MODE: Standard raycasting rendering
		paint_background(game, &game->buffer.img);
		if (game->minimap)
			render_minimap(game, &game->buffer.img);
		move_player(&game->player);
		raycasting(game, &game->player, &game->buffer.img);
		if (game->minimap)
			render_minimap(game, &game->buffer.img);
		mlx_put_image_to_window(game->mlx, game->win, game->buffer.img, 0, 0);
	}
	return (0);
}
```

**Purpose**: Main render loop called every frame. Orchestrates all rendering operations.

**Debug mode** (when `debug = true`):
1. Paint grey background
2. Draw all walls from map as textures
3. Move player based on input
4. Draw player sprite on top-down view
5. Draw FOV edges as red lines
6. Render minimap if enabled
7. Display buffer to window

**Production mode** (when `debug = false`):
1. Paint grey background
2. Render minimap if enabled (background layer)
3. Move player based on input
4. Run raycasting to draw 3D walls
5. Render minimap if enabled (overlay on top)
6. Display buffer to window

**Why minimap rendered twice in production?** Once as background (behind raycasted view), once on top (so it's visible).

**Used by**: Main game loop (called via MLX hook, ~60 FPS typically).

---

## Raycasting vs Minimap Difference

| Aspect | Raycasting | Minimap |
|--------|-----------|---------|
| **Perspective** | First-person (player's view) | Top-down (bird's eye) |
| **Algorithm** | DDA grid stepping | Pixel-by-pixel sampling |
| **Output** | Vertical wall columns | Circular view |
| **FOV** | Accurate first-person view | Shows player's actual viewcone |
| **Performance** | One ray per screen column | One sample per minimap pixel |

---

## Key Constants Used

- `PX`: Pixels per tile (used for debug visualization scaling)
- `RED`: Color constant for walls (0xFF0000 typically)
- `GREY`: Background color
- `XK_*`: X11 key codes for input handling

---

## Rendering Pipeline Summary

```
┌─────────────────────────────────────────┐
│ render() called every frame             │
└──────────────┬──────────────────────────┘
               │
               ├─ paint_background()
               │  └─ Fill screen with grey
               │
               ├─ render_minimap() [if enabled]
               │  └─ Draw circular top-down view
               │
               ├─ move_player()
               │  └─ Update position/rotation from flags
               │
               ├─ raycasting()
               │  └─ For each screen column:
               │     ├─ Create ray direction
               │     ├─ Find wall intersection (DDA)
               │     ├─ Calculate perpendicular distance
               │     ├─ Calculate wall height
               │     └─ Draw vertical wall stripe
               │
               ├─ render_minimap() [if enabled]
               │  └─ Draw on top as overlay
               │
               └─ mlx_put_image_to_window()
                  └─ Display buffer to screen
```

---

## Performance Characteristics

- **Raycasting**: O(screen_width × raycasting_steps) per frame
- **Pixel put**: O(1) per pixel (direct memory write)
- **No Z-buffering**: Works because DDA guarantees first hit is closest
- **No anti-aliasing**: Pixelated look (characteristic of classic raycasting)

---

## Common Issues & Solutions

**Fish-eye distortion:** Fixed by using perpendicular distance instead of direct ray distance.

**Wall gaps/cracks:** Result of discrete raycasting (one ray per pixel). More rays needed for smoother walls.

**Speed variations:** Rotating camera (changes ray directions) slightly faster/slower than moving (only updates position).

**Clipping issues:** DrawStart/DrawEnd clamping prevents drawing outside screen bounds.
