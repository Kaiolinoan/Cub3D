# Utility Functions — Function Breakdown

## utils.c

### Function: `print_error(char *msg)`

Print error message to stderr with red "Error" header.

**Logic:**
- Format: `\033[1;31mError\033[0m\n<message>\n` (ANSI color codes).
- Use `ft_dprintf()` to stderr (fd 2).

### Function: `array_len(char **array)`

Return length of null-terminated string array.

**Logic:**
- Count elements until NULL terminator.

### Function: `remove_new_line_in_array(char **arr)`

Strip `\n` from all strings in array.

**Logic:**
- For each string in array: find and replace `\n` with `\0`.

### Function: `remove_new_line(char *str)`

Strip `\n` from a single string.

**Logic:**
- Find first `\n` and replace with `\0`.

### Function: `ft_abs(double num)`

Return absolute value of double.

**Logic:**
- If negative, multiply by -1.

---

## clean_utils.c

### Function: `clear_images(t_game *game)`

Free all image buffers and textures via MLX.

**Logic:**
- Destroy backbuffer, wall textures, square textures, door frames.
- Call `clear_door()` to free door frame array.

### Function: `free_sprites_path(t_game *game)`

Free all texture path strings.

**Logic:**
- Free all sprite texture paths (NO, SO, WE, EA, squares, etc.).

### Function: `clear_game(t_game *game)`

Master cleanup function. Free all game resources.

**Logic:**
1. Clear images.
2. Free sprite paths.
3. Free map data (floor, ceiling colors, grid).
4. Free door array.
5. Destroy window and MLX context.
6. Free game struct.

### Function: `clear_matriz(char **matriz)`

Free a null-terminated 2D string array.

**Logic:**
- Free each row.
- Free array pointer.

---

## door_utils.c

### Function: `clear_door_frames(void *mlx, t_texture *frames, int count)`

Free door animation frame array.

**Logic:**
- For each frame: destroy image and free path.
- Free frame array.

### Function: `clear_door(t_game *game)`

Free door animation frames from game context.

### Function: `get_door_at(t_game *game, int x, int y)`

Find door at grid position (x, y).

**Logic:**
- Loop through door array.
- Return pointer if door found at position, NULL otherwise.

### Function: `get_char_at(char **grid, int y, int x)`

Safely read grid character with bounds checking.

**Logic:**
- Return `' '` (space) if out of bounds.
- Otherwise return character at grid[y][x].

### Function: `is_walkable(char c)`

Check if character represents walkable floor.

**Logic:**
- Return true for: `'0'`, `'N'`, `'S'`, `'E'`, `'W'`.

---

## init_utils.c

### Function: `set_image_path(t_texture *tex, char *path)`

Allocate and store texture file path.

**Logic:**
- `ft_strdup(path)` to allocate and copy.
- Store in tex->path.

### Function: `file_to_image(void *mlx, t_texture *dir)`

Load XPM file to MLX image.

**Logic:**
- Call `mlx_xpm_file_to_image()` with file path.
- Store image pointer in dir->img.img.

### Function: `get_dir_img_address(t_texture *dir)`

Get pixel buffer pointer from MLX image.

**Logic:**
- Call `mlx_get_data_addr()`.
- Store buffer address in dir->img.addr.

---

## parse_utils.c

### Function: `check_full_line(char *full_line)`

Check if accumulated line is not empty.

**Logic:**
- Ensure at least one non-newline character.

### Function: `check_empty_line_on_map(char *full_line)`

Check map has no internal empty lines.

**Logic:**
- Track if content found.
- If two consecutive newlines after content starts: error.

### Function: `ft_strjoin_and_free(char *s1, char *s2)`

Join two strings and free first one.

**Logic:**
- Allocate combined string.
- Copy s1 then s2.
- Free s1.
- Return combined.

### Function: `ft_strcmp(char *s1, char *s2)`

Compare two strings.

**Logic:**
- Standard string comparison (like libc strcmp).
- Return 0 if equal.

### Function: `check_extension(char *filename, char *extension)`

Check if filename has correct extension.

**Logic:**
- Find last dot in filename.
- Compare extension after dot.

---

## text_render_utils.c

### Function: `format_speed(double value, int multiplier)` (static)

Format floating-point speed value as string.

**Logic:**
1. Convert integer part to string.
2. Convert fractional part (* multiplier) to string.
3. Join with ".".

**Example:** 0.07 with multiplier 100 → "0.7"

### Function: `display_speeds(t_game *game)`

Render movement and rotation speed on screen.

**Logic:**
1. Format both speeds.
2. Prepend labels.
3. Draw text at position (win_w - 550, y).
4. Call `draw_string_scaled()` with scale 3.

---

## text_render_glyphs.c

### Function: `get_pattern_value_low(int index, int row)` (static)

Get bitmap pattern for digit/letter (0-9, A-Z, a-z).

**Logic:**
- Static array of 18 patterns (0-9, A-H).
- Each pattern is 5 rows of 4-bit values (representing 4×5 glyph).

### Function: `get_pattern_value_high(int index, int row)` (static)

Get bitmap pattern for letters I-Z, a-z.

### Function: `get_pattern_value(int index, int row)`

Get pattern for any character (delegates to low/high).

### Function: `get_digit_pattern(char digit, int row)`

Get bitmap row for a character.

**Logic:**
1. Map character to index (0-35).
2. Get pattern value.
3. Return row of bitmap.

---

## text_render.c

### Function: `put_scaled_block(t_game *game, t_ip pos, int scale, int color)` (static)

Draw a scaled pixel block (scale×scale square).

**Logic:**
- Double loop drawing scale×scale pixels.

### Function: `draw_glyph_layer(t_game *game, char c, t_ip pos, t_glyph_style style)` (static)

Draw one layer of a character (foreground or shadow).

**Logic:**
- For each row and column in glyph: check bitmap bit.
- If bit set, draw scaled block with given color.

### Function: `draw_char_scaled(t_game *game, char c, t_ip pos, int scale)` (static)

Draw character with shadow (black background, white foreground).

**Logic:**
1. Draw black shadow layer offset (1,1).
2. Draw white foreground layer at original position.

### Function: `draw_punct_char(t_game *game, char c, t_ip pos, int scale)` (static)

Draw special characters (:, ., space).

**Logic:**
- ':' = two dots.
- '.' = single dot.
- ' ' = empty (space).

### Function: `draw_string_scaled(t_game *game, char *str, t_ip pos, int scale)`

Draw an entire string scaled.

**Logic:**
1. Track horizontal offset (x position).
2. For each character:
   - If alphanumeric: draw with shadow via `draw_char_scaled()`.
   - If special: draw via `draw_punct_char()`.
   - Advance offset by character width.
