/**
 * PlayerColors
 * Contains list of player colors
 */


#ifndef __PLAYERCOLORS_H__
#define __PLAYERCOLORS_H__

#define MAXPLAYERCOUNT 8

const DWORD PLAYERCOLORS[MAXPLAYERCOUNT] =
{
    0xff000000, // player 0, null/neutral player
    0xffff0000, // player 1
    0xff00ff00, // player 2 etc.
    0xff0000ff,
    0xffffff00,
    0xffff00ff,
    0xff00ffff,
    0xffffffff
};

#endif
