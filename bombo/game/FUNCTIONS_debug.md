# debug.c — Function Breakdown

## Function: `draw_texture(t_game *game, t_img sprite, t_fp p, int size)`

```c
void	draw_texture(t_game *game, t_img sprite, t_fp p, int size)
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
			offset = (tex_y * sprite.line_length)
				+ (tex_x * sprite.bits_per_pixel / 8);
			color = *(unsigned int *)(sprite.addr + offset);
			my_pixel_put(game, tex_x + p.x, tex_y + p.y, color);
			tex_x++;
		}
		tex_y++;
	}
}
```

**Purpose**: Draw a square sprite texture at screen position (debug utility).

**Parameters:**
- `game`: Game struct with backbuffer.
- `sprite`: Image struct with pixel data.
- `p`: Screen position (top-left corner).
- `size`: Width and height of square to draw.

**Return Value:** None (void).

**Logic:**

1. Double loop over all pixels in square (0 to size×size).

2. For each pixel:
   - Calculate offset in sprite image buffer.
   - Read pixel color from texture.
   - Write to backbuffer at position (p.x + tex_x, p.y + tex_y).

**Used By:**
- Debug mode: Draw player position marker.

---

## Function: `render_elements(t_game *game)`

```c
void	render_elements(t_game *game)
{
	char	**map;
	int		y;
	int		x;

	y = 0;
	map = game->map->grid;
	while (map[y])
	{
		x = 0;
		while (map[y][x])
		{
			if (map[y][x] == '1')
				draw_texture(game, game->sprites.white_square.img,
					(t_fp){x * PX, y * PX}, PX);
			else if (map[y][x] == ' ')
				draw_texture(game, game->sprites.black_square.img,
					(t_fp){x * PX, y * PX}, PX);
			x++;
		}
		y++;
	}
}
```

**Purpose**: Render top-down 2D map view (debug visualization).

**Parameters:**
- `game`: Game struct with map and sprite textures.

**Return Value:** None (void).

**Logic:**

1. Loop through all grid cells.

2. For each cell:
   - If wall (`'1'`) → draw white square at (x*PX, y*PX).
   - If void/out-of-map (`' '`) → draw black square.
   - Otherwise (open floor `'0'`, player dir, door, etc.) → skip.

**Grid Display:**
- Each grid cell is PX×PX pixels (64×64).
- Walls appear white, voids appear black, open floor is grey (background).

---

## Function: `print_rays(t_game *game, t_player *player, double rayDir_x, double rayDir_y)` (static)

```c
static void	print_rays(t_game *game, t_player *player,
	double rayDir_x, double rayDir_y)
{
	double	j;
	double	world_x;
	double	world_y;
	double	screen_x;
	double	screen_y;

	j = 0;
	while (j < game->win_w)
	{
		world_x = player->player_x + rayDir_x * j;
		world_y = player->player_y + rayDir_y * j;
		screen_x = world_x * PX;
		screen_y = world_y * PX;
		if (world_x < 0 || world_y < 0
			|| world_y >= array_len(game->map->grid)
			|| world_x >= ft_strlen(game->map->grid[(int)world_y]))
			break ;
		if (game->map->grid[(int)(world_y)][(int)(world_x)] == '1')
			break ;
		my_pixel_put(game, screen_x, screen_y, RED);
		j += 0.05;
	}
}
```

**Purpose**: Draw a single raycasting ray on the debug map view (red line).

**Parameters:**
- `game`: Game struct with map and backbuffer.
- `player`: Player struct with position.
- `rayDir_x`, `rayDir_y`: Ray direction (typically per-column direction from player).

**Return Value:** None (void).

**Logic:**

1. Step along ray from player position in direction (rayDir_x, rayDir_y).

2. For each step:
   - Calculate world position: `(player_x + rayDir_x * j, player_y + rayDir_y * j)`.
   - Convert to screen coordinates: multiply by PX (64).
   - Check bounds: stop if out of map.
   - Check wall: stop if hit wall (`'1'`).
   - Draw red pixel.
   - Step forward by 0.05 units.

**Result:**
- Red ray from player to first wall hit.

---

## Function: `print_fov(t_game *game, t_player *player)`

```c
void	print_fov(t_game *game, t_player *player)
{
	int		x;
	double	camera_x;
	double	ray_dir_x;
	double	ray_dir_y;

	x = 0;
	while (x < game->win_w)
	{
		camera_x = 2 * x / (double)game->win_w - 1;
		ray_dir_x = player->dir_x + player->plane_x * camera_x;
		ray_dir_y = player->dir_y + player->plane_y * camera_x;
		print_rays(game, player, ray_dir_x, ray_dir_y);
		x++;
	}
}
```

**Purpose**: Draw the player's field of view (all raycasting rays) on the debug map.

**Parameters:**
- `game`: Game struct with window and map.
- `player`: Player struct with position and direction.

**Return Value:** None (void).

**Logic:**

1. For each screen column x (0 to win_w):

   a. Calculate normalized camera plane position: `camera_x = 2 * x / win_w - 1`.

   b. Calculate ray direction:
      - `ray_dir = dir + plane * camera_x`
      - Same formula as `init_ray()` in raycasting_init.c.

   c. Draw the ray via `print_rays()` (red line from player to wall).

**Result:**
- Dense array of red rays representing entire FOV.
- Shows visible walls in top-down view.

**Debug Visualization:**
- Player is a small white square (drawn by `draw_texture()`).
- FOV rays emanate from player to walls (red lines).
- Map layout is visible (white walls, black void).
