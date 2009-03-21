/**
 *
 * Sound-related stuff & defines
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __SOUNDS_H__
#define __SOUNDS_H__

/**
 * Different kinds of sounds
 */
enum SoundTypes 
{
    SOUND_DEBUG = 0,
    SOUND_READY,
    SOUND_SHOOT,
    SOUND_OK,
    SOUND_NO,
    __END = 0xff
};

/**
 * Different kinds of music
 */
enum MusicTypes
{
    MUSIC_BACKGROUND = 0,
    _END = 0xff
};

#endif // __SOUNDS_H__
