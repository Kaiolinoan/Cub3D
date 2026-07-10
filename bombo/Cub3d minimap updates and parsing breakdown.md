# Minimap Updates + Parsing Pipeline — Line-by-Line Breakdown

Two parts, matching the split you called out:

- **Part 1** — what changed in the *game-repo* minimap files (`minimap_fov.c`, `minimap_fov_utils.c`, `minimap_main.c`, `minimap_player.c`, `utils.c`) since `minimap_breakdown_uptodate.md` was written. Everything not listed here is unchanged from that doc.
- **Part 2** — a full fresh breakdown of the *parsing* files (`flood_fill.c`, `get_infos.c`, `parse_doors.c`, `parse_elements.c`, `parse_map.c`, `parse_rgb.c`), which weren't covered before.

---

# Part 1 — Minimap / utils.c: what's new (door support)

The only functional change across all five files is that **doors now render on the minimap and block the FOV cone visualization**, the same way walls do. Everything else (circle math, FOV ray interpolation, DDA-style line drawing, player dot, background/border render passes) is identical to the existing breakdown.

## `utils.c` — `minimap_tile_color(...)`

```c
	cell = game->map->grid[cell_y][cell_x];
	if (cell == '1')
		return (GREY);
	if (cell == ' ')
		return (rgb_to_color(game->map->ceiling));
	if (cell == 'D')
		return (MINIMAP_DOOR_COLOR);
	return (rgb_to_color(game->map->floor));
```

**What changed**: A new branch was inserted before the final floor fallback: `if (cell == 'D') return (MINIMAP_DOOR_COLOR);`.

**What it does**: Door tiles (`'D'`) now get their own distinct sentinel color (`MINIMAP_DOOR_COLOR`) instead of falling through to the generic floor color like every other walkable tile. This makes doors visually distinguishable on the minimap (e.g. a different hue from plain floor), and — more importantly — gives door tiles a unique, checkable color value that other functions can test for.

> Note this doesn't check the door's open/closed *state* — every door tile always renders as `MINIMAP_DOOR_COLOR` on the minimap regardless of whether it's open, closed, or mid-animation. The minimap shows door *positions*, not door *states*.

## `minimap_fov.c` — `raycast_to_wall(...)`

```c
	while (ray_distance < max_distance)
	{
		if (minimap_tile_color(game, ray_x, ray_y) == GREY
			|| minimap_tile_color(game, ray_x, ray_y) == MINIMAP_DOOR_COLOR)
			return (ray_distance);
		ray_x += dir.x * ray_step;
		ray_y += dir.y * ray_step;
		ray_distance += ray_step;
	}
```

**What changed**: The wall-hit condition used to be a single check (`== GREY`). It's now an OR of two checks: `== GREY` **or** `== MINIMAP_DOOR_COLOR`.

**What it does**: The minimap's FOV cone (the fan of thin rays marched out from the player to draw the "flashlight" overlay) now treats **every door tile as an opaque obstruction**, exactly like a wall — the ray-march stops there and the FOV cone gets clipped at the door, regardless of whether that specific door is actually open in the real 3D world at that moment.

**Consequence / design note**: This is a simplification, not a physically-accurate door simulation on the minimap. A door that's fully open in the raycasted 3D view will still cut off the minimap's FOV cone as if it were shut, since `raycast_to_wall` has no access to (and doesn't check) `door->state` — it only looks at the static map-character color. If you ever want the minimap FOV to "see through" open doors the way the real raycaster does, this function would need to look up the actual `t_door` at `(ray_x, ray_y)` via `get_door_at` and skip the stop-condition when `door->state == DOOR_OPEN`, similar to how `ray_hits_door` in `raycasting_door.c` handles the real 3D raycaster.

**Everything downstream is unaffected in structure**: `get_ray_end`, `draw_single_fov_ray`, `init_fov_dirs`, and `draw_minimap_fov` all consume `raycast_to_wall`'s return value the same way as before — they don't know or care *why* the ray stopped, only how far it travelled.

## `minimap_main.c` — background pixel logic extracted into its own function

The old breakdown described `render_minimap_background`'s per-pixel logic as inline. It's now been pulled out into a dedicated helper:

```c
static void	render_background_pixel(t_game *game, t_ip p, int radius_px,
	t_ip offset)
{
	double	poffx;
	double	poffy;
	double	dist_sq;

	poffx = p.x - radius_px;
	poffy = p.y - radius_px;
	dist_sq = poffx * poffx + poffy * poffy;
	if (dist_sq <= (double)(radius_px * radius_px))
		put_minimap_pixel(game, offset.x + p.x, offset.y + p.y,
			minimap_tile_color(game, game->player.player_x
				+ (poffx / (double)game->minimap_tile_size),
				game->player.player_y
				+ (poffy / (double)game->minimap_tile_size)));
}
```

**Purpose**: Handle exactly one pixel of the minimap's circular background — compute its offset from the minimap center, circle-clip it, convert it to a world coordinate, and draw it with whatever color `minimap_tile_color` returns (now including the new `MINIMAP_DOOR_COLOR` case above).

**Variables:**
- `p` (`t_ip`): The pixel being processed, in local `0..diameter` bounding-square coordinates (not yet offset to screen position).
- `radius_px`: Minimap circle radius in pixels.
- `offset` (`t_ip`): The minimap's top-left screen position (`MINIMAP_MARGIN, MINIMAP_MARGIN`).
- `poffx, poffy`: `p`'s offset from the center of the bounding square (i.e. from the circle's center).
- `dist_sq`: Squared distance from center — compared against `radius_px²` to circle-clip without needing `sqrt`.

**Logic:**
1. Compute the pixel's offset from center.
2. If it's outside the circle (`dist_sq > radius_px²`), do nothing — this pixel is corner-of-the-square-but-outside-the-circle, left untouched.
3. Otherwise, convert the pixel offset into a world-space coordinate by dividing by `minimap_tile_size` (pixels-per-tile) and adding the player's current world position — this is what keeps the minimap always centered on the player.
4. Call `minimap_tile_color` for that world coordinate and draw it via `put_minimap_pixel` at the pixel's actual screen position (`offset.x + p.x`, `offset.y + p.y`).

```c
static void	render_minimap_background(t_game *game, int radius_px,
		int diameter, t_ip offset)
{
	int	pixel_y;
	int	pixel_x;

	pixel_y = 0;
	while (pixel_y < diameter)
	{
		pixel_x = 0;
		while (pixel_x < diameter)
		{
			render_background_pixel(game, (t_ip){pixel_x, pixel_y},
				radius_px, offset);
			pixel_x++;
		}
		pixel_y++;
	}
}
```

**What changed**: The loop body is now a single call to `render_background_pixel` instead of inline per-pixel math. Purely a refactor — the outer double loop over the `diameter × diameter` bounding square is unchanged, and the resulting rendered image is identical to before. This is the same "extract a helper to fit Norminette's function-length limit" pattern seen elsewhere in the codebase (e.g. `fov_line_pixel`/`fov_line_step` being split out of `draw_minimap_fov_line`).

`render_minimap_border` and `render_minimap` themselves are byte-for-byte unchanged from the prior breakdown.

## `minimap_player.c`, `minimap_fov_utils.c`

No functional or structural changes from the previous breakdown — `draw_minimap_player`, `blend_color_with_white`, `minimap_in_circle`, `fov_line_step`, `fov_line_pixel`, and `draw_minimap_fov_line` are all identical.

---

# Part 2 — Parsing Pipeline

These files run once at startup, before the MLX window ever opens: `main.c` calls `initialize_game()` (which calls `get_map_details` from `parse_elements.c`), then `check_if_map_is_valid()` (from `parse_map.c`), which in turn drives door validation, spawn detection, and the flood-fill closure check.

**Overall call order:**
```
main()
 └─ initialize_game()
     └─ get_map_details()               (parse_elements.c)
         ├─ check_and_assign_coordinate()  — per line, parses header (NO/SO/WE/EA/F/C)
         │    └─ filter_element() → filter_rgb()  (parse_rgb.c, for F/C lines)
         └─ store_maze()                 (parse_map.c) — once all 6 header elements found
 └─ check_if_map_is_valid()              (parse_map.c)
     ├─ check_extension()
     ├─ check_maze_chars()               — validates characters, counts spawn points
     ├─ check_doors()                    (parse_doors.c) — validates & stores door positions
     ├─ get_starting_info()              (get_infos.c) — finds spawn tile & facing
     └─ flood_fill_prep()                (flood_fill.c) — confirms map is fully wall-enclosed
```

---

## `parse_elements.c`

Reads the `.cub` file line by line, extracting the six required header elements (four texture paths + two RGB colors) before handing off the remaining lines as the map body.

### Function: `get_file_fd(char *filename)` (static)

```c
static int	get_file_fd(char *filename)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		if (errno == EACCES)
			print_error("Permission denied");
		else if (errno == ENOENT)
			print_error("File does not exist");
		return (-1);
	}
	return (fd);
}
```

**Purpose**: Open the map file read-only, with specific, human-readable error messages for the two most common failure reasons.

**Variables:**
- `filename`: Path to the `.cub` file (`argv[1]`).
- `fd`: The resulting file descriptor, or `-1` on failure.

**Logic**: `open()` with `O_RDONLY`. If it fails, inspect `errno`: `EACCES` (permission denied) or `ENOENT` (file doesn't exist) each get a tailored message; any other `errno` value (e.g. `ENOTDIR`, `ELOOP`) silently returns `-1` with no message printed — a minor gap if the map path hits a less common error condition.

### Function: `init_elements(t_game *game)` (static)

```c
static t_elem	*init_elements(t_game *game)
{
	static t_elem	elements[5];

	elements[0].id = "NO";
	elements[0].target = (char **)&game->sprites.north.path;
	elements[1].id = "SO";
	elements[1].target = (char **)&game->sprites.south.path;
	elements[2].id = "WE";
	elements[2].target = (char **)&game->sprites.west.path;
	elements[3].id = "EA";
	elements[3].target = (char **)&game->sprites.east.path;
	elements[4].id = NULL;
	elements[4].target = NULL;
	return (elements);
}
```

**Purpose**: Build a small lookup table mapping the four texture-identifier tokens (`"NO"`, `"SO"`, `"WE"`, `"EA"`) to *pointers to the pointer fields* they should populate (`game->sprites.north.path`, etc.), so `filter_element` can loop generically instead of writing four separate `if` blocks.

**Variables:**
- `elements`: A `static t_elem[5]` array — `static` here means it's allocated once (not re-created on the stack every call) and its contents persist between calls, though since every field is unconditionally reassigned at the top of the function each time it's called, the `static` storage mainly avoids repeated stack allocation rather than being used for cross-call memory.
- `elements[i].id`: The 2-letter token expected in the `.cub` file (e.g. `"NO"` for north texture).
- `elements[i].target`: A `char **` — the *address of* the `.path` field on the corresponding sprite struct, cast to `char **`. Storing the address (not the value) is what lets `filter_element` write into the actual struct field through this generic table (`*(elements[i].target) = ...`).
- `elements[4]`: A `{NULL, NULL}` sentinel terminating the array, used by `filter_element`'s `while (elements[i].id)` loop.

**Logic**: Simple table construction — one row per known texture element, each pointing at where its parsed path string should be stored, plus a null terminator row.

### Function: `filter_element(char **arr, t_game *game)` (static)

```c
static int	filter_element(char **arr, t_game *game)
{
	t_elem	*elements;
	int		i;

	i = 0;
	elements = init_elements(game);
	while (elements[i].id)
	{
		if (!ft_strncmp(*arr, elements[i].id, 3))
		{
			if (*(elements[i].target))
				return (print_error("Double definition of element"), -1);
			else if (!check_extension(arr[1], ".xpm"))
				return (print_error("Element has an invalid extension"), -2);
			return (*(elements[i].target) = ft_strdup(arr[1]), 1);
		}
		i++;
	}
	return (filter_rgb(arr, game));
}
```

**Purpose**: Given a line already split into whitespace-separated tokens (`arr[0]` = the element identifier, `arr[1]` = its value), check whether it matches one of the four known texture identifiers; if so, validate and store it. If it matches none of them, delegate to `filter_rgb` (in `parse_rgb.c`) to check for `"C"`/`"F"` color lines instead.

**Variables:**
- `arr`: The line split by spaces (`arr[0]` = token like `"NO"`, `arr[1]` = the value like `"assets/wall_north.xpm"`).
- `elements`: The lookup table from `init_elements`.
- `i`: Loop index over `elements`.

**Logic:**
1. Loop through the 4 known element identifiers.
2. `ft_strncmp(*arr, elements[i].id, 3)` — compare the first token against this element's id, comparing **3** characters even though the ids (`"NO"`, `"SO"`, etc.) are only 2 characters long. This is intentional: comparing 3 bytes includes each string's null terminator, so `"NOX"` (a mistyped/extra-character token) would *not* match `"NO"` — the comparison requires an exact 2-character token, not just a 2-character prefix.
3. If matched:
   - If the target pointer is already non-`NULL` (this texture was already assigned earlier in the file), it's a duplicate — error out with `-1`.
   - Else if `arr[1]`'s extension isn't `.xpm`, error out with `-2`.
   - Else, duplicate `arr[1]` into the target field via `ft_strdup`, wrapped in a comma-expression so the assignment and the `return 1` happen in one statement, and return `1` (success, "this token was recognized and consumed").
4. If no element id matched at all, fall through to `filter_rgb(arr, game)` — the token might be `"C"` or `"F"` instead.

> **Return-value contract**: `1` = recognized & consumed, `0` = not recognized (only possible via `filter_rgb`'s fallthrough, if it also doesn't match), `-1` = duplicate definition, `-2` = bad extension/format. This convention is what `check_and_assign_coordinate` below relies on.

### Function: `check_and_assign_coordinate(t_game *g, char *line)` (static)

```c
static int	check_and_assign_coordinate(t_game *g, char *line)
{
	int			tmp;
	static int	count;
	char		**arr;

	if (count == 6)
		return (count);
	if (!line)
		return (print_error("Empty file"), -2);
	if (line && line[0] == '\n')
		return (0);
	arr = ft_split(line, ' ');
	if (!arr || !*arr || !arr[1])
		return (clear_matriz(arr), print_error("Invalid element"), -1);
	remove_new_line(*arr);
	tmp = filter_element(arr, g);
	clear_matriz(arr);
	if (tmp < 0)
		return (-1);
	else if (tmp && count < 6)
		return (count++);
	return (count);
}
```

**Purpose**: Process a single line from the `.cub` file during the "header" phase — try to parse it as one of the six required elements (4 textures + 2 colors) and track how many have been found so far via a `static` counter that persists across calls.

**Variables:**
- `line`: One line read from the file (via `get_next_line`, called by the caller `get_map_details`).
- `count` (`static int`): Running total of how many of the 6 required header elements have been successfully parsed so far, across **all** calls to this function for this file — `static` local variables retain their value between calls, which is what makes this work without needing to pass a counter around explicitly. **Caveat**: because it's `static`, this counter is *not* reset between different calls to `get_map_details` in the same program run — if `get_map_details` were ever called twice (e.g. to load two different maps in one process), `count` would carry over incorrectly. In this codebase it's only called once per program run, so it's safe in practice.
- `tmp`: Return value from `filter_element` for this line (`1`, `0`, `-1`, or `-2`).
- `arr`: The line split into whitespace-separated tokens.

**Logic:**
1. If all 6 elements are already found (`count == 6`), immediately return `6` without doing any more parsing — this is the switch-over point where subsequent lines are treated as the map body instead of header lines (the caller, `get_map_details`, checks for this exact condition to start accumulating map lines).
2. If `line` is `NULL` (end of file reached with `get_next_line`), that's an "empty file" error (or premature EOF before 6 elements found) — return `-2`.
3. If the line is blank (`line[0] == '\n'`), skip it (return `0`, "no progress, but not an error") — blank lines between header entries are tolerated.
4. Split the line by spaces into `arr`.
5. If the split failed, or produced no tokens, or is missing a second token (`arr[1]`), it's a malformed line — clean up and error with `-1`.
6. `remove_new_line(*arr)` — strip any trailing `\n` from the first token (the identifier) before comparing it.
7. Delegate the actual parsing/storage to `filter_element`.
8. Free the split array (`clear_matriz`).
9. If `filter_element` reported an error (`tmp < 0`), propagate `-1`.
10. If `filter_element` recognized and consumed the line (`tmp` truthy) and we haven't hit 6 yet, **increment `count` and return the pre-increment value** (postfix `count++` — the caller sees the count *before* this line's contribution, then `count` itself is now one higher for the next call).
11. Otherwise (line wasn't recognized as a header element, but also wasn't an error — shouldn't normally happen given `filter_element`'s contract, but defensively) return the current `count` unchanged.

### Function: `get_map_details(t_game *game, char *filename)`

```c
int	get_map_details(t_game *game, char *filename)
{
	int		count;
	char	*line;
	char	*full_line;
	int		fd;

	full_line = NULL;
	fd = get_file_fd(filename);
	if (fd < 0)
		return (false);
	while (1)
	{
		line = get_next_line(fd);
		count = check_and_assign_coordinate(game, line);
		if (!line || count < 0 || count > 6)
			break ;
		if (count == 6)
			full_line = ft_strjoin_and_free(full_line, line);
		free(line);
	}
	if (count > 0 && count < 6)
		print_error("Fewer elements than expexted");
	if (((count == 6) && (!store_maze(game, full_line))) || (count != 6))
		return (close(fd), free(line), false);
	return (close(fd), free(line), true);
}
```

**Purpose**: The top-level file-reading loop — reads the `.cub` file line by line, using `check_and_assign_coordinate` to consume the 6 header lines (in any order, anywhere before the map body) and then accumulates every subsequent line into one big `full_line` string, which eventually gets handed to `store_maze` to become the actual map grid.

**Variables:**
- `fd`: File descriptor from `get_file_fd`.
- `line`: Current line read via `get_next_line`.
- `full_line`: Accumulator string — every map-body line gets appended onto this (note: `ft_strjoin_and_free` frees the first argument and the passed-in `line`, per the project's established convention, and returns a newly grown string, effectively building up the whole map body as one string with embedded `\n`s).
- `count`: Result of `check_and_assign_coordinate` for the current line — used both to detect completion (`== 6`) and errors (`< 0`).

**Logic:**
1. Open the file; bail with `false` if it can't be opened.
2. Loop reading lines with `get_next_line` until end-of-file or an error/anomaly:
   - Pass the line to `check_and_assign_coordinate`.
   - **Loop-exit condition**: stop if `line` is `NULL` (EOF), or `count` is negative (a parsing error was flagged), or `count > 6` (shouldn't be reachable given the counter logic, but guarded defensively).
   - **Once `count == 6`** (all header elements found — this happens starting from the very call where `check_and_assign_coordinate` first returns `6`, which per its logic is actually the call *after* the 6th element was recognized, or any subsequent header-phase call once `count == 6` short-circuits at the top): append the current `line` onto `full_line` via `ft_strjoin_and_free` — meaning as soon as header parsing completes, this same loop iteration's line is treated as the first row of map data (and every line after it too).
   - Free `line` each iteration (its content has either been consumed into `full_line` or was a header line already fully processed by `check_and_assign_coordinate`).
3. After the loop: if `count` ended up strictly between `0` and `6` (some but not all header elements were found before EOF/error), print a "fewer elements than expected" warning.
4. Final validity check: succeed only if `count == 6` **and** `store_maze(game, full_line)` succeeds; any other combination (wrong count, or `store_maze` failing) returns `false`. Either way, close the fd and free `line` (note: `line` here refers to whatever the loop's *last* value was — typically `NULL` at EOF, so `free(NULL)` is a safe no-op).

> **Subtlety**: Because `check_and_assign_coordinate`'s `static count` returns the *pre-increment* count on a successful match, and returns `6` outright once the internal static counter has reached `6`, the exact line on which `full_line` starts accumulating is the line immediately following the one that pushed the internal counter to `6` — i.e., the map body starts right after the 6th header element line, which is the expected/correct behavior for a well-formed `.cub` file (all header lines first, blank line optionally, then the map grid).

---

## `parse_rgb.c`

Parses the `F` (floor) and `C` (ceiling) color lines, e.g. `F 220,100,0`.

### Function: `comma_count(char *str)` (static)

```c
static int	comma_count(char *str)
{
	size_t	i;
	size_t	count;

	i = 0;
	count = 0;
	while (str[i])
	{
		if (str[i] == ',')
			count++;
		i++;
	}
	return (count);
}
```

**Purpose**: Count how many commas appear in a string — used to strictly validate that an RGB spec has exactly 2 commas (i.e. exactly 3 comma-separated numbers, no more, no fewer, and no stray/duplicate commas like `"1,,2,3"` producing extra empty tokens that `ft_split` would otherwise silently absorb).

**Logic**: Straightforward linear scan incrementing `count` for every `,` character.

### Function: `fill_rgb(t_game *game, char **rgb, bool is_floor)` (static)

```c
static bool	fill_rgb(t_game *game, char **rgb, bool is_floor)
{
	if (is_floor)
	{
		game->map->floor = ft_calloc(1, sizeof(t_rgb));
		if (!game->map->floor)
			return (print_error(ALLOC_ERR), false);
		game->map->floor->r = (unsigned char)ft_atoi(rgb[0]);
		game->map->floor->g = (unsigned char)ft_atoi(rgb[1]);
		game->map->floor->b = (unsigned char)ft_atoi(rgb[2]);
	}
	else
	{
		game->map->ceiling = ft_calloc(1, sizeof(t_rgb));
		if (!game->map->ceiling)
			return (print_error(ALLOC_ERR), false);
		game->map->ceiling->r = (unsigned char)ft_atoi(rgb[0]);
		game->map->ceiling->g = (unsigned char)ft_atoi(rgb[1]);
		game->map->ceiling->b = (unsigned char)ft_atoi(rgb[2]);
	}
	return (true);
}
```

**Purpose**: Allocate and populate either `game->map->floor` or `game->map->ceiling` (a `t_rgb *`) from an already-validated 3-element array of numeric strings.

**Variables:**
- `rgb`: Array of exactly 3 numeric strings (validated by the caller, `parse_and_fill_rgb`), e.g. `["220", "100", "0"]`.
- `is_floor`: Which of the two color targets to populate.

**Logic**: Mirror-image `if`/`else` blocks (kept separate rather than using a `t_rgb **` pointer-to-pointer to stay simple/Norminette-friendly): allocate a zeroed `t_rgb`, bail with an allocation error if it fails, then convert each of the 3 string components via `ft_atoi` and store as `unsigned char` (implicitly truncating/wrapping if `ft_atoi` somehow returned something outside `0-255`, though the caller has already range-checked before this point).

### Function: `parse_and_fill_rgb(t_game *game, char **coord, bool is_floor)` (static)

```c
static bool	parse_and_fill_rgb(t_game *game, char **coord, bool is_floor)
{
	char	**rgb;
	char	*str;
	size_t	i;
	size_t	j;

	str = ft_strtrim(coord[1], "\n");
	if (!str)
		return (print_error(ALLOC_ERR), false);
	rgb = ft_split(str, ',');
	if (!rgb || !*rgb || !rgb[1] || !rgb[2] || rgb[3] || comma_count(str) != 2)
		return (free(str), print_error("Invalid RGB"), clear_matriz(rgb), 0);
	free(str);
	i = -1;
	while (rgb[++i])
	{
		j = -1;
		while (rgb[i][++j])
			if (!ft_isdigit(rgb[i][j]) && rgb[i][j])
				return (print_error(RGB_DIGIT_ERR), clear_matriz(rgb), false);
		if (ft_atoi(rgb[i]) < 0 || ft_atoi(rgb[i]) > 255 || *rgb[i] == '\n')
			return (print_error(RGB_RANGE_ERR), clear_matriz(rgb), false);
	}
	if (!fill_rgb(game, rgb, is_floor))
		return (clear_matriz(rgb), false);
	return (clear_matriz(rgb), true);
}
```

**Purpose**: Fully validate and parse an RGB triplet string (e.g. `"220,100,0"`) into three range-checked byte values, then hand off to `fill_rgb` to store them.

**Variables:**
- `coord`: The split line tokens — `coord[1]` is the RGB portion (e.g. `"220,100,0"`), `coord[0]` would be `"F"` or `"C"` (already checked by the caller).
- `str`: `coord[1]` with any trailing `\n` trimmed.
- `rgb`: `str` split on commas — should yield exactly 3 tokens.
- `i, j`: Loop indices — `i` over the 3 tokens, `j` over each character within a token. Both use the `size_t` unsigned "start at `-1`, pre-increment" idiom (`while (rgb[++i])`), which works because `(size_t)-1` wraps to the max unsigned value, and the pre-increment makes the first checked index `0`.

**Logic:**
1. Trim `coord[1]`. Bail on allocation failure.
2. Split by comma.
3. Validate structure: must have a first token, a second (`rgb[1]`), a third (`rgb[2]`), must **not** have a fourth (`rgb[3]` must be `NULL`/absent), and the raw string must contain exactly 2 commas total (`comma_count(str) != 2` catches things `ft_split` alone wouldn't, like a trailing comma producing an empty final token that might coincidentally still satisfy the `rgb[3]` check in some split implementations — this is a defense-in-depth check). Any failure here: free `str`, print "Invalid RGB", free `rgb`, return `0` (falsy).
4. Free `str` now that `rgb` holds the split copies.
5. For each of the 3 tokens (`i` loop):
   - For each character in the token (`j` loop): if it's not a digit **and** not the null terminator (the `&& rgb[i][j]` guard, though this is somewhat redundant since `while (rgb[i][++j])` already stops at the null terminator — so this condition can effectively never be true for the terminator itself; it reads as extra defensive redundancy), it's an invalid character — error with `RGB_DIGIT_ERR`.
   - After confirming all-digit, range-check the numeric value: must be `0-255` via `ft_atoi`, and also explicitly reject a token that's just a lone `\n` (`*rgb[i] == '\n'`) — a guard against a token that's technically "empty-ish" after trimming quirks, since `ft_atoi("\n")` would otherwise evaluate to `0` and silently pass the numeric range check despite being garbage input.
6. If all 3 tokens pass, call `fill_rgb` to actually store the parsed values into `game->map`.
7. Clean up `rgb` and return the final success/failure state.

### Function: `filter_rgb(char **arr, t_game *game)`

```c
int	filter_rgb(char **arr, t_game *game)
{
	if (!ft_strncmp(*arr, "C", 2))
	{
		if (!game->map->ceiling)
		{
			if (parse_and_fill_rgb(game, arr, false))
				return (1);
			return (-2);
		}
		return (print_error("Double definition of element"), -1);
	}
	if (!ft_strncmp(*arr, "F", 2))
	{
		if (!game->map->floor)
		{
			if (parse_and_fill_rgb(game, arr, true))
				return (1);
			return (-2);
		}
		return (print_error("Double definition of element"), -1);
	}
	return (0);
}
```

**Purpose**: The `filter_element` fallback — checks whether the current line's identifier token is `"C"` (ceiling) or `"F"` (floor), and if so, validates it hasn't already been set and delegates to `parse_and_fill_rgb`.

**Variables:**
- `arr`: The split line — `arr[0]` is the identifier (`"C"` or `"F"`), `arr[1]` is the RGB value string.
- `game->map->ceiling` / `game->map->floor`: Used as "already set?" guards — `NULL` means not yet parsed.

**Logic:**
1. `ft_strncmp(*arr, "C", 2)` — compare 2 bytes, i.e. the single character `'C'` plus its null terminator, so this rejects any token that's `"C"` followed by extra characters (e.g. a typo like `"Cx"`), requiring an exact single-character match, mirroring the same 3-byte-includes-null-terminator trick used in `filter_element` for the 2-character texture ids.
2. If it's `"C"`: if `ceiling` hasn't been set yet, try to parse and fill it (`is_floor = false`); return `1` on success or `-2` on parse failure. If `ceiling` was already set, it's a duplicate — error `-1`.
3. Same structure for `"F"` → `floor` (`is_floor = true`).
4. If the token matches neither `"C"` nor `"F"`, return `0` — "not recognized by this filter either," which propagates back up through `filter_element` and ultimately `check_and_assign_coordinate` as "line wasn't a valid header element" (only reachable there via the final `else` branch, since a `0` here isn't itself an error, just "no match").

---

## `parse_doors.c`

Validates door placement rules and records each door's grid position into `game->door[]`.

### Function: `check_door_position(char **grid, int y, int x)` (static)

```c
static bool	check_door_position(char **grid, int y, int x)
{
	char	up;
	char	down;
	char	left;
	char	right;

	up = get_char_at(grid, y - 1, x);
	down = get_char_at(grid, y + 1, x);
	left = get_char_at(grid, y, x - 1);
	right = get_char_at(grid, y, x + 1);
	if (up == 'D' || down == 'D' || left == 'D' || right == 'D')
		return (false);
	if (left == '1' && right == '1' && is_walkable(up) && is_walkable(down))
		return (true);
	if (up == '1' && down == '1' && is_walkable(left) && is_walkable(right))
		return (true);
	return (false);
}
```

**Purpose**: Enforce door-placement rules — a door must sit in a straight wall segment (flanked by walls on exactly one axis and walkable tiles on the perpendicular axis), and must not be directly adjacent to another door.

**Variables:**
- `grid`: The map's row array.
- `y, x`: This door's grid coordinates (row, column).
- `up, down, left, right`: The characters at the four orthogonally-adjacent cells, fetched via `get_char_at` — presumably a bounds-safe getter (defined elsewhere) that returns some sentinel (likely `' '` or similar) for out-of-bounds coordinates rather than crashing, which matters here since a door could legitimately be near a map edge.

**Logic:**
1. **No adjacent doors rule**: if any of the four neighbors is itself `'D'`, reject — prevents two doors from touching, which would likely break the center-opening slide animation and blocking-geometry math in `raycasting_door.c` (each door assumes it owns a full, isolated tile).
2. **Horizontal corridor case**: if `left` and `right` are both walls (`'1'`) and `up`/`down` are both walkable (via `is_walkable`, presumably true for floor/spawn characters and false for walls/void), this door sits in a wall running left-right, with an open passage above and below — a valid "doorway in a horizontal wall" configuration.
3. **Vertical corridor case**: symmetric — `up`/`down` both walls, `left`/`right` both walkable — a valid "doorway in a vertical wall."
4. If neither pattern matches (e.g. the door is in an open room, at a corner, or only partially flanked), reject.

### Function: `count_doors(char **grid)` (static)

```c
static int	count_doors(char **grid)
{
	int	i;
	int	j;
	int	count;

	i = 0;
	count = 0;
	while (grid[i])
	{
		j = 0;
		while (grid[i][j])
		{
			if (grid[i][j] == 'D')
				count++;
			j++;
		}
		i++;
	}
	return (count);
}
```

**Purpose**: Simple full-grid scan counting how many `'D'` characters exist, used to size the `game->door[]` array before populating it.

**Logic**: Standard nested loop over every row/column, incrementing `count` for each door character found.

### Function: `fill_doors(t_game *game)` (static)

```c
static bool	fill_doors(t_game *game)
{
	int	i;
	int	j;
	int	idx;

	i = 0;
	idx = 0;
	while (game->map->grid[i])
	{
		j = 0;
		while (game->map->grid[i][j])
		{
			if (game->map->grid[i][j] == 'D')
			{
				if (!check_door_position(game->map->grid, i, j))
					return (print_error("Invalid door position"), false);
				game->door[idx].x = j;
				game->door[idx].y = i;
				idx++;
			}
			j++;
		}
		i++;
	}
	return (true);
}
```

**Purpose**: Second full-grid scan (after `count_doors` sized the array) — for every door character found, validate its placement and record its coordinates into the next free slot of `game->door[]`.

**Variables:**
- `i, j`: Row/column scan indices.
- `idx`: Index into `game->door[]`, incremented only when a door is actually found and validated — so it always stays in sync with "how many doors have been recorded so far," independent of `i`/`j`.

**Logic**: For every `'D'` cell encountered: validate its position via `check_door_position`; if invalid, print an error and abort the whole parse (`false`). If valid, store `x = j` (column) and `y = i` (row) — consistent with the standard `x`=column, `y`=row convention used throughout the rest of the codebase (unlike the swapped naming quirk noted in `get_infos.c`'s `get_starting_dir`). Increment `idx` and continue. Returns `true` once every door in the grid has been processed without error.

### Function: `check_doors(t_game *game)`

```c
bool	check_doors(t_game *game)
{
	game->doors = count_doors(game->map->grid);
	if (game->doors == 0)
		return (true);
	game->door = ft_calloc(game->doors, sizeof(t_door));
	if (!game->door)
		return (print_error(ALLOC_ERR), false);
	return (fill_doors(game));
}
```

**Purpose**: The public entry point for door parsing, called from `check_if_map_is_valid` (`parse_map.c`). Counts, allocates, and populates the door array.

**Logic:**
1. Count doors and store the total in `game->doors`.
2. If there are none, that's perfectly valid (not every map needs doors) — return `true` immediately without allocating anything (`game->door` stays `NULL`, which is fine as long as every other function that iterates `game->door[]` also checks `game->doors == 0`/loops `0` times, e.g. `update_doors` in `door.c`, whose `while (i < game->doors)` naturally does nothing when `doors == 0`).
3. Otherwise, allocate exactly `game->doors` zeroed `t_door` structs. Bail on allocation failure.
4. Delegate the actual population/validation to `fill_doors`.

---

## `get_infos.c`

Locates the player's spawn tile and facing direction, and provides a small line-validation helper used by `get_infos.c`'s sibling files.

### Function: `get_starting_dir(int x, int y, t_player *player, t_map *map)` (static)

```c
static void	get_starting_dir(int x, int y, t_player *player, t_map *map)
{
	char	**grid;

	grid = map->grid;
	if (grid[x][y] == 'N')
		player->starting_direction = NORTH;
	else if (grid[x][y] == 'W')
		player->starting_direction = WEST;
	else if (grid[x][y] == 'E')
		player->starting_direction = EAST;
	else if (grid[x][y] == 'S')
		player->starting_direction = SOUTH;
}
```

**Purpose**: Read the character at grid position `(x, y)` and set `player->starting_direction` to the matching cardinal direction enum.

**Variables:**
- `x, y`: Despite the names, note how these are actually used: `grid[x][y]`. Given the caller (`get_starting_info`, below) invokes this as `get_starting_dir(i, j, ...)` where `i` is the row index and `j` is the column index, this function's `x` parameter actually receives the **row**, and `y` receives the **column** — the opposite of the usual `x`=column/`y`=row convention used everywhere else in this codebase (e.g. `game->player.player_x`). It's purely a local naming quirk internal to this one static helper; functionally it's correct (`grid[row][col]` is the right indexing), but the parameter names `x`/`y` here don't mean what they mean anywhere else in the project — worth keeping in mind if this function is ever touched again.
- `grid`: Local alias for `map->grid`.

**Logic**: A simple character-to-enum `if`/`else if` chain.

### Function: `get_starting_info(t_game *game)`

```c
void	get_starting_info(t_game *game)
{
	int		i;
	int		j;
	char	**grid;

	i = 0;
	grid = game->map->grid;
	while (grid[i])
	{
		j = 0;
		while (grid[i][j])
		{
			if (grid[i][j] == 'N' || grid[i][j] == 'S' || grid[i][j] == 'W'
				|| grid[i][j] == 'E')
			{
				game->map->start_x = j;
				game->map->start_y = i;
				get_starting_dir(i, j, &game->player, game->map);
			}
			j++;
		}
		i++;
	}
}
```

**Purpose**: Scan the whole map grid for the single spawn character (`N`/`S`/`E`/`W`) and record both its position (`map->start_x/y`) and facing direction (`player->starting_direction`).

**Variables:**
- `i, j`: Row/column scan indices.
- `grid`: Alias for `game->map->grid`.

**Logic**: Standard nested scan. When a spawn character is found: `start_x = j` (column) and `start_y = i` (row) — this *does* follow the standard `x`=column/`y`=row convention (unlike the internal quirk in `get_starting_dir` above). Then calls `get_starting_dir(i, j, ...)`, passing row first, column second — which, as noted, matches what that function's `grid[x][y]` indexing actually expects, so despite the confusing parameter names, the two functions are consistent with each other.

> **Relies on prior validation**: This function doesn't itself check that exactly one spawn character exists — that's `check_maze_chars`'s job (`parse_map.c`), called earlier in `check_if_map_is_valid`. If multiple spawn characters existed, this loop would simply overwrite `start_x`/`start_y`/`starting_direction` with the *last* one found in scan order — but that scenario is already rejected before this function ever runs.

### Function: `get_validated_full_line(char *full_line)`

```c
char	*get_validated_full_line(char *full_line)
{
	char	*new;

	new = ft_strtrim(full_line, "\n");
	free(full_line);
	if (!new)
		return (print_error(ALLOC_ERR), NULL);
	if (!check_full_line(new))
		return (print_error("There is no map"), free(new), NULL);
	if (!check_empty_line_on_map(new))
		return (print_error("Empty line on map"), free(new), NULL);
	return (new);
}
```

**Purpose**: Take the raw accumulated map-body string (`full_line`, built up in `get_map_details` by joining every post-header line) and validate/clean it before it's split into individual rows by `store_maze`.

**Variables:**
- `full_line`: The raw joined map-body string (may have leading/trailing newlines from the join process).
- `new`: The trimmed result.

**Logic:**
1. Trim leading/trailing `\n` characters from `full_line` (note: trims from both ends, not every internal newline — internal `\n`s are what separate the individual map rows and must be preserved).
2. Free the original `full_line` — this function takes ownership of and consumes its input, consistent with the project's established `_and_free`-style conventions elsewhere.
3. If trimming failed (allocation failure), error and return `NULL`.
4. `check_full_line(new)` — presumably checks the string isn't empty / actually contains map content (i.e. there *is* a map at all after the header). If it fails, "There is no map" error, free, return `NULL`.
5. `check_empty_line_on_map(new)` — presumably rejects blank lines embedded *within* the map body (as opposed to leading/trailing ones already trimmed away), since a blank row in the middle of a map grid would be ambiguous/invalid. If it fails, "Empty line on map" error, free, return `NULL`.
6. If all checks pass, return the cleaned string, ready for `ft_split(..., '\n')` in `store_maze`.

---

## `parse_map.c`

Final assembly of the map grid and the top-level validity gate.

### Function: `check_maze_chars(char **grid)` (static)

```c
static int	check_maze_chars(char **grid)
{
	size_t	i;
	size_t	j;
	int		count;

	i = 0;
	count = 0;
	while (grid[i])
	{
		j = 0;
		while (grid[i][j])
		{
			if (grid[i][j] && grid[i][j] != '0' && grid[i][j] != '1'
				&& grid[i][j] != ' ' && grid[i][j] != 'D')
			{
				if (grid[i][j] == 'N' || grid[i][j] == 'S'
					|| grid[i][j] == 'W' || grid[i][j] == 'E')
					count++;
				else
					return (-1);
			}
			j++;
		}
		i++;
	}
	return (count);
}
```

**Purpose**: Validate that every character in the map grid is one of the allowed symbols (`'0'` floor, `'1'` wall, `' '` void, `'D'` door, or exactly one spawn character), and count how many spawn characters exist.

**Variables:**
- `i, j`: Row/column scan indices (as `size_t`, unlike most other scan loops in this codebase which use `int` — a minor inconsistency, though harmless since map dimensions are always small positive numbers).
- `count`: Running total of spawn characters (`N`/`S`/`W`/`E`) found.

**Logic**: For every character, if it's *not* one of the always-allowed set (`'0'`, `'1'`, `' '`, `'D'`), check whether it's one of the four spawn characters — if so, increment `count`; if it's genuinely something else entirely (any other symbol), it's an invalid character and the whole function fails immediately with `-1`. Returns the total spawn-character count if no invalid characters were found — the caller then checks this count is exactly `1`.

### Function: `store_maze(t_game *game, char *full_line)`

```c
bool	store_maze(t_game *game, char *full_line)
{
	char	**temp;
	int		size;
	int		i;

	full_line = get_validated_full_line(full_line);
	if (!full_line)
		return (false);
	temp = ft_split(full_line, '\n');
	if (!temp || !*temp)
		return (free(full_line), false);
	size = array_len(temp);
	game->map->grid = ft_calloc(size + 1, sizeof(char *));
	if (!game->map->grid)
		return (free(full_line), clear_matriz(temp), print_error(ALLOC_ERR), 0);
	i = 0;
	while (temp[i])
	{
		game->map->grid[i] = ft_strdup(temp[i]);
		if (!game->map->grid[i])
			return (print_error(ALLOC_ERR), clear_matriz(game->map->grid), 0);
		i++;
	}
	game->map->grid[i] = NULL;
	return (free(full_line), clear_matriz(temp), true);
}
```

**Purpose**: Convert the raw accumulated map-body string into the actual `game->map->grid` array of row strings. Called once from `get_map_details` right after the header phase completes (`count == 6`).

**Variables:**
- `full_line`: Reassigned in-place to the validated/trimmed version returned by `get_validated_full_line` (the original raw string is freed inside that call).
- `temp`: `full_line` split on `'\n'` — one entry per map row.
- `size`: Number of rows (`array_len(temp)`).
- `game->map->grid`: The final destination array, allocated with `size + 1` slots (the `+1` for the `NULL` terminator).
- `i`: Loop index copying each row from `temp` into `game->map->grid`.

**Logic:**
1. Validate/clean the raw string via `get_validated_full_line`; bail if it fails.
2. Split into rows by `'\n'`.
3. If the split failed or produced zero rows, clean up and fail.
4. Allocate `game->map->grid` with room for every row plus a `NULL` terminator.
5. Copy (duplicate) each row string from `temp` into `game->map->grid`, checking each `ft_strdup` for failure. **Note**: on a `ft_strdup` failure partway through, this returns immediately after freeing `game->map->grid` itself (via `clear_matriz`) but does **not** free `full_line` or `temp` on that specific error path — a small leak on this particular (rare, OOM-only) failure branch, worth being aware of if you're doing a leak audit, though not something that would show up in normal testing.
6. Null-terminate `game->map->grid`.
7. On success: free `full_line`, free `temp` (the now-redundant split array, since its contents were duplicated rather than transferred), and return `true`.

### Function: `check_if_map_is_valid(char *filename, t_game *game)`

```c
bool	check_if_map_is_valid(char *filename, t_game *game)
{
	int	chars_nb;

	if (!check_extension(filename, ".cub"))
		return (print_error("Invalid file"), 0);
	chars_nb = check_maze_chars(game->map->grid);
	if (chars_nb < 0)
		return (print_error("Invalid character on map"), 0);
	else if (chars_nb == 0)
		return (print_error("There is no player starting position on map"), 0);
	else if (chars_nb > 1)
		return (print_error("Multiple player's starting position on map"), 0);
	if (!check_doors(game))
		return (false);
	get_starting_info(game);
	if (!flood_fill_prep(game->map))
		return (print_error("Map is not surrounded by walls"), 0);
	return (true);
}
```

**Purpose**: The single top-level "is this a legal, playable map?" gate, called from `main.c` right after `initialize_game` succeeds (i.e., after the grid has already been built by `store_maze`, but before the MLX window opens).

**Variables:**
- `filename`: The `.cub` path (re-checked for extension here, even though `get_map_details` already succeeded — a defensive/explicit re-validation).
- `chars_nb`: Result of `check_maze_chars` — either `-1` (invalid character present) or the count of spawn characters found (should be exactly `1`).

**Logic (in order, each step gating the next):**
1. Re-verify the file extension is `.cub`.
2. `check_maze_chars` — validate every character in the grid and count spawn points:
   - `< 0`: an outright invalid character exists → error.
   - `== 0`: no spawn character at all → error ("no player starting position").
   - `> 1`: more than one spawn character → error ("multiple starting positions") — this is what guarantees `get_starting_info`'s "last one wins" behavior (noted above) never actually matters in practice, since execution never reaches `get_starting_info` unless exactly one spawn character exists.
3. `check_doors` — validate and store every door's position (`parse_doors.c`). Any door-placement violation fails the whole map here.
4. `get_starting_info` — now safe to call (exactly one spawn point confirmed) — sets the player's spawn position and facing.
5. `flood_fill_prep` — confirm the walkable area is fully enclosed by walls (no gaps leaking to the map's edge) — see `flood_fill.c` below. Fails with "Map is not surrounded by walls" if the flood fill escapes.
6. If every step passes, the map is valid — return `true`.

---

## `flood_fill.c`

Confirms the player's reachable area is fully enclosed by walls — i.e., there's no "hole" in the map boundary through which the flood fill (and therefore the player, and the raycaster) could leak into unbounded/undefined space.

### Function: `flood_fill(int x, int y, char **grid, int height)` (static, recursive)

```c
static bool	flood_fill(int x, int y, char **grid, int height)
{
	if (y < 0 || y >= height || !grid[y] || x < 0
		|| x >= (int)ft_strlen(grid[y]) || grid[y][x] == ' ')
		return (0);
	else if (grid[y][x] == '1' || grid[y][x] == 'f')
		return (2);
	grid[y][x] = 'f';
	if (!flood_fill(x + 1, y, grid, height))
		return (0);
	if (!flood_fill(x - 1, y, grid, height))
		return (0);
	if (!flood_fill(x, y + 1, grid, height))
		return (0);
	if (!flood_fill(x, y - 1, grid, height))
		return (0);
	return (1);
}
```

**Purpose**: Recursively flood-fill outward from a starting tile, marking every reachable walkable tile as visited (`'f'`). If the fill ever reaches a genuine out-of-bounds/void cell (`' '` or literally off the edge of the grid array), that means the walkable area isn't fully enclosed by walls — a leak has been found, and the whole recursive chain unwinds reporting failure.

**Variables:**
- `x, y`: Current cell being visited (`x` = column, `y` = row — standard convention here).
- `grid`: A **working copy** of the map grid (passed in from `flood_fill_prep`, never the real `game->map->grid`), since this function mutates cells by overwriting them with `'f'`.
- `height`: Number of rows in the grid, used for the row-bounds check.

**Three-way return convention** (this function returns a `bool`, but is used with `!`-style truthiness checks, so all three of its actual return values matter):
- **`0` (false)** — "leak detected": either genuinely out of the array bounds (`y`/`x` negative or past the grid dimensions, or the row string itself is too short for this `x`), or the cell is void space (`' '`) — space that isn't walled off. Either way, this signals the enclosure check has failed, and it propagates all the way back up through every recursive caller via the `if (!flood_fill(...)) return (0);` guards.
- **`2` (truthy, but distinct from `1`)** — "hit a boundary, stop recursing here, but this is fine": either a wall (`'1'`) or a cell already visited in this fill (`'f'`, preventing infinite re-visiting/exponential blowup). Since `2` is non-zero, the `!flood_fill(...)` checks in the caller treat it as "success" for this branch — the recursion simply doesn't continue past this cell, but it doesn't indicate failure.
- **`1` (truthy)** — "this cell and everything reachable from all 4 of its neighbors was successfully explored without leaking" — the normal/expected success case once all 4 recursive calls each returned non-zero.

**Logic:**
1. **Bounds/void check**: if `y` is negative, `y >= height`, the row is `NULL`, `x` is negative, `x` is past the end of this particular row's string, or the cell is literal void (`' '`) — this is a leak. Return `0`.
2. **Already-handled check**: if the cell is a wall (`'1'`) or already marked visited (`'f'`), there's nothing more to explore from here — return `2` (stop, but not a failure).
3. **Mark and recurse**: otherwise, this is a fresh walkable cell — mark it `'f'` (visited) to prevent infinite recursion on revisits, then recursively flood-fill into each of the 4 orthogonal neighbors (right, left, down, up). If *any* of the 4 recursive calls returns `0` (a leak was found somewhere downstream), immediately propagate `0` upward without checking the remaining directions — short-circuit failure.
4. If all 4 directions were explored without any leak being found, return `1`.

> **Why the working copy matters**: because this function overwrites cells with `'f'`, it must never run directly on `game->map->grid` — doing so would corrupt the actual map data (e.g. turning floor tiles permanently into `'f'` characters, which nothing else in the renderer/parser recognizes as valid). `flood_fill_prep` (below) handles making a disposable copy specifically so this function is free to mutate it destructively.

### Function: `flood_fill_prep(t_map *map)`

```c
bool	flood_fill_prep(t_map *map)
{
	char	**copy;
	int		size;
	size_t	i;

	size = array_len(map->grid);
	copy = ft_calloc(size + 1, sizeof(char *));
	if (!copy)
		return (print_error(ALLOC_ERR), false);
	i = 0;
	while (map->grid[i])
	{
		copy[i] = ft_strdup(map->grid[i]);
		if (!copy[i])
			return (print_error(ALLOC_ERR), clear_matriz(copy), false);
		i++;
	}
	copy[i] = NULL;
	if (!flood_fill(map->start_x, map->start_y, copy, size))
		return (clear_matriz(copy), false);
	return (clear_matriz(copy), true);
}
```

**Purpose**: The public entry point for the enclosure check, called once from `check_if_map_is_valid`. Builds a disposable deep copy of the map grid, runs the recursive flood fill on the copy starting from the player's spawn point, and reports whether the map is fully enclosed.

**Variables:**
- `size`: Number of rows in the real map (`array_len(map->grid)`).
- `copy`: The disposable deep copy — allocated with `size + 1` row-pointer slots (`+1` for the `NULL` terminator), each row individually duplicated via `ft_strdup` so mutations to `copy` never touch `map->grid`.
- `i`: Copy-loop index.

**Logic:**
1. Determine row count and allocate the outer pointer array for the copy.
2. Deep-copy every row string from `map->grid` into `copy`, checking each duplication for allocation failure (cleaning up and failing immediately if one occurs).
3. Null-terminate the copy.
4. Run `flood_fill` starting at `map->start_x, map->start_y` (the player's spawn tile, already determined by `get_starting_info` before this function is called from `check_if_map_is_valid`) on the copy.
5. If the flood fill reports failure (a leak was found, i.e. `flood_fill` returned `0`), clean up the copy and return `false` — the caller reports "Map is not surrounded by walls".
6. Otherwise, clean up the copy and return `true` — the map is fully enclosed.

> **Why start from the spawn point specifically**: the flood fill only needs to confirm that *the region the player can actually reach* is enclosed — it doesn't matter if some entirely disconnected, unreachable pocket of the map file happens to be open to the void, since the player could never walk there anyway. Starting from `start_x`/`start_y` (rather than, say, cell `(0,0)`) correctly scopes the check to just the playable area.

---

# Cross-File Notes / Things Worth Double-Checking

- **`raycast_to_wall` (minimap) treats every door as always-closed for FOV purposes** — the minimap's flashlight cone will visually stop at a door even when that door is open in the real 3D scene. Not a bug per se (it's a reasonable simplification), but worth knowing if the minimap ever looks like it's "hiding" area behind an open door that the raycaster is otherwise rendering fine.
- **`get_starting_dir`'s parameter names (`x`, `y`) are swapped relative to their actual row/column meaning** compared to the rest of the codebase's `x`=column/`y`=row convention — purely cosmetic/confusing, not a functional bug, since the caller passes arguments in the matching order.
- **`store_maze`'s error path on a mid-copy `ft_strdup` failure** doesn't free `full_line`/`temp` — a minor leak that only triggers under allocation failure (OOM), not during normal operation.
- **`check_and_assign_coordinate`'s `count` is `static`**, so it accumulates across the whole program's lifetime rather than being scoped to one call to `get_map_details` — harmless given the program only ever parses one map per run, but would need resetting if that ever changed (e.g. a "reload map" feature).
- **Door validation (`check_door_position`) requires a straight, fully-flanked wall segment** — a door can't be placed at a corner, in an open room, next to another door, or anywhere that doesn't have exactly the "walls on one axis, walkable on the other" pattern. This is stricter than just "not a wall, not void" — it's worth knowing if a map with doors is being rejected unexpectedly and the error is "Invalid door position".