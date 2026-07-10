# render.c — Function Breakdown

## Function: `my_pixel_put(t_game *game, int x, int y, int color)`

```c
void	my_pixel_put(t_game *game, int x, int y, int color)
{
	char	*dst;
	int		offset;

	if (x < 0 || y < 0)
		return ;
	if (x >= game->win_w || y >= game->win_h)
		return ;
	offset = (y * game->buffer.line_length + x
			* (game->buffer.bits_per_pixel / 8));
	dst = game->buffer.addr + offset;
	*(unsigned int *)dst = color;
}
```

**Purpose**: Put a single pixel to the backbuffer at coordinates (x, y).

**Parameters:**
- `game`: Game struct containing window dimensions and backbuffer pointer.
- `x`, `y`: Screen coordinates (0,0 is top-left).
- `color`: Packed RGB color `0x00RRGGBB`.

**Return Value:** None (void).

**Logic:**
1. **Bounds check** — Clamp to window dimensions. If out of bounds, return without error.
2. **Compute offset** — Calculate byte offset in backbuffer:
   - `y * line_length` — skip to the correct row (line_length = bytes per scanline).
   - `x * (bits_per_pixel / 8)` — skip to the correct pixel (4 bytes per pixel for 32-bit color).
3. **Write pixel** — Cast backbuffer address to int pointer and write the color value.

**Performance Note:**
- Called once per pixel in raycasting loop (`draw_textured_line()`).
- Critical for performance—optimize carefully.

---

## Function: `create_trgb(int t, int r, int g, int b)` (static)

```c
static int	create_trgb(int t, int r, int g, int b)
{
	return (t << 24 | r << 16 | g << 8 | b);
}
```

**Purpose**: Pack transparency, red, green, blue into a single 32-bit color integer.

**Parameters:**
- `t`: Transparency/alpha channel (0-255).
- `r`, `g`, `b`: Red, green, blue channels (0-255).

**Return Value:**
- Packed color as `0xTTRRGGBB`.

**Logic:**
- Left-shift each channel to its correct position and OR them together.
- In this project's context, transparency is ignored (always 0x00 by convention).

---

## Function: `paint_plain_background(t_game *game, int color)` (static)

```c
static void	paint_plain_background(t_game *game, int color)
{
	int	y;
	int	x;

	y = 0;
	while (y < game->win_h)
	{
		x = 0;
		while (x < game->win_w)
			my_pixel_put(game, x++, y, color);
		y++;
	}
}
```

**Purpose**: Fill the entire backbuffer with a solid color.

**Parameters:**
- `game`: Game struct containing window dimensions and backbuffer.
- `color`: Solid color to fill with.

**Return Value:** None (void).

**Logic:**
- Double loop over all pixels.
- Call `my_pixel_put()` for each pixel with the same color.

**Used By:**
- Debug mode rendering (fills background with `GREY`).

---

## Function: `paint_background(t_game *game)` (static)

```c
static void	paint_background(t_game *game)
{
	int		y;
	int		x;
	int		c;
	int		f;

	y = 0;
	c = create_trgb(0, game->map->ceiling->r, game->map->ceiling->g,
			game->map->ceiling->b);
	f = create_trgb(0, game->map->floor->r, game->map->floor->g,
			game->map->floor->b);
	while (y < game->win_h)
	{
		x = 0;
		while (x < game->win_w)
		{
			if (y < game->win_h / 2)
				my_pixel_put(game, x, y, c);
			else
				my_pixel_put(game, x, y, f);
			x++;
		}
		y++;
	}
}
```

**Purpose**: Fill backbuffer with ceiling color (top half) and floor color (bottom half).

**Parameters:**
- `game`: Game struct containing window dimensions, backbuffer, and map color data.

**Return Value:** None (void).

**Logic:**
1. **Convert colors** — Pack `game->map->ceiling` and `game->map->floor` RGB structs into 32-bit color ints.
2. **Fill top half** — Pixels with `y < win_h / 2` get ceiling color.
3. **Fill bottom half** — Pixels with `y >= win_h / 2` get floor color.

**Used By:**
- 3D raycasting mode (prepares sky and ground before raycasting walls).

---

## Function: `render(t_game *game)`

```c
int	render(t_game *game)
{
	if (game->debug)
	{
		paint_plain_background(game, GREY);
		render_elements(game);
		(update_doors(game), move_player(game, &game->player));
		draw_texture(game, game->sprites.black_square.img,
			(t_fp){game->player.player_x * PX,
			game->player.player_y * PX}, 3);
		print_fov(game, &game->player);
		mlx_put_image_to_window(game->mlx, game->win, game->buffer.img, 0, 0);
		display_speeds(game);
	}
	else
	{
		paint_background(game);
		(update_doors(game), move_player(game, &game->player));
		raycasting(game);
		if (game->minimap)
			render_minimap(game);
		mlx_put_image_to_window(game->mlx, game->win, game->buffer.img, 0, 0);
		display_speeds(game);
	}
	return (0);
}
```

**Purpose**: Main render loop callback. Called every frame by MLX. Updates game state and renders current frame.

**Parameters:**
- `game`: Game struct containing all game state and graphics buffers.

**Return Value:**
- Always `0` (required by MLX loop hook).

**Logic — Debug Mode** (`game->debug == true`):
1. Fill background with grey.
2. `render_elements()` — Draw top-down map (white squares = walls, black = empty).
3. `update_doors()` — Update door animation states.
4. `move_player()` — Process movement keys and rotate player.
5. `draw_texture()` — Draw player position as 3×3 scaled square.
6. `print_fov()` — Draw all raycasting rays from player.
7. Display backbuffer on screen.
8. `display_speeds()` — Overlay movement/rotation speed text.

**Logic — 3D Raycasting Mode** (default):
1. `paint_background()` — Fill top half with ceiling color, bottom with floor color.
2. `update_doors()` — Update door states.
3. `move_player()` — Process input.
4. `raycasting()` — Cast rays and draw textured walls.
5. `render_minimap()` (if enabled) — Draw minimap overlay.
6. Display backbuffer on screen.
7. `display_speeds()` — Overlay debug text.

**Call Chain:**
- Registered with `mlx_loop_hook(game->mlx, &render, game)` in `mlx_main()`.
- Called ~60 times per second (tied to monitor refresh rate).
- The only function directly attached to the render loop.

**Performance Note:**
- This function is called every frame—optimize carefully.
- Total render time includes raycasting (O(width)), minimap (O(minimap_area)), and display.
