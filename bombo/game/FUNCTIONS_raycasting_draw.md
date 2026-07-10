# raycasting_draw.c — Function Breakdown

## Function: `draw_textured_line(t_game *game, t_ray *ray, int x, int y)`

```c
void	draw_textured_line(t_game *game, t_ray *ray, int x, int y)
{
	double			step;
	double			tex_pos;
	int				tex_y;
	unsigned int	color;

	step = (double)PX / ray->line_height;
	tex_pos = (ray->draw_start - game->win_h / 2
			+ ray->line_height / 2) * step;
	y = ray->draw_start;
	(load_texture(game, ray), find_wall_hit(game, ray));
	while (y < ray->draw_end)
	{
		tex_y = (int)tex_pos & (PX - 1);
		tex_pos += step;
		if (ray->tex_x < 0 || ray->tex_x >= PX || tex_y < 0 || tex_y >= PX)
		{
			y++;
			continue ;
		}
		color = color_gradient(ray,
				get_texture_pixel(&ray->texture.img, ray->tex_x, tex_y));
		if (!ray->is_door || color)
			my_pixel_put(game, x, y, color);
		y++;
	}
}
```

**Purpose**: Draw a vertical wall slice (texture column) for screen column x.

**Parameters:**
- `game`: Game struct with backbuffer and window dimensions.
- `ray`: Ray struct with wall geometry and texture coordinates.
- `x`: Screen column to draw (0 to win_w-1).
- `y`: Unused (overwritten immediately).

**Return Value:** None (void).

**Variables:**
- `step`: Texture Y sampling step per screen pixel.
- `tex_pos`: Current texture Y position.
- `tex_y`: Current texture Y coordinate (0-63).
- `color`: Sampled and shaded pixel color.

**Logic:**

1. **Calculate texture step** — `step = PX / line_height`:
   - How much to advance through texture per screen pixel.
   - If wall is tall (close): step < 1.0 (oversample, skips texture pixels).
   - If wall is short (far): step > 1.0 (undersample, samples every nth pixel).

2. **Initialize texture position** — `tex_pos = (draw_start - win_h/2 + line_height/2) * step`:
   - Adjust for where wall slice starts on screen.
   - Ensures texture maps correctly to visible portion.

3. **Load texture and find hit coordinate** — Call `load_texture()` and `find_wall_hit()`:
   - Select appropriate wall texture (NO/SO/WE/EA).
   - Calculate horizontal texture coordinate (which column of texture to sample).

4. **Loop from draw_start to draw_end** — For each screen pixel in the wall slice:

   a. **Calculate texture Y** — `tex_y = (int)tex_pos & (PX - 1)`:
      - Convert floating-point texture position to integer coordinate.
      - Mask with `(PX - 1)` to wrap around (equivalent to modulo PX).

   b. **Advance position** — `tex_pos += step` for next pixel.

   c. **Bounds check** — Skip if texture coordinates out of range:
      - This shouldn't happen if geometry is correct, but safety check.

   d. **Sample texture** — Call `get_texture_pixel()` to read pixel from texture at (tex_x, tex_y).

   e. **Apply shading** — Call `color_gradient()` to apply distance-based fog and side darkening.

   f. **Write pixel** — Call `my_pixel_put()` to write color to backbuffer:
      - If door: skip transparent pixels (color == 0).
      - If wall: always write.

   g. **Next pixel** — Increment y.

**Performance:**
- Called once per screen column (O(width × wall_height)).
- Innermost loop of rendering—critical for performance.
- Texture sampling is the most expensive operation.

**Texture Coordinate System:**
- Horizontal (tex_x): 0-63, determined by wall hit position.
- Vertical (tex_y): 0-63, determined by screen Y position.
- Both use modulo 64 to wrap around texture.
