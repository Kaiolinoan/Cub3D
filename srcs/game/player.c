#include "cub3d.h"

int player_moving(int keycode, void *param)
{
    t_game *game = param;
    
    if (keycode == XK_w)
        game->player.up = true;
    else if (keycode == XK_s)
        game->player.down = true;
    else if (keycode == XK_a)
        game->player.left = true;
    else if (keycode == XK_d)
        game->player.right = true;
    else if (keycode == XK_Left)
        game->player.rotate_l = true;
    else if (keycode == XK_Right)
        game->player.rotate_r = true;
    else if (keycode == XK_Escape)
        finish_game(game);
    return (0);
}
int player_idle(int keycode, void *param)
{
    t_game *game = param;
    
    if (keycode == XK_w )
        game->player.up = false;
    else if (keycode == XK_s)
        game->player.down = false;
    else if (keycode == XK_a)
        game->player.left = false;
    else if (keycode == XK_d)
        game->player.right = false;
    else if (keycode == XK_Left)
        game->player.rotate_l = false;
    else if (keycode == XK_Right)
        game->player.rotate_r = false;
    return (0);
}

void rotate(t_player *player, double rot)
{
    int old_dir_x;

    old_dir_x = player->dir_x;
    player->dir_x = player->dir_x * cos(rot) - player->dir_y * sin(rot);
    player->dir_y =     old_dir_x * sin(rot) + player->dir_y * cos(rot);
}

void move_player(t_player *player)
{
    double speed;
    double rot_speed;

    speed = 0.5;
    rot_speed = 0.03;
    if (player->up)
    {
        player->player_x += player->dir_x * speed;
        player->player_y += player->dir_y * speed;
    }
    if (player->down)
    {
        player->player_x -= player->dir_x * speed;
        player->player_y -= player->dir_y * speed;
    }
    // if (player->left)
    //     player->player_x -= player->dir_x * speed;
    // if (player->right)
    //     player->player_x += player->dir_x * speed;
    if (player->rotate_l)
        rotate(player, rot_speed);
    if (player->rotate_r)
        rotate(player, -rot_speed);
}
void initialize_player(t_game *game, t_player *player)
{
    player->player_x = game->map->start_x;
    player->player_y = game->map->start_y;
    if (player->starting_direction == NORTH)
    {
        player->dir_x = 0;
        player->dir_y = -1;
    }
    if (player->starting_direction == SOUTH)
    {
        player->dir_x = 0;
        player->dir_y = 1;
    }
    if (player->starting_direction == EAST)
    {
        player->dir_x = 1;
        player->dir_y = 0;
    }
    if (player->starting_direction == WEST)
    {
        player->dir_x = -1;
        player->dir_y = 0;
    }
}

void render_player(t_game *game,t_img *img)
{
    int px;
    int py;
    
    move_player(&game->player);
    px = game->player.player_x * PX;
    py = game->player.player_y * PX;
    my_pixel_put(img, px, py, RED);
    for (int i = 0; i < 200; i++)
        my_pixel_put(img,px + game->player.dir_x * i, py + game->player.dir_y * i, RED);
    draw_texture(img, game->sprites.south.img, game->player.player_x * PX, game->player.player_y * PX, 20);
}
