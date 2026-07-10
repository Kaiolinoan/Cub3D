# raycasting2.c — Function Breakdown

## Function: `load_door_texture(t_game *game, t_ray *ray)` (static)

```c
static void	load_door_texture(t_game *game, t_ray *ray)
{
	t_door	*door;
	int		frame;

	door = get_door_at(game, ray->map_x, ray->map_y);
	if (!door)
		return ;
	frame = door->frame;
	if (frame >= DOOR_ANIM_FRAMES)
		frame = DOOR_ANIM_FRAMES - 1;
	if (frame < 0)
		frame = 0;
	ray->texture = game->sprites.door_frames[frame];
}
```

**Purpose**: Load the appropriate door animation frame texture for the current ray based on door state.

**Parameters:**
- `game`: Game struct with door list and sprite frames.
- `ray`: Ray struct with hit grid coordinates (map_x, map_y).

**Return Value:** None (void).

**Variables:**
- `door`: Pointer to the door at (map_x, map_y).
- `frame`: Current animation frame index (0 to DOOR_ANIM_FRAMES-1).

**Logic:**
1. Find door at ray hit cell via `get_door_at()`.
2. If no door exists, return (shouldn't happen if `ray->is_door` was set).
3. Get current frame index from door state.
4. Clamp frame to valid range [0, DOOR_ANIM_FRAMES-1].
5. Load the corresponding texture from `game->sprites.door_frames[frame]`.

---

## Function: `load_texture(t_game *game, t_ray *ray)`

```c
void	load_texture(t_game *game, t_ray *ray)
{
	if (ray->is_door)
		return (load_door_texture(game, ray));
	if (ray->side == 0)
	{
		if (ray->ray_dir_x > 0)
			ray->texture = game->sprites.east;
		else
			ray->texture = game->sprites.west;
	}
	else
	{
		if (ray->ray_dir_y > 0)
			ray->texture = game->sprites.south;
		else
			ray->texture = game->sprites.north;
	}
}
```

**Purpose**: Select the appropriate wall texture based on ray hit orientation and direction.

**Parameters:**
- `game`: Game struct with sprite textures.
- `ray`: Ray struct with hit side and direction information.

**Return Value:** None (void).

**Logic:**

1. **If door hit** — Load appropriate door frame texture.

2. **If vertical wall hit** (`ray->side == 0`):
   - Ray hit a vertical grid line (crossed X-axis).
   - If ray going right (`ray_dir_x > 0`) → `texture = east`.
   - Else (ray going left) → `texture = west`.

3. **If horizontal wall hit** (`ray->side == 1`):
   - Ray hit a horizontal grid line (crossed Y-axis).
   - If ray going down (`ray_dir_y > 0`) → `texture = south`.
   - Else (ray going up) → `texture = north`.

**Texture Orientation:**
- East/West textures are used for vertical walls.
- North/South textures are used for horizontal walls.
- Allows different textures for different wall orientations (visual variety).

---

## Function: `find_wall_hit(t_game *game, t_ray *ray)`

```c
void	find_wall_hit(t_game *game, t_ray *ray)
{
	double	wall_x;

	if (ray->side == 0)
		wall_x = game->player.player_y + ray->perp_wall_dist * ray->ray_dir_y;
	else
		wall_x = game->player.player_x + ray->perp_wall_dist * ray->ray_dir_x;
	wall_x -= floor(wall_x);
	ray->tex_x = (int)(wall_x * PX);
	if (ray->side == 0 && ray->ray_dir_x > 0)
		ray->tex_x = PX - ray->tex_x - 1;
	if (ray->side == 1 && ray->ray_dir_y < 0)
		ray->tex_x = PX - ray->tex_x - 1;
}
```

**Purpose**: Calculate the X coordinate within the wall texture (which part of the texture to sample horizontally).

**Parameters:**
- `game`: Game struct with player position.
- `ray`: Ray struct with perpendicular distance, ray direction, and hit side.

**Return Value:** None (void). Updates `ray->tex_x`.

**Variables:**
- `wall_x`: World coordinate on the hit wall (interpolated along perpendicular).
- `ray->tex_x`: Texture X coordinate (0 to PX-1, where PX=64).

**Logic:**

1. **Calculate wall intersection point** — Project the perpendicular distance along the ray:
   - If vertical wall hit (`side == 0`):
     - Wall_x = player_y + perp_dist * ray_dir_y.
     - The Y coordinate on the hit wall.
   - If horizontal wall hit (`side == 1`):
     - Wall_x = player_x + perp_dist * ray_dir_x.
     - The X coordinate on the hit wall.

2. **Get fractional part** — `wall_x -= floor(wall_x)` gives value in [0.0, 1.0).
   - This represents where on the wall the ray hit (relative position within grid cell).

3. **Convert to texture pixel** — `tex_x = (int)(wall_x * PX)`.
   - Scale fractional position (0.0-1.0) to texture range (0 to PX-1).
   - PX = 64 (texture size), so `tex_x ∈ [0, 63]`.

4. **Flip horizontally if needed** — Correct for ray direction:
   - Vertical hit going right (side==0, dir_x>0): `tex_x = PX - tex_x - 1`.
   - Horizontal hit going up (side==1, dir_y<0): `tex_x = PX - tex_x - 1`.
   - This ensures textures aren't mirrored.

**Example:**
```
Wall Y = 5.3 (at a vertical wall)
Fractional part = 0.3
tex_x = (int)(0.3 * 64) = 19 (sample pixel 19 of 64 from left)
```

---

## Function: `get_texture_pixel(t_img *texture, int x, int y)`

```c
unsigned int	get_texture_pixel(t_img *texture, int x, int y)
{
	int	offset;

	offset = y * texture->line_length + x * (texture->bits_per_pixel / 8);
	return (*(unsigned int *)(texture->addr + offset));
}
```

**Purpose**: Sample a single pixel from a texture image.

**Parameters:**
- `texture`: Texture image struct with address, line length, bits per pixel.
- `x`, `y`: Pixel coordinates within the texture (0 to PX-1).

**Return Value:**
- Packed RGB color `0x00RRGGBB`.

**Logic:**
1. Compute byte offset: `y * line_length + x * bytes_per_pixel`.
2. Dereference as unsigned int and return the color value.

**Used By:**
- `draw_textured_line()` — Samples texture for each pixel of wall slice.

---

## Function: `calculate_wall(t_game *game, t_ray *ray)`

```c
void	calculate_wall(t_game *game, t_ray *ray)
{
	if (ray->side == 0)
		ray->perp_wall_dist = ray->side_dist_x - ray->delta_dist_x;
	else
		ray->perp_wall_dist = ray->side_dist_y - ray->delta_dist_y;
	if (ray->perp_wall_dist == 0)
		ray->perp_wall_dist = 0.0001;
	ray->line_height = (int)(game->win_h / ray->perp_wall_dist);
	ray->draw_start = -ray->line_height / 2 + game->win_h / 2;
	if (ray->draw_start < 0)
		ray->draw_start = 0;
	ray->draw_end = ray->line_height / 2 + game->win_h / 2;
	if (ray->draw_end >= game->win_h)
		ray->draw_end = game->win_h - 1;
}
```

**Purpose**: Calculate perpendicular distance to wall, wall slice height, and pixel range to draw on screen.

**Parameters:**
- `game`: Game struct with window height.
- `ray`: Ray struct with DDA side distances and delta distances; will be populated with wall geometry.

**Return Value:** None (void).

**Variables:**
- `ray->perp_wall_dist`: Perpendicular distance to wall (avoids fish-eye).
- `ray->line_height`: Height of wall slice in pixels.
- `ray->draw_start`: Top pixel to draw.
- `ray->draw_end`: Bottom pixel to draw.

**Logic:**

1. **Calculate perpendicular distance** — Undo the DDA distance increment:
   - If vertical hit (`side == 0`): `perp_dist = side_dist_x - delta_dist_x`.
   - If horizontal hit (`side == 1`): `perp_dist = side_dist_y - delta_dist_y`.
   - This corrects for the final step increment in DDA.

2. **Prevent division by zero** — If `perp_dist == 0`, set to small value `0.0001`.

3. **Calculate wall height** — Perspective projection:
   - `line_height = win_h / perp_dist`.
   - Closer walls are taller, farther walls are shorter.
   - At distance 1: height ≈ window height.
   - At distance 2: height ≈ half window height.

4. **Calculate draw range** — Center wall in viewport:
   - `draw_start = -line_height/2 + win_h/2` (top of wall).
   - `draw_end = line_height/2 + win_h/2` (bottom of wall).
   - Clamp to screen bounds [0, win_h-1].

**Screen Positioning:**
```
Window height = 1080
Wall at distance 1: line_height = 1080, draws from 0 to 1079 (full screen)
Wall at distance 2: line_height = 540, draws from 270 to 809 (center half)
Wall at distance 12: line_height = 90, draws from 495 to 585 (thin center)
```
