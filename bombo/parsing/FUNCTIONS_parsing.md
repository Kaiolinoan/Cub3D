# Parsing Functions — Function Breakdown

## parse_elements.c

### Function: `get_file_fd(char *filename)` (static)

Opens a map file and returns file descriptor.

**Logic:**
- `open(filename, O_RDONLY)` opens file for reading.
- Returns fd on success, `-1` on failure.
- Prints specific error messages (permission, not found, etc.).

### Function: `init_elements(t_game *game)` (static)

Initializes array of elements (texture directives) to parse.

**Variables:**
- Static array with 5 elements: NO, SO, WE, EA, and NULL terminator.
- Each element maps a directive ID to its target pointer in game->sprites.

### Function: `filter_element(char **arr, t_game *game)` (static)

Checks if line matches a texture element (NO, SO, WE, EA).

**Logic:**
- Compare first token with element IDs.
- If match found, store path pointer.
- If already assigned (double definition), error.
- Otherwise, call `filter_rgb()` to check for color directives.

**Return Values:**
- `1` = element processed.
- `-1` = error (double definition).
- `0` = not an element.

### Function: `check_and_assign_coordinate(t_game *g, char *line)` (static)

Parse a single line and assign element if it matches a directive.

**Logic:**
- Split line by spaces.
- Remove newlines.
- Call `filter_element()` to process.
- Count valid elements (must be 6: NO, SO, WE, EA, F, C).

### Function: `get_map_details(t_game *game, char *filename)`

Main parsing function. Reads `.cub` file and extracts all elements and map.

**Logic:**
1. Open file.
2. Read lines one at a time (via `get_next_line()`).
3. For first 6 lines: call `check_and_assign_coordinate()` to extract elements.
4. After elements found, accumulate remaining lines as map.
5. Call `store_maze()` to process and store map grid.
6. Close file and return success/failure.

**Return Values:**
- `true` if all 6 elements found and map parsed.
- `false` on any error.

---

## parse_map.c

### Function: `check_maze_chars(char **grid)` (static)

Validate map characters and count player starting positions.

**Logic:**
- Loop through all grid cells.
- Allowed characters: `'0'`, `'1'`, `' '`, `'D'`, `'N'`, `'S'`, `'E'`, `'W'`.
- Count player start positions (N/S/E/W).
- Return count on success, `-1` on invalid character.

### Function: `store_maze(t_game *game, char *full_line)`

Store map grid into game->map->grid.

**Logic:**
1. Trim and validate line.
2. Split by newlines.
3. Allocate grid array.
4. Copy each row via `ft_strdup()`.
5. Null-terminate array.

### Function: `check_if_map_is_valid(char *filename, t_game *game)`

Master map validation function.

**Logic:**
1. Check file extension (must be `.cub`).
2. Validate characters (count player positions).
3. Validate doors via `check_doors()`.
4. Extract starting position via `get_starting_info()`.
5. Check map is surrounded by walls via `flood_fill_prep()`.

---

## parse_rgb.c

### Function: `comma_count(char *str)` (static)

Count commas in RGB string (must be exactly 2).

### Function: `fill_rgb(t_game *game, char **rgb, bool is_floor)` (static)

Allocate and populate t_rgb struct.

**Logic:**
- Allocate t_rgb.
- Parse three strings as integers (0-255).
- Store in game->map->floor or game->map->ceiling.

### Function: `parse_and_fill_rgb(t_game *game, char **coord, bool is_floor)` (static)

Parse RGB string from directive and validate.

**Logic:**
1. Trim string.
2. Split by commas.
3. Validate exactly 3 values and 2 commas.
4. Validate all are digits.
5. Validate range [0, 255].
6. Call `fill_rgb()`.

### Function: `filter_rgb(char **arr, t_game *game)`

Check if line is F (floor) or C (ceiling) color directive.

**Logic:**
- If `"F"`: parse floor color.
- If `"C"`: parse ceiling color.
- Prevent double definitions.

---

## get_infos.c

### Function: `get_starting_dir(int x, int y, t_player *player, t_map *map)` (static)

Determine player starting direction from grid character (N/S/E/W).

### Function: `get_starting_info(t_game *game)`

Find player starting position and direction in map.

**Logic:**
- Loop through grid.
- Find character matching N/S/E/W.
- Set game->map->start_x/y.
- Call `get_starting_dir()` to set player->starting_direction.

### Function: `get_validated_full_line(char *full_line)`

Trim and validate accumulated map lines.

**Logic:**
1. Trim newlines.
2. Check line is not empty.
3. Check no empty lines within map.

---

## flood_fill.c

### Function: `flood_fill(int x, int y, char **grid, int height)` (static)

Recursive flood-fill from player position.

**Logic:**
1. Check bounds and valid cell.
2. If wall or already visited, return success.
3. Mark cell visited.
4. Recursively fill all 4 neighbors.
5. Return false if out of bounds (map not enclosed).

### Function: `flood_fill_prep(t_map *map)`

Prepare and execute flood-fill validation.

**Logic:**
1. Copy map grid (don't modify original).
2. Call `flood_fill()` from starting position.
3. If successful, map is enclosed.
4. Free copy and return result.

---

## parse_doors.c

### Function: `check_door_position(char **grid, int y, int x)` (static)

Validate door placement (must be between open spaces on perpendicular walls).

**Logic:**
- Check all 4 neighbors.
- Must be aligned: either (left/right are walls, up/down open) or vice versa.

### Function: `count_doors(char **grid)`

Count all 'D' characters in map.

### Function: `fill_doors(t_game *game)`

Allocate and populate door array.

**Logic:**
- Loop through grid.
- For each 'D': validate position, store in game->door[].

### Function: `check_doors(t_game *game)`

Master door validation.

**Logic:**
1. Count doors.
2. Allocate door array if count > 0.
3. Fill doors array.
4. Validate all door positions.
