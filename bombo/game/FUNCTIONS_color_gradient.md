# color_gradient.c — Function Breakdown

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

**Purpose**: Calculate a brightness multiplier (0.0–1.3) based on wall distance for distance-based fog effect.

**Parameters:**
- `ray`: Pointer to the current ray struct containing perpendicular distance to wall hit.

**Return Value:**
- A `double` brightness factor in range `[0.0, 1.3]`:
  - `0.0` = fully black (wall beyond 12 units).
  - `1.0` = normal brightness.
  - `>1.0` = overbright (capped at 1.3x for close-up walls).

**Variables:**
- `max_dist`: Hardcoded fog cutoff distance (12.0 world units). Walls farther than this appear fully black.
- `ray->perp_wall_dist`: Perpendicular distance to wall (from `calculate_wall()` in `raycasting2.c`). Avoids fish-eye distortion.
- `factor`: Computed brightness multiplier.

**Logic:**
1. If wall distance ≥ `max_dist`, return 0 (fully black fog). *(Note: Returns `0x000000` which is a hex int literal, evaluated as 0.0 in double context—unusual but works.)*
2. Compute `factor` as a product:
   - Inverse falloff: `1.5 / (1 + dist * 0.3)` — brightness drops off smoothly with distance.
   - Linear fog: `1 - (dist / max_dist)` — linear fade to black over 12 units.
3. Clamp result to `[0.0, 1.3]`:
   - Allow slight overbright effect close up (up to 1.3x).
   - Ensure no negative values.

**Example:**
- At distance 0: `factor ≈ 1.3` (bright near-wall).
- At distance 6: `factor ≈ 0.6` (medium darkness).
- At distance 12+: `factor = 0.0` (fog cutoff).

---

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

**Purpose**: Apply distance-based shading and side-based darkening to a wall texture pixel color.

**Parameters:**
- `ray`: Ray struct containing wall distance, side orientation.
- `color`: RGB color as packed int `0x00RRGGBB`.

**Return Value:**
- Shaded color as packed int `0x00RRGGBB`.

**Variables:**
- `factor`: Brightness multiplier from `get_factor()`.
- `r`, `g`, `b`: Individual color channels extracted and darkened.

**Logic:**
1. Get brightness factor for this distance via `get_factor()`.
2. Extract RGB channels:
   - `r = (color >> 16) & 0xFF` — red (top 8 bits).
   - `g = (color >> 8) & 0xFF` — green (middle 8 bits).
   - `b = color & 0xFF` — blue (low 8 bits).
3. Darken by multiplying each channel by the factor.
4. Clamp each channel to max 255 (after factor multiplication may exceed 255 if factor > 1.0).
5. Repack into `0x00RRGGBB` format.
6. **Side darkening**: If `ray->side == 1` (horizontal wall, ray hit N/S side):
   - Right-shift color by 1 and mask with `0x7F7F7F` to halve brightness.
   - This makes N/S walls appear darker than E/W walls (creates visual relief).
7. Return the shaded color.

**Example:**
- Input color: `0xFF0000` (pure red), distance 0, side 0 (E/W):
  - Factor ≈ 1.3, result ≈ `0xFF0000` (clamped, still red).
- Input color: `0xFF0000`, distance 0, side 1 (N/S):
  - Factor ≈ 1.3 → `0xFF0000`, then darkened → ≈ `0x7F0000` (dark red).
- Input color: `0xFF0000`, distance 12, side 0:
  - Factor = 0 → `0x000000` (black).

**Call Chain:**
- Used by `draw_textured_line()` in `raycasting_draw.c` to shade each pixel of wall slices.
