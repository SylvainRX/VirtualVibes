#include <DebugMacros.h>
#include <GameEvents.h>
#include <Macros.h>
#include <Messages.h>
#include <Telegram.h>
#include <VirtualListExtensions.h>

#include "ArrowScheduler.h"

#include <Printer.h>

friend class TimerManager;

void ArrowScheduler::constructor()
{
    Base::constructor();
}

void ArrowScheduler::destructor()
{
    ArrowScheduler::discardAllMessages(this);

    Base::destructor();
}

void ArrowScheduler::setupSchedule(
    const ArrowSchedule* arrowSchedule,
    PlaybackTicksProvider playbackTicksProvider,
    uint32 millisecondToSuccessLocation,
    uint16 targetTimerResolutionUS
)
{
    this->arrowSchedule = arrowSchedule;
    this->playbackTicksProvider = playbackTicksProvider;
    this->millisecondToSuccessLocation = millisecondToSuccessLocation;
    this->songTicksDelay = MS_TO_TICKS(millisecondToSuccessLocation, targetTimerResolutionUS);
    this->nextArrowGroupPosition = 0;
    this->nextArrowGroup = this->arrowSchedule->scheduledArrows[this->nextArrowGroupPosition];
}

void ArrowScheduler::endSchedule()
{
    ArrowScheduler::discardAllMessages(this);
}

void ArrowScheduler::run()
{
    #ifdef ARROW_SCHEDULER_DISABLE
    return;
    #endif
    #ifdef ARROW_SCHEDULER_VALIDATE_SCHEDULE
    bool isValid = ArrowScheduler::validateArrowSchedule(this);
    if(!isValid)
        return;
    #endif
    ArrowScheduler::loop(this);
}

void ArrowScheduler::loop()
{
    if(ArrowScheduler::isFinished(this))
    {
        ArrowScheduler::sendMessageToSelf(this, kMessageArrowSchedulerEnd, this->millisecondToSuccessLocation + 1000, 0);
        return;
    }

    if(this->nextArrowGroup.timestamp <= ArrowScheduler::getPlayBackTicks(this) + this->songTicksDelay)
    {
        ArrowScheduler::process(this, this->nextArrowGroup.orientations);
        this->nextArrowGroup = this->arrowSchedule->scheduledArrows[++this->nextArrowGroupPosition];
    }
    ArrowScheduler::sendMessageToSelf(this, kMessageArrowSchedulerLoop, 20, 0);
}

bool ArrowScheduler::isFinished()
{
    return this->arrowSchedule->scheduledArrows[this->nextArrowGroupPosition].orientations == O_END;
}

int32 ArrowScheduler::getPlayBackTicks()
{
    return this->playbackTicksProvider.method(this->playbackTicksProvider.object);
}

bool ArrowScheduler::handleMessage(Telegram telegram)
{
    switch(Telegram::getMessage(telegram))
    {
        case kMessageArrowSchedulerLoop:
            ArrowScheduler::loop(this);
            return true;
        case kMessageArrowSchedulerEnd:
            ArrowScheduler::endSchedule(this);
            return true;
    }
    return Base::handleMessage(this, telegram);
}

#ifdef ARROW_SCHEDULER_VALIDATE_SCHEDULE
bool ArrowScheduler::validateArrowSchedule()
{
    uint32 lastTimestamp = 0;
    bool awaitingLeftEnd = false, awaitingRightEnd = false, awaitingUpEnd = false, awaitingDownEnd = false;

    uint16 i = 0;
    do
    {
        uint32 timestamp = this->arrowSchedule->scheduledArrows[i].timestamp;
        uint16 orientations = this->arrowSchedule->scheduledArrows[i].orientations;

        if (i > 0 && timestamp <= lastTimestamp)
        {
            PRINT_TEXT("Error: Timestamps must be strictly increasing.", 1, 4);
            PRINT_TEXT("Timestamp: ", 1, 5);
            PRINT_INT(timestamp, 12, 5);
            PRINT_INT(lastTimestamp, 12, 6);
            return false;
        }
        lastTimestamp = timestamp;

        if (awaitingLeftEnd && (orientations & (O_LEFT | O_LEFT_FOREGROUND | O_LEFT_START)))
        {
            PRINT_TEXT("Error: O_LEFT or related flag appears before", 1, 4);
            PRINT_TEXT("O_LEFT_END.", 1, 5);
            PRINT_TEXT("Timestamp: ", 1, 6);
            PRINT_INT(timestamp, 12, 6);
            return false;
        }
        if (awaitingRightEnd && (orientations & (O_RIGHT | O_RIGHT_FOREGROUND | O_RIGHT_START)))
        {
            PRINT_TEXT("Error: O_RIGHT or related flag appears before", 1, 4);
            PRINT_TEXT("O_RIGHT_END.", 1, 5);
            PRINT_TEXT("Timestamp: ", 1, 6);
            PRINT_INT(timestamp, 12, 6);
            return false;
        }
        if (awaitingUpEnd && (orientations & (O_UP | O_UP_START)))
        {
            PRINT_TEXT("Error: O_UP or related flag appears before", 1, 4);
            PRINT_TEXT("O_UP_END.", 1, 5);
            PRINT_TEXT("Timestamp: ", 1, 6);
            PRINT_INT(timestamp, 12, 6);
            return false;
        }
        if (awaitingDownEnd && (orientations & (O_DOWN | O_DOWN_START)))
        {
            PRINT_TEXT("Error: O_DOWN or related flag appears before", 1, 4);
            PRINT_TEXT("O_DOWN_END.", 1, 6);
            PRINT_TEXT("Timestamp: ", 1, 6);
            PRINT_INT(timestamp, 12, 6);
            return false;
        }

        if(orientations & O_LEFT_START) awaitingLeftEnd = true;
        if(orientations & O_RIGHT_START) awaitingRightEnd = true;
        if(orientations & O_UP_START) awaitingUpEnd = true;
        if(orientations & O_DOWN_START) awaitingDownEnd = true;

        if(orientations & O_LEFT_END) awaitingLeftEnd = false;
        if(orientations & O_RIGHT_END) awaitingRightEnd = false;
        if(orientations & O_UP_END) awaitingUpEnd = false;
        if(orientations & O_DOWN_END) awaitingDownEnd = false;
    }
    while (this->arrowSchedule->scheduledArrows[++i].orientations != O_END);

    // Ensure all started holds have matching ends
    if (awaitingLeftEnd || awaitingRightEnd || awaitingUpEnd || awaitingDownEnd)
    {
        PRINT_TEXT("Error: Schedule ends with a start flag", 1, 4);
        PRINT_TEXT("missing a corresponding end flag.", 1, 5);
        return false;
    }

    return true;
}
#endif
