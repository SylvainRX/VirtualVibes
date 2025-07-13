#include <ArrowType.h>
#include <Body.h>
#include <DebugMacros.h>
#include <GameEvents.h>
#include <Macros.h>
#include <Messages.h>
#include <ScoreAttemptResult.h>
#include <ScoreManager.h>
#include <ScrollingSpeed.h>
#include <Telegram.h>
#include <Vector3D.h>
#include <VirtualListExtensions.h>
#include <VUEngine.h>

#include "Lane.h"

#if defined LANE_PRINT_MAX_ARROW_COUNT
#include <Printer.h>
#endif

friend class TimerManager;
friend class VirtualList;
friend class VirtualNode;

static uint8 laneCount = 0;
static uint8 allLanesArrowCount = 0;

static VirtualList arrowsList[LANE_LOCATIONS_COUNT];

static VirtualList Lane::arrowsFor(uint8 laneLocation)
{
    if(isDeleted(arrowsList[laneLocation]))
        arrowsList[laneLocation] = new VirtualList();
    return arrowsList[laneLocation];
}

static void Lane::deleteArrowsFor(uint8 laneLocation)
{
    if(isDeleted(arrowsList[laneLocation]))
        return;
    delete arrowsList[laneLocation];
    arrowsList[laneLocation] = NULL;
}

static uint8 Lane::canAddArrow()
{
    bool canAddArrow = allLanesArrowCount != MAX_ARROWS;

    #ifdef LANE_PRINT_MAX_ARROW_COUNT
    if(canAddArrow == false)
        PRINT_TEXT("FAILED TO ADD ARROW", 1, PRINTING_HEIGHT - 2);
    #endif

    return canAddArrow;
}

static void Lane::incrementAllLanesArrowCount()
{
    allLanesArrowCount++;

    #ifdef LANE_PRINT_MAX_ARROW_COUNT
    PRINT_TEXT("All lanes arrow count:    (+)", 1, PRINTING_HEIGHT - 1);
    PRINT_INT(allLanesArrowCount, 24, PRINTING_HEIGHT - 1);
    #endif
}

static void Lane::decrementAllLanesArrowCount()
{
    allLanesArrowCount--;

    #ifdef LANE_PRINT_MAX_ARROW_COUNT
    PRINT_TEXT("All lanes arrow count:    (-)", 1, PRINTING_HEIGHT - 1);
    PRINT_INT(allLanesArrowCount, 24, PRINTING_HEIGHT - 1);
    #endif
}

static void Lane::decrementAllLanesArrowCountBy(uint8 count)
{
    allLanesArrowCount -= count;

    #ifdef LANE_PRINT_MAX_ARROW_COUNT
    PRINT_TEXT("All lanes arrow count:    ", 1, PRINTING_HEIGHT - 1);
    PRINT_INT(allLanesArrowCount, 24, PRINTING_HEIGHT - 1);
    #endif
}

void Lane::constructor(LaneConfig config)
{
    Base::constructor();

    this->arrowScheduler = config.arrowScheduler;
    this->successLocation = config.successLocation;
    this->arrowOrigin = config.arrowOrigin;
    this->stage = config.stage;
    this->laneLocation = config.laneLocation;
    this->shareArrowList = config.shareArrowList;

    this->laneId = laneCount++;
    this->arrowNormalDisplacementPerElapsedTime = Lane::arrowNormalDisplacementPerElapsedTime(config.scrollingSpeed);
    this->arrowElapsedTimePerDisplacement = Lane::arrowElapsedTimePerDisplacement(config.scrollingSpeed);
    this->millisecondsToSuccessLocation = Lane::millisecondsToSuccessLocation(this);

    this->arrows = this->shareArrowList ? Lane::arrowsFor(config.laneLocation) : new VirtualList();
    this->laneArrows = new VirtualList();

    this->lastTimerManagerTotalElapsedMilliseconds = 0;
    this->currentArrowDisplacement = 0;
    this->slowDown = false;

    SoundManager::addEventListener(
        SoundManager::getInstance(),
        (ListenerObject)this,
        kEventPlaySounds
    );

    Lane::start(this);
}

void Lane::destructor()
{
    if(!isDeleted(this->arrows))
        Lane::decrementAllLanesArrowCountBy(VirtualList::getCount(this->arrows));

    if(this->shareArrowList)
        Lane::deleteArrowsFor(this->laneLocation);
    else
        delete this->arrows;

    delete this->laneArrows;

    this->arrows = NULL;
    laneCount--;

    Base::destructor();
}

void Lane::start()
{
    Lane::sendMessageToSelf(this, kMessageLaneUpdate, 0, 0);
}

Arrow Lane::shootArrowWithSpec(ActorSpec* arrowSpec, uint8 arrowType)
{
    if(Lane::canAddArrow() == false)
        return NULL;

    Lane::incrementAllLanesArrowCount();
    PositionedActor arrowPositionedActor = {arrowSpec, this->arrowOrigin, {0, 0, 0}, {1, 1, 1}, 0, NULL, NULL, NULL, false};
    Arrow arrow = Arrow::safeCast(Stage::spawnChildActor(this->stage, (const PositionedActor* const)&arrowPositionedActor, false));
    ArrowConfig arrowConfig = (ArrowConfig){
        this->laneId,
        arrowType,
        this->successLocation,
        ListenerObject::safeCast(this)
    };
    Arrow::setExtraInfo(arrow, &arrowConfig);
    VirtualList::pushBack(this->arrows, arrow);
    VirtualList::pushBack(this->laneArrows, arrow);
    return arrow;
}

bool Lane::attemptScore()
{
    Arrow arrow = Arrow::safeCast(VirtualList::front(this->arrows));

    if(isDeleted(arrow) || Arrow::getArrowType(arrow) != kArrowTypePress || Arrow::laneId(arrow) != this->laneId || Arrow::isFailed(arrow))
        return false;

    Vector3D arrowPosition = *Arrow::getPosition(arrow);
    Vector3D vectorToSuccessLocation = Vector3D::sub(arrowPosition, this->successLocation);
    uint16 distanceToSuccessLocation = __METERS_TO_PIXELS(Vector3D::length(vectorToSuccessLocation));

    uint8 scoreAttemptResult = ScoreManager::handleScoreAttempt(
        ScoreManager::getInstance(),
        distanceToSuccessLocation,
        this->laneLocation,
        Arrow::getArrowType(arrow)
    );

    if(scoreAttemptResult == kScoreAttemptResultSuccess)
        Lane::removeArrow(this, arrow);
    else if(scoreAttemptResult == kScoreAttemptResultFailure)
        Lane::failArrow(this, arrow);

    return true;
}

void Lane::failArrow(Arrow arrow)
{
    if(Arrow::isFailed(arrow))
        return;

    ScoreManager::handleArrowHit(ScoreManager::getInstance());
    Arrow::fail(arrow);
}

void Lane::removeArrow(Arrow arrow)
{
    VirtualList::removeData(this->arrows, arrow);
    VirtualList::removeData(this->laneArrows, arrow);
    Lane::decrementAllLanesArrowCount();
    Arrow::deleteMyself(arrow);
}

void Lane::arrowDidReachSuccessLocation(Arrow arrow)
{
    Arrow::hide(arrow);
}

void Lane::arrowDidReachFailureLocation(Arrow arrow)
{
    Lane::failArrow(this, arrow);
    Lane::removeArrow(this, arrow);
}

uint32 Lane::millisecondsToSuccessLocation()
{
    return Lane::millisecondsToLocation(this, this->successLocation);
}

uint32 Lane::millisecondsToLocation(Vector3D targetLocation)
{
    fixed_t metersToTargetLocation = Vector3D::length(Vector3D::sub(Vector3D::getFromScreenPixelVector(this->arrowOrigin), targetLocation));
    return __FIXED_TO_I(__FIXED_DIV(metersToTargetLocation, this->arrowNormalDisplacementPerElapsedTime)) * this->arrowElapsedTimePerDisplacement;
}

static fixed_t Lane::arrowNormalDisplacementPerElapsedTime(uint8 scrollingSpeed)
{
    switch(scrollingSpeed)
    {
        case kScrollingSpeedSlow: return ARROW_SLOW_DISPLACEMENT_PER_ELAPSED_TIME;
        case kScrollingSpeedNormal: return ARROW_NORMAL_DISPLACEMENT_PER_ELAPSED_TIME;
        case kScrollingSpeedFast: return ARROW_FAST_DISPLACEMENT_PER_ELAPSED_TIME;
    }
    return 0;
}

static uint8 Lane::arrowElapsedTimePerDisplacement(uint8 scrollingSpeed)
{
    switch(scrollingSpeed)
    {
        case kScrollingSpeedSlow: return ARROW_SLOW_ELAPSED_TIME_PER_DISPLACEMENT;
        case kScrollingSpeedNormal: return ARROW_NORMAL_ELAPSED_TIME_PER_DISPLACEMENT;
        case kScrollingSpeedFast: return ARROW_FAST_ELAPSED_TIME_PER_DISPLACEMENT;
    }
    return 0;
}

void Lane::updateArrowNormalDisplacement(uint8 scrollingSpeed)
{
    this->arrowNormalDisplacementPerElapsedTime = Lane::arrowNormalDisplacementPerElapsedTime(scrollingSpeed);
    this->arrowElapsedTimePerDisplacement = Lane::arrowElapsedTimePerDisplacement(scrollingSpeed);
    this->millisecondsToSuccessLocation = Lane::millisecondsToSuccessLocation(this);
}

void Lane::update()
{
    TimerManager timerManager = TimerManager::getInstance();
    uint32 updateElapsedMilliseconds = timerManager->totalElapsedMilliseconds - this->lastTimerManagerTotalElapsedMilliseconds;
    if(updateElapsedMilliseconds >= this->arrowElapsedTimePerDisplacement)
    {
        this->lastTimerManagerTotalElapsedMilliseconds = timerManager->totalElapsedMilliseconds;

        if(!this->slowDown)
            this->currentArrowDisplacement = __FIXED_MULT(this->arrowNormalDisplacementPerElapsedTime, __FIXED_DIV(__I_TO_FIXED(updateElapsedMilliseconds), __I_TO_FIXED(this->arrowElapsedTimePerDisplacement)));
        else
            this->currentArrowDisplacement = this->currentArrowDisplacement > 0 ? __FIXED_MULT(this->currentArrowDisplacement, __F_TO_FIXED(0.99f)) : this->currentArrowDisplacement - __F_TO_FIXED(0.01f);

        if(VirtualList::isNotEmpty(this->laneArrows))
        {
            for(VirtualNode node = this->laneArrows->tail; NULL != node; node = node->previous)
                Arrow::displace(Arrow::safeCast(node->data), this->currentArrowDisplacement);

            Arrow::checkForPosition(Arrow::safeCast(this->laneArrows->head->data));
        }
        Lane::displacedArrowsBy(this, this->currentArrowDisplacement);
    }

    Lane::sendMessageToSelf(this, kMessageLaneUpdate, 10, 0);
}

void Lane::displacedArrowsBy(fixed_t displacement __attribute__((unused))) {}

void Lane::stop()
{
    this->slowDown = true;
    Lane::discardMessages(this, kMessageArrowReachedFailureLocation);
    Lane::discardMessages(this, kMessageArrowReachedSuccessLocation);
}

bool Lane::handleMessage(Telegram telegram)
{
    switch(Telegram::getMessage(telegram))
    {
        case kMessageLaneUpdate:
            Lane::update(this);
            return true;
        case kMessageArrowReachedFailureLocation:
            Lane::arrowDidReachFailureLocation(this, Arrow::safeCast(Telegram::getSender(telegram)));
            return true;
        case kMessageArrowReachedSuccessLocation:
            Lane::arrowDidReachSuccessLocation(this, Arrow::safeCast(Telegram::getSender(telegram)));
            return true;
    }
    return Base::handleMessage(this, telegram);
}
