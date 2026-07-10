# Cub3D Complete Function Breakdown Index

This directory contains comprehensive function breakdowns for every C source file in the Cub3D project. Each file is documented separately with detailed explanations of purpose, logic, variables, and call chains.

## File Organization

### Core Program Flow
- **[FUNCTIONS_main.md](FUNCTIONS_main.md)** — Program entry point and initialization sequence

### Graphics & Rendering Pipeline
- **[game/FUNCTIONS_render.md](game/FUNCTIONS_render.md)** — Main render loop and backbuffer management
- **[game/FUNCTIONS_raycasting_init.md](game/FUNCTIONS_raycasting_init.md)** — Ray initialization for DDA algorithm
- **[game/FUNCTIONS_raycasting.md](game/FUNCTIONS_raycasting.md)** — DDA grid traversal and wall detection
- **[game/FUNCTIONS_raycasting2.md](game/FUNCTIONS_raycasting2.md)** — Wall geometry calculation and texture mapping
- **[game/FUNCTIONS_raycasting_draw.md](game/FUNCTIONS_raycasting_draw.md)** — Texture sampling and wall rendering
- **[game/FUNCTIONS_color_gradient.md](game/FUNCTIONS_color_gradient.md)** — Distance-based shading and fog effect

### Player & Camera Control
- **[game/FUNCTIONS_player.md](game/FUNCTIONS_player.md)** — Player initialization and input event handling
- **[game/FUNCTIONS_movement.md](game/FUNCTIONS_movement.md)** — Player movement with collision detection
- **[game/FUNCTIONS_keys_handle.md](game/FUNCTIONS_keys_handle.md)** — Keyboard input processing and parameter adjustment

### Interactive Elements
- **[game/FUNCTIONS_door.md](game/FUNCTIONS_door.md)** — Door state management and animation
- **[game/FUNCTIONS_door_frames.md](game/FUNCTIONS_door_frames.md)** — Door animation frame loading
- **[game/FUNCTIONS_door_sound.md](game/FUNCTIONS_door_sound.md)** — Audio playback for door events
- **[game/FUNCTIONS_raycasting_door.md](game/FUNCTIONS_raycasting_door.md)** — Door collision with raycasting

### Minimap Visualization
- **[game/minimap/FUNCTIONS_minimap.md](game/minimap/FUNCTIONS_minimap.md)** — All minimap functions (rendering, FOV, player marker)

### Window & MLX Graphics
- **[game/FUNCTIONS_mlx.md](game/FUNCTIONS_mlx.md)** — MLX initialization, window, hooks, and main event loop
- **[game/FUNCTIONS_initialization.md](game/FUNCTIONS_initialization.md)** — Game state and texture initialization

### Map Parsing & Validation
- **[parsing/FUNCTIONS_parsing.md](parsing/FUNCTIONS_parsing.md)** — Map file parsing, validation, and element extraction

### Utilities & Helpers
- **[utils/FUNCTIONS_utils.md](utils/FUNCTIONS_utils.md)** — Error handling, memory management, text rendering
- **[game/FUNCTIONS_debug.md](game/FUNCTIONS_debug.md)** — Debug visualization (top-down map view)

---

## Quick Function Lookup

### By File
| File | Functions | Purpose |
|------|-----------|---------|
| main.c | `main()` | Program entry, orchestration |
| render.c | `my_pixel_put()`, `paint_background()`, `render()` | Main render loop |
| raycasting_init.c | `init_ray()`, `raycasting()` | Per-column ray setup and main raycasting loop |
| raycasting.c | `out_of_bounds()`, `cast_ray()`, `check_raydir_x/y()` | DDA grid traversal |
| raycasting2.c | `load_texture()`, `find_wall_hit()`, `calculate_wall()` | Wall detection and geometry |
| raycasting_draw.c | `draw_textured_line()` | Texture sampling and rendering |
| color_gradient.c | `get_factor()`, `color_gradient()` | Shading and distance-based darkening |
| player.c | `initialize_player()`, `player_moving()`, `player_idle()` | Player initialization and input |
| movement.c | `rotate()`, `move_player()`, `update_player_x/y()` | Movement with collision |
| keys_handle.c | `handle_keys()`, `handle_minimap_keys()`, `handle_speed_keys()` | Keyboard processing |
| door.c | `update_doors()`, `is_door_blocking()`, `player_near_door()` | Door mechanics |
| door_frames.c | `load_door_frames()`, `load_door_frame()` | Door animation loading |
| door_sound.c | `play_sound()`, `init_door_audio()` | Audio playback |
| raycasting_door.c | `ray_hits_door()` | Door collision detection |
| debug.c | `render_elements()`, `print_fov()` | Debug visualization |
| mlx.c | `mlx_main()`, `finish_game()`, `mouse_move()` | Graphics setup and event loop |
| initialization.c | `initialize_game()`, `initialize_images()` | State initialization |
| parse_elements.c | `get_map_details()` | Map file parsing |
| parse_map.c | `check_if_map_is_valid()`, `store_maze()` | Map validation |
| parse_rgb.c | `filter_rgb()` | Color parsing |
| parse_doors.c | `check_doors()` | Door validation |
| get_infos.c | `get_starting_info()` | Player position detection |
| flood_fill.c | `flood_fill_prep()` | Map enclosure validation |
| utils.c | `print_error()`, `array_len()`, `ft_abs()` | General utilities |
| clean_utils.c | `clear_game()`, `clear_images()` | Memory cleanup |
| door_utils.c | `get_door_at()`, `is_walkable()` | Door helpers |
| init_utils.c | `file_to_image()`, `get_dir_img_address()` | Image initialization |
| parse_utils.c | `check_extension()`, `ft_strcmp()` | Parsing helpers |
| text_render.c | `draw_string_scaled()`, `draw_char_scaled()` | Text rendering |
| text_render_utils.c | `display_speeds()` | Speed display |
| text_render_glyphs.c | `get_digit_pattern()` | Character bitmap patterns |
| minimap/utils.c | `minimap_tile_color()`, `rgb_to_color()` | Minimap utilities |
| minimap/minimap_main.c | `render_minimap()` | Main minimap rendering |
| minimap/minimap_player.c | `draw_minimap_player()` | Player marker on minimap |
| minimap/minimap_fov.c | `draw_minimap_fov()` | FOV visualization |
| minimap/minimap_fov_utils.c | `draw_minimap_fov_line()`, `blend_color_with_white()` | FOV helpers |

---

## Key Execution Flows

### Frame Rendering (Per-Frame)
```
render() in render.c
├─ paint_background() — fill ceiling/floor
├─ update_doors() — animate doors
├─ move_player() — process movement input
├─ raycasting() in raycasting_init.c
│  └─ For each screen column:
│     ├─ init_ray() — setup DDA
│     ├─ cast_ray() — walk grid to wall
│     ├─ calculate_wall() — perspective projection
│     └─ draw_textured_line() — render wall slice
├─ render_minimap() — if enabled
└─ mlx_put_image_to_window() — display frame
```

### Raycasting Pipeline (Per Column)
```
init_ray() — setup DDA parameters
├─ check_raydir_x() — compute X stepping
└─ check_raydir_y() — compute Y stepping

cast_ray() — DDA grid traversal
├─ Alternate X/Y stepping
├─ Check bounds
└─ Stop on wall or door hit

calculate_wall() — perspective math
├─ Perpendicular distance
├─ Wall height calculation
└─ Screen bounds computation

draw_textured_line() — rendering
├─ load_texture() — select wall/door texture
├─ find_wall_hit() — horizontal texture coordinate
└─ For each screen pixel:
   ├─ Sample texture pixel
   ├─ color_gradient() — apply shading
   └─ my_pixel_put() — write to backbuffer
```

### Game Initialization
```
main()
├─ initialize_game() — parse map, allocate structs
├─ check_if_map_is_valid() — full map validation
└─ mlx_main() — graphics setup & event loop
   ├─ mlx_init() — X11 context
   ├─ initialize_images() — load textures
   ├─ initialize_player() — set position/direction
   ├─ Register hooks (keyboard, mouse, close)
   └─ mlx_loop() — event/render loop [blocks]
```

### Map Parsing
```
get_map_details()
├─ Parse 6 configuration elements:
│  ├─ NO, SO, WE, EA (texture paths)
│  ├─ F, C (floor, ceiling colors)
│  └─ Map grid (lines after config)
├─ store_maze() — save grid
└─ Return success/failure

check_if_map_is_valid()
├─ Validate characters
├─ Count player positions (must be 1)
├─ Validate doors (proper alignment)
├─ get_starting_info() — find player start
└─ flood_fill_prep() — ensure map enclosed
```

---

## Key Data Structures (from structs.h)

### t_game
Main game state container.

### t_player
Player position, direction, camera plane, input flags.

### t_ray
Current ray's parameters for DDA traversal and rendering.

### t_door
Door position, animation state, current frame.

### t_map
Map grid, dimensions, starting position, colors.

### t_img / t_texture
Image buffer (address, dimensions, format) and texture metadata.

---

## Important Constants

| Constant | Value | Purpose |
|----------|-------|---------|
| PX | 64 | Texture size (64×64 pixels) |
| DOOR_ANIM_FRAMES | 16 | Door animation frame count |
| DOOR_RANGE | ~0.5 | Player proximity to trigger door |
| MINIMAP_MARGIN | 12 | Minimap offset from window edge |
| FOV (game->fov) | 0.66 | Camera plane width |

---

## Performance Considerations

- **Raycasting**: O(width × avg_grid_traversal) — most expensive
- **Minimap**: O(minimap_pixels) — expensive if large
- **Door animation**: O(num_doors) — negligible
- **Movement collision**: O(1) grid lookups — fast
- **Text rendering**: O(text_length) — negligible

---

## Notes for Understanding

1. **DDA Algorithm**: The raycasting uses Digital Differential Analyzer (DDA) to step through grid cells efficiently, stopping at walls.

2. **Perpendicular Distance**: Used instead of ray length to avoid fish-eye distortion (stretching at screen edges).

3. **Perspective Projection**: Wall height scales inversely with distance: `height = win_h / distance`.

4. **Texture Mapping**: Each wall texture is 64×64 pixels. The algorithm calculates where on the texture each pixel maps to and samples accordingly.

5. **Door Mechanics**: Doors animate smoothly between frame 0 (closed) and frame 15 (open). Ray-casting checks if rays pass through the opening or hit the door frame.

6. **Minimap**: Circles centered on player, showing map layout with FOV overlay. Can toggle visibility and adjust zoom/pan.

7. **Input State Pattern**: Movement keys set flags on press, clear on release. Main loop checks flags every frame and applies movement accordingly for smooth input.
