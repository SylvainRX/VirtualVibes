#ifndef SONG_PLAYER_H_
#define SONG_PLAYER_H_

#include <LevelSong.h>
#include <ListenerObject.h>
#include <Sound.h>

class SongPlayer : ListenerObject
{
    LevelSong levelSong;
    Sound sound;
    SoundSpec soundSpec;
    ListenerObject listenerObject;
    uint16 durationIndex;
    uint16 volumeIndex;
    uint16 trackSize;
    uint32 totalTicks;
    uint32 mainSoundTrackTimeStamp;
    uint32 lastMainSoundTrackCursor;

    void constructor();
    void play(const LevelSong levelSong, ListenerObject listenerObject);
    void playLevelSong(
        const LevelSong levelSong,
        uint8 musicSpeed,
        uint32 longestMillisecondsToSuccessLocation,
        ListenerObject listenerObject
    );
    void stop();
    void setMusicSpeed(uint8 musicSpeed);
    uint16 getTargetTimerResolutionUS();
    int32 getElapsedTicks();

    override bool handleMessage(Telegram telegram);
}

#endif
