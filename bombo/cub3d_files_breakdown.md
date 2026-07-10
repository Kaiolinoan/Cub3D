# Cub3D Source Files — Line-by-Line Breakdown

This document covers every `.c` file uploaded (excluding the minimap files, which already have their own breakdown). Same format as `minimap_breakdown_uptodate.md`: for every function — purpose, code, variable meanings, and logic flow.

---

# `main.c`

## Function: `main(int argc, char **argv)`

```c
int	main(int argc, char **argv)
{
	t_game	*game;

	if (argc != 2)
		return (print_error("Invalid argument"), 1);
	game = initialize_game(argv[1]);
	if (!game)
		return (1);
	if (!check_if_map_is_valid(argv[1], game))
		return (clear_game(game), 1);
	// game->debug = true;
	mlx_main(game);
	clear_game(game);
	return (0);
}
```

**Purpose**: Program entry point. Validates arguments, builds the game state, validates the map, runs the game loop, then cleans up.

**Variables:**
- `argc, argv`: Standard C main arguments. `argv[1]` is expected to be the path to the `.cub` map file.
- `game`: Pointer to the heap-allocated `t_game` struct that holds the entire game state (map, player, sprites, window, etc.).

**Line-by-line:**
1. `if (argc != 2)` — the program must be called with exactly one argument (the map file). If not, prints an error and exits with code `1`.
2. `game = initialize_game(argv[1])` — parses the map file and allocates/populates the `t_game` struct (see `initialization.c`). Returns `NULL` on failure.
3. `if (!game) return (1);` — bail out if initialization failed (error already printed inside `initialize_game`).
4. `check_if_map_is_valid(argv[1], game)` — runs full map validation (flood-fill closure check, character validation, etc. — defined elsewhere). If invalid, frees `game` and exits with `1`.
5. `// game->debug = true;` — commented-out toggle. When uncommented, forces the debug top-down rendering mode (`render_elements`/`print_fov` path in `render.c`) instead of the raycaster.
6. `mlx_main(game)` — sets up the MLX window, images, hooks, and starts the render loop (blocks until the window closes).
7. `clear_game(game)` — frees all allocated game resources once the loop ends.
8. `return (0)` — normal exit.

> **Note**: Because `mlx_main` calls `mlx_loop`, which blocks until `finish_game()` calls `exit()`, the `clear_game(game)` on line 7 of `main` is usually dead code in practice — the real cleanup path is through `finish_game()` in `mlx.c`. It still matters as a fallback if `mlx_loop` were to return normally.

---

# `color_gradient.c`

Handles distance-based shading (darker walls further away) and side-based shading (N/S walls vs E/W walls).

## Function: `get_factor(t_ray *ray)` (static)

```c
static double	get_factor(t_ray *ray)
{
	double		max_dist;
	double		factor;

	max_dist = 12.0;
	if (ray->perp_wall_dist >= max_dist)
		return (0x000000);
	factor = (1.5 / (1.0 + ray->perp_wall_dist * 0.3))
		* (1.0 - (ray->perp_wall_dist / max_dist));
	if (factor > 1.3)
		factor = 1.3;
	if (factor < 0.0)
		factor = 0.0;
	return (factor);
}
```

**Purpose**: Compute a brightness multiplier (0.0–1.3) based on how far the wall slice is from the player.

**Variables:**
- `ray->perp_wall_dist`: Perpendicular distance from the player to the wall hit (avoids fish-eye distortion). Comes from `calculate_wall()` in `raycasting2.c`.
- `max_dist`: Hardcoded fog cutoff of 12.0 world units — anything farther gets zero brightness.
- `factor`: The multiplier ultimately returned.

**Logic:**
1. If the wall is at or beyond `max_dist`, return `0` (fully black) — **bug note**: `0x000000` is an `int` literal being returned from a `double`-returning function; it evaluates to `0.0`, so this line is functionally correct but the hex notation is misleading/unnecessary here since it's just `0`.
2. Otherwise compute `factor` as a product of:
   - An inverse falloff term `1.5 / (1 + dist * 0.3)` — brightness drops off as distance increases.
   - A linear fog term `1 - (dist / max_dist)` — additional linear fade toward `max_dist`.
3. Clamp `factor` to the range `[0.0, 1.3]` (allows slight "overbright" close up, capped at 1.3x).

## Function: `color_gradient(t_ray *ray, int color)`

```c
int	color_gradient(t_ray *ray, int color)
{
	double			factor;
	unsigned int	r;
	unsigned int	g;
	unsigned int	b;

	factor = get_factor(ray);
	r = (unsigned int)(((color >> 16) & 0xFF) * factor);
	g = (unsigned int)(((color >> 8) & 0xFF) * factor);
	b = (unsigned int)((color & 0xFF) * factor);
	if (r > 255)
		r = 255;
	if (g > 255)
		g = 255;
	if (b > 255)
		b = 255;
	color = (r << 16) | (g << 8) | b;
	if (ray->side == 1)
		color = (color >> 1) & 0x7F7F7F;
	return (color);
}
```

**Purpose**: Take a raw texel color and darken/lighten it according to distance (`get_factor`) and wall orientation (`side`).

**Variables:**
- `color` (param, then reused): Input packed `0xRRGGBB` color; overwritten at the end with the final shaded color.
- `factor`: Brightness multiplier from `get_factor`.
- `r, g, b`: Individual channels extracted via bit-shifting/masking, then scaled by `factor`.

**Line-by-line:**
1. `factor = get_factor(ray)` — get the distance-based brightness multiplier.
2. Extract and scale each channel: `(color >> 16) & 0xFF` isolates red, `(color >> 8) & 0xFF` isolates green, `color & 0xFF` isolates blue; each multiplied by `factor`.
3. Clamp each channel to `255` in case `factor > 1.0` pushed a channel over range (since `factor` can be up to `1.3`).
4. Repack the channels into a single `int`: `(r << 16) | (g << 8) | b`.
5. `if (ray->side == 1)` — if the ray hit a north/south-facing wall (as opposed to east/west), halve the color's brightness via `(color >> 1) & 0x7F7F7F` (bit-shifting all three channels right by 1, then masking off the top bit of each byte to prevent channel bleed-through). This gives N/S walls a visibly darker tone than E/W walls, a classic raycaster shading trick.

---

# `debug.c`

Debug/top-down rendering path, used only when `game->debug == true`.

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

**Purpose**: Blit an entire square sprite image (`sprite`) onto the screen buffer at pixel position `p`, at `size × size` pixels. Used in debug mode to draw map tiles and the player marker.

**Variables:**
- `sprite` (`t_img`): The source image (already loaded via MLX, with `.addr`, `.line_length`, `.bits_per_pixel` set).
- `p` (`t_fp`): Top-left screen position (float pixel struct with `.x`, `.y`) to draw at.
- `size`: Width/height of the square to copy (e.g. `PX` for a full tile, or `3` for the tiny player dot).
- `tex_y, tex_x`: Loop counters over the sprite's rows/columns.
- `offset`: Byte offset into the sprite's raw pixel buffer for `(tex_x, tex_y)`.
- `color`: The 32-bit pixel value read from the sprite at that offset.

**Logic**: Nested loop over every pixel of a `size × size` square. For each pixel, compute its byte offset in the source image's raw buffer (`row * line_length + col * bytes_per_pixel`), read the color, and write it to the destination buffer via `my_pixel_put` at `(p.x + tex_x, p.y + tex_y)`.

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

**Purpose**: In debug mode, draw the entire map top-down as a grid of tile sprites — walls (`'1'`) as white squares, void (`' '`) as black squares. Floor tiles (anything else) are left untouched (background color shows through).

**Variables:**
- `map`: Shortcut alias to `game->map->grid` (array of row strings).
- `y, x`: Row/column indices into the map grid.

**Logic**: Double loop over every row and character of the map. For each cell, look up its map coordinate in screen pixels (`x * PX, y * PX`, where `PX` is the tile pixel size) and draw the appropriate `PX × PX` sprite via `draw_texture`.

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

**Purpose**: Draw a single visual ray (a thin red line) from the player outward in direction `(rayDir_x, rayDir_y)` until it exits the map or hits a wall. Used to visualize the FOV cone in debug mode.

**Variables:**
- `rayDir_x, rayDir_y`: Direction vector for this ray (not normalized to length 1 — it's a raw camera-plane direction from `print_fov`).
- `j`: Step distance along the ray, incremented by `0.05` world units each iteration (fine granularity for a smooth-looking line). Note: `j` is used both as the "distance traveled" and capped at `game->win_w`, which is really a pixel-count bound being (mis)used as a world-distance bound — a quirk of debug-only code, not meant to be physically precise.
- `world_x, world_y`: Current position along the ray in world (tile) coordinates.
- `screen_x, screen_y`: `world_x/y` converted to pixel coordinates by multiplying by `PX`.

**Logic**: Step along the ray in small `0.05`-unit increments. At each step:
1. Compute world position, then screen position.
2. If the position goes out of the map bounds (negative, or beyond the last row/column), stop.
3. If the map cell at the current position is a wall (`'1'`), stop.
4. Otherwise plot a red pixel and continue.

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

**Purpose**: Draw the player's entire field of view as a fan of red rays (debug visualization), one per screen column.

**Variables:**
- `x`: Screen column index, `0` to `win_w - 1`.
- `camera_x`: Normalized camera-plane coordinate for this column, ranging from `-1` (leftmost) to `+1` (rightmost). Standard raycaster formula: `2 * x / win_w - 1`.
- `ray_dir_x, ray_dir_y`: The direction vector for this column, computed the same way the real raycaster does it (`dir + plane * camera_x`), so the debug rays match what the raycaster would actually cast.

**Logic**: For every screen column, compute the camera-space direction and delegate the actual line-drawing to `print_rays`.

---

# `door.c`

State machine driving door opening/closing based on player proximity.

## Function: `player_near_door(t_player *player, t_door *door, double range)` (static)

```c
static bool	player_near_door(t_player *player, t_door *door, double range)
{
	double	dx;
	double	dy;

	dx = player->player_x - (door->x + 0.5);
	dy = player->player_y - (door->y + 0.5);
	return (sqrt(dx * dx + dy * dy) <= range);
}
```

**Purpose**: Boolean proximity check — is the player within `range` world-units of this door's center?

**Variables:**
- `door->x, door->y`: Integer map-grid coordinates of the door tile.
- `door->x + 0.5, door->y + 0.5`: Center of the door tile (tiles are unit squares, so the center is offset by `0.5` in each axis).
- `dx, dy`: Vector from the door center to the player.
- `range`: Distance threshold, passed in as `DOOR_RANGE`.

**Logic**: Standard Euclidean distance check: `sqrt(dx² + dy²) <= range`.

## Function: `advance_door_animation(t_door *door)` (static)

```c
static void	advance_door_animation(t_door *door)
{
	if (door->state == DOOR_OPENING)
	{
		door->frame++;
		if (door->frame >= DOOR_ANIM_FRAMES)
		{
			door->frame = DOOR_ANIM_FRAMES;
			door->state = DOOR_OPEN;
		}
	}
	else if (door->state == DOOR_CLOSING)
	{
		door->frame--;
		if (door->frame <= 0)
		{
			door->frame = 0;
			door->state = DOOR_CLOSED;
		}
	}
}
```

**Purpose**: Advance the door's animation frame by one step (per game tick) and transition state once the animation completes.

**Variables:**
- `door->frame`: Current animation frame index, `0` (fully closed) to `DOOR_ANIM_FRAMES` (fully open — note this is used as the *count*, so valid drawn frames are `0..DOOR_ANIM_FRAMES-1`, and `DOOR_ANIM_FRAMES` itself is a sentinel meaning "fully open, no texture frame needed").
- `door->state`: One of `DOOR_CLOSED`, `DOOR_OPENING`, `DOOR_OPEN`, `DOOR_CLOSING`.

**Logic:**
- If **opening**: increment `frame`. Once it reaches or exceeds `DOOR_ANIM_FRAMES`, clamp it there and flip state to `DOOR_OPEN` (fully open, animation done).
- If **closing**: decrement `frame`. Once it reaches or goes below `0`, clamp it there and flip state to `DOOR_CLOSED`.
- If state is `DOOR_OPEN` or `DOOR_CLOSED` already, this function does nothing (no `else` branch needed — those are steady states).

## Function: `update_door_state(t_door *door, bool near)` (static)

```c
static void	update_door_state(t_door *door, bool near)
{
	if (near && (door->state == DOOR_CLOSED || door->state == DOOR_CLOSING))
	{
		door->state = DOOR_OPENING;
		play_sound(DOOR_OPEN_SOUND);
	}
	else if (!near && (door->state == DOOR_OPEN || door->state == DOOR_OPENING))
	{
		door->state = DOOR_CLOSING;
		play_sound(DOOR_CLOSE_SOUND);
	}
	advance_door_animation(door);
}
```

**Purpose**: Decide whether to start opening or closing a door based on proximity, then advance its animation one tick.

**Variables:**
- `near`: Result of `player_near_door` for this door on this frame.

**Logic:**
1. If the player is **near** and the door is currently `CLOSED` or already `CLOSING` (i.e., not already opening/open) → switch to `DOOR_OPENING` and trigger the open sound. This also handles the "reverse mid-close" case: a door that was closing and the player comes back gets redirected to opening immediately.
2. Else if the player is **not near** and the door is `OPEN` or `OPENING` → switch to `DOOR_CLOSING` and trigger the close sound. Symmetric reverse-mid-open handling.
3. Regardless of whether a transition happened, call `advance_door_animation` to step the frame counter for this tick.

> **Note**: Because sounds are only triggered on the *transition* into `OPENING`/`CLOSING` (not every frame), a door won't spam `paplay` every tick — it plays once per direction change.

## Function: `update_doors(t_game *game)`

```c
void	update_doors(t_game *game)
{
	int		i;
	bool	near;

	i = 0;
	while (i < game->doors)
	{
		near = player_near_door(&game->player, &game->door[i], DOOR_RANGE);
		update_door_state(&game->door[i], near);
		i++;
	}
}
```

**Purpose**: Per-frame update — iterate every door in the level and update its state/animation.

**Variables:**
- `game->doors`: Count of doors in the level (array length for `game->door[]`).
- `i`: Loop index over the door array.
- `near`: Per-door proximity flag computed fresh each frame.
- `DOOR_RANGE`: Constant proximity threshold (world units) used for every door.

**Logic**: Simple loop — for each door, compute proximity and delegate to `update_door_state`. Called once per frame from `render()`.

## Function: `is_door_blocking(t_game *game, int x, int y)`

```c
bool	is_door_blocking(t_game *game, int x, int y)
{
	t_door	*door;

	door = get_door_at(game, x, y);
	if (!door)
		return (false);
	return (door->state != DOOR_OPEN);
}
```

**Purpose**: Collision check helper — is the door tile at map coordinate `(x, y)` currently blocking movement?

**Variables:**
- `x, y`: Map grid coordinates being checked.
- `door`: Pointer to the matching `t_door` struct, found via `get_door_at` (defined elsewhere — likely scans `game->door[]` for matching `.x`/`.y`).

**Logic**: If there's no door at that tile, it can't be blocking (`false`) — this guards against being called on a non-door tile. Otherwise, the door blocks movement unless it is fully `DOOR_OPEN` — so `DOOR_CLOSED`, `DOOR_OPENING`, and `DOOR_CLOSING` all still block the player from walking through, only a fully-open door lets you pass.

> **Used by**: `movement.c`'s `update_player_x`/`update_player_y` for collision, and `raycasting_door.c` indirectly informs rendering (not blocking).

---

# `door_frames.c`

Loads the 14 XPM animation frames for door opening/closing.

## Function: `build_frame_path(int i)` (static)

```c
static char	*build_frame_path(int i)
{
	char	*index_str;
	char	*path;
	char	*str2;

	index_str = ft_itoa(i);
	if (!index_str)
		return (NULL);
	str2 = ft_strjoin_and_free(index_str, ".xpm");
	if (!str2)
		return (free(index_str), NULL);
	path = ft_strjoin_and_free(ft_strdup("assets/animation/door_"), str2);
	if (!path)
		return (free(str2), NULL);
	return (free(str2), path);
}
```

**Purpose**: Build the file path string `"assets/animation/door_<i>.xpm"` for animation frame index `i`.

**Variables:**
- `i`: Frame index (`0` to `DOOR_ANIM_FRAMES - 1`).
- `index_str`: `i` converted to a string via `ft_itoa` (e.g. `"7"`).
- `str2`: `index_str` joined with `".xpm"` → e.g. `"7.xpm"`. Per the project's `ft_strjoin_and_free` convention, this call frees `index_str` internally and returns a new string.
- `path`: `"assets/animation/door_"` (freshly duplicated via `ft_strdup` since `ft_strjoin_and_free` frees its first argument, and a string literal can't be freed) joined with `str2` → final result e.g. `"assets/animation/door_7.xpm"`.

**Logic / error handling**: Each allocation step is checked. If any step fails, previously-allocated intermediate strings are freed before returning `NULL`, avoiding leaks on the failure path. On success, `str2` is freed (its content has been copied into `path` by the join) and `path` is returned.

## Function: `load_door_frame(t_game *game, t_texture *frame, int i)` (static)

```c
static bool	load_door_frame(t_game *game, t_texture *frame, int i)
{
	char	*path;

	path = build_frame_path(i);
	if (!path)
		return (print_error(ALLOC_ERR), false);
	if (!set_image_path(frame, path))
		return (free(path), false);
	free(path);
	if (!file_to_image(game->mlx, frame))
		return (false);
	if (!get_dir_img_address(frame))
		return (mlx_destroy_image(game->mlx, frame->img.img),
			frame->img.img = NULL, false);
	return (true);
}
```

**Purpose**: Load a single door animation frame — build its path, store the path on the texture struct, load the XPM into an MLX image, and fetch the raw pixel address for direct pixel reads.

**Variables:**
- `frame` (`t_texture *`): Output struct — gets its `.path` (via `set_image_path`) and its `.img` (an MLX image + address/line_length/bpp/endian, via `file_to_image` and `get_dir_img_address`) populated.
- `path`: The built file path string for this frame, freed once no longer needed.

**Logic / error handling:**
1. Build the path; bail with an allocation error message if it fails.
2. Store the path onto `frame` via `set_image_path`; free the local `path` copy either way (whether `set_image_path` succeeded or failed) — freed before the failure return and also freed on the success path via the standalone `free(path);` line.
3. Load the actual XPM pixel data into an MLX image (`file_to_image`).
4. Fetch the raw pixel buffer address so pixels can be read directly for texture mapping (`get_dir_img_address`). If this fails, the partially-created MLX image is destroyed and its pointer nulled out before returning `false`, to avoid a dangling/leaked image handle.

## Function: `load_door_frames(t_game *game, t_sprites *sprites)`

```c
bool	load_door_frames(t_game *game, t_sprites *sprites)
{
	int	i;

	sprites->door_frames = ft_calloc(DOOR_ANIM_FRAMES, sizeof(t_texture));
	if (!sprites->door_frames)
		return (print_error(ALLOC_ERR), false);
	i = 0;
	while (i < DOOR_ANIM_FRAMES)
	{
		if (!load_door_frame(game, &sprites->door_frames[i], i))
		{
			clear_door_frames(game->mlx, sprites->door_frames, i);
			sprites->door_frames = NULL;
			return (false);
		}
		i++;
	}
	return (true);
}
```

**Purpose**: Allocate the array of `DOOR_ANIM_FRAMES` texture slots and load every animation frame into it. Called once at startup from `initialize_images`.

**Variables:**
- `sprites->door_frames`: Array of `t_texture`, one per animation frame (allocated with `ft_calloc` so all fields start zeroed — important so unfilled/failed slots are safely `NULL`/`0`).
- `i`: Loop index / frame counter.

**Logic:**
1. Allocate `DOOR_ANIM_FRAMES` zeroed `t_texture` slots.
2. Loop over each index, calling `load_door_frame` to populate that slot.
3. If any frame fails to load, call `clear_door_frames(game->mlx, sprites->door_frames, i)` to free only the `i` frames that were successfully loaded so far (avoids freeing uninitialized/garbage slots), null out the array pointer, and return `false`.
4. If all frames load successfully, return `true`.

---

# `door_sound.c`

Non-blocking door sound playback via `paplay` (WSL/PulseAudio), using `fork`/`exec` so the game doesn't stall waiting for audio to finish.

## Function: `init_door_audio(void)`

```c
void	init_door_audio(void)
{
	signal(SIGCHLD, SIG_IGN);
}
```

**Purpose**: Tell the OS to automatically reap child processes (the forked `paplay` calls) without the parent needing to `wait()` on them.

**Logic**: `signal(SIGCHLD, SIG_IGN)` sets the "child terminated" signal handler to be ignored, which on POSIX systems has the side effect of preventing zombie processes from accumulating — each forked `paplay` process is cleaned up by the kernel automatically once it exits. Called once at startup from `mlx_main`.

## Function: `silence_std_fds(void)` (static)

```c
static void	silence_std_fds(void)
{
	int	fd;

	fd = open("/dev/null", O_WRONLY);
	if (fd < 0)
		return ;
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);
	close(fd);
}
```

**Purpose**: Redirect the child process's stdout and stderr to `/dev/null` so `paplay`'s own console output/errors don't clutter or interfere with the game's terminal.

**Variables:**
- `fd`: File descriptor for `/dev/null`, opened write-only.

**Logic:**
1. Open `/dev/null` for writing. If it fails, silently give up (return) — not a fatal error, just means sound output won't be silenced.
2. `dup2(fd, STDOUT_FILENO)` and `dup2(fd, STDERR_FILENO)` — duplicate the `/dev/null` descriptor onto file descriptors `1` (stdout) and `2` (stderr), so any writes to those now go to `/dev/null`.
3. Close the original `fd` (no longer needed once duplicated onto 1 and 2).

**Called from**: `play_sound`, inside the forked child, right before `execlp`.

## Function: `play_sound(char *path)`

```c
void	play_sound(char *path)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		return ;
	if (pid == 0)
	{
		silence_std_fds();
		execlp("paplay", "paplay", path, NULL);
		_exit(1);
	}
}
```

**Purpose**: Play a `.wav`/audio file asynchronously (non-blocking) by forking a child process that execs `paplay`.

**Variables:**
- `path`: Path to the audio file to play (e.g. `DOOR_OPEN_SOUND` / `DOOR_CLOSE_SOUND` constants).
- `pid`: Result of `fork()` — `0` in the child, the child's PID in the parent, negative on failure.

**Logic:**
1. `fork()` — duplicate the current process.
2. `if (pid < 0)` — fork failed (e.g., resource limits); silently give up, game continues without sound.
3. `if (pid == 0)` — this branch only runs in the **child** process:
   - `silence_std_fds()` — redirect stdout/stderr to `/dev/null`.
   - `execlp("paplay", "paplay", path, NULL)` — replace the child process image with `paplay <path>`. If successful, this line never returns (the child *becomes* `paplay`).
   - `_exit(1)` — only reached if `execlp` itself failed (e.g., `paplay` not found on `PATH`); exits the child immediately with status `1`, using `_exit` (not `exit`) to avoid flushing/duplicating any inherited stdio buffers from the parent.
4. The **parent** process (where `pid > 0`) falls through and returns immediately — it does *not* wait for the child, which is what makes this non-blocking. Cleanup of the finished child is handled automatically thanks to `SIGCHLD` being ignored (`init_door_audio`).

---

# `initialization.c`

Startup sequence: image loading and the top-level `t_game` allocation/setup.

## Function: `init_path(t_sprites *sprites)`

```c
bool	init_path(t_sprites *sprites)
{
	if (!set_image_path(&sprites->black_square, "assets/black_square-64.xpm"))
		return (false);
	if (!set_image_path(&sprites->white_square, "assets/square-64.xpm"))
		return (false);
	return (true);
}
```

**Purpose**: Set the file paths for the two debug-mode tile sprites (used by `render_elements` in `debug.c`).

**Variables:**
- `sprites->black_square, sprites->white_square`: `t_texture` structs whose `.path` field is set by `set_image_path`.

**Logic**: Sequentially set each path, bailing out with `false` if either assignment fails (e.g., allocation failure inside `set_image_path`).

> Note the wall/floor direction textures (`east`, `west`, `north`, `south`) don't have their paths set here — they're presumably set earlier during map parsing (from the `.cub` file's `NO`/`SO`/`WE`/`EA` texture lines), before `initialize_images` runs.

## Function: `init_images(t_game *game, t_sprites *sprites)`

```c
bool	init_images(t_game *game, t_sprites *sprites)
{
	if (!file_to_image(game->mlx, &sprites->east))
		return (false);
	if (!file_to_image(game->mlx, &sprites->west))
		return (false);
	if (!file_to_image(game->mlx, &sprites->north))
		return (false);
	if (!file_to_image(game->mlx, &sprites->south))
		return (false);
	if (!file_to_image(game->mlx, &sprites->white_square))
		return (false);
	if (!file_to_image(game->mlx, &sprites->black_square))
		return (false);
	return (true);
}
```

**Purpose**: Load the six core sprite XPMs (four wall-direction textures + two debug tile sprites) from disk into MLX images.

**Logic**: Sequential calls to `file_to_image`, one per sprite, each short-circuiting to `false` on the first failure. Straightforward "load these N things, fail fast" pattern — kept under the Norminette function-length/line limits by not folding these into a loop over a pointer array (which the codebase could do, but doesn't here).

## Function: `init_address(t_sprites *sprites)`

```c
bool	init_address(t_sprites *sprites)
{
	if (!get_dir_img_address(&sprites->east))
		return (false);
	if (!get_dir_img_address(&sprites->west))
		return (false);
	if (!get_dir_img_address(&sprites->north))
		return (false);
	if (!get_dir_img_address(&sprites->south))
		return (false);
	if (!get_dir_img_address(&sprites->white_square))
		return (false);
	if (!get_dir_img_address(&sprites->black_square))
		return (false);
	return (true);
}
```

**Purpose**: For each of the same six sprites, fetch the raw pixel buffer address (`.addr`, `.line_length`, `.bits_per_pixel`, `.endian`) needed for direct pixel access during texture mapping.

**Logic**: Same sequential fail-fast pattern as `init_images`, just calling `get_dir_img_address` instead of `file_to_image`. Must run *after* `init_images`, since you need a loaded MLX image before you can query its data address.

## Function: `initialize_images(t_game *game, t_sprites *sprites)`

```c
bool	initialize_images(t_game *game, t_sprites *sprites)
{
	game->buffer.img = mlx_new_image(game->mlx, game->win_w, game->win_h);
	if (!game->buffer.img)
		return (print_error(MLX_IMG), false);
	game->buffer.addr = mlx_get_data_addr(game->buffer.img,
			&game->buffer.bits_per_pixel, &game->buffer.line_length,
			&game->buffer.endian);
	if (!game->buffer.addr)
		return (print_error(MLX_ADDR), false);
	if (!init_path(sprites))
		return (false);
	if (!init_images(game, sprites))
		return (false);
	if (!init_address(sprites))
		return (false);
	if (!load_door_frames(game, sprites))
		return (false);
	return (true);
}
```

**Purpose**: Top-level image-setup orchestrator, called once from `mlx_main` after the window is created. Sets up the off-screen draw buffer and loads every sprite the game needs.

**Variables:**
- `game->buffer` (`t_img`): The single off-screen image every frame is drawn into via `my_pixel_put`, then blitted to the window in one call (`mlx_put_image_to_window`) — this is the standard "double buffering" technique to avoid flicker/tearing.
- `game->buffer.img`: MLX image handle for the buffer, sized `win_w × win_h`.
- `game->buffer.addr/bits_per_pixel/line_length/endian`: Raw pixel-buffer metadata needed to write pixels directly.

**Logic (in order):**
1. Create the off-screen buffer image at window size. Fail with an `MLX_IMG` error if creation fails.
2. Fetch its raw data address. Fail with `MLX_ADDR` if that fails.
3. Set debug sprite paths (`init_path`).
4. Load all sprite images from disk (`init_images`).
5. Fetch pixel addresses for all sprites (`init_address`).
6. Load the 14 door animation frames (`load_door_frames`).
7. Return `true` only if every step succeeded.

## Function: `initialize_game(char *filename)`

```c
t_game	*initialize_game(char *filename)
{
	t_game	*game;

	game = ft_calloc(1, sizeof(t_game));
	if (!game)
		return (print_error(ALLOC_ERR), NULL);
	game->map = ft_calloc(1, sizeof(t_map));
	if (!game->map)
		return (clear_game(game), print_error(ALLOC_ERR), NULL);
	if (!get_map_details(game, filename))
		return (clear_game(game), NULL);
	game->minimap = true;
	game->minimap_tile_size = 20;
	game->minimap_radius_tiles = 7.0;
	game->movement_speed = 0.07;
	game->rotation_speed = 0.05;
	game->fov = 0.66;
	return (game);
}
```

**Purpose**: The very first setup step (called from `main`). Allocates the top-level `t_game` and `t_map` structs, parses the map file, and sets initial runtime defaults (before any MLX/window setup happens — that's `mlx_main`'s job later).

**Variables:**
- `filename`: Path to the `.cub` map file (from `argv[1]`).
- `game`: The newly allocated, zero-initialized game state (`ft_calloc` ensures every field starts at `0`/`NULL`/`false`).
- `game->map`: Nested allocation for map-specific data (grid, dimensions, floor/ceiling colors, start position, etc.).

**Line-by-line:**
1. Allocate and zero `t_game`. Bail with an allocation error if it fails.
2. Allocate and zero `t_map`, attach it to `game->map`. On failure, `clear_game(game)` is still called even though `game->map` is `NULL` — implies `clear_game` must be `NULL`-safe for `game->map`.
3. `get_map_details(game, filename)` — parses the `.cub` file: textures, colors, and the grid itself. On failure, cleans up and returns `NULL` (error presumably already printed inside `get_map_details`).
4. Set gameplay defaults directly on `game`:
   - `minimap = true` — minimap visible by default.
   - `minimap_tile_size = 20` — 20 px per tile initially.
   - `minimap_radius_tiles = 7.0` — visible radius of 7 tiles.
   - `movement_speed = 0.07` — world units moved per frame when a movement key is held.
   - `rotation_speed = 0.05` — radians rotated per frame when a rotation key is held.
   - `fov = 0.66` — camera plane magnitude, which determines field-of-view width (used to set `plane_x/y` in `player.c`; `0.66` corresponds to roughly a 66° FOV, the classic Wolfenstein-style raycaster default).
5. Return the fully constructed `game` pointer.

> **Note**: `game->win_w`/`win_h` are *not* set here — they're set later in `mlx_main` (via `mlx_get_screen_size` or the debug-mode calculation), since MLX itself isn't initialized yet at this point.

---

# `keys_handle.c`

Translates raw X11 keycodes into game-state flags. Split into three concerns: movement/mouse toggle, minimap controls, and speed tuning.

## Function: `handle_keys(int keycode, t_player *player)`

```c
void	handle_keys(int keycode, t_player *player)
{
	if (keycode == XK_w)
		player->up = true;
	else if (keycode == XK_s)
		player->down = true;
	else if (keycode == XK_a)
		player->left = true;
	else if (keycode == XK_d)
		player->right = true;
	else if (keycode == XK_Left)
		player->rotate_l = true;
	else if (keycode == XK_Right)
		player->rotate_r = true;
	if (keycode == XK_space)
	{
		if (player->mouse_flag == false)
			player->mouse_flag = true;
		else
			player->mouse_flag = false;
	}
}
```

**Purpose**: On `KeyPress`, set the corresponding boolean movement/rotation flag to `true`; these flags are read every frame by `move_player` (`movement.c`) to decide which direction(s) to move. Also toggles mouse-look on `Space`.

**Variables:**
- `keycode`: The X11 keysym for the pressed key.
- `player->up/down/left/right`: WASD strafe/forward/back flags.
- `player->rotate_l/rotate_r`: Arrow-key rotation flags.
- `player->mouse_flag`: Whether mouse-look is currently active; flipped (toggled) each time `Space` is pressed.

**Logic**: A chain of `if/else if` for WASD and arrow keys (mutually exclusive per call, since only one keycode arrives per event) sets the matching flag `true`. A separate, non-`else`-chained `if (keycode == XK_space)` toggles `mouse_flag` — kept separate because it's not part of the WASD/arrow mutual-exclusion chain and needs its own toggle logic.

## Function: `handle_minimap_size(int keycode, t_game *game)` (static)

```c
static void	handle_minimap_size(int keycode, t_game *game)
{
	if (keycode == XK_plus || keycode == XK_equal)
		game->minimap_radius_tiles += 0.5;
	else if (keycode == XK_minus)
	{
		if (game->minimap_radius_tiles > 1.0)
			game->minimap_radius_tiles -= 0.5;
	}
}
```

**Purpose**: Adjust how many world-tiles the minimap shows (`minimap_radius_tiles`) — `+`/`=` zooms out (shows more of the map), `-` zooms in, with a floor of `1.0` tile radius so it can never shrink to nothing or invert.

**Variables:**
- `game->minimap_radius_tiles`: Visible radius in world-tile units (see minimap breakdown for how this is used downstream).

**Logic**: `+` or `=` (both commonly on the same physical key, shift vs. no-shift) increases radius by `0.5`. `-` decreases it by `0.5`, but only if the result would stay above `1.0`.

## Function: `handle_minimap_zoom(int keycode, t_game *game)` (static)

```c
static void	handle_minimap_zoom(int keycode, t_game *game)
{
	double	radius_px;

	if (keycode == XK_bracketright)
	{
		if (game->minimap_tile_size < 40)
		{
			radius_px = game->minimap_radius_tiles * game->minimap_tile_size;
			game->minimap_tile_size += 2;
			game->minimap_radius_tiles = radius_px / game->minimap_tile_size;
		}
	}
	else if (keycode == XK_bracketleft)
	{
		if (game->minimap_tile_size > 4)
		{
			radius_px = game->minimap_radius_tiles * game->minimap_tile_size;
			game->minimap_tile_size -= 2;
			game->minimap_radius_tiles = radius_px / game->minimap_tile_size;
		}
	}
}
```

**Purpose**: Adjust `minimap_tile_size` (pixels per tile — controls minimap "zoom level" / how large each tile appears) using `]`/`[`, while keeping the **on-screen pixel size** of the minimap constant by simultaneously recomputing `minimap_radius_tiles`.

**Variables:**
- `radius_px`: The minimap's current radius in *screen pixels*, computed as `radius_tiles * tile_size`. This is captured *before* changing `tile_size`, so it can be preserved.

**Logic:**
- `]` (`XK_bracketright`): if `tile_size` is under the cap of `40`, save the current pixel radius, increase `tile_size` by `2` (each tile now renders bigger — more zoomed in), then recompute `radius_tiles = radius_px / new_tile_size` so the minimap's on-screen circle stays the same physical size while showing fewer, larger tiles.
- `[` (`XK_bracketleft`): symmetric — if `tile_size` is above a floor of `4`, shrink it by `2` (more tiles fit in the same circle), and recompute `radius_tiles` upward to compensate.

> This is the "zoom" control (changes how large each tile looks) as distinct from `handle_minimap_size`'s "extent" control (changes how many tiles are visible while keeping their size the same) — together they let the player independently control zoom level and how much of the map is visible on the minimap circle.

## Function: `handle_minimap_keys(int keycode, t_game *game)`

```c
void	handle_minimap_keys(int keycode, t_game *game)
{
	if (keycode == XK_m)
		display_minimap(game);
	handle_minimap_size(keycode, game);
	handle_minimap_zoom(keycode, game);
}
```

**Purpose**: Dispatcher for all minimap-related key presses — toggling visibility (`M`) and delegating to the size/zoom handlers.

**Logic**: `M` calls `display_minimap` (toggles `game->minimap`, see minimap breakdown). Then unconditionally calls both `handle_minimap_size` and `handle_minimap_zoom`, each of which internally checks if `keycode` matches any key they care about — so at most one of the four minimap-adjustment keys actually does anything per call, but all four are checked every time this function runs.

## Function: `handle_speed_keys(int keycode, t_game *game)`

```c
void	handle_speed_keys(int keycode, t_game *game)
{
	if (keycode == XK_1)
	{
		if (game->movement_speed > 0.01)
			game->movement_speed -= 0.01;
	}
	else if (keycode == XK_2)
	{
		if (game->movement_speed < 0.7)
			game->movement_speed += 0.01;
	}
	else if (keycode == XK_Down)
	{
		if (game->rotation_speed > 0.005)
			game->rotation_speed -= 0.005;
	}
	else if (keycode == XK_Up)
		game->rotation_speed += 0.005;
}
```

**Purpose**: Live-tune movement and rotation speed at runtime — `1`/`2` for movement speed down/up, `Up`/`Down` for rotation speed up/down.

**Variables:**
- `game->movement_speed`: World units moved per frame while a direction key is held (used in `movement.c`).
- `game->rotation_speed`: Radians rotated per frame while an arrow key is held / scales mouse-look sensitivity (used in `movement.c` and `mlx.c`'s `mouse_move`).

**Logic**: Each key adjusts its respective speed by a small fixed step, with bounds checks:
- `1`: decrease `movement_speed` by `0.01`, floored so it never drops to/below `0.01`.
- `2`: increase `movement_speed` by `0.01`, capped so it never reaches/exceeds `0.7`.
- `Down` (arrow): decrease `rotation_speed` by `0.005`, floored above `0.005`.
- `Up` (arrow): increase `rotation_speed` by `0.005` — **note**: this branch has no upper cap, unlike the other three, so rotation speed can be increased indefinitely.

---

# `mlx.c`

Window/MLX lifecycle: creation, hooks, main loop, and mouse-look.

## Function: `finish_game(void *param)`

```c
int	finish_game(void *param)
{
	t_game	*game;

	game = param;
	clear_game(game);
	exit(EXIT_SUCCESS);
	return (0);
}
```

**Purpose**: Hooked to the window's `DestroyNotify` event (and called directly on `Escape`) — the game's actual shutdown path.

**Variables:**
- `param`: Generic `void *` (MLX hook callback signature), cast to `t_game *`.

**Logic**: Free all game resources via `clear_game`, then immediately `exit(EXIT_SUCCESS)` — this terminates the process outright rather than returning control to `mlx_loop`, since MLX's X11 event loop doesn't have a clean "stop looping" API in this setup. The `return (0)` after `exit()` is unreachable but required to satisfy the hook's `int`-returning signature/Norminette.

## Function: `mouse_move(int x, int y, void *param)`

```c
int	mouse_move(int x, int y, void *param)
{
	t_game	*game;
	int		delta_x;
	int		center_x;
	int		center_y;

	game = param;
	center_x = game->win_w / 2;
	center_y = game->win_h / 2;
	if ((x == center_x && y == center_y) || (game->player.mouse_flag == false))
		return (0);
	delta_x = x - center_x;
	rotate(&game->player, delta_x * game->rotation_speed * 0.01);
	mlx_mouse_move(game->mlx, game->win, center_x, center_y);
	mlx_mouse_hide(game->mlx, game->win);
	return (0);
}
```

**Purpose**: Mouse-look implementation. Hooked to `PointerMotionMask`. Rotates the player based on horizontal mouse movement, then re-centers the cursor so it can keep detecting relative movement indefinitely (an "infinite mouse" trick, since raw X11 doesn't have native pointer-lock).

**Variables:**
- `x, y`: Current mouse position in window pixel coordinates (from the motion event).
- `center_x, center_y`: The window's center point — the cursor is always warped back here.
- `delta_x`: How far the mouse moved horizontally since last being re-centered.

**Logic:**
1. Compute the window center.
2. Guard clause: if the event position *is* the center (meaning this event was caused by our own `mlx_mouse_move` re-centering call, not a real user movement — preventing infinite feedback), or if mouse-look is toggled off (`mouse_flag == false`), do nothing and return.
3. Otherwise compute `delta_x` — positive means the mouse moved right.
4. Rotate the player by `delta_x * rotation_speed * 0.01` radians — note this reuses `rotation_speed` (the same value tuned by arrow keys) scaled down by an extra `0.01` factor to make mouse sensitivity reasonable relative to per-pixel mouse deltas (which can be tens of pixels per event, vs. arrow-key rotation which is a fixed per-frame step).
5. `mlx_mouse_move` — warp the cursor back to the window center.
6. `mlx_mouse_hide` — keep the cursor hidden during mouse-look (called every motion event, which is redundant but harmless — likely defensive against some window managers un-hiding it).

## Function: `mlx_main(t_game *game)`

```c
void	mlx_main(t_game *game)
{
	game->mlx = mlx_init();
	if (!game->mlx)
		return (print_error(MLX), clear_game(game));
	if (game->debug)
	{
		game->win_w = ft_strlen(*game->map->grid) * 64;
		game->win_h = array_len(game->map->grid) * 64;
	}
	else
		mlx_get_screen_size(game->mlx, &game->win_w, &game->win_h);
	game->win = mlx_new_window(game->mlx, game->win_w, game->win_h, "Cub3D");
	if (!game->win)
		return (print_error(MLX_WIN), clear_game(game));
	if (!initialize_images(game, &game->sprites))
		return ;
	initialize_player(game, &game->player);
	init_door_audio();
	mlx_hook(game->win, KeyPress, KeyPressMask, player_moving, game);
	mlx_hook(game->win, KeyRelease, KeyReleaseMask, player_idle, game);
	mlx_hook(game->win, 6, PointerMotionMask, mouse_move, game);
	mlx_hook(game->win, DestroyNotify, StructureNotifyMask, finish_game, game);
	mlx_loop_hook(game->mlx, &render, game);
	mlx_loop(game->mlx);
}
```

**Purpose**: The full MLX bootstrap — init the library, size and create the window, load images, set up the player and input hooks, and enter the render loop. Called once from `main`.

**Variables:**
- `game->mlx`: The MLX library instance handle.
- `game->win_w, game->win_h`: Window dimensions in pixels.
- `game->win`: The created X11 window handle.

**Line-by-line:**
1. `mlx_init()` — initialize the MLX library/X11 connection. If it fails, print an `MLX` error and clean up (note: no `exit`/`return` value needed since this function is `void` and the caller (`main`) just continues to `clear_game` afterward regardless).
2. Window sizing:
   - **Debug mode**: size the window exactly to the map's dimensions in a fixed `64px`-per-tile grid (`ft_strlen` of the first row for width, `array_len` of the grid for height) — this only works correctly if the map is rectangular (every row the same length), otherwise the window width is based on row `0` only.
   - **Normal mode**: use the full screen size via `mlx_get_screen_size`.
3. Create the window at the computed size with title `"Cub3D"`. Error out with `MLX_WIN` on failure.
4. `initialize_images` — load the buffer and all sprites (see `initialization.c`). If this fails, return immediately (error already printed inside).
5. `initialize_player` — set the player's starting position/direction/camera-plane based on the map's spawn tile and facing (see `player.c`).
6. `init_door_audio` — set up `SIGCHLD` handling for non-blocking sound playback.
7. Register event hooks:
   - `KeyPress` → `player_moving` (sets movement/rotation/toggle flags, and handles `Escape`).
   - `KeyRelease` → `player_idle` (clears movement/rotation flags).
   - Event `6` (raw X11 code for `MotionNotify`) with `PointerMotionMask` → `mouse_move`.
   - `DestroyNotify` (window closed via window manager, e.g. clicking the X button) → `finish_game`.
8. `mlx_loop_hook` — register `render` to be called every iteration of the main loop (i.e., once per frame).
9. `mlx_loop` — enter the blocking X11/MLX event loop. This call does not return until the process exits (via `finish_game`'s `exit()` call).

---

# `movement.c`

Per-frame player rotation and translation, including per-axis collision detection (with sliding).

## Function: `rotate(t_player *player, double rot)`

```c
void	rotate(t_player *player, double rot)
{
	double	old_dir_x;
	double	old_dir_y;
	double	old_plane_x;
	double	old_plane_y;

	old_dir_x = player->dir_x;
	old_dir_y = player->dir_y;
	old_plane_x = player->plane_x;
	old_plane_y = player->plane_y;
	player->dir_x = old_dir_x * cos(rot) - old_dir_y * sin(rot);
	player->dir_y = old_dir_x * sin(rot) + old_dir_y * cos(rot);
	player->plane_x = old_plane_x * cos(rot) - old_plane_y * sin(rot);
	player->plane_y = old_plane_x * sin(rot) + old_plane_y * cos(rot);
}
```

**Purpose**: Rotate both the player's view direction vector and camera-plane vector by angle `rot` radians, using a standard 2D rotation matrix. This is what turns the player's view left/right.

**Variables:**
- `rot`: Angle in radians to rotate by (positive = counter-clockwise in standard math convention, though the actual on-screen turn direction depends on the coordinate system convention used elsewhere).
- `old_dir_x/y, old_plane_x/y`: Snapshots of the pre-rotation vectors, needed because the 2D rotation formula for each new component depends on *both* old `x` and old `y` — without snapshotting, updating `dir_x` first would corrupt the value used to then compute `dir_y`.

**Math**: Standard 2D rotation matrix applied to both `(dir_x, dir_y)` and `(plane_x, plane_y)`:
```
x' = x*cos(rot) - y*sin(rot)
y' = x*sin(rot) + y*cos(rot)
```
Rotating both vectors together keeps them perpendicular (the camera plane must always stay perpendicular to the view direction for the raycaster's projection math to remain correct).

## Function: `update_player_x(t_game *game, double dir, double speed, bool sum)` (static)

```c
static void	update_player_x(t_game *game, double dir, double speed, bool sum)
{
	double	new_x;
	double	padding;
	double	collision_x;

	padding = 0.2;
	if (sum)
	{
		new_x = game->player.player_x + dir * speed;
		collision_x = new_x + dir * padding;
	}
	else
	{
		new_x = game->player.player_x - dir * speed;
		collision_x = new_x - dir * padding;
	}
	if (game->map->grid[(int)game->player.player_y][(int)collision_x] == '1')
		return ;
	if (game->map->grid[(int)game->player.player_y][(int)collision_x] == 'D'
	&& is_door_blocking(game, (int)collision_x, (int)game->player.player_y))
		return ;
	game->player.player_x = new_x;
}
```

**Purpose**: Attempt to move the player along the X axis by `dir * speed`, with collision detection against walls and closed doors, including a small "padding" buffer so the player doesn't visually clip into wall faces.

**Variables:**
- `dir`: A direction component (either `player->dir_x` for forward/back, or `player->plane_x` for strafing left/right) — the caller decides which axis-component vector to pass in.
- `speed`: Movement speed for this frame (`game->movement_speed`).
- `sum`: If `true`, move in the `+dir` direction (add); if `false`, move in `-dir` (subtract) — this lets the same function serve both "forward" and "backward"/strafe-left and strafe-right by flipping the sign.
- `padding`: `0.2` world units — extra distance checked *beyond* the actual new position, so collision is detected slightly before the player's collision point would touch the wall, preventing the camera from poking through wall textures.
- `new_x`: The candidate new X position, without padding.
- `collision_x`: The X position actually checked against the map grid — `new_x` pushed further out by `padding` in the direction of travel.

**Logic:**
1. Compute `new_x` and `collision_x` depending on `sum` (add or subtract `dir*speed`/`dir*padding`).
2. Look up the map cell at `(collision_x, player_y)` — note Y stays fixed since this is an X-only move check.
3. If that cell is a wall (`'1'`), reject the move (return without updating `player_x`).
4. If that cell is a door (`'D'`) **and** `is_door_blocking` says it's not fully open, reject the move.
5. Otherwise, commit: `game->player.player_x = new_x`.

> This is an axis-separated collision system (X and Y checked/resolved independently) which naturally produces "wall sliding" — e.g. moving diagonally into a wall corner still allows sliding along the wall on the other axis, since `update_player_y` is a separate call that isn't blocked just because `update_player_x` was.

## Function: `update_player_y(t_game *game, double dir, double speed, bool sum)` (static)

```c
static void	update_player_y(t_game *game, double dir, double speed, bool sum)
{
	double	new_y;
	double	padding;
	double	collision_y;

	padding = 0.35;
	if (sum)
	{
		new_y = game->player.player_y + dir * speed;
		collision_y = new_y + dir * padding;
	}
	else
	{
		new_y = game->player.player_y - dir * speed;
		collision_y = new_y - dir * padding;
	}
	if (game->map->grid[(int)collision_y][(int)game->player.player_x] == '1')
		return ;
	if (game->map->grid[(int)collision_y][(int)game->player.player_x] == 'D'
	&& is_door_blocking(game, (int)game->player.player_x, (int)collision_y))
		return ;
	game->player.player_y = new_y;
}
```

**Purpose**: Same as `update_player_x`, mirrored for the Y axis (X held fixed this time).

**Variables**: Identical structure to `update_player_x`, but note **`padding = 0.35`** here vs. `0.2` for X — a larger vertical padding, likely tuned empirically/asymmetrically for how the map/textures look, or to compensate for the screen's aspect ratio making vertical clipping more visually noticeable.

**Logic**: Same pattern as `update_player_x`: compute candidate/collision positions, check the map cell at `(player_x, collision_y)` for a wall or blocking door, commit or reject.

## Function: `check_direction_to_move(t_game *game, t_player *player)` (static)

```c
static void	check_direction_to_move(t_game *game, t_player *player)
{
	double	speed;

	speed = game->movement_speed;
	if (player->up)
	{
		update_player_x(game, player->dir_x, speed, true);
		update_player_y(game, player->dir_y, speed, true);
	}
	if (player->down)
	{
		update_player_x(game, player->dir_x, speed, false);
		update_player_y(game, player->dir_y, speed, false);
	}
	if (player->left)
	{
		update_player_x(game, player->plane_x, speed, false);
		update_player_y(game, player->plane_y, speed, false);
	}
	if (player->right)
	{
		update_player_x(game, player->plane_x, speed, true);
		update_player_y(game, player->plane_y, speed, true);
	}
}
```

**Purpose**: Read the player's held-key flags (set by `handle_keys`/cleared by `player_idle`) and issue the corresponding X/Y collision-checked movement calls.

**Variables:**
- `speed`: Cached copy of `game->movement_speed` for this frame.
- `player->up/down`: Move forward/backward along the **view direction** vector (`dir_x`, `dir_y`).
- `player->left/right`: Strafe using the **camera plane** vector (`plane_x`, `plane_y`), which is always perpendicular to the view direction — so strafing moves sideways relative to where the player is looking.

**Logic**: Each of the four flags, if set, calls both `update_player_x` and `update_player_y` with the appropriate direction vector and `sum` flag:
- `up`: `+dir` (forward).
- `down`: `-dir` (backward).
- `left`: `-plane` (strafe left).
- `right`: `+plane` (strafe right).

All four `if`s are independent (not `else if`), so e.g. holding `up` + `right` simultaneously moves diagonally (forward-right) since both movement calls execute in the same frame.

## Function: `move_player(t_game *game, t_player *player)`

```c
void	move_player(t_game *game, t_player *player)
{
	check_direction_to_move(game, player);
	if (player->rotate_l)
		rotate(player, -game->rotation_speed);
	if (player->rotate_r)
		rotate(player, game->rotation_speed);
}
```

**Purpose**: Top-level per-frame player update, called from `render()` every frame. Handles both translation and keyboard-driven rotation (mouse rotation is handled separately/immediately in `mouse_move`, not here).

**Logic**: Delegate translation to `check_direction_to_move`. Then, if the left/right arrow-rotation flags are set, call `rotate` with `-rotation_speed` (turn left) or `+rotation_speed` (turn right). Both can't meaningfully both apply in a useful way if held together (they'd cancel per-frame, alternating based on `if` order — but since both `if`s run, holding both simultaneously would rotate left then immediately right by the same amount, net zero).

---

# `player.c`

Key-event dispatch entry points and player initialization from map spawn data.

## Function: `player_moving(int keycode, void *param)`

```c
int	player_moving(int keycode, void *param)
{
	t_game	*game;

	game = param;
	if (keycode == XK_Escape)
		finish_game(game);
	handle_keys(keycode, &game->player);
	handle_minimap_keys(keycode, game);
	handle_speed_keys(keycode, game);
	return (0);
}
```

**Purpose**: The `KeyPress` hook callback — the single entry point for all key-down handling, dispatching to the three specialized handler modules.

**Variables:**
- `keycode`: X11 keysym of the pressed key.
- `param`: Generic hook parameter, cast to `t_game *`.

**Logic:**
1. If `Escape` was pressed, call `finish_game` directly (which exits the process — so nothing after this line runs for an `Escape` press, though the code doesn't `return` immediately after calling it, since `finish_game` never returns).
2. Otherwise (or rather, for every keypress, since `finish_game` doesn't return control here on Escape), dispatch to:
   - `handle_keys` — WASD/arrow movement flags + mouse toggle.
   - `handle_minimap_keys` — minimap show/hide/zoom/size.
   - `handle_speed_keys` — movement/rotation speed tuning.
3. Return `0` (MLX hook convention).

## Function: `player_idle(int keycode, void *param)`

```c
int	player_idle(int keycode, void *param)
{
	t_game	*game;

	game = param;
	if (keycode == XK_w)
		game->player.up = false;
	else if (keycode == XK_s)
		game->player.down = false;
	else if (keycode == XK_a)
		game->player.left = false;
	else if (keycode == XK_d)
		game->player.right = false;
	else if (keycode == XK_Left)
		game->player.rotate_l = false;
	else if (keycode == XK_Right)
		game->player.rotate_r = false;
	return (0);
}
```

**Purpose**: The `KeyRelease` hook callback — clears the corresponding movement/rotation flag when a key is released, so the player stops moving in that direction.

**Logic**: Mirror image of `handle_keys`'s WASD/arrow branch, but sets flags to `false` instead of `true`. Note there's no release-handling for `Space` (mouse-look toggle) here, since that's a one-shot toggle on press, not a held state.

## Function: `set_player_dir_values(t_player *player, double dx, double dy)` (static)

```c
static void	set_player_dir_values(t_player *player, double dx, double dy)
{
	player->dir_x = dx;
	player->dir_y = dy;
}
```

**Purpose**: Tiny setter — assigns the player's view direction vector. Exists mainly to keep `initialize_player` within Norminette's parameter-count/line-count limits by bundling the two assignments into one call.

## Function: `set_player_plane_values(t_player *player, double px, double py)` (static)

```c
static void	set_player_plane_values(t_player *player, double px, double py)
{
	player->plane_x = px;
	player->plane_y = py;
}
```

**Purpose**: Same idea as `set_player_dir_values`, but for the camera-plane vector.

## Function: `initialize_player(t_game *game, t_player *player)`

```c
void	initialize_player(t_game *game, t_player *player)
{
	player->player_x = game->map->start_x;
	player->player_y = game->map->start_y;
	if (player->starting_direction == NORTH)
	{
		set_player_dir_values(player, 0, -1);
		set_player_plane_values(player, game->fov, 0);
	}
	else if (player->starting_direction == SOUTH)
	{
		set_player_dir_values(player, 0, 1);
		set_player_plane_values(player, -game->fov, 0);
	}
	else if (player->starting_direction == EAST)
	{
		set_player_dir_values(player, 1, 0);
		set_player_plane_values(player, 0, game->fov);
	}
	else if (player->starting_direction == WEST)
	{
		set_player_dir_values(player, -1, 0);
		set_player_plane_values(player, 0, -game->fov);
	}
}
```

**Purpose**: Set the player's starting world position and initial view direction/camera-plane vectors, based on the map's parsed spawn point and facing character (`N`/`S`/`E`/`W` from the `.cub` file).

**Variables:**
- `game->map->start_x/y`: Spawn tile coordinates, parsed from the map file (location of the `N`/`S`/`E`/`W` character).
- `player->starting_direction`: Enum (`NORTH`/`SOUTH`/`EAST`/`WEST`) also parsed from the map's spawn character.
- `game->fov`: The camera-plane magnitude (`0.66` by default), which controls field-of-view width.

**Logic**: For each of the four cardinal starting directions, set:
- **NORTH**: facing `(0, -1)` (up, since Y increases downward in this grid convention), camera plane `(fov, 0)` — perpendicular to direction, pointing along +X.
- **SOUTH**: facing `(0, 1)` (down), plane `(-fov, 0)`.
- **EAST**: facing `(1, 0)` (right), plane `(0, fov)`.
- **WEST**: facing `(-1, 0)` (left), plane `(0, -fov)`.

Each direction/plane pair is perpendicular, and the plane's sign is chosen consistently so that the camera plane always points to the player's *right* relative to their facing direction — this is what makes screen-left correspond to world-left as the player rotates.

---

# `raycasting.c`

Core DDA (Digital Differential Analysis) raycasting algorithm — the classic Wolfenstein-3D-style grid raymarching technique.

## Function: `out_of_bounds(t_game *game, t_ray *ray)`

```c
bool	out_of_bounds(t_game *game, t_ray *ray)
{
	if (ray->map_y < 0 || ray->map_y >= game->win_h || ray->map_x < 0)
		return (true);
	if (ray->map_x >= (int)strlen(game->map->grid[ray->map_y]))
		return (true);
	return (false);
}
```

**Purpose**: Safety check during ray-marching — has the ray's current grid cell walked off the edge of the map array? Prevents out-of-bounds reads of `game->map->grid`.

**Variables:**
- `ray->map_x, ray->map_y`: The ray's current integer grid cell, updated each DDA step in `cast_ray`.

**Logic:**
1. If `map_y` is negative or `>= game->win_h` — **note**: this compares a *map row index* against `game->win_h` (window height in pixels), which looks like it should instead be checking against the map's row count (e.g. `array_len(game->map->grid)`). Since `win_h` is typically far larger than the number of map rows (window height in pixels vs. a small map), this bound is effectively too permissive and doesn't actually catch a ray that has walked past the last row of the map — it would instead fail later when `game->map->grid[ray->map_y]` is accessed with an out-of-array index. This is worth double-checking/fixing if you see crashes or garbage reads with rays travelling in certain directions.
2. Also reject negative `map_x`.
3. Separately, check `map_x` against the *actual* length of the specific row string (`strlen(game->map->grid[ray->map_y])`) — this part correctly handles non-rectangular maps (rows of different lengths).
4. Return `false` (in bounds) only if none of the above trip.

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

**Purpose**: The core DDA loop — steps the ray one grid cell at a time (always stepping into whichever neighboring cell is closer, X or Y) until it hits a wall, a closed/partially-open door, or goes out of bounds.

**Variables:**
- `tile` (param, reused as local): The character at the ray's current grid cell.
- `ray->side_dist_x/y`: Distance from the ray's start to the next X or Y grid line, respectively (this is what's compared to decide which axis to step next — standard DDA).
- `ray->delta_dist_x/y`: How far along the ray you travel to cross one full grid cell in X or Y.
- `ray->map_x/y`: Current grid cell.
- `ray->step_x/y`: `+1` or `-1`, direction to step in each axis (set by `check_raydir_x/y`).
- `ray->side`: Which axis was stepped on the *last* iteration (`0` = X-side hit, i.e. a vertical wall face; `1` = Y-side hit, a horizontal wall face) — this determines shading (`color_gradient`) and texture direction (`load_texture`) later.
- `ray->hit`: Loop-terminating flag, set to `1` once a wall or blocking door is found.
- `ray->is_door`: Set `true` if the hit was specifically a door tile that's rendering as (partially) solid.

**Algorithm (DDA):**
1. Loop until `ray->hit` becomes true.
2. Compare `side_dist_x` vs `side_dist_y` — step into whichever grid line is closer:
   - If X is closer: advance `side_dist_x` by one cell's worth (`delta_dist_x`), step `map_x` by `step_x`, mark `side = 0`.
   - Else: advance `side_dist_y`, step `map_y`, mark `side = 1`.
3. Bounds check — return early (ray simply never hits anything, e.g. an open map edge) if out of bounds.
4. Read the tile character at the new cell.
5. If it's a wall (`'1'`), the ray stops here (`hit = 1`).
6. If it's a door (`'D'`) **and** `ray_hits_door` says this specific ray should be blocked by the door's current animation state/geometry, stop here too, additionally flagging `is_door = true` for downstream texture/rendering logic. If the door tile doesn't block this ray (e.g., it's open enough at this exact horizontal position), the loop continues past the door tile as if it weren't there.

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

**Purpose**: Initialize the X-axis DDA state for a ray, before the main `cast_ray` loop begins.

**Variables:**
- `ray->ray_dir_x`: X component of this ray's direction vector (from `dir + plane * camera_x`).
- `ray->delta_dist_x`: How far (in ray-length units) you travel to cross one full grid cell horizontally — `|1 / ray_dir_x|`. Set to a huge sentinel (`1e30`) if `ray_dir_x` is exactly `0`, to avoid division by zero (a perfectly vertical ray never crosses another vertical grid line, so this distance is effectively infinite).
- `ray->step_x`: `+1` if the ray points rightward (`ray_dir_x >= 0`), `-1` if leftward.
- `ray->side_dist_x`: Initial distance from the player's exact position to the first vertical grid line the ray will cross.
  - If moving left (`ray_dir_x < 0`): distance from `player_x` back to the current cell's left edge (`map_x`, since we're inside that cell): `(player_x - map_x) * delta_dist_x`.
  - If moving right: distance from `player_x` to the current cell's right edge (`map_x + 1.0`): `(map_x + 1.0 - player_x) * delta_dist_x`.

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

**Purpose**: Exact Y-axis mirror of `check_raydir_x` — same logic, same reasoning, applied to `ray_dir_y`, `delta_dist_y`, `step_y`, `side_dist_y`, and `map_y`/`player_y`.

---

# `raycasting_door.c`

Determines whether a given ray should be blocked by a door, accounting for the door's current animation frame (a door slides open from the center outward, so a ray can pass through the "open gap" near the middle while still being blocked near the door frame's edges).

## Function: `door_tex_x(t_ray *ray, double wall_x)` (static)

```c
static int	door_tex_x(t_ray *ray, double wall_x)
{
	int	tex_x;

	tex_x = (int)(wall_x * PX);
	if (ray->side == 0 && ray->ray_dir_x > 0)
		tex_x = PX - tex_x - 1;
	if (ray->side == 1 && ray->ray_dir_y < 0)
		tex_x = PX - tex_x - 1;
	return (tex_x);
}
```

**Purpose**: Convert a normalized `wall_x` (0.0–1.0 position across the door tile's face) into a texture-column pixel index (`0` to `PX-1`), flipping it for certain ray directions so textures aren't drawn mirrored.

**Variables:**
- `wall_x`: Fractional position across the tile face where the ray crossed (`0.0` = one edge, `1.0` = the other).
- `tex_x`: The resulting texture column, initially `wall_x * PX` (`PX` = pixel size of a texture, e.g. `64`).

**Logic**: This mirrors the same flipping logic used for regular walls in `find_wall_hit` (`raycasting2.c`) — for two of the four side/direction combinations, the texture needs to be read right-to-left instead of left-to-right (`PX - tex_x - 1`) so that textures appear correctly oriented regardless of which direction the ray approached the surface from.

## Function: `ray_hits_door(t_game *game, t_ray *ray)`

```c
bool	ray_hits_door(t_game *game, t_ray *ray)
{
	t_door	*door;
	double	perp;
	double	wall_x;
	int		tex_x;
	int		open_px;

	door = get_door_at(game, ray->map_x, ray->map_y);
	if (!door || door->state == DOOR_CLOSED)
		return (true);
	if (door->state == DOOR_OPEN || door->frame >= DOOR_ANIM_FRAMES - 1)
		return (false);
	if (ray->side == 0)
		perp = ray->side_dist_x - ray->delta_dist_x;
	else
		perp = ray->side_dist_y - ray->delta_dist_y;
	if (ray->side == 0)
		wall_x = game->player.player_y + perp * ray->ray_dir_y;
	else
		wall_x = game->player.player_x + perp * ray->ray_dir_x;
	wall_x -= floor(wall_x);
	tex_x = door_tex_x(ray, wall_x);
	open_px = (int)((double)door->frame / DOOR_ANIM_FRAMES * (PX / 2));
	return (abs(tex_x - PX / 2) >= open_px);
}
```

**Purpose**: For a ray currently examining a `'D'` tile in `cast_ray`, decide whether this specific ray — at this specific horizontal crossing position — is blocked by the door's geometry, based on how far open it currently is.

**Variables:**
- `door`: The matching `t_door` struct for this tile (via `get_door_at`).
- `perp`: Perpendicular distance to the point where the ray crosses this grid cell's boundary (same formula as `calculate_wall` in `raycasting2.c`, computed early/locally here since the ray hasn't finished its full DDA loop yet).
- `wall_x`: Fractional position (`0.0`–`1.0`) across the door tile's face where the ray crosses — computed the same way as for regular walls in `find_wall_hit`, then wrapped into `[0,1)` via `wall_x -= floor(wall_x)`.
- `tex_x`: `wall_x` converted to a texture-column pixel index via `door_tex_x`.
- `open_px`: How many pixels of the door have "opened" (slid away) from the center, based on the current animation `frame` — a linear fraction of the tile's half-width (`PX / 2`), scaled by `frame / DOOR_ANIM_FRAMES`.

**Logic:**
1. If there's no door struct for this cell, or the door is fully `DOOR_CLOSED`, treat it as fully solid → blocks the ray (`return true`).
2. If the door is fully `DOOR_OPEN`, or its animation frame is at/past the last frame (`DOOR_ANIM_FRAMES - 1`, essentially fully open visually even if state hasn't flipped to `DOOR_OPEN` yet), it doesn't block at all (`return false`).
3. Otherwise, the door is mid-animation (opening or closing) — compute exactly where across the tile face the ray crosses (`perp`, then `wall_x`), convert to a texture-space X pixel (`tex_x`).
4. Compute `open_px` — how far (in texture pixels) the door has slid open from the center line (`PX / 2`) on **each** side, proportional to the current frame.
5. Final check: `abs(tex_x - PX / 2) >= open_px` — is this ray's crossing point *outside* the currently-open gap around the center? If the ray's `tex_x` is farther from center than the door has opened, it's still blocked by solid door material (`true`). If it's within the opened gap (closer to center than `open_px`), the ray passes through (`false`).

> This is the "center-opening split door" formula referenced in memory — the door visually splits at its middle and each half slides outward, so the blocking region shrinks symmetrically from the center as `frame` increases.

---

# `raycasting_draw.c`

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

**Purpose**: Draw one vertical textured wall slice (one screen column) — the heart of the raycaster's rendering. Maps the full-height wall texture onto whatever pixel range (`draw_start`..`draw_end`) this particular wall slice occupies on screen (which may be much taller or shorter than the texture itself, hence the texture Y needs its own step/scaling).

**Variables:**
- `x`: The screen column being drawn (fixed for this call).
- `y`: The screen row currently being drawn, iterating from `ray->draw_start` to `ray->draw_end`.
- `ray->line_height`: The *full* (potentially off-screen) height this wall slice would occupy if not clipped to the window — used to compute the vertical texture scaling factor, even though `draw_start`/`draw_end` are clamped to the visible window range.
- `step`: How much to advance the texture's Y-coordinate (`tex_pos`) per on-screen pixel — `PX / line_height`. If `line_height` is large (wall is close, tall on-screen), `step` is small (each screen row maps to a fraction of a texture row — texture appears "stretched"/magnified). If `line_height` is small (wall far away), `step` is large (texture appears compressed).
- `tex_pos`: Running fractional position into the texture's Y axis, initialized to account for any clipping at the top of the screen (`draw_start` may be `0` even though the wall slice conceptually starts above the screen, at a negative Y — this initialization backs out what texture row corresponds to the visible `draw_start`).
- `tex_y`: `tex_pos` truncated to an integer texture row, wrapped into `[0, PX-1]` via a bitwise AND with `PX - 1` (a fast modulo, valid since `PX` is a power of two).
- `color`: The shaded pixel color for this screen pixel, from `color_gradient` applied to the raw texel.

**Logic:**
1. Compute the vertical texture step and the starting texture position (accounting for top-of-screen clipping).
2. `y = ray->draw_start` — start at the first visible screen row for this slice. **Note**: assigning to the `y` parameter here overwrites whatever was passed in — the caller (`raycasting_init.c`) always passes `0` for `y`, so this reassignment is effectively how the function receives its real starting row; a slightly unusual pattern (using a parameter as a throwaway/local) but valid.
3. `(load_texture(game, ray), find_wall_hit(game, ray))` — a comma-expression calling both `load_texture` (pick which sprite/door-frame texture applies to this ray, see `raycasting2.c`) and `find_wall_hit` (compute the horizontal texture X coordinate, `ray->tex_x`) before the pixel loop begins. Using the comma operator here is a way to pack two statements while staying within whatever line-count constraint motivated it.
4. Loop over each screen row from `draw_start` to `draw_end`:
   - Compute `tex_y` from the running `tex_pos`, then advance `tex_pos` by `step` for the next iteration.
   - Bounds-check both `tex_x` and `tex_y` against `[0, PX)`; if either is out of range, skip this pixel (shouldn't normally happen, but guards against edge-case texture coordinate math).
   - Read the raw texel color and pass it through `color_gradient` for distance/side shading.
   - `if (!ray->is_door || color)` — for **regular walls**, always draw. For **door textures specifically**, only draw the pixel if `color` is non-zero (truthy) — this lets door XPMs use pure-black (`0x000000`) pixels as a transparency mask, so the "gap" area of a partially-open door shows the wall/floor behind it rather than an opaque black rectangle. (This does mean a door texture can never legitimately contain true black — any real black pixel in the sprite would also be treated as "transparent".)
   - Write the pixel via `my_pixel_put`.

---

# `raycasting_init.c`

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

**Purpose**: Set up a fresh `t_ray` struct for screen column `x` before casting it — the per-column setup step of the raycasting pipeline.

**Variables:**
- `x`: Screen column index.
- `ray->camera_x`: Normalized camera-plane coordinate for this column (`-1` to `+1`), same formula as in `debug.c`'s `print_fov`.
- `ray->ray_dir_x/y`: This column's ray direction vector — the player's forward direction plus the camera plane scaled by `camera_x`, which is what fans the rays out across the FOV.
- `ray->map_x/y`: Starting grid cell — the player's current tile (truncated to `int`).
- `ray->hit`, `ray->is_door`: Reset to `false` for the new ray.

**Logic**: Compute the camera-space direction for this column, initialize the ray's starting grid cell to the player's tile, reset hit/door flags, and call `check_raydir_x`/`check_raydir_y` to set up the DDA step/delta/side-distance state (see `raycasting.c`).

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

**Purpose**: The top-level raycasting entry point, called once per frame from `render()`. Casts one ray per screen column and draws the resulting wall slice.

**Variables:**
- `ray`: A single `t_ray` struct, reused (re-initialized) for every column — not an array, since each column is processed and drawn fully before moving to the next.
- `x`: Current screen column, `0` to `win_w - 1`.

**Logic**: For every screen column:
1. `init_ray` — set up the ray's direction and DDA starting state for this column.
2. `cast_ray` — march the DDA loop until it hits a wall/door or exits the map.
3. `calculate_wall` — compute the perpendicular distance and the on-screen vertical extent (`draw_start`/`draw_end`/`line_height`) of this wall slice (see `raycasting2.c`).
4. `draw_textured_line` — render the textured, shaded vertical slice to the screen buffer at column `x`.

---

# `raycasting2.c`

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

**Purpose**: For a ray that hit a door tile, select the correct animation-frame texture to use for rendering, based on the door's current `frame`.

**Variables:**
- `door`: The `t_door` struct for the hit tile.
- `frame`: Local, clamped copy of `door->frame`, used to index `game->sprites.door_frames[]`.

**Logic:**
1. Look up the door; if somehow missing (shouldn't happen if `ray->is_door` was set correctly), silently return, leaving `ray->texture` unset/stale for this call.
2. Clamp `frame` into the valid array index range `[0, DOOR_ANIM_FRAMES - 1]` — necessary because `door->frame` itself is allowed to equal `DOOR_ANIM_FRAMES` exactly (as the "fully open" sentinel set in `advance_door_animation`), which would be one-past-the-end of the `door_frames[]` array if used directly as an index.
3. Assign `ray->texture` to the corresponding frame's `t_texture`.

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

**Purpose**: Pick which sprite texture applies to this ray's hit — either a door animation frame, or one of the four cardinal wall textures based on which face was hit and which direction the ray was travelling.

**Logic:**
1. If the ray hit a door, delegate entirely to `load_door_texture`.
2. Otherwise, determine the wall texture by combining `ray->side` (which axis was hit last — `0` = a vertical/X-facing wall, `1` = a horizontal/Y-facing wall) with the sign of the corresponding ray direction component:
   - `side == 0` (vertical wall face) + `ray_dir_x > 0` (ray travelling rightward, so it hit the wall's **west**-facing side from outside, i.e. we're looking at the wall's east side... — practically: ray moving right hits the *east* texture) → `sprites.east`. Otherwise (`ray_dir_x <= 0`, moving left) → `sprites.west`.
   - `side == 1` (horizontal wall face) + `ray_dir_y > 0` (moving downward) → `sprites.south`. Otherwise (moving upward) → `sprites.north`.

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

**Purpose**: Compute the exact horizontal position where the ray crosses the wall face (`wall_x`, `0.0`–`1.0`), then convert that to a texture column (`ray->tex_x`), applying the same left/right flip correction seen in `door_tex_x`.

**Variables:**
- `wall_x`: The fractional position across the wall tile's face where the ray actually crosses — computed by projecting the player's position along the *perpendicular axis* out to the hit point:
  - If `side == 0` (hit a vertical face): use the player's **Y** position plus the ray's Y-travel over the perpendicular distance — this gives where along the wall's vertical face (in world Y) the hit occurred.
  - If `side == 1` (horizontal face): use the player's **X** position plus X-travel — where along the wall's horizontal face (world X) the hit occurred.
- `wall_x -= floor(wall_x)` — discard the integer part, keeping only the fractional position within the single tile (`[0, 1)`).
- `ray->tex_x`: `wall_x` scaled to `[0, PX)` texture-column space.

**Logic**: Same texture-flip logic as `door_tex_x` — for a vertical face hit while moving rightward, or a horizontal face hit while moving upward, the texture X needs to be mirrored (`PX - tex_x - 1`) to avoid mirrored-looking textures, since the "positive" wall_x direction doesn't consistently correspond to the same visual left-to-right direction for every combination of side/ray-direction.

## Function: `get_texture_pixel(t_img *texture, int x, int y)`

```c
unsigned int	get_texture_pixel(t_img *texture, int x, int y)
{
	int	offset;

	offset = y * texture->line_length + x * (texture->bits_per_pixel / 8);
	return (*(unsigned int *)(texture->addr + offset));
}
```

**Purpose**: Generic raw-pixel-read helper — given a loaded MLX image and a pixel coordinate, return its 32-bit color value directly from the raw buffer (bypassing MLX's higher-level, slower per-pixel API).

**Variables:**
- `texture->line_length`: Bytes per row in the image's raw buffer (may include padding beyond just `width * bytes_per_pixel`, which is why it must be used rather than assuming `width * bpp/8`).
- `texture->bits_per_pixel`: Typically `32` for MLX images; divided by `8` to get bytes per pixel.
- `offset`: Byte offset into `texture->addr` for pixel `(x, y)`.

**Logic**: Standard raw-buffer pixel address math: `row * line_length + column * bytes_per_pixel`, then reinterpret those 4 bytes as an `unsigned int` color value.

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

**Purpose**: After a ray finishes its DDA loop and hits something, compute the perpendicular (fish-eye-corrected) distance to the hit, and from that, the on-screen vertical extent of the wall slice.

**Variables:**
- `ray->perp_wall_dist`: The corrected distance — using `side_dist - delta_dist` (rather than the raw straight-line distance from player to hit point) is the classic DDA trick to avoid the "fish-eye" lens distortion that a naive distance calculation would produce, since it measures distance strictly along the ray's own axis-aligned component, decoupled from the ray's angle relative to the player's forward direction.
- Divide-by-zero guard: if `perp_wall_dist` is exactly `0` (ray hit immediately, player standing right at a wall), nudge it to a tiny `0.0001` to avoid a division-by-zero in the next line.
- `ray->line_height`: The full (unclamped) height in pixels this wall slice would occupy — inversely proportional to distance (`win_h / perp_wall_dist`), so closer walls appear taller.
- `ray->draw_start`: The screen row where the wall slice begins — centered vertically (`win_h / 2`) minus half the line height, clamped to `0` so it never goes off the top of the screen.
- `ray->draw_end`: Symmetric — center plus half the line height, clamped to `win_h - 1` so it never overflows past the bottom.

**Logic**: This is the standard raycaster "wall height" projection formula. The clamping of `draw_start`/`draw_end` is what allows `line_height` itself to exceed the screen (for walls very close to the player) without ever attempting to draw outside the buffer — `draw_textured_line`'s texture-Y math (`tex_pos` initialization) already accounts for this clamping so the *visible* portion of an oversized wall slice still samples the correct part of the texture.

---

# `render.c`

The lowest-level pixel-write primitive, background painting, and the main per-frame orchestrator.

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

**Purpose**: The single foundational pixel-writing function used everywhere in the codebase (debug rendering, raycasting, minimap, doors) — writes one pixel directly into the off-screen `game->buffer`'s raw memory.

**Variables:**
- `x, y`: Target pixel coordinates in the buffer/window.
- `color`: Packed `0x00RRGGBB` (or `0xTTRRGGBB`) color value to write.
- `offset`: Byte offset into the buffer for `(x, y)`.
- `dst`: Pointer into the buffer's raw memory at that offset.

**Logic:**
1. Bounds-check `x`/`y` against `0` and the window dimensions; silently no-op if out of range — this is what makes every caller throughout the codebase safe to call with unchecked/derived coordinates without individually bounds-checking first.
2. Compute the byte offset using the buffer's `line_length` (bytes per row, may include padding) and `bits_per_pixel / 8` (bytes per pixel).
3. Cast the offset pointer to `unsigned int *` and write the full 32-bit color in one operation.

## Function: `create_trgb(int t, int r, int g, int b)` (static)

```c
static int	create_trgb(int t, int r, int g, int b)
{
	return (t << 24 | r << 16 | g << 8 | b);
}
```

**Purpose**: Pack separate transparency/red/green/blue channel values into a single 32-bit color int.

**Variables:**
- `t`: "Transparency"/alpha-like top byte (MLX images use a `TRGB` format where the top byte is often unused/ignored by the renderer, hence always passed as `0` by callers here).
- `r, g, b`: Color channels, each `0`–`255`.

**Logic**: Standard bit-packing: shift each channel into its byte position and OR them together.

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

**Purpose**: Fill the entire screen buffer with a single flat color. Used in debug mode (filled with `GREY`) before drawing the top-down map elements on top.

**Variables:**
- `y, x`: Row/column loop counters over every pixel in the window.
- `color`: The single fill color for the whole buffer.

**Logic**: Simple double loop over every pixel, writing `color` to each. Note the inner loop increments `x` inline within the `my_pixel_put` call (`x++`) rather than as a separate statement — a compact style choice that still executes the same as a trailing `x++;` would, evaluated before the next iteration's condition check.

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

**Purpose**: Paint the "sky"/ceiling and "ground"/floor background for the normal (non-debug) raycasting mode, using the colors parsed from the `.cub` map file. Called every frame *before* raycasting draws the wall slices on top.

**Variables:**
- `c`: Packed ceiling color, computed once outside the pixel loop (from `game->map->ceiling`, a `t_rgb*`).
- `f`: Packed floor color, computed once outside the loop (from `game->map->floor`).
- `y, x`: Pixel loop counters.

**Logic**: For every pixel, if it's in the top half of the screen (`y < win_h / 2`), paint it the ceiling color; otherwise paint it the floor color — a simple horizontal split down the vertical center, simulating a flat horizon line before any walls are drawn over it. Colors are pre-computed once (`c`, `f`) rather than recomputed per-pixel, avoiding redundant bit-shifting work across the ~millions of pixels touched per frame.

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

**Purpose**: The main per-frame render function, registered via `mlx_loop_hook` in `mlx_main`, so it runs once per iteration of the MLX event loop (i.e., every frame). Branches entirely on `game->debug` into two parallel pipelines.

**Variables**: None local — purely an orchestrator over other functions and `game` sub-state.

**Debug-mode pipeline:**
1. `paint_plain_background(game, GREY)` — flat grey fill.
2. `render_elements(game)` — draw the top-down map grid (white/black squares for walls/void).
3. `(update_doors(game), move_player(game, &game->player))` — a comma-expression running both the door state-machine update and the player movement update in a single statement (again, likely a Norminette line-count workaround). Note this couples the two calls together stylistically but they're logically independent — order here means doors update *before* the player moves this frame.
4. Draw a tiny `3×3` black square at the player's position (a simple player marker dot) via `draw_texture`, converting the player's world coordinates to pixel coordinates by multiplying by `PX`.
5. `print_fov(game, &game->player)` — draw the red FOV ray-fan visualization.
6. `mlx_put_image_to_window` — blit the fully-drawn off-screen buffer to the actual window in one call (avoids flicker from drawing pixel-by-pixel directly to the window).
7. `display_speeds(game)` — presumably an on-screen HUD/debug text overlay showing current movement/rotation speed (defined elsewhere, not in the uploaded files).

**Normal-mode pipeline:**
1. `paint_background(game)` — ceiling/floor flat-color fill.
2. Update doors and move the player (same comma-expression pattern).
3. `raycasting(game)` — cast all rays and draw every textured wall slice.
4. `if (game->minimap) render_minimap(game)` — conditionally overlay the minimap circle (only if the player hasn't toggled it off with `M`).
5. Blit the buffer to the window.
6. Show the speed HUD.

**Return value**: `0` — MLX loop-hook callbacks are expected to return an `int`, though the return value isn't meaningfully used by MLX's loop mechanism here.

---

# Cross-File Data Flow Summary

| Concern | Files involved | Flow |
|---|---|---|
| **Startup** | `main.c` → `initialization.c` → `mlx.c` | Parse map → allocate `t_game` → init MLX/window/images/player → enter loop |
| **Per-frame input** | `mlx.c` (hooks) → `player.c` → `keys_handle.c` | `KeyPress`/`KeyRelease` events → `player_moving`/`player_idle` → flag setters |
| **Per-frame update** | `render.c` → `door.c`, `movement.c` | `render()` calls `update_doors()` and `move_player()` every frame |
| **Collision** | `movement.c` ↔ `door.c` | `update_player_x/y` consult `is_door_blocking()` for `'D'` tiles |
| **Raycasting** | `raycasting_init.c` → `raycasting.c` → `raycasting2.c` / `raycasting_door.c` → `raycasting_draw.c` | Per-column ray setup → DDA marching (consulting door geometry) → distance/height calc → textured pixel draw |
| **Shading** | `raycasting2.c`, `color_gradient.c` | Raw texel → distance/side-based brightness → final pixel color |
| **Doors** | `door.c` (state machine) → `door_frames.c` (assets) → `door_sound.c` (audio) → `raycasting_door.c`/`raycasting2.c` (rendering) | Proximity drives state/frame → frame selects texture and blocking geometry → sound plays on state transitions |
| **Debug mode** | `main.c` (toggle) → `debug.c`, `render.c` | Alternate top-down rendering path, bypassing the raycaster entirely |
| **Output** | `render.c`'s `my_pixel_put` | Universal low-level pixel write used by every drawing function in every file above |
