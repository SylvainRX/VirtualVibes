#ifndef PLAYBACK_TICKS_PROVIDER_H_
#define PLAYBACK_TICKS_PROVIDER_H_

typedef struct PlaybackTicksProvider {
    int32 (*method)(void* object);
    void* object;
} PlaybackTicksProvider;

#endif
