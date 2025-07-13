#ifndef GAME_DPAD_INPUT_HANDLER_H_
#define GAME_DPAD_INPUT_HANDLER_H_

#include <DPadButton.h>
#include <ForegroundArrowLane.h>
#include <ListenerObject.h>
#include <LongArrowLane.h>
#include <ShortArrowLane.h>

typedef struct GameDPadInputHandlerConfig
{
    ListenerObject* inputEventSenders;
    DPadButton leftDPadButton;
    DPadButton rightDPadButton;
    DPadButton upDPadButton;
    DPadButton downDPadButton;
    ShortArrowLane shortArrowLeftLane;
    ShortArrowLane shortArrowRightLane;
    ShortArrowLane shortArrowUpLane;
    ShortArrowLane shortArrowDownLane;
    LongArrowLane longArrowLeftLane;
    LongArrowLane longArrowRightLane;
    LongArrowLane longArrowUpLane;
    LongArrowLane longArrowDownLane;
    ForegroundArrowLane foregroundArrowLeftLane;
    ForegroundArrowLane foregroundArrowRightLane;
} GameDPadInputHandlerConfig;

class GameDPadInputHandler : ListenerObject
{
    ListenerObject* inputEventSenders;
    DPadButton leftDPadButton;
    DPadButton rightDPadButton;
    DPadButton upDPadButton;
    DPadButton downDPadButton;
    ShortArrowLane shortArrowLeftLane;
    ShortArrowLane shortArrowRightLane;
    ShortArrowLane shortArrowUpLane;
    ShortArrowLane shortArrowDownLane;
    LongArrowLane longArrowLeftLane;
    LongArrowLane longArrowRightLane;
    LongArrowLane longArrowUpLane;
    LongArrowLane longArrowDownLane;
    ForegroundArrowLane foregroundArrowLeftLane;
    ForegroundArrowLane foregroundArrowRightLane;

    void constructor(GameDPadInputHandlerConfig config);
    override bool onEvent(ListenerObject eventFirer, uint16 eventCode);
}

#endif
