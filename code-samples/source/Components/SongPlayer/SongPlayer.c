#include <DebugMacros.h>
#include <Macros.h>
#include <Messages.h>
#include <MusicSpeed.h>
#include <SoundManager.h>
#include <Telegram.h>
#include <TimerManager.h>
#include <VirtualList.h>
#include <KeypadManager.h>

#include "SongPlayer.h"

#ifdef SONG_PLAYER_PRINT_PLAYBACK_TICKS
#include <Math.h>
#include <Printer.h>
#endif

friend class Sound;
friend class SoundTrack;
friend class VirtualList;
friend class VirtualNode;

void SongPlayer::constructor()
{
    Base::constructor();

    this->sound = NULL;
}

void SongPlayer::destructor()
{
    SongPlayer::releaseSound(this);

    Base::destructor();
}

void SongPlayer::play(const LevelSong levelSong, ListenerObject listenerObject)
{
    SongPlayer::playLevelSong(this, levelSong, kMusicSpeedNormal, 0, listenerObject);
}

void SongPlayer::playLevelSong(
    const LevelSong levelSong,
    uint8 musicSpeed,
    uint32 longestMillisecondsToSuccessLocation,
    ListenerObject listenerObject
)
{
    KeypadManager::disable();

    SongPlayer::releaseSound(this);
    this->levelSong = levelSong;
    this->soundSpec = *levelSong.soundSpec;
    this->soundSpec.targetTimerResolutionUS = SongPlayer::getTargetTimerResolution(this, musicSpeed);
    SongPlayer::setTimerSettings(this, this->soundSpec.targetTimerResolutionUS);
    this->sound = SoundManager::getSound((SoundSpec*)&this->soundSpec, listenerObject);
    Sound::addEventListener(this->sound, (ListenerObject)this, kEventSoundFinished);
    Sound::addEventListener(this->sound, listenerObject, kEventSoundFinished);
    SongPlayer::applyTimerSettings(this);
    Sound::autoReleaseOnFinish(this->sound, false);

    SongPlayer::playSound(this, longestMillisecondsToSuccessLocation);

    #ifdef SONG_PLAYER_PRINT_PLAYBACK_TICKS
    SongPlayer::startPrintingTicks(this);
    #endif

    KeypadManager::enable();
}

void SongPlayer::playSound(uint32 longestMillisecondsToSuccessLocation)
{
    this->sound->volumeReduction = 0;
    this->sound->playbackType = kSoundPlaybackNormal;
    this->sound->state = kSoundPlaying;
    this->sound->totalElapsedTicks = -__I_TO_FIX7_9_EXT(MS_TO_TICKS(longestMillisecondsToSuccessLocation, this->soundSpec.targetTimerResolutionUS));

    for(VirtualNode node = this->sound->soundTracks->head; NULL != node; node = node->next)
    {
        SoundTrack soundTrack = SoundTrack::safeCast(node->data);
        soundTrack->elapsedTicks = -__I_TO_FIX7_9_EXT(MS_TO_TICKS(longestMillisecondsToSuccessLocation, this->soundSpec.targetTimerResolutionUS));
    }
}

void SongPlayer::stop()
{
    if(isDeleted(this->sound) || !Sound::isPlaying(this->sound))
        return;

    Sound::pause(this->sound);
    Sound::stop(this->sound);
    SongPlayer::releaseSound(this);

    #ifdef SONG_PLAYER_PRINT_PLAYBACK_TICKS
    SongPlayer::discardMessages(this, kMessagePrintSoundInfo);
    #endif
}

void SongPlayer::releaseSound()
{
    if(isDeleted(this->sound))
        return;

    Sound::release(this->sound);
    this->sound = NULL;
}

void SongPlayer::setTimerSettings(uint16 targetTimerResolutionUS)
{
    TimerManager::setResolution(__TIMER_100US);
    TimerManager::setTargetTimePerInterruptUnits(kUS);
    TimerManager::setTargetTimePerInterrupt(targetTimerResolutionUS);
}

void SongPlayer::applyTimerSettings()
{
    TimerManager::applySettings(true);
}

void SongPlayer::setMusicSpeed(uint8 musicSpeed)
{
    if(isDeleted(this->sound))
        return;

    uint16 targetTimerResolutionUS = SongPlayer::getTargetTimerResolution(this, musicSpeed);
    this->soundSpec.targetTimerResolutionUS = targetTimerResolutionUS;
    SongPlayer::setTimerSettings(this, targetTimerResolutionUS);
    Sound::setSpeed(this->sound, __F_TO_FIX7_9_EXT(1.0f));
    SongPlayer::applyTimerSettings(this);
}

uint16 SongPlayer::getTargetTimerResolution(uint8 musicSpeed)
{
    switch(musicSpeed)
    {
        case kMusicSpeedSlow: return this->levelSong.slowTargetTimerResolutionUS;
        case kMusicSpeedNormal: return this->levelSong.normalTargetTimerResolutionUS;
        case kMusicSpeedFast: return this->levelSong.fastTargetTimerResolutionUS;
    }
    return 0;
}

uint16 SongPlayer::getTargetTimerResolutionUS()
{
    return this->soundSpec.targetTimerResolutionUS;
}

int32 SongPlayer::getElapsedTicks()
{
    if (isDeleted(this->sound))
        return 0;
    return Sound::getTotalElapsedTicks(this->sound);
}

#ifdef SONG_PLAYER_PRINT_PLAYBACK_TICKS

void SongPlayer::startPrintingTicks()
{
    this->totalTicks = this->sound->mainSoundTrack->ticks;
    this->lastMainSoundTrackCursor = 0;
    this->mainSoundTrackTimeStamp = 0;
    PRINT_TEXT("Ticks:      ", 1, 1);
    PRINT_TEXT("TS:    0 [0]", 1, 2);
    SongPlayer::printPlaybackTicks(this);
}

void SongPlayer::printPlaybackTicks()
{
    if(isDeleted(this->sound))
        return;

    uint32 elapsedTicks = SongPlayer::getElapsedTicks(this);
    uint32 elapsedTicksDigitCount = Math::getDigitsCount(elapsedTicks);
    PRINT_TEXT("Ticks: ", 1, 1);
    PRINT_TEXT("TS:    ", 1, 2);
    PRINT_INT(elapsedTicks, 8, 1);
    PRINT_TEXT("/", 8 + elapsedTicksDigitCount, 1);
    PRINT_INT(this->totalTicks, 9 + elapsedTicksDigitCount, 1);

    if(this->sound->mainSoundTrack->cursor > this->lastMainSoundTrackCursor)
    {
        do this->mainSoundTrackTimeStamp += this->sound->mainSoundTrack->soundTrackSpec->trackKeyframes[this->lastMainSoundTrackCursor].tick;
        while(++this->lastMainSoundTrackCursor <= this->sound->mainSoundTrack->cursor);

        uint32 mainSoundTrackTimeStampDigitCount = Math::getDigitsCount(this->mainSoundTrackTimeStamp);
        uint32 lastMainSoundTrackCursorDigitCount = Math::getDigitsCount(this->lastMainSoundTrackCursor);
        PRINT_TEXT("TS:", 1, 2);
        PRINT_INT(this->mainSoundTrackTimeStamp, 8, 2);
        PRINT_TEXT(" [", 8 + mainSoundTrackTimeStampDigitCount, 2);
        PRINT_INT(this->lastMainSoundTrackCursor, 10 + mainSoundTrackTimeStampDigitCount, 2);
        PRINT_TEXT("]", 10 + mainSoundTrackTimeStampDigitCount + lastMainSoundTrackCursorDigitCount, 2);
    }

    SongPlayer::sendMessageToSelf(this, kMessagePrintSoundInfo, 10, 0);
}

#endif

bool SongPlayer::handleMessage(Telegram telegram)
{
    switch(Telegram::getMessage(telegram))
    {
        #ifdef SONG_PLAYER_PRINT_PLAYBACK_TICKS
        case kMessagePrintSoundInfo:
            SongPlayer::printPlaybackTicks(this);
            return true;
        #endif

        case kEventSoundFinished:
            SongPlayer::releaseSound(this);
            return true;

        default:
            return Base::handleMessage(this, telegram);
    }

}
