#ifndef SHORT_ARROW_LANE_H_
#define SHORT_ARROW_LANE_H_

#include <Lane.h>

typedef struct ShortArrowLaneConfig
{
    LaneConfig laneConfig;
    ActorSpec* arrowSpec;
} ShortArrowLaneConfig;

class ShortArrowLane : Lane
{
    ShortArrowLaneConfig config;
    uint16 dPadPressEvent;
    uint16 dPadReleaseEvent;

    void constructor(ShortArrowLaneConfig config);
    void shootArrow();
    override bool onEvent(ListenerObject eventFirer, uint16 eventCode);
}

#endif
