/**
 * PlayerColors
 * Contains list of player colors
 */

#include <tchar.h>

#ifndef __PLAYERCOLORS_H__
#define __PLAYERCOLORS_H__

#define MAXPLAYERCOUNT 8

const TCHAR g_ppPlayerMaterialNames[MAXPLAYERCOUNT][256] =
{
    _T("PlayerMaterial0"),
    _T("PlayerMaterial1"),
    _T("PlayerMaterial2"),
    _T("PlayerMaterial3"),
    _T("PlayerMaterial4"),
    _T("PlayerMaterial5"),
    _T("PlayerMaterial6"),
    _T("PlayerMaterial7"),
};

const DWORD PLAYERCOLORS[MAXPLAYERCOUNT] =
{
    0xff000000, // player 0, null/neutral player
    0xffff0000, // player 1
    0xff00ff00, // player 2 etc.
    0xff0000ff,
    0xffff7700,
    0xffff00ff,
    0xff00ffff,
    0xffffffff
};

#endif
