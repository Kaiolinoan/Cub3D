# Cub3D — Math & Concepts Explained (No Prior Knowledge Assumed)

This is a "why does this math work" companion to the function breakdowns. Every concept below is something the codebase actually uses, explained from the ground up, with the real numbers/constants from your project plugged into the examples.

**Table of contents**
1. [Vectors — the one idea everything else builds on](#1-vectors)
2. [Direction vector: where the player is looking](#2-direction-vector)
3. [The camera plane: what it is and why it controls FOV](#3-camera-plane)
4. [`camera_x`: turning a screen column into a ray](#4-camera_x)
5. [Trigonometry refresher: sin, cos, radians](#5-trig)
6. [Rotation matrices: how `rotate()` actually works](#6-rotation)
7. [DDA: the grid-marching algorithm behind raycasting](#7-dda)
8. [The fish-eye problem and perpendicular distance](#8-fisheye)
9. [Turning distance into wall height on screen](#9-projection)
10. [Texture mapping: picking which pixel of the image to show](#10-texture-mapping)
11. [Color packing and bit manipulation (hex colors)](#11-bit-packing)
12. [Distance formulas and "is this pixel inside a circle?"](#12-distance-circle)
13. [Linear interpolation (lerp)](#13-lerp)
14. [Line drawing without a library (the FOV lines on the minimap)](#14-line-drawing)
15. [Floating-point housekeeping: `floor()`, fractional parts, power-of-two wraparound](#15-float-tricks)
16. [Bonus: `fork`/`exec`/signals for the door sound](#16-fork-exec)

---

## 1. Vectors

A **vector** here is just a pair of numbers `(x, y)` that represents an *arrow* — it has a direction and a length. In this codebase, every vector is stored as two separate `double` fields (e.g. `dir_x` and `dir_y`), never as a struct with a `.x`/`.y` — but conceptually they're always paired.

Two things you do with vectors constantly in this code:

- **Add them**: `(a.x + b.x, a.y + b.y)` — combines two arrows into one (used when combining direction + camera plane to get a ray).
- **Scale them**: `(a.x * k, a.y * k)` — stretches or shrinks the arrow without changing its direction (if `k` is positive) or flips it (if `k` is negative).

**Length (magnitude)** of a vector `(x, y)` is `sqrt(x² + y²)` — this is just the Pythagorean theorem, treating the vector as the hypotenuse of a right triangle with legs `x` and `y`.

```
        (x, y)
         /|
        / |
  len  /  | y
      /   |
     /____|
       x
```

A vector is **normalized** (or "unit length") when its length is exactly `1`. You normalize a vector by dividing both components by its current length:

```c
len = sqrt(x*x + y*y);
x = x / len;
y = y / len;
```

You'll see this exact pattern in `draw_single_fov_ray` (`minimap_fov.c`) — it computes a direction, measures its length, and divides by that length to get a "pure direction, no magnitude" vector before using it.

---

## 2. Direction vector

`player->dir_x`, `player->dir_y` together form the **direction vector**: a unit-length arrow pointing exactly where the player is facing. If the player faces north, `dir = (0, -1)` (in this project's coordinate system, negative Y is "up"/north, since Y increases downward — same as screen/array coordinates, row 0 is the top).

| Facing | `dir_x` | `dir_y` |
|---|---|---|
| North | 0 | -1 |
| South | 0 | 1 |
| East | 1 | 0 |
| West | -1 | 0 |

This is set in `initialize_player` (`player.c`) based on the spawn character in the map, and updated every frame by `rotate()` (`movement.c`) whenever the player turns.

---

## 3. Camera plane

This is the concept your `rotate` question was really about. Here's the intuition first, math second.

**Intuition**: Imagine the player is standing behind a movie camera, looking through the lens in the `dir` direction. The **camera plane** is a short, invisible line segment centered on the player, held up *perpendicular* to where they're looking — like a ruler held sideways across the camera lens. This ruler represents the width of what the camera can "see" — the field of view. Rays are cast not just straight ahead, but fanned out across the full width of that ruler, one ray per screen column.

```
                    plane vector (perpendicular to dir)
        left edge <──────●──────> right edge
                          │
                          │ dir (facing direction)
                          │
                          ▼
                       player
```

**Why perpendicular?** Because the screen is a flat rectangle held up in front of the camera — like a window. The rays that go through the left edge of that window and the right edge of that window are the "widest" rays the camera can see, and the line connecting where those two edges are is, geometrically, perpendicular to the direction you're looking (just like your monitor screen is perpendicular to your straight-ahead gaze when you sit facing it directly).

**In code**, the camera plane is `player->plane_x`, `player->plane_y` — a second vector, always kept perpendicular to `dir`. You get a perpendicular vector in 2D by swapping the x/y components and flipping the sign of one of them: if `dir = (dx, dy)`, a perpendicular vector is `(-dy, dx)` or `(dy, -dx)`.

Looking at `initialize_player`:
```c
// facing NORTH: dir = (0, -1)
set_player_plane_values(player, game->fov, 0);   // plane = (fov, 0)
```
Here `dir = (0, -1)` and `plane = (fov, 0)` — indeed, swap-and-negate: `(-dy, dx) = (-(-1), 0) = (1, 0)`, scaled by `fov`. So the plane is always built as a 90°-rotated, rescaled copy of `dir`.

**Why does the plane's *length* control field of view?** Because it controls how wide that "ruler" is relative to how far away it is (the "1 unit" distance implied by `dir` being unit-length). A longer plane vector = a wider ruler = you can see more sideways = wider FOV, at the cost of more distortion at the edges (like a wide-angle camera lens). This project uses `game->fov = 0.66`, which is the classic value from the original Wolfenstein-3D-style tutorials, corresponding to roughly a 66° horizontal field of view. Try the math: `2 * atan(0.66) ≈ 66°`.

---

## 4. `camera_x`

Every screen column needs its *own* ray, fanning out smoothly from the left edge of the camera plane to the right edge. `camera_x` is how each column picks its position along that ruler.

```c
ray->camera_x = 2 * x / (double)game->win_w - 1;
```

Here `x` is the current screen column (`0` to `win_w - 1`). This formula rescales `x` from the range `[0, win_w]` into the range `[-1, +1]`:

- Column `0` (far left of screen) → `camera_x = -1`
- Column `win_w / 2` (middle of screen) → `camera_x = 0`
- Column `win_w - 1` (far right) → `camera_x ≈ +1`

Then the ray direction for that column is:

```c
ray_dir_x = dir_x + plane_x * camera_x;
ray_dir_y = dir_y + plane_y * camera_x;
```

Read this as: *"start at the straight-ahead direction, then nudge sideways by some fraction of the camera plane."* At `camera_x = 0` (screen center), you get exactly `dir` (straight ahead, no sideways nudge). At `camera_x = -1` (left edge), you get `dir - plane` — as far left as the camera plane reaches. At `camera_x = +1` (right edge), you get `dir + plane` — as far right as it reaches. Every column in between gets a proportional blend — this is genuinely the same linear interpolation idea covered in [section 13](#13-lerp), just applied to build ray directions instead of drawing lines.

This exact formula appears three times in your codebase: `init_ray` (`raycasting_init.c`, the real 3D raycaster), `print_fov` (`debug.c`, the debug visualization), and `init_fov_dirs` (`minimap_fov.c`, computing the two extreme rays for the minimap's FOV cone — it only needs the two edges, `camera_x = -1` and `+1`, since the minimap interpolates between them separately in `draw_single_fov_ray`).

---

## 5. Trigonometry refresher

You need exactly two functions: `sin` and `cos`. Given an angle `θ` (theta), measured in **radians** (not degrees — C's math library always uses radians):

- `cos(θ)` and `sin(θ)` are, respectively, the x and y coordinates of a point on a circle of radius 1, after rotating `θ` radians counter-clockwise from the "3 o'clock" position `(1, 0)`.

```
              (0,1)
                │
       cos(θ),  │
       sin(θ)   │
          \     │
           \    │
            \θ  │
     ────────●──┼──── (1,0)
             (0,0)
```

**Radians vs degrees**: A full circle is `2π` radians (≈ 6.283), which equals 360°. Half a circle (`π` radians, ≈3.1416) is 180°. A quarter circle (`π/2`, ≈1.5708) is 90°. If you ever need to convert: `radians = degrees * π / 180`.

**Why this matters here**: rotating a vector by an angle is done entirely with `sin`/`cos` — see the next section.

---

## 6. Rotation matrices: how `rotate()` works

```c
player->dir_x = old_dir_x * cos(rot) - old_dir_y * sin(rot);
player->dir_y = old_dir_x * sin(rot) + old_dir_y * cos(rot);
```

This is the standard **2D rotation formula**. If you have a point/vector `(x, y)` and you want to rotate it by angle `θ` around the origin `(0,0)`, the new point `(x', y')` is:

```
x' = x·cos(θ) − y·sin(θ)
y' = x·sin(θ) + y·cos(θ)
```

You don't need to derive this from scratch to use it, but here's the intuition: `cos(θ)` and `sin(θ)` tell you where the "unit x-axis direction" (`1, 0`) ends up after rotating by `θ` — it lands at `(cos θ, sin θ)`. Similarly the "unit y-axis direction" (`0, 1`) lands at `(-sin θ, cos θ)`. Any vector `(x, y)` can be thought of as "`x` copies of the x-axis direction, plus `y` copies of the y-axis direction" — so after rotation, it becomes "`x` copies of *where the x-axis direction went*, plus `y` copies of *where the y-axis direction went*," which is exactly the formula above.

**Why `rotate()` applies this to *two* vectors** (`dir` and `plane`): because turning the player means both "which way you're facing" and "which way your camera's sideways ruler points" need to rotate together, by the same angle, to stay perpendicular to each other. If you only rotated `dir` and left `plane` alone, your field of view would end up pointing in a stale direction relative to where you're now facing.

**Worked example** (matches the docstring you already had from `FUNCTIONS_movement.md`): player facing north, `dir = (0, -1)`, `plane = (0.66, 0)`. Rotate by `θ = π/2` (90°), where `cos(π/2) = 0`, `sin(π/2) = 1`:

```
new_dir_x = 0·0 − (−1)·1 = 1
new_dir_y = 0·1 + (−1)·0 = 0
→ new dir = (1, 0)   (now facing EAST)

new_plane_x = 0.66·0 − 0·1 = 0
new_plane_y = 0.66·1 + 0·0 = 0.66
→ new plane = (0, 0.66)
```

Both vectors turned together by exactly 90°, and they're still perpendicular to each other afterward (you can check: the dot product `dir_x*plane_x + dir_y*plane_y = 1*0 + 0*0.66 = 0` — a dot product of `0` is the standard test for "these two vectors are perpendicular").

**Why the code saves `old_dir_x/y` etc. first**: the formula for the new `x` needs the *old* `y`, and vice versa. If you computed `dir_x` first and overwrote `player->dir_x` immediately, then the formula for `dir_y` (which also needs `dir_x`) would accidentally use the *already-rotated* `x` instead of the original — giving a wrong, "double-rotated-ish" result. Snapshotting into local variables (`old_dir_x`, etc.) avoids this.

**Positive vs. negative angle**: a positive `rot` turns counter-clockwise in standard math convention. In `move_player` (`movement.c`), the left arrow key calls `rotate(player, -rotation_speed)` and the right arrow calls `rotate(player, +rotation_speed)` — so in this project's convention, negative = turn left (on screen), positive = turn right, which is just a byproduct of how the Y axis points downward in this coordinate system (like screen/array coordinates) rather than upward (like a standard math graph).

---

## 7. DDA: the grid-marching algorithm behind raycasting

**DDA** stands for **Digital Differential Analysis** (or "Analyzer"). It's an algorithm for walking along a straight line, one grid cell at a time, without ever computing decimal/fractional intersection points along the way except at the very start. This is *the* algorithm that makes real-time raycasters like this one fast — Wolfenstein 3D (1992) popularized exactly this technique.

**The problem it solves**: you have a ray starting at the player's (fractional!) position, heading off in some direction. You want to know: what's the *first* wall cell this ray hits, walking through a grid of square tiles?

**The naive-but-slow approach** would be to take tiny steps (like `debug.c`'s `print_rays` actually does, stepping by `0.05` units at a time!) and check the grid cell at each tiny step. That works, but it's slow and can skip over thin walls if your step size is too big.

**DDA's insight**: instead of tiny fixed steps, jump directly from grid-line to grid-line. A ray only ever needs to check a new cell when it crosses either a vertical grid line (moving from one column to the next) or a horizontal grid line (moving from one row to the next) — so DDA computes exactly how far away each of those *next* crossings is, and always jumps to whichever one is closer.

**The key variables** (from `check_raydir_x`/`check_raydir_y` in `raycasting.c`):

- **`delta_dist_x`**: "if I move along this ray, how far do I have to travel to cross exactly one full grid cell in the X direction?" This is a fixed value for the whole ray (it doesn't change as the ray travels), computed as `|1 / ray_dir_x|`. Why? Because `ray_dir_x` tells you how much X changes *per unit of travel* along the ray — so its reciprocal tells you how much travel is needed *per unit of X* — i.e., per one grid cell.
- **`side_dist_x`**: "starting from right now, how far do I have to travel along the ray to reach the *next* vertical grid line?" Unlike `delta_dist_x`, this one **does** change as the ray travels — every time the ray crosses a vertical line, this needs to jump forward by another `delta_dist_x` to point at the *next* one.
- **`step_x`**: just `+1` or `-1` — which direction (left or right) the ray is moving through grid columns.
- The same three exist for Y (`delta_dist_y`, `side_dist_y`, `step_y`).

**The main loop** (`cast_ray` in `raycasting.c`):

```c
if (ray->side_dist_x < ray->side_dist_y)
{
    // the next vertical grid line is CLOSER than the next horizontal one
    ray->side_dist_x += ray->delta_dist_x;   // schedule the *following* vertical line
    ray->map_x += ray->step_x;               // actually step into the next column
    ray->side = 0;                           // remember: we just crossed a vertical line
}
else
{
    // symmetric, for horizontal
    ray->side_dist_y += ray->delta_dist_y;
    ray->map_y += ray->step_y;
    ray->side = 1;
}
```

Every iteration, compare "how far to the next vertical line" vs "how far to the next horizontal line," and step into whichever is closer. This guarantees you visit every single grid cell the ray actually passes through, in the correct order, without ever missing one — and you never need to compute a square root or a fractional intersection point mid-loop, which is why it's fast enough to run once per screen column, every single frame.

```
Grid lines:        Ray path (DDA visits ●):
  |   |   |   |
──┼───┼───┼───┼──
  │   │ ● │   │
──┼───●───┼───┼──     Ray starts inside a cell, and DDA
  │ ●●│   │   │       jumps cell-to-cell along grid lines,
──┼───┼───┼───┼──     never missing one.
  │ P │   │   │   (P = player)
──┼───┼───┼───┼──
```

**`ray->side`** (0 or 1) is a byproduct that turns out to be extremely useful later: it tells you whether the wall you hit is a "vertical-line wall" (a wall running north-south, hit by crossing an X grid line — these get textured with the east/west sprites) or a "horizontal-line wall" (running east-west, hit by crossing a Y grid line — textured with north/south sprites), and it's also what drives the N/S-vs-E/W darkening in `color_gradient` (`side == 1` gets darkened).

---

## 8. The fish-eye problem and perpendicular distance

If you naively used the straight-line (Euclidean/Pythagorean) distance from the player to each wall hit to decide how tall to draw that column, you'd get a warped, "fish-eye lens" look — walls would appear to bulge outward near the edges of the screen, because rays cast toward the screen edges travel at an angle and therefore cover more real-world distance to reach a wall that's actually "the same distance away" as one seen dead ahead.

```
             screen
    ╲          │          ╱
     ╲         │         ╱
      ╲        │        ╱     <- if you used the actual ray length,
       ╲       │       ╱         edge rays would report LARGER distances
        ╲      │      ╱          than the center ray, even for a flat wall
         ╲_____|_____╱           straight ahead of the player — causing
               P                 a false "bulge/curve" effect.
```

**The fix**: instead of the true ray length, use the **perpendicular distance** — the distance measured straight out from the camera plane (i.e., "how far forward, ignoring how far sideways"), which is the same for every point along a flat wall directly ahead, regardless of which screen column's ray reached it.

**How the code computes it** (`calculate_wall` in `raycasting2.c`):

```c
if (ray->side == 0)
    ray->perp_wall_dist = ray->side_dist_x - ray->delta_dist_x;
else
    ray->perp_wall_dist = ray->side_dist_y - ray->delta_dist_y;
```

By the time `cast_ray`'s DDA loop finishes, `side_dist_x` (or `_y`) has already been advanced *one step past* the wall it actually hit (because the loop adds `delta_dist_x` *before* checking whether the new cell is a wall). Subtracting one `delta_dist_x` back off undoes that last "look-ahead" step, leaving exactly the distance to the grid line where the hit happened — and because this distance was computed by DDA using only the X-axis (or Y-axis) component of the ray's travel, not the full diagonal ray length, it's already "perpendicular-only," free of the fish-eye distortion.

---

## 9. Turning distance into wall height on screen

Once you know how far away a wall is (the fish-eye-corrected `perp_wall_dist`), you need to decide how tall to draw that column on screen. This uses the same idea as **perspective in real life**: things farther away look smaller.

```c
ray->line_height = (int)(game->win_h / ray->perp_wall_dist);
```

This is an **inverse relationship** — as distance doubles, height halves. Why divide, and why by `win_h` specifically? Picture a wall of a fixed real-world height (say "1 world unit tall," matching the width of one grid tile) placed at distance `d`. As `d` grows, the *angle* that wall subtends in your field of view shrinks roughly proportionally to `1/d` (this is the small-angle approximation of similar triangles — the same reason a person looks smaller the farther away they stand). Since screen height is a fixed number of pixels representing your full vertical field of view, scaling that fixed pixel budget by `1/d` gives you the on-screen pixel height. Using `win_h` (rather than some other constant) as the numerator is what calibrates things so a wall exactly `1` unit away fills the *entire* screen height — which lines up with the coordinate system where grid cells are `1` unit wide/tall and the player's `dir` vector is unit-length.

Then the wall slice is centered vertically on the screen:

```c
ray->draw_start = -ray->line_height / 2 + game->win_h / 2;   // top of the wall slice
ray->draw_end   =  ray->line_height / 2 + game->win_h / 2;   // bottom of the wall slice
```

`win_h / 2` is the screen's vertical center. Subtracting/adding half the wall's height from/to that center point gives you the top and bottom edges — same idea as centering any rectangle: `center - half_height` to `center + half_height`.

Both `draw_start` and `draw_end` get clamped to `[0, win_h - 1]` afterward, because for very close walls, `line_height` can be far taller than the actual screen — you don't want to try to write pixels above row 0 or below the last row.

---

## 10. Texture mapping: picking which pixel of the image to show

"Texture mapping" means: for a given point on a 3D wall, which pixel of the flat 2D image (the XPM sprite) should be shown there? You need to solve this in two independent directions — horizontal (which *column* of the texture) and vertical (which *row*).

### Horizontal (`tex_x`, in `find_wall_hit`, `raycasting2.c`)

First, figure out *where along the wall's face* (as a fraction from `0.0` to `1.0`) the ray actually struck:

```c
if (ray->side == 0)
    wall_x = game->player.player_y + ray->perp_wall_dist * ray->ray_dir_y;
else
    wall_x = game->player.player_x + ray->perp_wall_dist * ray->ray_dir_x;
wall_x -= floor(wall_x);
```

This projects the player's position forward along the ray by the perpendicular distance, giving the exact world coordinate of the hit point. Since walls are hit at integer grid-line boundaries in the perpendicular axis, the *other* axis (Y if you hit a vertical/X-side wall, X if you hit a horizontal/Y-side wall) tells you where along that wall's face you are — but it's still a big number like `7.83`; subtracting `floor(wall_x)` (its integer part) strips that down to just the "how far across *this one tile*" fraction, `0.83`.

Then scale that `0.0`–`1.0` fraction up to actual texture pixel coordinates: `tex_x = (int)(wall_x * PX)` where `PX` is the texture size (`64` in this project) — e.g. a fraction of `0.83` maps to texture column `53`.

Finally, a mirroring correction (`tex_x = PX - tex_x - 1` in certain side/direction combinations) — without this, textures would appear backwards (mirror-flipped) for walls approached from certain directions, because "wall_x increasing" doesn't consistently correspond to "moving right as the player sees it" across all four possible side/direction combinations.

### Vertical (`tex_y`, in `draw_textured_line`, `raycasting_draw.c`)

The vertical axis is different because it's about mapping the texture's fixed `64` pixels onto a wall slice whose on-screen height (`line_height`) varies every frame (near walls are tall, far walls are short) — this is a **resampling** problem.

```c
step = (double)PX / ray->line_height;
```

`step` answers: "for every 1 pixel I move down the *screen*, how many pixels should I move through the *texture*?" If the wall is drawn very tall (close wall, `line_height` large), `step` is small (less than 1) — meaning you sample the *same* texture row for several consecutive screen rows (stretching/magnifying the texture, like zooming into an image). If the wall is drawn short (far wall), `step` is large (more than 1) — you skip several texture rows for each screen row (compressing/minifying it).

```c
tex_y = (int)tex_pos & (PX - 1);
tex_pos += step;
```

`tex_pos` accumulates fractionally by `step` every screen row; truncating it to an `int` and masking with `(PX - 1)` (see [section 15](#15-float-tricks) for why that's a fast way to "wrap into range `0..63`") gives the actual texture row to sample this screen row.

---

## 11. Color packing and bit manipulation (hex colors)

Colors throughout this codebase are stored as a single integer, in the format `0x00RRGGBB` — 8 bits (2 hex digits) each for red, green, and blue, packed into one 32-bit `int`, with the top 8 bits unused/zero (sometimes called the "alpha"/"transparency" byte, though MLX in this project's usage doesn't actually use it for transparency).

```
   00        RR        GG        BB
┌────────┬────────┬────────┬────────┐
│ 8 bits │ 8 bits │ 8 bits │ 8 bits │
└────────┴────────┴────────┴────────┘
 bits 31-24 bits 23-16 bits 15-8  bits 7-0
```

Each hex digit represents 4 bits, and each pair of hex digits (2 digits = 8 bits) represents a value from `0` (`0x00`) to `255` (`0xFF`) — this is exactly the range a single color channel needs (`unsigned char` is 8 bits, `0`–`255`).

**Extracting a channel** uses two operations together — a right-shift (`>>`) to move the bits you want down to the bottom, and a bitwise AND (`&`) with `0xFF` to discard everything except the bottom 8 bits:

```c
r = (color >> 16) & 0xFF;   // move red's bits down to the bottom 8, then mask off everything else
g = (color >> 8) & 0xFF;    // green needs less shifting since it's already closer to the bottom
b = color & 0xFF;           // blue is already at the bottom — no shift needed, just mask
```

Why the `& 0xFF` is needed even after shifting: shifting `0x00RRGGBB` right by 16 gives you `0x000000RR` — the red byte is now correctly positioned at the bottom, but there could be leftover garbage or unwanted high bits in some contexts. Masking with `0xFF` (binary `11111111`) forces every bit above position 7 to `0`, guaranteeing you get *only* that one clean byte.

**Packing a color back together** does the reverse — shift each channel *up* into its correct position, then OR (`|`) them together (OR is safe here specifically because each channel occupies a completely separate, non-overlapping range of bits, so ORing them can't accidentally corrupt another channel):

```c
color = (r << 16) | (g << 8) | b;
```

**The N/S wall darkening trick**, from `color_gradient`:

```c
color = (color >> 1) & 0x7F7F7F;
```

Shifting the *entire packed color* right by 1 bit divides every channel by 2 simultaneously (a cheap way to halve brightness without doing three separate floating-point multiplications). The catch: shifting the whole 32-bit value right by 1 can leak one bit from the bottom of, say, the red byte into the top of the green byte (since they're adjacent in memory). Masking with `0x7F7F7F` (binary `01111111` repeated for each byte) clears exactly that top leaked bit from each channel, cleaning up the bleed-through.

---

## 12. Distance formulas and "is this pixel inside a circle?"

**Euclidean distance** between two points `(x1, y1)` and `(x2, y2)` is the straight-line distance, from the Pythagorean theorem:

```
distance = sqrt( (x2-x1)² + (y2-y1)² )
```

Used directly in `player_near_door` (`door.c`) to check how close the player is to a door.

**Checking if a point is inside a circle** (used constantly in the minimap code — `minimap_in_circle`, the minimap border, the player marker dot) is a variation on the same formula: a point `(x, y)` is inside a circle of radius `r` centered at `(cx, cy)` exactly when its distance to the center is `≤ r`:

```
sqrt((x-cx)² + (y-cy)²) ≤ r
```

**The square-root-avoidance trick**: computing `sqrt()` is relatively expensive, and you don't actually need the *exact* distance — you only need to know whether it's `≤ r`. Since both sides of an inequality involving only non-negative numbers can be squared without changing which side is bigger, this is exactly equivalent to:

```
(x-cx)² + (y-cy)² ≤ r²
```

...which needs zero square roots. This is exactly what `minimap_in_circle` does:

```c
center_offset_x_sq = (x - center.x) * (x - center.x);
center_offset_y_sq = (y - center.y) * (y - center.y);
return (center_offset_x_sq + center_offset_y_sq <= radius_px * radius_px);
```

Since this check runs for *every pixel* in the minimap's bounding square (potentially tens of thousands of times per frame), avoiding a `sqrt()` call in the hot path is a meaningful performance win.

---

## 13. Linear interpolation (lerp)

"Lerp" means: given two values (or two points, or two vectors) `A` and `B`, and a fraction `t` between `0` and `1`, compute the value that's `t` of the way from `A` to `B`. The formula:

```
lerp(A, B, t) = A + (B - A) * t
```

- `t = 0` → gives exactly `A`.
- `t = 1` → gives exactly `B`.
- `t = 0.5` → gives the exact midpoint between `A` and `B`.
- Any `t` in between → a proportional blend.

You've actually already seen this idea in [section 4](#4-camera_x) (`dir + plane * camera_x` is a lerp-flavored blend, just not clamped to `[0,1]` since `camera_x` ranges `-1` to `1`). It shows up explicitly, using `t` in the `[0,1]` range, in `draw_single_fov_ray` (`minimap_fov.c`), which draws the minimap's fan of FOV lines:

```c
ray_t = (double)ray.idx / (double)ray.total;
dir.x = ray.left.x + (ray.right.x - ray.left.x) * ray_t;
dir.y = ray.left.y + (ray.right.y - ray.left.y) * ray_t;
```

`ray.idx` counts from `0` up to `ray.total` (the number of rays being drawn for the FOV fan), so `ray_t` sweeps smoothly from `0` to `1` as `idx` increases. Plugging that into the lerp formula smoothly blends from the `left` boundary direction to the `right` boundary direction, producing an evenly-spaced fan of rays across the whole field of view — exactly analogous to how `camera_x` fans out the *real* 3D raycaster's rays across the screen, just parameterized `0..1` instead of `-1..1`.

---

## 14. Line drawing without a library (the FOV lines on the minimap)

`draw_minimap_fov_line` (`minimap_fov_utils.c`) draws a straight line between two points using only integer/pixel steps — no built-in "draw a line" function exists in this project's pixel-writing toolkit (`my_pixel_put` only writes one pixel at a time), so the line has to be walked manually, one pixel at a time, similar in spirit to the classic **Bresenham line algorithm** (though this implementation is a simpler floating-point variant rather than Bresenham's exact integer-only version).

**The core idea**: figure out how far apart the two points are in each axis, use the *larger* of the two axis-distances to decide how many steps the line needs (so you never skip pixels along the "dominant" direction), and take a proportionally smaller step along the other axis each time.

```c
sx = end.x - start.x;
sy = end.y - start.y;
steps = ft_abs(sx);
if (ft_abs(sy) > steps)
    steps = ft_abs(sy);
```

`steps` becomes the larger of the horizontal and vertical spans — this guarantees the line gets at least one pixel drawn per unit of its longest dimension, so it looks continuous without gaps.

```c
static void	fov_line_step(double *x, double *y, double sx, double sy)
{
	if (ft_abs(sx) > ft_abs(sy))
	{
		*x += sx / ft_abs(sx);
		*y += sy / ft_abs(sx);
	}
	else
	{
		*x += sx / ft_abs(sy);
		*y += sy / ft_abs(sy);
	}
}
```

Each call to this function advances the current position by exactly `1` pixel along whichever axis has the larger total span (`sx / |sx|` is just `+1` or `-1` — a sign-extraction trick), and by a *proportionally smaller* amount along the other axis (`sy / |sx|` — the actual slope of the line, scaled down). This is the same idea as "rise over run" from the slope of a line in algebra: if a line is mostly horizontal, for every 1 pixel you move right, you only move a small fraction of a pixel up/down; this function computes exactly that fraction and applies it every step.

---

## 15. Floating-point housekeeping

### `floor()` and fractional parts

`floor(x)` rounds a decimal number *down* to the nearest whole number (e.g. `floor(7.83) = 7.0`, and importantly, `floor(-2.3) = -3.0` — it always rounds toward negative infinity, not toward zero, which matters when a player's coordinate could be negative in some edge calculation).

A very common pattern in this codebase is extracting just the fractional part of a number — "how far into this tile am I, ignoring which tile":

```c
wall_x -= floor(wall_x);   // e.g. 7.83 - 7.0 = 0.83
```

### Casting to `int` vs. `floor()`

You'll notice both `(int)x` and `floor(x)` used in different places for "round down." For *positive* numbers they behave identically. `(int)` casting in C actually **truncates toward zero**, which only differs from `floor()` for negative numbers (`(int)(-2.3) = -2`, but `floor(-2.3) = -3.0`). Since world/map coordinates in this project are always non-negative (nobody walks to negative map coordinates), this distinction never actually causes a bug here — but it's worth knowing they aren't universally interchangeable.

### The power-of-two wraparound trick: `& (PX - 1)`

```c
tex_y = (int)tex_pos & (PX - 1);
```

Normally, to wrap a number into the range `0..N-1` (so it "loops back around" instead of going out of bounds), you'd use the modulo operator: `x % N`. But when `N` is a **power of two** (like `PX = 64 = 2⁶`), there's a faster equivalent using a bitwise AND: `x & (N - 1)`.

Why this works: `64` in binary is `1000000`. `63` (`PX - 1`) in binary is `0111111` — exactly 6 ones. ANDing any number with `0111111` keeps only its lowest 6 bits and zeroes everything above — which is mathematically identical to "the remainder after dividing by 64," because any multiple of 64 has all zeros in those lowest 6 bits by definition. This trick only works when the divisor is a power of two; it would silently give the wrong answer for something like `x % 60`.

---

## 16. Bonus: `fork`/`exec`/signals for the door sound

Not math, but genuinely unfamiliar CS concepts if you haven't hit them before, and they show up in `door_sound.c`.

- **`fork()`** duplicates the currently running program into two separate, independent processes — the "parent" (the original Cub3D game) and a "child" (an identical copy, at least momentarily). Both continue running from the exact same point in the code right after the `fork()` call. The way your code tells which one it's in: `fork()` *returns* the child's process ID in the parent, but returns `0` in the child — so `if (pid == 0)` is "code that only the child runs."
- **`exec` (family of functions, e.g. `execlp`)** replaces the *currently running program* inside a process with a completely different program, without creating a new process. So `fork()` + `exec` together is the standard Unix pattern for "run another program without freezing the current one": fork to get a second, independent process, then have that second process turn itself into `paplay` (the audio player command) via `exec`. Meanwhile, the original (parent) process's `fork()` call already returned, so it keeps running the game immediately — it doesn't wait around for the sound to finish. That's what makes the door sound non-blocking.
- **Signals** (`SIGCHLD`) are the OS's way of notifying a process that something happened — in this case, "one of your child processes just finished." Normally, a finished child process becomes a "zombie" (a placeholder entry in the process table) until the parent explicitly acknowledges it. `signal(SIGCHLD, SIG_IGN)` tells the OS "don't bother notifying me, just clean these up automatically," which is exactly what you want for fire-and-forget sound effects where the game doesn't care exactly when the sound finishes playing.