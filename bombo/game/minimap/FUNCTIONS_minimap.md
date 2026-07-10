# Minimap Functions — Function Breakdown

## minimap/utils.c

### Function: `display_minimap(t_game *game)`

Toggle minimap visibility on/off.

**Logic:**
- `game->minimap = !game->minimap` (toggle boolean).

### Function: `minimap_map_height(t_map *map)`

Get map height (number of rows).

**Logic:**
- Return `array_len(map->grid)`.

### Function: `rgb_to_color(t_rgb *rgb)`

Convert RGB struct to packed 32-bit color.

**Logic:**
- Pack: `(r << 16) | (g << 8) | b`.

### Function: `put_minimap_pixel(t_game *game, int x, int y, unsigned int color)`

Put pixel to backbuffer with bounds checking.

**Logic:**
- Check if (x, y) in screen bounds.
- Call `my_pixel_put()` if valid.

### Function: `minimap_tile_color(t_game *game, double world_x, double world_y)`

Get minimap color for world tile position.

**Logic:**
1. Convert world coordinates to grid cell.
2. Bounds check.
3. Return color based on cell type:
   - `'1'` (wall) → GREY.
   - `' '` (void) → ceiling color.
   - `'D'` (door) → door color.
   - Else (open floor) → floor color.

---

## minimap/minimap_main.c

### Function: `render_background_pixel(t_game *game, t_ip p, int radius_px, t_ip offset)` (static)

Render a single minimap background pixel.

**Logic:**
1. Check if pixel is within circular minimap boundary.
2. Calculate world coordinate from pixel offset.
3. Get tile color at world position.
4. Write to backbuffer.

### Function: `render_minimap_background(t_game *game, int radius_px, int diameter, t_ip offset)` (static)

Render minimap circular background (map display).

**Logic:**
- Double loop over all pixels in minimap diameter.
- For each pixel, call `render_background_pixel()`.

### Function: `render_minimap_border(t_game *game, int radius_px, int diameter, t_ip offset)` (static)

Render minimap circular border (2-pixel outline).

**Logic:**
1. Loop over all pixels in minimap area.
2. Calculate distance from minimap center.
3. If pixel is on circle perimeter (within 2 pixels), draw grey.

### Function: `render_minimap(t_game *game)`

Master minimap render function.

**Logic:**
1. Calculate minimap radius (in pixels) and diameter.
2. Calculate offset (MINIMAP_MARGIN).
3. Render background.
4. Render FOV rays and boundaries.
5. Render border.
6. Draw player marker.

---

## minimap/minimap_player.c

### Function: `draw_minimap_player(t_game *game, int center_x, int center_y)`

Draw player dot at minimap center.

**Logic:**
1. Calculate player marker radius (30% of tile size).
2. Loop over all pixels in marker square.
3. If pixel is within marker circle, write player color (red).

---

## minimap/minimap_fov.c

### Function: `raycast_to_wall(t_game *game, t_dp start, t_dp dir, double max_distance)` (static)

Cast ray from start in direction until wall or max distance.

**Logic:**
1. Step along ray in small increments (0.05 units).
2. At each step, check tile color:
   - If wall (GREY) or door → return distance.
   - If beyond max_distance → return max_distance.
3. Track accumulated distance.

### Function: `get_ray_end(t_game *game, int cx, int cy, t_dp dir)`

Calculate endpoint of FOV boundary ray.

**Logic:**
1. Raycast from player position to find wall distance.
2. Scale distance by tile size (pixels).
3. Calculate endpoint in screen coordinates.

### Function: `draw_single_fov_ray(t_game *game, int cx, int cy, t_fov_ray ray)` (static)

Draw one FOV boundary ray.

**Logic:**
1. Interpolate ray direction from left to right based on index.
2. Normalize direction.
3. Get ray endpoint via `get_ray_end()`.
4. Draw line from center to endpoint.

### Function: `init_fov_dirs(t_game *game, t_dp *left, t_dp *right)` (static)

Initialize left and right FOV boundary directions.

**Logic:**
- Left: `dir - plane` (player direction minus camera plane).
- Right: `dir + plane` (player direction plus camera plane).

### Function: `draw_minimap_fov(t_game *game, int center_x, int center_y)`

Render FOV fan (all boundary rays from player to visible walls).

**Logic:**
1. Get left and right boundary directions.
2. Calculate number of rays to draw (proportional to minimap size).
3. For each ray index, interpolate direction and draw.

---

## minimap/minimap_fov_utils.c

### Function: `blend_color_with_white(unsigned int base_color, double white_opacity)`

Blend color toward white by opacity factor.

**Logic:**
- For each channel: `new = base + (255 - base) * opacity`.
- Higher opacity = whiter.

### Function: `minimap_in_circle(int x, int y, t_ip center, int radius_px)`

Check if pixel is within circle.

**Logic:**
- Calculate squared distance from center.
- Compare to radius² (avoid sqrt for performance).

### Function: `fov_line_step(double *x, double *y, double sx, double sy)` (static)

Advance line drawing position (Bresenham-like).

**Logic:**
1. Determine primary axis (larger of |sx|, |sy|).
2. Step by 1 on primary, proportionally on secondary.

### Function: `fov_line_pixel(t_game *game, t_ip start, double lx, double ly)` (static)

Draw single FOV line pixel.

**Logic:**
1. Check if pixel is within minimap circle.
2. Blend floor color with white for visibility.
3. Write to backbuffer.

### Function: `draw_minimap_fov_line(t_game *game, t_ip start, t_dp end)`

Draw line from start to end (FOV ray).

**Logic:**
1. Calculate line direction and steps.
2. For each step along line, draw pixel.
3. Use `fov_line_step()` to advance smoothly.
