# raycasting_door.c — Function Breakdown

## Function: `door_tex_x(t_ray *ray, double wall_x)` (static)

```c
static int	door_tex_x(t_ray *ray, double wall_x)
{
	int	tex_x;

	tex_x = (int)(wall_x * PX);
	if (ray->side == 0 && ray->ray_dir_x > 0)
		tex_x = PX - tex_x - 1;
	if (ray->side == 1 && ray->ray_dir_y < 0)
		tex_x = PX - tex_x - 1;
	return (tex_x);
}
```

**Purpose**: Calculate texture X coordinate for door wall.

**Parameters:**
- `ray`: Ray struct with hit side and direction.
- `wall_x`: Fractional position on wall where ray hit (0.0 to 1.0).

**Return Value:**
- Texture X coordinate (0 to PX-1).

**Logic:**
1. Convert fractional position to texture pixel: `tex_x = (int)(wall_x * PX)`.
2. Flip horizontally for certain ray directions (same logic as `find_wall_hit()` in raycasting2.c).

---

## Function: `ray_hits_door(t_game *game, t_ray *ray)`

```c
bool	ray_hits_door(t_game *game, t_ray *ray)
{
	t_door	*door;
	double	perp;
	double	wall_x;
	int		tex_x;
	int		open_px;

	door = get_door_at(game, ray->map_x, ray->map_y);
	if (!door || door->state == DOOR_CLOSED)
		return (true);
	if (door->state == DOOR_OPEN || door->frame >= DOOR_ANIM_FRAMES - 1)
		return (false);
	if (ray->side == 0)
		perp = ray->side_dist_x - ray->delta_dist_x;
	else
		perp = ray->side_dist_y - ray->delta_dist_y;
	if (ray->side == 0)
		wall_x = game->player.player_y + perp * ray->ray_dir_y;
	else
		wall_x = game->player.player_x + perp * ray->ray_dir_x;
	wall_x -= floor(wall_x);
	tex_x = door_tex_x(ray, wall_x);
	open_px = (int)((double)door->frame / DOOR_ANIM_FRAMES * (PX / 2));
	return (abs(tex_x - PX / 2) >= open_px);
}
```

**Purpose**: Determine if a ray hits an (possibly open) door or passes through the opening.

**Parameters:**
- `game`: Game struct with door list.
- `ray`: Ray struct with hit position and direction.

**Return Value:**
- `true` if door blocks the ray, `false` if ray passes through opening.

**Logic:**

1. **Get door** — Find door at ray hit cell via `get_door_at()`.

2. **Already open** — If no door or door fully open → `false` (ray passes).

3. **Fully closed** — If door closed → `true` (ray blocks).

4. **Partially open** — For partially open doors, check if ray hits the door frame:

   a. **Calculate perpendicular distance** — Same logic as `calculate_wall()` in raycasting2.c.

   b. **Calculate wall_x** — Where on the wall the ray hit.

   c. **Calculate tex_x** — Texture X coordinate of hit (0 to 63).

   d. **Calculate opening width** — How wide the opening is:
      - `open_px = frame / DOOR_ANIM_FRAMES * (PX / 2)`
      - Frame 0: opening = 0 pixels (fully closed).
      - Frame DOOR_ANIM_FRAMES-1: opening = 32 pixels (half open).

   e. **Check if ray hits door frame**:
      - Door center is at tex_x = 32 (middle of 64-pixel texture).
      - Ray hits door if `abs(tex_x - 32) >= open_px`.
      - Ray passes through if `abs(tex_x - 32) < open_px` (hits opening).

**Example:**
- Door frame 8 (halfway open): opening ≈ 16 pixels.
- Ray hits texture column 25: `abs(25 - 32) = 7 < 16` → passes through.
- Ray hits texture column 15: `abs(15 - 32) = 17 >= 16` → hits door frame.

**Door Opening Animation:**
- Door frame determines how much it's open, which determines opening width.
- As door animates from frame 0 to DOOR_ANIM_FRAMES-1, opening width increases.
- Allows smooth, graphically realistic door opening.
