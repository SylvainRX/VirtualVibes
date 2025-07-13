#include <ArrowOrientation.h>
#include <BgmapSprite.h>
#include <Body.h>
#include <DownArrow.h>
#include <LeftArrow.h>
#include <Macros.h>
#include <Messages.h>
#include <RightArrow.h>
#include <Telegram.h>
#include <UpArrow.h>

#include "Arrow.h"

friend class Body;

void Arrow::constructor(ArrowSpec* arrowSpec, int16 internalId, const char* const name)
{
    Base::constructor((ActorSpec*)&arrowSpec->actorSpec, internalId, name);
    this->isFailed = false;
    this->didReachSuccessLocation = false;
    this->didReachFailureLocation = false;
    Container::streamOut(this, false);

    switch(arrowSpec->orientation)
    {
        case kArrowOrientationLeft:
            Arrow::mutateTo(this, LeftArrow::getClass());
            break;
        case kArrowOrientationRight:
            Arrow::mutateTo(this, RightArrow::getClass());
            break;
        case kArrowOrientationUp:
            Arrow::mutateTo(this, UpArrow::getClass());
            break;
        case kArrowOrientationDown:
            Arrow::mutateTo(this, DownArrow::getClass());
            break;
    }
}

void Arrow::destructor()
{
    Arrow::discardAllMessages(this);

    Base::destructor();
}

void Arrow::setExtraInfo(void* extraInfo)
{
    ArrowConfig* config = (ArrowConfig*)extraInfo;
    this->laneId = config->laneId;
    this->arrowType = config->arrowType;
    this->successLocation = config->successLocation;
    this->listener = config->listener;
}

void Arrow::fail()
{
    Sprite::hide(this->normalSprite);
    if(!isDeleted(this->failedSprite))
        Sprite::show(this->failedSprite);
    this->isFailed = true;
}

bool Arrow::isFailed()
{
    return this->isFailed;
}

uint8 Arrow::getArrowType()
{
    return this->arrowType;
}

uint8 Arrow::laneId()
{
    return this->laneId;
}

void Arrow::ready(bool recursive)
{
    this->normalSprite = Sprite::safeCast(Arrow::getComponentAtIndex(this, kSpriteComponent, 0));
    this->failedSprite = Sprite::safeCast(Arrow::getComponentAtIndex(this, kSpriteComponent, 1));
    if(!isDeleted(this->failedSprite))
        Sprite::hide(this->failedSprite);
    Base::ready(this, recursive);
}

void Arrow::displace(fixed_t distance __attribute__((unused))) {}

fixed_t Arrow::distanceFromSuccessLocation() { return 0; }

void Arrow::checkForPosition()
{
    fixed_t distanceToSuccessLocation = Arrow::distanceFromSuccessLocation(this);

    if(distanceToSuccessLocation <= 0 && this->didReachSuccessLocation == false)
    {
        Arrow::sendMessageTo(this, this->listener, kMessageArrowReachedSuccessLocation, 0, 0);
        this->didReachSuccessLocation = true;
    }
    else if(distanceToSuccessLocation <= -ARROW_SUCCESS_LOCATION_RADIUS && this->didReachFailureLocation == false)
    {
        Arrow::sendMessageTo(this, this->listener, kMessageArrowReachedFailureLocation, 0, 0);
        this->didReachFailureLocation = true;
    }
}
