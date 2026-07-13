# door_frames.c — Function Breakdown

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

**Purpose**: Construct the file path for a door animation frame.

**Parameters:**
- `i`: Frame index (0 to DOOR_ANIM_FRAMES-1).

**Return Value:**
- Allocated string with path like `"assets/animation/door_0.xpm"`, or `NULL` on failure.

**Logic:**

1. **Convert index to string** — `ft_itoa(i)` converts frame number to string (e.g., "0", "15").

2. **Append extension** — Join with `".xpm"` (e.g., "0.xpm").

3. **Prepend directory** — Join with `"assets/animation/door_"` (e.g., "assets/animation/door_0.xpm").

4. **Free intermediates** — Clean up temporary strings, keep only final path.

**File Convention:**
- All door frames: `assets/animation/door_0.xpm` through `door_(DOOR_ANIM_FRAMES-1).xpm`.
- Numbered sequentially: 0 = closed, DOOR_ANIM_FRAMES-1 = open.

---

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

**Purpose**: Load a single door animation frame from XPM file into MLX image.

**Parameters:**
- `game`: Game struct with MLX context.
- `frame`: Texture struct to populate with image.
- `i`: Frame index.

**Return Value:**
- `true` on success, `false` on any failure.

**Logic:**

1. **Build path** — Call `build_frame_path()` to construct file path.

2. **Set texture path** — Call `set_image_path()` to allocate and store path in texture struct.

3. **Convert to image** — Call `file_to_image()` to load XPM file into MLX image.

4. **Get pixel address** — Call `get_dir_img_address()` to retrieve image buffer pointer.
   - If this fails, destroy the image to avoid resource leak.

5. Return success/failure status.

---

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

**Purpose**: Load all door animation frame images (0 through DOOR_ANIM_FRAMES-1).

**Parameters:**
- `game`: Game struct with MLX context.
- `sprites`: Sprites struct to populate with door frame array.

**Return Value:**
- `true` if all frames load successfully, `false` on any failure.

**Logic:**

1. **Allocate frame array** — Allocate space for DOOR_ANIM_FRAMES texture structs.

2. **Load frames** — Loop through each frame index:
   - Call `load_door_frame()` to load frame `i`.
   - If loading fails, clean up already-loaded frames via `clear_door_frames()` and return `false`.

3. Return `true` if all frames loaded successfully.

**Error Handling:**
- Partial failure is handled: already-loaded frames are freed before returning.
- If any frame fails to load, the entire door system is disabled.

**Array Size:**
- DOOR_ANIM_FRAMES typically 16 (defined in header).
- Loads 16 XPM files (door_0.xpm through door_15.xpm).
