# How Raycasting Actually Works — DDA Walked Through Step by Step

You already know the camera plane sets up *which direction* each ray points ([see `math_and_concepts.md`](#) section 3–4). This document picks up right after that: **once you have a ray direction, how do you actually find the wall it hits, and how far away that wall is?** That's the job of `delta_dist`, `side_dist`, and `perp_wall_dist` — the three names that felt like a wall of jargon. We'll build each one up from a real-life picture, then run one single example all the way through the real numbers.

---

## The big picture, in one paragraph

The player is standing somewhere on a grid of square tiles (the map). A ray shoots out from the player in some direction. You need to know: which tile does it first hit a wall in, and how far away is that? The dumb way is to creep forward in tiny steps checking "wall yet? wall yet?" — slow, and risks skipping through thin walls. The smart way — **DDA** — instead jumps straight from one grid-line crossing to the next, only ever pausing exactly at the edges of tiles, never in between. `delta_dist` and `side_dist` are the two pieces of bookkeeping that make those jumps possible.

---

## 1. Delta distance — "the cost of crossing one tile"

**Real-life picture**: You're walking across a city laid out in square blocks, on some diagonal path. Ask yourself: *"if I keep walking in this exact direction, how much total walking distance does it take before I've moved exactly one block's width to the east?"*

The answer depends entirely on *how diagonal* your path is:

- Walking almost due east (barely drifting north/south) → you cross a block's width almost immediately, after very little walking. **Small delta distance.**
- Walking almost due north (barely drifting east/west) → you have to walk a huge distance north before you've drifted even one block's width east. **Huge delta distance.**

That's it — that's the whole idea. `delta_dist_x` is *"how much total ray-travel does it take to cross exactly one tile's width in X"*, and it only depends on the direction, not on where you're currently standing.

**The formula**: `delta_dist_x = |1 / ray_dir_x|`. This matches the analogy perfectly: if `ray_dir_x` is small (the ray barely moves in X per unit of travel), then `1 / ray_dir_x` is *large* — you need lots of travel to accumulate one tile's worth of X movement. If `ray_dir_x` is close to `1` (the ray moves almost entirely in X), then `delta_dist_x` is close to `1` — barely any travel needed. And if `ray_dir_x` is exactly `0` (a perfectly vertical ray, never moving in X at all), you'd need to divide by zero — so the code just says "infinitely far" (`1e30`), meaning "this ray will *never* cross a vertical line, don't bother."

There's a separate `delta_dist_y`, computed the exact same way but for "how much travel to cross one tile's height in Y."

---

## 2. Side distance — "distance to the very next line, from right here"

**Real-life picture**: Same city-block walk, but now the question is different: *"I'm currently standing in the middle of a block, not at its edge. How far do I have to walk, right now, from exactly where I am, to reach the very next north-south street?"*

This is a **one-time, current-position-dependent** distance — unlike `delta_dist` (which is a fixed "cost per tile" that never changes for a given ray), `side_dist` starts out as "however far it happens to be from your exact starting spot to the nearest line," and then gets updated every time you cross a line.

**The formula**, split by which way you're heading:

```c
if (ray->ray_dir_x < 0)
    side_dist_x = (player_x - map_x) * delta_dist_x;      // distance back to the LEFT edge of current tile
else
    side_dist_x = (map_x + 1.0 - player_x) * delta_dist_x; // distance forward to the RIGHT edge
```

**Real-life version**: if you're standing 30% of the way across a block and walking further into it (rightward), the *remaining* 70% of the block is what's left to cross — so you scale that leftover fraction of the block by "how much travel one full block costs" (`delta_dist_x`) to get "how much travel is left to reach the edge." If you're instead walking backward (leftward), you only need to cover the 30% you've already walked back through.

---

## 3. The DDA loop — racing two distances against each other

Now you have two numbers: `side_dist_x` ("how far to the next north-south line") and `side_dist_y` ("how far to the next east-west line"). **Whichever one is smaller tells you which line you'll reach first.** Step into that tile, then immediately reschedule: the *next* line of that same type is exactly one more `delta_dist` further away, so you add `delta_dist` to that side_dist to set up the next race.

```c
if (side_dist_x < side_dist_y)
{
    side_dist_x += delta_dist_x;   // schedule the FOLLOWING vertical line
    map_x += step_x;               // actually step into the next column
    side = 0;                      // remember: we just crossed a vertical line
}
else
{
    side_dist_y += delta_dist_y;
    map_y += step_y;
    side = 1;
}
```

**Real-life picture**: you're comparing "the next north-south street is 40 yards ahead" against "the next east-west street is 65 yards ahead." 40 < 65, so you'll hit the north-south street first — walk there, and now ask: "okay, when's the *following* north-south street?" — that's another full block-crossing away, so add `delta_dist_x` (the cost of one more block) to get the new distance-to-schedule. Meanwhile, the east-west street is still 65 yards away — untouched, still valid, no need to touch it until you actually reach it.

Every time you cross a line, check the tile you just entered — if it's a wall, stop. If not, repeat the race.

---

## 4. Full worked example, start to finish

Let's actually run the numbers instead of talking abstractly.

**Setup**: player standing at world position `(2.5, 2.5)` — dead center of tile `(2, 2)`. Ray direction (not normalized, and that's fine — DDA doesn't need it to be): `ray_dir_x = 1.0`, `ray_dir_y = 0.5` (mostly heading east, drifting south a bit).

**Step 0 — compute delta distances** (fixed for this whole ray):
```
delta_dist_x = |1 / 1.0| = 1.0
delta_dist_y = |1 / 0.5| = 2.0
```
Notice `delta_dist_y` is bigger — makes sense, since the ray moves *slower* in Y (drifting) than in X (its main direction), so it takes more travel to rack up one full tile of Y movement.

**Step 0 — compute initial side distances**. Both directions are positive, so we're stepping right (`step_x = +1`) and down (`step_y = +1`):
```
side_dist_x = (map_x + 1 - player_x) * delta_dist_x = (2 + 1 - 2.5) * 1.0 = 0.5
side_dist_y = (map_y + 1 - player_y) * delta_dist_y = (2 + 1 - 2.5) * 2.0 = 1.0
```
Makes sense: the player is exactly halfway across the starting tile in both X and Y, but because `delta_dist_y` is double `delta_dist_x`, it "costs more travel" to close that same half-tile gap in Y.

**Now the race, step by step:**

| Iter | `side_dist_x` | `side_dist_y` | Smaller? | Action | New `map_x, map_y` | Tile at new cell |
|---|---|---|---|---|---|---|
| 1 | 0.5 | 1.0 | X | step X, `side_dist_x += 1.0 → 1.5` | (3, 2) | `'0'` — floor, keep going |
| 2 | 1.5 | 1.0 | Y | step Y, `side_dist_y += 2.0 → 3.0` | (3, 3) | `'0'` — floor, keep going |
| 3 | 1.5 | 3.0 | X | step X, `side_dist_x += 1.0 → 2.5` | (4, 3) | `'1'` — **wall! stop.** |

Three steps, and the ray landed exactly on tile `(4, 3)`, having crossed two vertical lines and one horizontal line along the way — without ever checking a single point that *wasn't* an actual grid-line crossing. That's the entire performance win of DDA in miniature.

Note the last crossing was a **vertical** line (we stepped in X), so `ray->side = 0` — this hit is a "vertical wall face," which is what later decides it gets textured with the east/west sprite instead of north/south, and gets full brightness instead of the darkened N/S tint.

---

## 5. Perpendicular distance — why not just use the total distance traveled?

You now know the ray hit tile `(4, 3)`. How far away is that, really? You *could* add up the total distance travelled (`0.5 + 1.0 + 1.0 = 2.5`... but wait, that's not even right either — those three numbers aren't distances travelled per step, they're the running side_dist_x/y values, which don't directly add up to a real distance in an obvious way). This is exactly why raycasters don't compute distance that way — and even if you carefully computed the *true* diagonal distance from `(2.5,2.5)` to the hit point, you'd run face-first into the fish-eye warping problem covered in `math_and_concepts.md` section 8: edge rays would report bigger distances than center rays for a wall that's actually flat and equidistant, making everything look bulgy.

**The fix, and the trick that computes it almost for free**: back up exactly one `delta_dist` from whichever `side_dist` just triggered the hit.

```c
if (ray->side == 0)
    perp_wall_dist = side_dist_x - delta_dist_x;
else
    perp_wall_dist = side_dist_y - delta_dist_y;
```

**Real-life picture for *why* this works**: remember, every time you cross a line, the loop immediately schedules the *next* line of that type by adding a full `delta_dist` — it does this *before* checking whether the tile you just entered is even a wall. So by the time you discover "oh, this tile IS a wall, stop," the `side_dist` variable has already been bumped one block-crossing too far ahead — it's pointing at the *next* line beyond the one you actually just crossed. Subtracting one `delta_dist_x` back off undoes that one extra bump, landing you exactly back on the distance to the line you actually hit.

**Checking it against our example**: we hit the wall via an X-step, with `side_dist_x = 2.5` at the moment of the hit (having just been bumped from `1.5` to `2.5` right before the wall check). So:
```
perp_wall_dist = 2.5 - delta_dist_x = 2.5 - 1.0 = 1.5
```
**Sanity check this independently**: the player started at `player_x = 2.5`. The wall was found at `map_x = 4`, meaning the wall's near face sits at world-X `= 4.0`. The ray needed to travel `4.0 - 2.5 = 1.5` units of X to get there. Since `ray_dir_x = 1.0` (the ray moves 1 unit of X per 1 unit of ray-travel), that X-distance of `1.5` corresponds to exactly `1.5` units of ray-travel. **Matches perfectly.** And crucially — this number only used the X-axis math the whole way through, completely ignoring how much the ray *also* drifted in Y to get there, which is exactly the "ignore the sideways component, measure straight-ahead only" fish-eye fix.

**Why it's specifically *perpendicular*, in the picture-taking sense from before**: this distance is measuring "how far forward, purely along the direction that matters for the camera plane's straight-ahead axis" — not the diagonal ray path. Every ray that hits the same flat wall, no matter how sharply angled that particular ray was, will report the *same* perpendicular distance for that wall — which is exactly what keeps flat walls looking flat instead of curved.

---

## 6. From `perp_wall_dist` to pixels on screen (quick recap)

Once you have this one clean number, everything downstream is straightforward and already covered elsewhere, but here's how it chains together:

```
perp_wall_dist  →  line_height = win_h / perp_wall_dist       (closer = taller; see math_and_concepts.md §9)
                →  draw_start / draw_end (centered on screen)
                →  wall_x (which fraction across the tile face was hit, using perp_wall_dist again)
                →  tex_x (which texture column to sample; see math_and_concepts.md §10)
```

Notice `perp_wall_dist` is reused a *second* time to compute `wall_x` in `find_wall_hit` — the exact same "project forward by the perpendicular distance" idea, just now asking "where across the wall's face did I land" instead of "how far away is the wall."

---

## 7. Cheat sheet — plain-English definitions

| Variable | Plain-English meaning |
|---|---|
| `ray_dir_x`, `ray_dir_y` | The direction this one ray is heading (not necessarily length 1). |
| `delta_dist_x` | Fixed "cost" (in ray-travel distance) to cross exactly one tile's width in X, given this ray's direction. Never changes for this ray. |
| `delta_dist_y` | Same, but for one tile's height in Y. |
| `side_dist_x` | "How far from right now until I hit the next north-south grid line" — starts based on where inside the tile the player currently stands, then re-scheduled by `+= delta_dist_x` every time a vertical line is crossed. |
| `side_dist_y` | Same idea, for the next east-west grid line. |
| `step_x`, `step_y` | Just `+1` or `-1` — which way (right/left, down/up) you're stepping through grid columns/rows. |
| `side` | `0` if the *last* line you crossed was vertical (north-south) — meaning you're looking at a wall running north-south; `1` if it was horizontal. |
| `perp_wall_dist` | The "ignore the sideways drift, only count the straight-ahead component" distance to the wall — computed by undoing one extra `delta_dist` bump from whichever `side_dist` triggered the hit. This is the fish-eye-corrected distance actually used for sizing the wall on screen. |

---

## 8. One more wrinkle: doors reuse the exact same math, mid-flight

`ray_hits_door` (`raycasting_door.c`) needs to know "where exactly, horizontally, does this ray cross the door's tile face" *before* the DDA loop has technically finished (a door might only partly block the ray, so the loop can't simply stop and call `calculate_wall` the normal way). It computes its own local `perp` using the exact same subtraction trick:

```c
if (ray->side == 0)
    perp = ray->side_dist_x - ray->delta_dist_x;
else
    perp = ray->side_dist_y - ray->delta_dist_y;
```

Same idea, same formula, just computed a little earlier/locally so the door-blocking check has a distance to work with before deciding whether the ray actually stops here or keeps traveling through the door's opening.