#ifndef LANE_H_
#define LANE_H_

#include <Arrow.h>
#include <ArrowScheduler.h>
#include <ListenerObject.h>
#include <LaneLocation.h>
#include <Stage.h>
#include <VirtualList.h>

#define MAX_ARROWS 25

typedef struct LaneConfig
{
    ArrowScheduler arrowScheduler;
    Vector3D successLocation;
    uint8 scrollingSpeed;
    ScreenPixelVector arrowOrigin;
    Stage stage;
    int8 laneLocation;
    bool shareArrowList;
} LaneConfig;

abstract class Lane : ListenerObject
{
    uint8 laneId;
    ArrowScheduler arrowScheduler;
    VirtualList arrows;
    VirtualList laneArrows;
    ScreenPixelVector arrowOrigin;
    Vector3D successLocation;
    Stage stage;
    fixed_t arrowNormalDisplacementPerElapsedTime;
    uint8 arrowElapsedTimePerDisplacement;
    uint8 laneLocation;
    uint32 millisecondsToSuccessLocation;
    bool shareArrowList;
    uint32 lastTimerManagerTotalElapsedMilliseconds;
    fixed_t currentArrowDisplacement;
    bool slowDown;

    void constructor(LaneConfig config);
    void start();
    Arrow shootArrowWithSpec(ActorSpec* arrowSpec, uint8 arrowType);
    virtual bool attemptScore();
    virtual void failArrow(Arrow arrow);
    virtual void removeArrow(Arrow arrow);
    virtual void arrowDidReachSuccessLocation(Arrow arrow);
    virtual void arrowDidReachFailureLocation(Arrow arrow);
    uint32 millisecondsToSuccessLocation();
    void updateArrowNormalDisplacement(uint8 scrollingSpeed);
    void stop();
    virtual void displacedArrowsBy(fixed_t displacement);
    override bool handleMessage(Telegram telegram);
}

#endif
