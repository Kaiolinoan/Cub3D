# Cub3D — Math & Concepts Explained (No Prior Knowledge Assumed)

Every section here follows the same pattern: **a real-life comparison first**, then the actual math, then how it shows up in your code. If a term sounds technical, skip straight to the "real-life version" box — that's the part meant to actually stick.

**Table of contents**
1. [Vectors — giving directions, like "walk 3 steps right"](#1-vectors)
2. [Direction vector — which way your nose is pointing](#2-direction-vector)
3. [The camera plane — the invisible window in front of the camera](#3-camera-plane)
4. [`camera_x` — sweeping a flashlight left to right](#4-camera_x)
5. [Trig refresher — the Ferris wheel](#5-trig)
6. [Rotation — turning a car, headlights and mirrors together](#6-rotation)
7. [DDA — a taxi driver who only turns at intersections](#7-dda)
8. [Fish-eye — why phone panoramas look warped](#8-fisheye)
9. [Distance → size on screen — cars on a highway](#9-projection)
10. [Texture mapping — wallpapering a wall](#10-texture-mapping)
11. [Hex colors — a paint-mixing dial](#11-bit-packing)
12. [Circle checks — did the stone land inside the chalk circle?](#12-distance-circle)
13. [Lerp — a dimmer switch](#13-lerp)
14. [Line drawing — connect-the-dots on graph paper](#14-line-drawing)
15. [Floating point tricks — rounding down at checkout, and odometers wrapping around](#15-float-tricks)
16. [Bonus: fork/exec — sending a clone to fetch coffee](#16-fork-exec)

---

## 1. Vectors

**Real-life version**: Imagine you're giving someone directions in a parking lot: *"walk 3 steps to the right, and 4 steps forward."* That instruction — a direction plus a distance — is a **vector**. It's not a place, it's a *move*. In this code, every vector is just two numbers, `x` and `y`, that together mean "this much sideways, this much forward/back."

Two things you constantly do with an instruction like that:

- **Add two of them together**: *"walk 3 right + 4 forward" then also "2 right + 1 forward"* → do both, one after another, and you end up 5 right and 5 forward total. That's vector addition: `(a.x + b.x, a.y + b.y)`.
- **Scale one**: *"walk 3 right and 4 forward, but twice as far"* → 6 right, 8 forward. Same direction, just longer. That's `(x * k, y * k)`.

**Length**: if you actually paced out "3 right, 4 forward," how far are you from where you started, as the crow flies? Not 3, not 4, not 7 — it's the diagonal, and Pythagoras tells us it's `sqrt(3² + 4²) = 5`. This is why vector length in code is always `sqrt(x*x + y*y)`.

```
        (3, 4)
         /|
        / |
   5   /  | 4
      /   |
     /____|
       3
```

**"Normalizing"** a vector just means: keep the same direction, but rescale it so the length is exactly `1`. Real-life version: you have a direction ("northeast-ish") but you want to express it as "one single step in that exact direction," not "3 steps right and 4 forward." You get there by dividing both numbers by the length: `x/5, y/5` in the example above.

---

## 2. Direction vector

**Real-life version**: Think of a compass needle glued to the top of the player's head, always pointing exactly where they're facing. `player->dir_x, dir_y` *is* that needle — a vector of length 1 (a "pure direction, no distance" instruction, like in section 1).

Since this project's grid has row `0` at the *top* (like a spreadsheet or a piece of graph paper you read top-to-bottom), "north" actually means "negative Y":

| Facing | `dir_x` | `dir_y` |
|---|---|---|
| North (up) | 0 | -1 |
| South (down) | 0 | 1 |
| East (right) | 1 | 0 |
| West (left) | -1 | 0 |

---

## 3. The camera plane

You said you liked this one, so here's the fuller version.

**Real-life version**: Hold your phone up to take a photo. Your eye/the camera lens is the "player." The direction you're pointing the phone is the "direction vector." But a photo isn't just a single point straight ahead — it captures a whole *rectangle* of the world. The edges of your phone's screen, held up in front of you, form a flat little rectangle that's always turned to directly face you — it's never tilted sideways relative to your gaze, it's always dead-on perpendicular to it. **That flat rectangle (or, since this game is 2D-top-down-conceptually, just a flat line segment) is the camera plane.**

```
                    the "phone screen" (camera plane)
        left edge <──────●──────> right edge
                          │
                          │ direction you're pointing
                          │
                          ▼
                        you
```

Zoom your phone's camera out ("wide angle") and that rectangle effectively gets *wider* relative to how far away it feels — you capture more of the scene side-to-side. Zoom in ("telephoto") and it gets *narrower* — you capture less side-to-side, but it feels more "zoomed." **That's exactly what field of view (FOV) means**, and it's exactly why, in the code, the *length* of the camera plane vector controls how wide the field of view is: a longer plane = a wider imaginary window = seeing more stuff to the sides at once.

**Why must it be perpendicular (a perfect 90° angle) to the direction?** Because that's just what "a screen held up facing you" *means*. If you tilted your phone's screen sideways relative to your line of sight, it wouldn't be capturing a straight-on photo anymore — it'd be catching things at an angle, distorted. Keeping the plane perpendicular is what keeps the "photo" undistorted and centered.

In code, you get a perpendicular vector by swapping `x` and `y` and flipping the sign of one of them. If `dir = (0, -1)` (facing north), swap-and-negate gives `(1, 0)` — and sure enough:
```c
set_player_plane_values(player, game->fov, 0);   // plane = (0.66, 0)
```

---

## 4. `camera_x`

**Real-life version**: Imagine you're not taking one photo, but instead sweeping a thin flashlight beam slowly from the far-left edge of your camera's view to the far-right edge, pausing once for every single pixel-wide strip of the photo. `camera_x` is just "how far along that sweep are we right now" — expressed as a number from `-1` (all the way left) to `0` (dead center) to `+1` (all the way right).

```c
ray->camera_x = 2 * x / (double)game->win_w - 1;
```

If `x` is the current screen column out of `win_w` total columns, this formula just rescales "which column am I on" into that `-1` to `+1` range. Then:

```c
ray_dir_x = dir_x + plane_x * camera_x;
```

**Real-life version of this line**: *"Start by facing straight ahead (`dir`). Then, if this flashlight strip is toward the left of the sweep, nudge your beam a little toward the left edge of your camera window (`plane`); if it's toward the right, nudge right instead — by an amount proportional to how far through the sweep you are."* At dead center (`camera_x = 0`) you get no nudge at all — straight ahead. At the far edges, you get the *full* nudge — all the way to the plane's left or right tip.

---

## 5. Trig refresher

**Real-life version**: Think of a Ferris wheel, or a clock's second hand, sweeping around a circle. At any moment, that hand is pointing in some direction, and you can ask two questions about it: *"how far right/left of center is the tip?"* and *"how far up/down from center is the tip?"* Those two answers are exactly `cos` and `sin`.

```
              12 o'clock
                │
       cos(θ),  │
       sin(θ)   │
          \     │
           \    │
            \θ  │
     ────────●──┼──── 3 o'clock
             center
```

`cos(θ)` = how far sideways the tip is. `sin(θ)` = how far up/down the tip is. `θ` (theta) is just "how far around the circle you've swept" — measured in **radians** instead of degrees, where a full trip around (360°) equals `2π` (≈6.28) radians, and a quarter trip (90°, straight up from 3 o'clock) is `π/2` (≈1.57) radians. C's math functions always want radians, never degrees.

You don't need to memorize the values — you just need to know: *feeding an angle into `cos`/`sin` gives you back "how far sideways" and "how far up/down" a hand of that angle would be pointing.* That's the entire building block for rotation, next.

---

## 6. Rotation

**Real-life version**: You're driving a car and you turn the steering wheel. Your headlights (pointing forward — the `dir` vector) swing to a new direction. But so does everything else rigidly attached to the car — your side mirrors, the hood ornament, all of it turns by the *exact same angle*, at the *exact same time*, so the car still "makes sense" as a rigid shape. If only the headlights turned and the mirrors stayed pointing the old way, the car would look broken.

That's exactly why `rotate()` in the code turns **both** `dir` (the headlights) and `plane` (think of it as the car's width, side mirror to side mirror) by the same angle — they have to stay locked together, perpendicular to each other, like a rigid car frame turning as one piece.

The actual turning math, for any point/vector `(x, y)` rotated by angle `θ`:

```
new_x = x·cos(θ) − y·sin(θ)
new_y = x·sin(θ) + y·cos(θ)
```

**Real-life version of *why* this formula works**: think of your position as "so many steps in the sideways direction, plus so many steps in the forward direction." When you rotate, "sideways" and "forward" themselves rotate too — `cos(θ)`/`sin(θ)` tell you exactly where the *old* sideways direction ends up pointing after the turn, and the formula just re-adds up your steps using those *new* rotated directions instead of the old ones.

**Worked example**: facing north (`dir = (0,-1)`), turn 90°. `cos(90°) = 0`, `sin(90°) = 1`.
```
new_x = 0·0 − (−1)·1 = 1
new_y = 0·1 + (−1)·0 = 0
→ now facing (1, 0) = EAST
```
Exactly what you'd expect: turn 90° from facing north, and you're facing east.

**Why the code copies the old values first** (`old_dir_x`, etc.) before overwriting: it's like needing both your *old* North-South and East-West readings at the same time to compute the *new* ones. If you overwrite one before you're done using it for the other calculation, you'd be mixing an old reading with an already-updated one — like recalculating your car's new heading using one already-turned wheel and one not-yet-turned wheel. Snapshotting avoids that mix-up.

---

## 7. DDA (the grid-marching algorithm)

**Real-life version**: Imagine a taxi driving through a city laid out in a perfect grid, like Manhattan. The driver doesn't creep forward inch by inch checking "am I at an intersection yet?" every few feet — that'd be painfully slow. Instead, a smart driver thinks: *"the next cross-street ahead of me is exactly 200 feet away; the next side-street is 350 feet away — I'll drive straight to whichever one comes first, then re-check."* They only ever "pause and look around" exactly at intersections, never in between.

That's DDA. Instead of a ray creeping forward in tiny fixed steps (which is genuinely what the slow debug-mode ray in `debug.c` does — steps of `0.05` at a time!), DDA always jumps straight to the next grid-line crossing, whichever direction (vertical or horizontal) is closer, and only checks the map *there*.

```
Grid lines:        Ray path (● = places DDA actually checks):
  |   |   |   |
──┼───┼───┼───┼──
  │   │ ● │   │
──┼───●───┼───┼──     The ray never "checks" the empty space
  │ ●●│   │   │       between intersections — it jumps
──┼───┼───┼───┼──     directly from corner to corner.
  │ P │   │   │   (P = player)
──┼───┼───┼───┼──
```

Every step of the loop asks: *"is the next vertical street-crossing or the next horizontal street-crossing closer?"* — steps toward whichever is closer, and only then checks "is there a wall here?" This is dramatically faster than tiny fixed steps, and it's guaranteed to never "jump over" a thin wall by accident, because it always lands exactly on every grid line it crosses.

---

## 8. Fish-eye

**Real-life version**: You've probably seen a GoPro or phone "panorama" photo where straight walls look like they're bulging outward into a curve, especially near the edges of the shot. That warped look is called the "fish-eye" effect, and it happens because the edges of a wide photo are looking at things *at an angle*, so light has to travel a longer diagonal path to reach the camera than light coming from straight ahead — even for a wall that's actually perfectly flat and the same distance away the whole time.

```
             screen
    ╲          │          ╱
     ╲         │         ╱
      ╲        │        ╱     <- if you measured the actual
       ╲       │       ╱         diagonal distance to the wall
        ╲______|______╱          for each ray, the edges would
               you                report farther, even for a
                                   flat wall straight ahead
```

If this raycaster used the *actual* diagonal ray-length to decide how tall to draw each column, dead-straight walls would look like they're bulging — exactly like that panorama photo. The fix: instead of "how far did the ray actually travel," measure "how far forward, ignoring how far sideways" — imagine dropping a plumb line straight down from the wall hit point to the camera's imaginary "screen," and measuring along *that* line instead of the diagonal ray. That's `perp_wall_dist` (**perpendicular** wall distance) — and because it ignores the sideways component, it comes out the *same* for every ray hitting the same flat wall, no matter which screen column that ray belongs to. No more bulge.

---

## 9. Distance → size on screen

**Real-life version**: Stand on the shoulder of a highway and watch cars go by. A car right next to you looks huge — it fills your whole field of view. That same car, a quarter-mile down the road, looks tiny — maybe the size of your thumbnail held at arm's length. Nothing about the car changed size; it just got farther away, and farther-away things take up less of your view. This is called **perspective**, and it's the single most basic fact about how eyes and cameras work.

```c
ray->line_height = (int)(game->win_h / ray->perp_wall_dist);
```

This line is the code's version of "farther = smaller." Dividing by distance means: double the distance, and you get *half* the height. Walk twice as close, and the wall on screen gets *twice* as tall. It's the same relationship as the car on the highway — just expressed as `screen_height / distance` instead of "eyeballing it."

Then the wall gets centered vertically:
```c
draw_start = -line_height / 2 + win_h / 2;   // top edge
draw_end   =  line_height / 2 + win_h / 2;   // bottom edge
```
**Real-life version**: if you know how tall a picture frame is and where its middle should hang on the wall, you find the top and bottom edges by going up half the frame's height from the middle, and down half the height from the middle. Same idea here — `win_h / 2` is the middle of the screen, and you go up/down half the wall-slice's height from there.

---

## 10. Texture mapping

**Real-life version**: Imagine wallpapering a wall with a repeating pattern, like stripes. Two separate questions have to be answered for every point on the wall: **"which part of the wallpaper roll, left-to-right, lands at this spot?"** and **"which part of the roll, top-to-bottom, lands here?"** Those are the horizontal and vertical texture-mapping problems, and this code solves them completely separately.

**Horizontal**: figure out how far across *this one wall tile* (as a fraction, like "38% of the way across") the ray actually hit, then scale that fraction up to a pixel column in the actual texture image (which is `64` pixels wide in this project) — `38%` of `64` pixels is roughly column `24`. There's also a flip/mirror correction, because depending on which side of the wall you're looking at and which direction you approached from, "the wallpaper's left edge" doesn't always correspond to "the screen's left side" — without correcting for that, textures would sometimes render backwards, like a mirror image.

**Vertical — real-life version**: think of a photograph being stretched or squished to fit different-sized picture frames. A wall right next to you gets drawn very tall on screen (a "big frame"), so the same 64-pixel-tall texture image needs to be *stretched* to fill it — meaning several consecutive screen rows might show the *same* row of the original texture (like zooming into a photo). A wall far away gets drawn short (a "small frame"), so the texture needs to be *squished* — several texture rows get skipped for every screen row. The `step` variable in the code is exactly "how much of the original texture to skip (or repeat) per screen pixel," calculated from how stretched or squished this particular wall slice is.

---

## 11. Hex colors (bit packing)

**Real-life version**: Imagine a light-up sign with three separate dimmer dials — one for red bulbs, one for green, one for blue — each dial going from `0` (off) to `255` (full brightness). Mixing all three dials gives you any color. A "hex color" like `0xFF8800` is just a compact way of writing down all three dial settings at once: `FF` = red dial at max (255), `88` = green dial at about half (136), `00` = blue dial off.

```
   00        RR        GG        BB
┌────────┬────────┬────────┬────────┐
│ unused │  red    │ green  │  blue  │
└────────┴────────┴────────┴────────┘
```

**Reading one dial's setting out of the combined number** is what `(color >> 16) & 0xFF` does. Real-life version: imagine the three dial readings written on a strip of paper as one long number, like `255136000`, and you want to tear off just the middle chunk (green). You'd first "shift" the strip so the piece you want lines up at the end (`>> 16` slides the paper over), then you'd "mask off" (cover up with your hand, i.e. `& 0xFF`) everything except the last two digits you actually care about, so nothing else leaks into your answer.

**Combining three dial readings back into one number** (`(r << 16) | (g << 8) | b`) is the reverse — slide each dial's number into its correct "slot" on the strip of paper, then lay them all down together since they each occupy a completely separate slot and can't overlap or interfere with each other.

---

## 12. Circle checks

**Real-life version**: You draw a circle in the sand with a stick, then throw a stone at it. How do you know if the stone landed *inside* the circle? You could measure the exact distance from the stone to the center and compare it to the radius — but there's a shortcut that avoids a slower calculation (a square root): just compare the *squared* distance to the *squared* radius instead. If one is bigger, so is the other — squaring doesn't change which one wins, as long as both are positive — so you get the same yes/no answer for way less work.

```c
return (center_offset_x_sq + center_offset_y_sq <= radius_px * radius_px);
```

This exact "did the stone land inside the circle" check is run for *every single pixel* of the minimap, potentially tens of thousands of times a frame — which is exactly why the shortcut (skip the square root, just compare squared values) matters here: it's the difference between "instant" and "the game visibly slows down."

---

## 13. Lerp (linear interpolation)

**Real-life version**: A dimmer switch, or a GPS saying "you're 40% of the way to your destination." If you know the start value and the end value, and you know what fraction of the way there you are, you can compute exactly where you are in between — no need to physically measure it.

```
value = start + (end - start) * fraction
```

At `fraction = 0`, you're exactly at `start`. At `fraction = 1`, exactly at `end`. At `fraction = 0.4`, you're 40% of the way from `start` toward `end` — like the GPS example.

This shows up explicitly when the minimap draws its fan of "field of view" lines, sweeping smoothly from the leftmost visible direction to the rightmost:
```c
ray_t = (double)ray.idx / (double)ray.total;   // "what fraction through the fan am I"
dir.x = ray.left.x + (ray.right.x - ray.left.x) * ray_t;
```
**Real-life version**: like slowly panning a camera from the far-left edge of a view to the far-right edge, and asking "at the 40%-through-the-pan mark, exactly which direction is the camera pointing?" — that's a lerp between the leftmost and rightmost directions.

---

## 14. Line drawing

**Real-life version**: connect-the-dots on a piece of graph paper, where you can only ever move to an adjacent square (never draw a smooth diagonal line through the middle of squares) — you have to approximate a straight line using a staircase of tiny horizontal/vertical steps.

The trick to making that staircase *look* like a smooth straight line rather than something jagged and wrong: figure out whether the line is "mostly sideways" or "mostly up-and-down" (whichever total distance is bigger), and always take one full graph-paper step in *that* dominant direction each turn, while only taking a *fractional* nudge in the other direction — proportional to the actual slope of the line, like "rise over run" from algebra class. Do this enough times, and the staircase reads as a smooth line from a normal viewing distance — which is exactly what the minimap's FOV lines use to draw a straight ray from the player out to the wall.

---

## 15. Floating point tricks

**`floor()` — real-life version**: rounding a price *down* at checkout — `$7.83` becomes `$7`. It always rounds toward the smaller whole number, even for negatives (`-2.3` rounds down to `-3`, not up to `-2` — it always goes toward more-negative, not toward zero).

**Fractional part — real-life version**: think of a road with mile markers. If you're at mile `7.83`, "how far past the last mile marker am I" is `7.83 - 7 = 0.83` — 83% of the way to the next marker. That's exactly `wall_x -= floor(wall_x)` — strip off "which whole tile am I in" and keep only "how far across *this* tile."

**The `& (PX - 1)` wraparound trick — real-life version**: think of a car's odometer's last two digits, or a clock. When the clock hits `12`, it doesn't keep counting to `13, 14, 15...` — it wraps back around to `1`. Normally you'd compute a wraparound like this with the remainder/modulo operator (`%`), but there's a shortcut when the wrap-around point is a power of two (like this project's texture size, `64`) — you can use a bitwise AND instead, which is a faster operation for a computer to do, and gives the exact same wraparound result. It's a speed trick specific to powers of two; it wouldn't work correctly for wrapping around at, say, every 60 (like a real clock's seconds).

---

## 16. Bonus: fork/exec (the door sound)

**Real-life version**: You're cooking dinner and you also want a coffee, but the coffee maker takes 3 minutes and you don't want to stand there waiting — you'd rather keep cooking. So you ask a friend (or clone yourself, `fork()`) to go handle the coffee entirely, independently, while you keep doing your own thing. Your clone walks over to the coffee machine and effectively *becomes* "the coffee-making process" (`exec` — a running process turning into a completely different program), while you, the original, never stopped cooking.

That's exactly the pattern for playing a door sound: the game "forks" — clones itself — then that clone immediately turns itself into the `paplay` (audio player) program and starts playing the sound file, while the original game process goes right back to rendering frames, never pausing to wait for the sound to finish.

**Real-life version of `SIGCHLD`/signals**: when your coffee-fetching friend/clone finishes, normally you'd want a little heads-up ("hey, I'm done") so you can properly say thanks and consider the errand closed — otherwise they'd just awkwardly linger, "finished but not yet acknowledged." Telling the OS `signal(SIGCHLD, SIG_IGN)` is like telling a very efficient assistant: *"don't bother notifying me when errands finish, just handle the cleanup yourself automatically."* That way, dozens of door-sound clones over the course of a play session never pile up as unacknowledged leftover processes.