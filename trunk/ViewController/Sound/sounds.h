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
    SOUND_READY = 0,
    SOUND_SHOOT,
    SOUND_EXPLOSION,
    SOUND_AKNOWLEDGEMENT,
    SOUND_ATTACK,
    SOUND_CONSTRUCTION_COMPLETE,
    SOUND_YES,
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
