# door.c — Function Breakdown

## Function: `player_near_door(t_player *player, t_door *door, double range)` (static)

```c
static bool	player_near_door(t_player *player, t_door *door, double range)
{
	double	dx;
	double	dy;

	dx = player->player_x - (door->x + 0.5);
	dy = player->player_y - (door->y + 0.5);
	return (sqrt(dx * dx + dy * dy) <= range);
}
```

**Purpose**: Check if player is within activation range of a door.

**Parameters:**
- `player`: Player struct with position.
- `door`: Door struct with grid coordinates.
- `range`: Activation radius (typically `DOOR_RANGE`).

**Return Value:**
- `true` if player is within range of door, `false` otherwise.

**Logic:**

1. **Calculate distance** — Euclidean distance from player to door center:
   - Door center: `(door->x + 0.5, door->y + 0.5)` (middle of grid cell).
   - Distance: `sqrt(dx² + dy²)`.

2. **Check range** — Return `true` if distance ≤ `range`.

---

## Function: `advance_door_animation(t_door *door)` (static)

```c
static void	advance_door_animation(t_door *door)
{
	if (door->state == DOOR_OPENING)
	{
		door->frame++;
		if (door->frame >= DOOR_ANIM_FRAMES)
		{
			door->frame = DOOR_ANIM_FRAMES;
			door->state = DOOR_OPEN;
		}
	}
	else if (door->state == DOOR_CLOSING)
	{
		door->frame--;
		if (door->frame <= 0)
		{
			door->frame = 0;
			door->state = DOOR_CLOSED;
		}
	}
}
```

**Purpose**: Advance door animation frame based on current state.

**Parameters:**
- `door`: Door struct with state and frame index.

**Return Value:** None (void).

**Logic:**

- **DOOR_OPENING** — Increment frame:
  - Increment `frame` (0 → DOOR_ANIM_FRAMES).
  - When frame reaches max, set state to `DOOR_OPEN`.

- **DOOR_CLOSING** — Decrement frame:
  - Decrement `frame` (DOOR_ANIM_FRAMES → 0).
  - When frame reaches 0, set state to `DOOR_CLOSED`.

- **DOOR_OPEN** or **DOOR_CLOSED** — No change (animation paused).

**Animation Frames:**
- 0 = fully closed, DOOR_ANIM_FRAMES = fully open.
- Frame used to select animation sprite.

---

## Function: `update_door_state(t_door *door, bool near)` (static)

```c
static void	update_door_state(t_door *door, bool near)
{
	if (near && (door->state == DOOR_CLOSED || door->state == DOOR_CLOSING))
	{
		door->state = DOOR_OPENING;
		play_sound(DOOR_OPEN_SOUND);
	}
	else if (!near && (door->state == DOOR_OPEN || door->state == DOOR_OPENING))
	{
		door->state = DOOR_CLOSING;
		play_sound(DOOR_CLOSE_SOUND);
	}
	advance_door_animation(door);
}
```

**Purpose**: Update door state based on player proximity and trigger animations/sounds.

**Parameters:**
- `door`: Door struct to update.
- `near`: `true` if player is within activation range.

**Return Value:** None (void).

**Logic:**

- **Player near & door not moving toward open** → Start opening:
  - If state is `CLOSED` or `CLOSING`, set to `OPENING`.
  - Play door open sound.

- **Player not near & door not moving toward closed** → Start closing:
  - If state is `OPEN` or `OPENING`, set to `CLOSING`.
  - Play door close sound.

- **Advance animation** → Call `advance_door_animation()` every frame.

**State Transitions:**
```
CLOSED → [player near] → OPENING → [full frame] → OPEN
  ↑                                              ↓
  └─────────────── [player far] ← CLOSING ←─────┘
```

---

## Function: `update_doors(t_game *game)`

```c
void	update_doors(t_game *game)
{
	int		i;
	bool	near;

	i = 0;
	while (i < game->doors)
	{
		near = player_near_door(&game->player, &game->door[i], DOOR_RANGE);
		update_door_state(&game->door[i], near);
		i++;
	}
}
```

**Purpose**: Update all doors in the game (proximity detection, animation, sound).

**Parameters:**
- `game`: Game struct with player and door list.

**Return Value:** None (void).

**Logic:**

1. Loop through all doors in `game->door[]` array.

2. For each door:
   - Check if player is near via `player_near_door()`.
   - Update door state and animation via `update_door_state()`.

**Called Every Frame:**
- Part of render loop (called from `render()` in `render.c`).

---

## Function: `is_door_blocking(t_game *game, int x, int y)`

```c
bool	is_door_blocking(t_game *game, int x, int y)
{
	t_door	*door;

	door = get_door_at(game, x, y);
	if (!door)
		return (false);
	return (door->state != DOOR_OPEN);
}
```

**Purpose**: Check if a door at grid position (x, y) blocks movement.

**Parameters:**
- `game`: Game struct with door list.
- `x`, `y`: Grid coordinates.

**Return Value:**
- `true` if a door exists at (x, y) and is not fully open, `false` otherwise.

**Logic:**

1. Find door at (x, y) via `get_door_at()`.

2. If no door found, return `false` (not blocking).

3. If door exists, return `true` if state is NOT `DOOR_OPEN`:
   - `DOOR_CLOSED` → blocking.
   - `DOOR_OPENING` → blocking (partially open).
   - `DOOR_CLOSING` → blocking (partially open).
   - `DOOR_OPEN` → not blocking.

**Used By:**
- Movement collision detection in `update_player_x()` and `update_player_y()`.
- Raycasting in `ray_hits_door()` to check if door sprite blocks rays.
