# Offset Explanation for Minimap Rendering

## What is an Offset?

An **offset** is the distance from a reference point to another point. In the minimap, we use offsets to convert between different coordinate systems.

---

## Key Offset Concepts

### 1. **Pixel Offset** (from circle center)
```c
pixel_offset_x = pixel_x - radius_px;
pixel_offset_y = pixel_y - radius_px;
```

**Example:**
- Circle center is at pixel position `(70, 70)` on the minimap square (140×140).
- If we're at pixel `(100, 70)`:
  - `pixel_offset_x = 100 - 70 = 30` (30 pixels right of center)
  - `pixel_offset_y = 70 - 70 = 0` (on the center line)
- If we're at pixel `(0, 0)` (top-left corner):
  - `pixel_offset_x = 0 - 70 = -70` (70 pixels left of center)
  - `pixel_offset_y = 0 - 70 = -70` (70 pixels up from center)

**Purpose:** Tells us the position relative to the minimap center, not the absolute pixel position.

---

### 2. **World Coordinate Conversion** (pixel offset → world tiles)
```c
world_x = player_world_x + (pixel_offset_x / MINIMAP_TILE_SIZE);
world_y = player_world_y + (pixel_offset_y / MINIMAP_TILE_SIZE);
```

**Example:**
- Player is at world position `(10, 10)` (10 tiles from origin).
- `MINIMAP_TILE_SIZE = 14` pixels per tile.
- For pixel at `(100, 70)` with `pixel_offset_x = 30, pixel_offset_y = 0`:
  - `world_x = 10 + (30 / 14) = 10 + 2.14 = 12.14` (about 2 tiles right of player)
  - `world_y = 10 + (0 / 14) = 10` (same tile row as player)

**Purpose:** Convert a pixel position on the minimap to actual map coordinates for checking what tile is there (wall, floor, void).

---

### 3. **Screen Position Offset** (adding margins)
```c
screen_x = offset_x + pixel_x;
screen_y = offset_y + pixel_y;
```

Where `offset_x = MINIMAP_MARGIN = 12`, `offset_y = MINIMAP_MARGIN = 12`.

**Example:**
- Minimap's top-left corner starts at screen position `(12, 12)`.
- If a pixel is at minimap position `(0, 0)`:
  - `screen_x = 12 + 0 = 12` (12 pixels from left edge of window)
  - `screen_y = 12 + 0 = 12` (12 pixels from top edge of window)
- If a pixel is at minimap position `(70, 70)` (the center):
  - `screen_x = 12 + 70 = 82`
  - `screen_y = 12 + 70 = 82`

**Purpose:** Position the minimap UI on the screen with a margin, rather than starting at pixel (0, 0).

---

## Visual Diagram

```
World Space (Map Tiles)
┌─────────────────────┐
│                     │
│   Player at (10,10) │
│       *             │
│                     │
└─────────────────────┘
          ↓ (sample nearby tiles)
          ↓
Minimap Space (Pixels)
┌─────────────────────────────┐
│  Screen at (12, 12)         │
│  ┌─────────────────────────┐│
│  │                         ││
│  │   * Center (70, 70)     ││
│  │                         ││
│  └─────────────────────────┘│  ← 140×140 pixel square
│  Radius: 70 pixels          │
└─────────────────────────────┘
          ↓ (add screen offset)
          ↓
Screen Space (Window Pixels)
┌─────────────────────────────┐
│                             │
│            (12,12)          │
│            ┌─────────────┐  │
│            │  *  center  │  │
│            │  (82, 82)   │  │
│            └─────────────┘  │
│            Minimap visible  │
└─────────────────────────────┘
```

---

## Step-by-Step Example: Rendering One Minimap Pixel

**Scenario:** Draw the minimap with player at world `(10, 10)`.

```
Step 1: Loop pixel_x=50, pixel_y=70

Step 2: Calculate pixel offset from circle center
  pixel_offset_x = 50 - 70 = -20  (20 pixels left of center)
  pixel_offset_y = 70 - 70 = 0    (on center line)

Step 3: Convert to world coordinates
  world_x = 10 + (-20 / 14) = 10 - 1.43 = 8.57
  world_y = 10 + (0 / 14) = 10

Step 4: Check map at world (8.57, 10)
  → Rounds to grid cell (8, 10)
  → Check game->map->grid[10][8]
  → Returns color (e.g., WALL_COLOR if '1')

Step 5: Calculate screen position
  screen_x = 12 + 50 = 62
  screen_y = 12 + 70 = 82

Step 6: Draw pixel
  put_minimap_pixel(game, img, 62, 82, WALL_COLOR);
```

---

## Summary

| Offset Type | Formula | Purpose |
|-------------|---------|---------|
| **Pixel offset** | `pixel - radius_px` | Position relative to minimap center |
| **World offset** | `pixel_offset / MINIMAP_TILE_SIZE` | Convert pixels to tile distances |
| **World coord** | `player_pos + world_offset` | Get actual map tile to sample |
| **Screen offset** | `minimap_pixel + MINIMAP_MARGIN` | Position minimap UI on screen |

---

## Key Constants

- `MINIMAP_TILE_SIZE = 14` — How many pixels represent one world tile.
- `MINIMAP_RADIUS_TILES = 5.0` — How many tiles are visible (radius).
- `MINIMAP_MARGIN = 12` — Pixels from screen edge to minimap.
- `radius_px = MINIMAP_RADIUS_TILES * MINIMAP_TILE_SIZE = 70` — Circle radius in pixels.
