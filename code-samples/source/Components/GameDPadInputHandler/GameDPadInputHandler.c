#include <DebugMacros.h>
#include <GameEvents.h>
#include <Messages.h>
#include <Telegram.h>

#include "GameDPadInputHandler.h"

void GameDPadInputHandler::constructor(GameDPadInputHandlerConfig config)
{
    Base::constructor();

    this->inputEventSenders = config.inputEventSenders;
    this->leftDPadButton = config.leftDPadButton;
    this->rightDPadButton = config.rightDPadButton;
    this->upDPadButton = config.upDPadButton;
    this->downDPadButton = config.downDPadButton;
    this->shortArrowLeftLane = config.shortArrowLeftLane;
    this->shortArrowRightLane = config.shortArrowRightLane;
    this->shortArrowUpLane = config.shortArrowUpLane;
    this->shortArrowDownLane = config.shortArrowDownLane;
    this->longArrowLeftLane = config.longArrowLeftLane;
    this->longArrowRightLane = config.longArrowRightLane;
    this->longArrowUpLane = config.longArrowUpLane;
    this->longArrowDownLane = config.longArrowDownLane;
    this->foregroundArrowLeftLane = config.foregroundArrowLeftLane;
    this->foregroundArrowRightLane = config.foregroundArrowRightLane;

    GameDPadInputHandler::addListenerToInputEventSenders(this);
}

void GameDPadInputHandler::destructor()
{
    GameDPadInputHandler::discardAllMessages(this);

    this->foregroundArrowRightLane = NULL;
    this->foregroundArrowLeftLane = NULL;
    this->longArrowDownLane = NULL;
    this->longArrowUpLane = NULL;
    this->longArrowRightLane = NULL;
    this->longArrowLeftLane = NULL;
    this->shortArrowDownLane = NULL;
    this->shortArrowUpLane = NULL;
    this->shortArrowRightLane = NULL;
    this->shortArrowLeftLane = NULL;
    this->downDPadButton = NULL;
    this->upDPadButton = NULL;
    this->rightDPadButton = NULL;
    this->leftDPadButton = NULL;
    this->inputEventSenders = NULL;

    Base::destructor();
}

void GameDPadInputHandler::handleLeftPress()
{
    ForegroundArrowLane::releaseTriggerButton(this->foregroundArrowLeftLane);
    DPadButton::press(this->leftDPadButton);
    if(ShortArrowLane::attemptScore(this->shortArrowLeftLane))
        return;
    if(LongArrowLane::attemptScore(this->longArrowLeftLane))
        return;
}

void GameDPadInputHandler::handleLeftRelease()
{
    DPadButton::release(this->leftDPadButton);
    LongArrowLane::releaseDPadButton(this->longArrowLeftLane);
}

void GameDPadInputHandler::handleRightPress()
{
    ForegroundArrowLane::releaseTriggerButton(this->foregroundArrowRightLane);
    DPadButton::press(this->rightDPadButton);
    if(ShortArrowLane::attemptScore(this->shortArrowRightLane))
        return;
    if(LongArrowLane::attemptScore(this->longArrowRightLane))
        return;
}

void GameDPadInputHandler::handleRightRelease()
{
    DPadButton::release(this->rightDPadButton);
    LongArrowLane::releaseDPadButton(this->longArrowRightLane);
}

void GameDPadInputHandler::handleUpPress()
{
    DPadButton::press(this->upDPadButton);
    if(ShortArrowLane::attemptScore(this->shortArrowUpLane))
        return;
    if(LongArrowLane::attemptScore(this->longArrowUpLane))
        return;
}

void GameDPadInputHandler::handleUpRelease()
{
    DPadButton::release(this->upDPadButton);
    LongArrowLane::releaseDPadButton(this->longArrowUpLane);
}

void GameDPadInputHandler::handleDownPress()
{
    DPadButton::press(this->downDPadButton);
    if(ShortArrowLane::attemptScore(this->shortArrowDownLane))
        return;
    if(LongArrowLane::attemptScore(this->longArrowDownLane))
        return;
}

void GameDPadInputHandler::handleDownRelease()
{
    DPadButton::release(this->downDPadButton);
    LongArrowLane::releaseDPadButton(this->longArrowDownLane);
}

void GameDPadInputHandler::handleLeftTriggerPress()
{
    ForegroundArrowLane::attemptScore(this->foregroundArrowLeftLane);
    LongArrowLane::releaseDPadButton(this->longArrowLeftLane);
    DPadButton::foreground(this->leftDPadButton);
}

void GameDPadInputHandler::handleLeftTriggerRelease()
{
    ForegroundArrowLane::releaseTriggerButton(this->foregroundArrowLeftLane);
    DPadButton::background(this->leftDPadButton);
}

void GameDPadInputHandler::handleRightTriggerPress()
{
    ForegroundArrowLane::attemptScore(this->foregroundArrowRightLane);
    LongArrowLane::releaseDPadButton(this->longArrowRightLane);
    DPadButton::foreground(this->rightDPadButton);
}

void GameDPadInputHandler::handleRightTriggerRelease()
{
    ForegroundArrowLane::releaseTriggerButton(this->foregroundArrowRightLane);
    DPadButton::background(this->rightDPadButton);
}

bool GameDPadInputHandler::onEvent(ListenerObject eventFirer, uint16 eventCode)
{
    switch(eventCode)
    {
        case kEventDPadPressedLeft:
            GameDPadInputHandler::handleLeftPress(this);
            return true;
        case kEventDPadReleasedLeft:
            GameDPadInputHandler::handleLeftRelease(this);
            return true;
        case kEventDPadPressedRight:
            GameDPadInputHandler::handleRightPress(this);
            return true;
        case kEventDPadReleasedRight:
            GameDPadInputHandler::handleRightRelease(this);
            return true;
        case kEventDPadPressedUp:
            GameDPadInputHandler::handleUpPress(this);
            return true;
        case kEventDPadReleasedUp:
            GameDPadInputHandler::handleUpRelease(this);
            return true;
        case kEventDPadPressedDown:
            GameDPadInputHandler::handleDownPress(this);
            return true;
        case kEventDPadReleasedDown:
            GameDPadInputHandler::handleDownRelease(this);
            return true;
        case kEventDPadPressedLeftTrigger:
            GameDPadInputHandler::handleLeftTriggerPress(this);
            return true;
        case kEventDPadReleasedLeftTrigger:
            GameDPadInputHandler::handleLeftTriggerRelease(this);
            return true;
        case kEventDPadPressedRightTrigger:
            GameDPadInputHandler::handleRightTriggerPress(this);
            return true;
        case kEventDPadReleasedRightTrigger:
            GameDPadInputHandler::handleRightTriggerRelease(this);
            return true;
    }
    return Base::onEvent(this, eventFirer, eventCode);
}

void GameDPadInputHandler::addListenerToInputEventSenders()
{
    for(uint8 i = 0; !isDeleted(this->inputEventSenders[i]); i++)
    {
        ListenerObject inputEventSender = this->inputEventSenders[i];
        ListenerObject::addEventListener(
            inputEventSender,
            ListenerObject::safeCast(this),
            kEventDPadPressedLeft
        );
        ListenerObject::addEventListener(
            inputEventSender,
            ListenerObject::safeCast(this),
            kEventDPadReleasedLeft
        );
        ListenerObject::addEventListener(
            inputEventSender,
            ListenerObject::safeCast(this),
            kEventDPadPressedRight
        );
        ListenerObject::addEventListener(
            inputEventSender,
            ListenerObject::safeCast(this),
            kEventDPadReleasedRight
        );
        ListenerObject::addEventListener(
            inputEventSender,
            ListenerObject::safeCast(this),
            kEventDPadPressedUp
        );
        ListenerObject::addEventListener(
            inputEventSender,
            ListenerObject::safeCast(this),
            kEventDPadReleasedUp
        );
        ListenerObject::addEventListener(
            inputEventSender,
            ListenerObject::safeCast(this),
            kEventDPadPressedDown
        );
        ListenerObject::addEventListener(
            inputEventSender,
            ListenerObject::safeCast(this),
            kEventDPadReleasedDown
        );
        ListenerObject::addEventListener(
            inputEventSender,
            ListenerObject::safeCast(this),
            kEventDPadPressedLeftTrigger
        );
        ListenerObject::addEventListener(
            inputEventSender,
            ListenerObject::safeCast(this),
            kEventDPadReleasedLeftTrigger
        );
        ListenerObject::addEventListener(
            inputEventSender,
            ListenerObject::safeCast(this),
            kEventDPadPressedRightTrigger
        );
        ListenerObject::addEventListener(
            inputEventSender,
            ListenerObject::safeCast(this),
            kEventDPadReleasedRightTrigger
        );
    }
}
