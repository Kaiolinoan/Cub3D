# Minimap Customization Guide

## Changing Minimap Size

Edit these constants in `includes/cub3d.h`:

### 1. **Tile Size** (Controls overall minimap dimensions)
```c
#define MINIMAP_TILE_SIZE 14
```
- **Increase** (e.g., 20): Makes the minimap physically larger on screen
- **Decrease** (e.g., 8): Makes the minimap physically smaller on screen

### 2. **Radius in Tiles** (Controls how much map is visible)
```c
#define MINIMAP_RADIUS_TILES 5.0
```
- **Increase** (e.g., 7.0): **Zoom out** — Shows more of the map at once
- **Decrease** (e.g., 3.0): **Zoom in** — Shows less of the map (focused view)

### 3. **Position** (Controls minimap location)
```c
#define MINIMAP_MARGIN 12
```
- **Increase**: Moves minimap further from the top-left corner
- **Decrease**: Moves minimap closer to the corner

## Quick Examples

| Goal | Change |
|------|--------|
| Zoom in (see more detail) | Decrease `MINIMAP_RADIUS_TILES` to 2.0-3.0 |
| Zoom out (see more map) | Increase `MINIMAP_RADIUS_TILES` to 8.0-10.0 |
| Make minimap bigger | Increase `MINIMAP_TILE_SIZE` to 20-25 |
| Make minimap smaller | Decrease `MINIMAP_TILE_SIZE` to 8-10 |

## How It Works

- **MINIMAP_TILE_SIZE** × **MINIMAP_RADIUS_TILES** × 2 = total minimap diameter in pixels
- Example: `14 × 5.0 × 2 = 140` pixels diameter

The minimap displays a circular view centered on the player, with the radius determined by `MINIMAP_RADIUS_TILES`.
