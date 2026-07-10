# initialization.c — Function Breakdown

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

**Purpose**: Initialize file paths for debug mode square textures used to draw the top-down map view.

**Parameters:**
- `sprites`: Pointer to the sprites struct where texture paths are stored.

**Return Value:**
- `true` on success, `false` on allocation failure.

**Logic:**
- Set path for black square (represents walls/obstacles in debug mode).
- Set path for white square (represents open floor in debug mode).
- Both are 64x64 XPM files stored in `assets/` directory.

---

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

**Purpose**: Load all XPM texture files into MLX image objects.

**Parameters:**
- `game`: Game struct (contains MLX context).
- `sprites`: Sprites struct with texture path and image pointers.

**Return Value:**
- `true` on success, `false` if any texture fails to load.

**Logic:**
- For each texture (east, west, north, south walls + debug squares):
  - Call `file_to_image()` to convert `.xpm` file to MLX image.
  - If conversion fails, return `false` (error printed in `file_to_image()`).
- All textures must load or entire initialization fails.

---

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

**Purpose**: Get memory addresses of all loaded texture images for fast pixel access during rendering.

**Parameters:**
- `sprites`: Sprites struct with loaded image objects.

**Return Value:**
- `true` on success, `false` if any address retrieval fails.

**Logic:**
- For each texture:
  - Call `get_dir_img_address()` to retrieve MLX image buffer pointer.
  - This is required for sampling pixels during raycasting.
  - If retrieval fails, return `false`.

---

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

**Purpose**: Master initialization function for all graphics buffers and textures.

**Parameters:**
- `game`: Game struct (contains window dimensions and MLX context).
- `sprites`: Sprites struct to populate with loaded textures.

**Return Value:**
- `true` on complete success, `false` on any failure.

**Variables:**
- `game->buffer.img`: Backbuffer image for off-screen rendering.
- `game->buffer.addr`: Pixel data pointer for the backbuffer.
- `game->buffer.bits_per_pixel`, `line_length`, `endian`: MLX image metadata.

**Initialization Order:**
1. **Create backbuffer image** — offscreen rendering surface (full window size).
2. **Get backbuffer address** — pointer to pixel data.
3. **Set debug texture paths** — allocate and set paths for black/white squares.
4. **Load direction textures** — convert east/west/north/south XPM files to MLX images.
5. **Get texture addresses** — retrieve pixel pointers for each texture.
6. **Load door animation frames** — initialize all door open/close frame images.
7. Return `true` if all steps succeed.

**Call Chain:**
- Called by `mlx_main()` after window creation.
- Failure at any step halts initialization and returns `false`.

---

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

**Purpose**: Top-level game struct allocation and initialization with map data and default settings.

**Parameters:**
- `filename`: Path to the `.cub` map file.

**Return Value:**
- Pointer to newly allocated and populated `t_game` struct on success.
- `NULL` on failure (errors printed as they occur).

**Variables:**
- `game`: Heap-allocated game state struct.

**Initialization Steps:**

1. **Allocate game struct** — `ft_calloc(1, sizeof(t_game))` initializes to zero.

2. **Allocate map struct** — `game->map` for storing map grid, player position, colors, etc.

3. **Parse map file** — `get_map_details()` reads `.cub` file:
   - Extracts texture paths (NO, SO, WE, EA).
   - Extracts colors (Floor, Ceiling).
   - Reads map grid into `game->map->grid`.
   - Sets starting position into `game->map->start_x/y`.
   - If parsing fails, clean up and return `NULL`.

4. **Set default gameplay parameters:**
   - `minimap = true` — minimap initially visible (toggle with M).
   - `minimap_tile_size = 20` — pixels per world tile on minimap.
   - `minimap_radius_tiles = 7.0` — visible radius in world tile units.
   - `movement_speed = 0.07` — base movement speed per frame (adjustable with 1/2 keys).
   - `rotation_speed = 0.05` — base rotation speed per frame (adjustable with Up/Down keys).
   - `fov = 0.66` — camera plane width (field of view).

5. Return populated game struct.

**Error Handling:**
- If allocation fails at any step, previous allocations are freed via `clear_game()` before returning `NULL`.

**Call Chain:**
- Called from `main()`.
- Calls `get_map_details()` from `parsing/parse_elements.c`.
- Later called by `mlx_main()` to initialize graphics on the populated game struct.
