#ifndef ARROW_SCHEDULER_H_
#define ARROW_SCHEDULER_H_

#include <ArrowSchedule.h>
#include <ListenerObject.h>
#include <PlaybackTicksProvider.h>
#include <VirtualList.h>

abstract class ArrowScheduler : ListenerObject
{
    const ArrowSchedule* arrowSchedule;
    PlaybackTicksProvider playbackTicksProvider;
    uint32 millisecondToSuccessLocation;

    uint16 songTicksDelay;
    uint32 nextArrowGroupPosition;
    ScheduledArrowGroup nextArrowGroup;

    void constructor();
    void setupSchedule(
        const ArrowSchedule* arrowSchedule,
        PlaybackTicksProvider playbackTicksProvider,
        uint32 millisecondToSuccessLocation,
        uint16 targetTimerResolutionUS
    );
    void run();
    void endSchedule();
    virtual void process(uint16 orientations) = 0;

    override bool handleMessage(Telegram telegram);
}

#endif
