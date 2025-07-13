#include <ArrowType.h>
#include <GameEvents.h>
#include <ShortArrowScheduler.h>

#include "ShortArrowLane.h"

void ShortArrowLane::constructor(ShortArrowLaneConfig config)
{
    Base::constructor(config.laneConfig);

    this->config = config;
    this->dPadPressEvent = ShortArrowLane::getDPadPressEvent(this);
    this->dPadReleaseEvent = ShortArrowLane::getDPadReleaseEvent(this);

    if(this->arrowScheduler != NULL)
        ArrowScheduler::addEventListener(
            this->arrowScheduler,
            ListenerObject::safeCast(this),
            ShortArrowLane::getSchedulerEventCode(this)
        );
}

void ShortArrowLane::destructor()
{
    if(this->arrowScheduler != NULL)
        ArrowScheduler::removeEventListener(
            this->arrowScheduler,
            ListenerObject::safeCast(this),
            ShortArrowLane::getSchedulerEventCode(this)
        );

    Base::destructor();
}

bool ShortArrowLane::onEvent(ListenerObject eventFirer, uint16 eventCode)
{
    switch(eventCode)
    {
        case kEventShortArrowSchedulerShootArrowLeft:
        case kEventShortArrowSchedulerShootArrowRight:
        case kEventShortArrowSchedulerShootArrowUp:
        case kEventShortArrowSchedulerShootArrowDown:
            ShortArrowLane::shootArrow(this);
            return true;
    }
    return Base::onEvent(this, eventFirer, eventCode);
}

void ShortArrowLane::shootArrow()
{
    Lane::shootArrowWithSpec(this, this->config.arrowSpec, kArrowTypePress);
}

uint16 ShortArrowLane::getSchedulerEventCode()
{
    switch(this->laneLocation)
    {
        case kLaneLocationLeft: return kEventShortArrowSchedulerShootArrowLeft;
        case kLaneLocationRight: return kEventShortArrowSchedulerShootArrowRight;
        case kLaneLocationUp: return kEventShortArrowSchedulerShootArrowUp;
        case kLaneLocationDown: return kEventShortArrowSchedulerShootArrowDown;
    }
    return 0;
}

uint16 ShortArrowLane::getDPadPressEvent()
{
    switch(this->laneLocation)
    {
        case kLaneLocationLeft: return kEventDPadPressedLeft;
        case kLaneLocationRight: return kEventDPadPressedRight;
        case kLaneLocationUp: return kEventDPadPressedUp;
        case kLaneLocationDown: return kEventDPadPressedDown;
    }
    return 0;
}

uint16 ShortArrowLane::getDPadReleaseEvent()
{
    switch(this->laneLocation)
    {
        case kLaneLocationLeft: return kEventDPadReleasedLeft;
        case kLaneLocationRight: return kEventDPadReleasedRight;
        case kLaneLocationUp: return kEventDPadReleasedUp;
        case kLaneLocationDown: return kEventDPadReleasedDown;
    }
    return 0;
}
