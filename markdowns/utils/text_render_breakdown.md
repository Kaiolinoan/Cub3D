# Text Rendering — Glyph Patterns & Function Breakdown

This is the system that draws the "Movement Speed: X.XX" / "Rotation Speed: X.XXX" HUD text in the corner of the screen — entirely by hand, with no font library, no MLX text function, nothing. Every letter and digit is drawn pixel-by-pixel from a hardcoded pattern baked directly into the source code.

---

## Part 1 — What is a "glyph pattern"?

**Glyph** is just the technical word for "the little picture of one character" — the specific shape that makes an `'A'` look like an `'A'` rather than a `'B'`. A **glyph pattern**, in this code, is that shape stored as a tiny grid of on/off pixels, hardcoded as plain numbers.

### The grid

Every character in this font is drawn on a **4-pixels-wide, 5-pixels-tall** grid. Picture a piece of graph paper with 4 columns and 5 rows, where you color in some squares and leave others blank to spell out a shape:

```
col:  0 1 2 3
row0: ▢ ■ ■ ▢     ← top of the letter
row1: ■ ▢ ▢ ■
row2: ■ ■ ■ ■     ← the crossbar
row3: ■ ▢ ▢ ■
row4: ■ ▢ ▢ ■     ← bottom of the letter
```
(That's roughly what an `'A'` looks like on this 4×5 grid — colored squares `■` = pixel on, blank `▢` = pixel off.)

### How one row gets stored as a single number

Each row has exactly 4 squares, and each square is either "on" or "off" — that's exactly what a 4-bit binary number can represent, where each bit is one square:

```
row0:  ▢ ■ ■ ▢   →   binary 0110   →   written in code as 0b0110
```

Reading left to right, `0110` means "square 0 off, square 1 on, square 2 on, square 3 off" — which is exactly the top row of the 'A' above. The `0b` prefix in C just means "the number that follows is written in binary, not decimal."

This is exactly what you're looking at in `text_render_glyphs.c`:
```c
{0b0111, 0b1001, 0b1001, 0b1001, 0b0111},   // this whole line is ONE character's glyph
```
Five numbers, one per row, four bits each — one complete 4×5 character drawing, hardcoded as five tiny binary numbers sitting in a row.

### Why split into two arrays (`_low` and `_high`)?

There are 36 total characters supported (digits `0`-`9` = 10, letters `A`-`Z` = 26). Rather than one giant 36-row table, it's split into two 18-row tables (`get_pattern_value_low` for characters 0–17, `get_pattern_value_high` for characters 18–35) — almost certainly just to keep each individual function shorter (a common pattern in this codebase, e.g. `check_raydir_x`/`check_raydir_y` being separate functions for the same reason). `get_pattern_value` is the traffic cop that picks which of the two tables to look in.

### Mapping a character to a row index

```c
if (digit >= '0' && digit <= '9')
    index = digit - '0';                    // '0'→0, '1'→1, ... '9'→9
else if (digit >= 'A' && digit <= 'Z')
    index = 10 + (digit - 'A');              // 'A'→10, 'B'→11, ... 'Z'→35
else
    index = 10 + (digit - 'a');              // lowercase reuses the SAME patterns as uppercase
```
**Real-life version**: this is exactly like looking up a word in a dictionary by counting how many letters past `'A'` your letter is, then adding that offset to a fixed starting point. `digit - '0'` or `digit - 'A'` works because characters in C are secretly just numbers under the hood (`'A'` is `65`, `'B'` is `66`, and so on, in ASCII) — subtracting two characters gives you "how many letters apart are they," which becomes your array index.

Notice **lowercase letters get the exact same patterns as uppercase** — there's no separate lowercase glyph set, `'a'` through `'z'` just reuse `'A'`-`'Z'`'s pictures via the same `10 + offset` formula. So this HUD text always looks like uppercase, no matter what case you actually pass in.

### Reading one bit back out (which pixel is on?)

Given a 4-bit row pattern like `0b1001`, how do you check "is column 2 on"? This is `draw_glyph_layer`'s job:
```c
if ((pattern >> (3 - col)) & 1)
```
**Real-life version**: imagine the 4-bit number as 4 light switches lined up in a row: `[bit3][bit2][bit1][bit0]`. To check just one switch, you slide the whole row over (`>>`, shift right) until the switch you care about lands in the very last position, then you mask off everything except that last position (`& 1`) to read just that one switch's on/off state.

Why `3 - col` instead of just `col`? Because column `0` (the *leftmost* pixel on screen) corresponds to *bit 3* (the *leftmost* digit as written in `0b1001`), and column `3` (rightmost pixel) corresponds to *bit 0* (rightmost digit as written). Reading a binary literal left-to-right in the source code naturally matches reading pixels left-to-right on screen, and `3 - col` is exactly the flip needed to make those two "left-to-right"s line up correctly.

---

## Part 2 — Function-by-function breakdown

### `text_render_glyphs.c`

#### `get_pattern_value_low(int index, int row)` (static)
A hardcoded lookup table (`p[18][5]`) holding the glyph patterns for characters `0`-`9` and `A`-`H` (indices `0`-`17`). Just returns `p[index][row]` — the raw 4-bit pattern for that character's given row. `static const` means this table is built once and lives for the whole program, never modified.

#### `get_pattern_value_high(int index, int row)` (static)
Same idea, a second `p[18][5]` table, holding characters `I`-`Z` (indices `0`-`17` *within this table*, but representing overall indices `18`-`35`).

#### `get_pattern_value(int index, int row)`
```c
int	get_pattern_value(int index, int row)
{
	if (index < 18)
		return (get_pattern_value_low(index, row));
	return (get_pattern_value_high(index - 18, row));
}
```
The dispatcher: if the overall index is below `18`, look in the low table directly. Otherwise, look in the high table, but first subtract `18` (`index - 18`) since the high table's *own* indices only go `0`-`17`, even though it represents the *overall* range `18`-`35`.

#### `get_digit_pattern(char digit, int row)`
The public entry point. Takes a raw character (e.g. `'S'`) and a row number (`0`-`4`), and returns that row's 4-bit pattern.
1. First checks the character is actually a digit or letter (rejects punctuation, symbols, etc. — those are handled completely separately, see `draw_punct_char` below).
2. Converts the character into an index `0`-`35` (as described in Part 1).
3. Double-checks the index and row are both in valid range (`0-35` and `0-4`) before looking anything up — a safety guard against, e.g., accidentally being asked for row `7` of a 5-row glyph.
4. Returns `0` (meaning "empty row, draw nothing") for anything unrecognized — this is what makes unsupported characters silently invisible rather than crashing or drawing garbage.

---

### `text_render.c`

#### `put_scaled_block(t_game *game, t_ip pos, int scale, int color)` (static)
```c
static void	put_scaled_block(t_game *game, t_ip pos, int scale, int color)
{
	int	sx;
	int	sy;

	sy = 0;
	while (sy < scale)
	{
		sx = 0;
		while (sx < scale)
		{
			if (pos.x + sx >= 0 && pos.y + sy >= 0
				&& pos.x + sx < game->win_w && pos.y + sy < game->win_h)
				my_pixel_put(game, pos.x + sx, pos.y + sy, color);
			sx++;
		}
		sy++;
	}
}
```
**Purpose**: draw one solid `scale × scale` pixel square. Since the font's "native resolution" is only 4×5 tiny squares, and that would be microscopic on a real monitor, every single one of those tiny squares gets blown up into a `scale × scale` block of real screen pixels (this project calls `draw_string_scaled` with `scale = 3`, so every "font pixel" becomes an actual 3×3 block on screen).

**Real-life version**: like a mosaic — instead of one tiny tile per "pixel" of the picture, you use a 3×3 cluster of identical tiles, so the whole mosaic ends up 3 times bigger in both directions but still shows the same picture, just chunkier.

Also does its own manual bounds-checking per pixel (`pos.x + sx >= 0`, etc.) before writing — belt-and-suspenders, since `my_pixel_put` already bounds-checks internally too, but doesn't hurt to be safe here.

#### `draw_glyph_layer(t_game *game, char c, t_ip pos, t_glyph_style style)` (static)
```c
static void	draw_glyph_layer(t_game *game, char c, t_ip pos,
	t_glyph_style style)
{
	int		row;
	int		col;
	int		pattern;
	t_ip	block_pos;

	row = 0;
	while (row < 5)
	{
		pattern = get_digit_pattern(c, row);
		col = 0;
		while (col < 4)
		{
			if ((pattern >> (3 - col)) & 1)
			{
				block_pos.x = pos.x + col * style.scale;
				block_pos.y = pos.y + row * style.scale;
				put_scaled_block(game, block_pos, style.scale, style.color);
			}
			col++;
		}
		row++;
	}
}
```
**Purpose**: draw one full character's glyph, in one solid color, at one screen position. This is the function that actually walks the 4×5 grid described in Part 1.

**Variables:**
- `c`: the character to draw.
- `pos`: top-left screen position to start drawing at.
- `style` (`t_glyph_style`): bundles `scale` (block size) and `color` together — used because this function gets called *twice per character* with two different styles (see `draw_char_scaled` next).
- `row, col`: walk the 4×5 grid, `0`-`4` and `0`-`3`.
- `pattern`: this row's 4-bit on/off pattern, fetched fresh at the start of each row via `get_digit_pattern`.

**Logic**: For each of the 5 rows, fetch that row's pattern. For each of the 4 columns in that row, check if the corresponding bit is set (Part 1's `(pattern >> (3 - col)) & 1)` trick). If it is, work out where on screen that "font pixel" belongs — `col * scale` pixels right, `row * scale` pixels down from the character's top-left corner — and draw a `scale × scale` block there via `put_scaled_block`.

#### `draw_char_scaled(t_game *game, char c, t_ip pos, int scale)` (static)
```c
static void	draw_char_scaled(t_game *game, char c, t_ip pos, int scale)
{
	t_glyph_style	style;
	t_ip			offset_pos;

	style.scale = scale;
	style.color = 0x000000;
	draw_glyph_layer(game, c, pos, style);
	offset_pos.x = pos.x + 1;
	offset_pos.y = pos.y + 1;
	style.color = 0xFFFFFF;
	draw_glyph_layer(game, c, offset_pos, style);
}
```
**Purpose**: draw one character with a simple drop-shadow/outline effect, so the text stays readable no matter what's behind it (map, walls, minimap — anything could be showing through in the HUD corner).

**Logic**: draws the *entire glyph twice*:
1. First pass: solid **black** (`0x000000`), at the exact requested position.
2. Second pass: solid **white** (`0xFFFFFF`), at the position shifted `1` pixel right and `1` pixel down.

**Real-life version**: like writing a word in black marker, then writing the exact same word in white marker slightly up-and-to-the-left of the first one, directly on top. Since the white layer is drawn *second* (so it ends up on top) but *offset*, the black layer only remains visible peeking out along the bottom-right edge of each white shape — creating a thin black outline/shadow on two sides of every letter, which is what keeps white text from disappearing into a light-colored background.

#### `draw_punct_char(t_game *game, char c, t_ip pos, int scale)` (static)
```c
static int	draw_punct_char(t_game *game, char c, t_ip pos, int scale)
{
	if (c == ':')
	{
		my_pixel_put(game, pos.x, pos.y + scale, 0xFFFFFF);
		my_pixel_put(game, pos.x, pos.y + 3 * scale, 0xFFFFFF);
		return (2 * scale);
	}
	if (c == '.')
	{
		my_pixel_put(game, pos.x, pos.y + 4 * scale, 0xFFFFFF);
		return (2 * scale);
	}
	if (c == ' ')
		return (2 * scale);
	return (0);
}
```
**Purpose**: handle the handful of non-alphanumeric characters this font supports, since they don't fit the 4×5 letter/digit grid system at all — they're drawn as one-off special cases, single raw pixels rather than full glyphs.

**Logic:**
- **`:`** (colon) — draws two single white pixels, vertically spaced (`y + scale` and `y + 3*scale`), forming the two dots of a colon. No black outline layer here (unlike letters/digits) — just plain white pixels.
- **`.`** (period) — one single white pixel near the bottom of the character cell (`y + 4*scale`).
- **` `** (space) — draws nothing at all, just reports a width to advance by.
- **Anything else** (truly unsupported symbols) — draws nothing *and* reports `0` width, meaning the next character would be drawn at the exact same spot, overlapping — an edge case that only matters if a caller ever passes a character outside `0-9`, `A-Z`, `a-z`, `:`, `.`, or space; this codebase's actual usage (`display_speeds`) never does.

**Return value**: every branch returns "how many pixels wide this character was," which `draw_string_scaled` uses to know how far to move before drawing the *next* character.

#### `draw_string_scaled(t_game *game, char *str, t_ip pos, int scale)`
```c
void	draw_string_scaled(t_game *game, char *str, t_ip pos, int scale)
{
	int		i;
	int		offset;
	t_ip	char_pos;

	i = 0;
	offset = 0;
	while (str[i])
	{
		char_pos.x = pos.x + offset;
		char_pos.y = pos.y;
		if ((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'A'
				&& str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z'))
		{
			draw_char_scaled(game, str[i], char_pos, scale);
			offset += (4 + 1) * scale;
		}
		else
			offset += draw_punct_char(game, str[i], char_pos, scale);
		i++;
	}
}
```
**Purpose**: the public entry point — draw an entire string, one character at a time, left to right, each one positioned correctly after the last.

**Variables:**
- `offset`: running horizontal distance (in pixels) from the string's starting position — grows after every character.
- `char_pos`: this specific character's top-left drawing position (`pos.x + offset`, `pos.y` — always the same row, `y` never changes, since this is single-line text only).

**Logic**: walk the string character by character.
- If it's a letter or digit: draw the full black+white glyph (`draw_char_scaled`), then advance `offset` by `(4 + 1) * scale` — the glyph's own `4`-square width, plus `1` extra square of blank spacing between characters, all scaled up together. (Real-life version: like typing on a typewriter — one character-width, plus a little breathing room, before the next key strikes.)
- Otherwise (punctuation/space/anything else): delegate to `draw_punct_char`, which both draws *and* returns however many pixels to advance by.

---

### `text_render_utils.c`

#### `format_speed(double value, int multiplier)` (static)
```c
static char	*format_speed(double value, int multiplier)
{
	char	*int_str;
	char	*frac_str;
	char	*tmp;

	int_str = ft_itoa((int)value);
	frac_str = ft_itoa((int)((value - (int)value) * multiplier));
	tmp = ft_strjoin_and_free(int_str, ".");
	tmp = ft_strjoin_and_free(tmp, frac_str);
	free(frac_str);
	return (tmp);
}
```
**Purpose**: manually format a `double` like `0.07` into a display string like `"0.7"` — since this hand-rolled font/string system has no equivalent of `printf("%.2f", ...)`, the decimal formatting has to be built by hand out of integer conversions.

**Variables:**
- `value`: the raw speed value (e.g. `game->movement_speed = 0.07`).
- `multiplier`: how many decimal places to keep — `100` gives 2 digits (used for movement speed), `1000` gives 3 digits (used for rotation speed).
- `int_str`: the whole-number part, e.g. `(int)0.07 = 0`, turned into `"0"`.
- `frac_str`: the fractional part, isolated by `value - (int)value` (e.g. `0.07 - 0 = 0.07`), then scaled up by `multiplier` and truncated to an int (e.g. `0.07 * 100 = 7.0` → `(int)7.0 = 7`), turned into `"7"`.

**Logic**: build `int_str` and `frac_str` as separate strings, then glue them together with a literal `"."` in between: `"0" + "." + "7"` → `"0.7"`.

> **Formatting quirk worth knowing**: this doesn't zero-pad the fractional part. If the true fractional value were, say, `0.05` with `multiplier = 100`, `(int)(0.05 * 100) = 5`, and `ft_itoa(5)` gives `"5"` — not `"05"` — so the final string would read `"0.5"` instead of the "intended" `"0.05"`. This can make the displayed number look 10× bigger than it actually is for fractional values with a leading zero. Not a crash or functional bug, just a display quirk worth being aware of if the on-screen speed numbers ever look surprising.

#### `display_speeds(t_game *game)`
```c
void	display_speeds(t_game *game)
{
	char	*speed_str;
	char	*rotation_str;
	char	*full_speed;
	char	*full_rotation;
	t_ip	pos;

	speed_str = format_speed(game->movement_speed, 100);
	rotation_str = format_speed(game->rotation_speed, 1000);
	full_speed = ft_strjoin_and_free(ft_strdup("Movement Speed:"), speed_str);
	full_rotation = ft_strjoin_and_free(ft_strdup("Rotation Speed:"),
			rotation_str);
	free(speed_str);
	free(rotation_str);
	pos.x = game->win_w - 550;
	pos.y = 20;
	draw_string_scaled(game, full_speed, pos, 3);
	pos.y = 50;
	draw_string_scaled(game, full_rotation, pos, 3);
	free(full_speed);
	free(full_rotation);
}
```
**Purpose**: the actual HUD entry point, called every frame from `render()` (`render.c`), in both debug mode and normal 3D mode. Builds the two label strings and draws them in the top-right corner.

**Logic:**
1. Format both speed values into decimal strings (2 decimal places for movement, 3 for rotation — matching the tuning granularity of the `1`/`2`/`Up`/`Down` speed-adjustment keys in `keys_handle.c`, which step by `0.01` and `0.005` respectively).
2. Prefix each with a label string (`"Movement Speed:"` / `"Rotation Speed:"`) via string joining.
3. Free the now-redundant intermediate strings.
4. Position the first line at `x = win_w - 550, y = 20` — anchored `550` pixels in from the *right* edge of the window (so it stays in the top-right corner regardless of window size), `20` pixels down from the top.
5. Draw it at `scale = 3` (so every native "font pixel" becomes a 3×3 block on screen — a glyph ends up `4×3 = 12` pixels wide, `5×3 = 15` pixels tall).
6. Move down to `y = 50` for the second line (a `30`-pixel gap between the two lines).
7. Draw the rotation speed line the same way.
8. Free the final built strings.

---

## Part 3 — The whole pipeline, top to bottom

```
render()                                    (render.c, every frame)
 └─ display_speeds(game)                    (text_render_utils.c)
     ├─ format_speed(...)  →  "0.7", "0.05" style strings
     ├─ build "Movement Speed:0.7" etc.
     └─ draw_string_scaled(game, str, pos, 3)      (text_render.c)
         └─ for each character in the string:
             ├─ if letter/digit → draw_char_scaled(...)
             │    └─ draw_glyph_layer(...) × 2   (black layer, then offset white layer)
             │         └─ for each of 5 rows:
             │             ├─ get_digit_pattern(char, row)   (text_render_glyphs.c)
             │             │    └─ get_pattern_value(index, row)
             │             │         └─ looks up the hardcoded 4-bit pattern
             │             └─ for each of 4 columns:
             │                 └─ if that bit is set → put_scaled_block(...)
             │                      └─ my_pixel_put(...) for every pixel in the scaled block
             └─ else (punctuation/space) → draw_punct_char(...)
```

**In one sentence**: every character is a hand-drawn 4×5 grid of on/off bits baked directly into the source code as binary literals, read out bit-by-bit, blown up into chunky squares, drawn twice (black-then-offset-white) for a cheap outline effect, and laid out left to right with fixed-width spacing — no font files, no MLX text API, just raw bit-checking and pixel-writing.