#ifndef ARROW_H_
#define ARROW_H_

#include <Actor.h>
#include <BgmapSprite.h>

typedef struct ArrowSpec
{
    /// Actor spec
    ActorSpec actorSpec;

    /// Arrow Orientation
    uint8 orientation;
} ArrowSpec;

typedef const ArrowSpec ArrowROMSpec;

typedef struct ArrowConfig
{
    uint8 laneId;
    uint8 arrowType;
    Vector3D successLocation;
    ListenerObject listener;

} ArrowConfig;

class Arrow : Actor
{
    uint8 laneId;
    uint8 arrowType;
    Vector3D successLocation;
    ListenerObject listener;
    bool isFailed;

    Vector3D displacement;
    bool didReachSuccessLocation;
    bool didReachFailureLocation;

    Sprite normalSprite;
    Sprite failedSprite;

    void constructor(ArrowSpec* ArrowSpec, int16 internalId, const char* const name);
    void fail();
    bool isFailed();
    uint8 getArrowType();
    uint8 laneId();
    virtual void displace(fixed_t distance);
    virtual fixed_t distanceFromSuccessLocation();
    void checkForPosition();
    void stop();

    override void setExtraInfo(void* extraInfo);
    override void ready(bool recursive);
}

#endif
