#ifndef ROUTER_H_
#define ROUTER_H_

#include <GameState.h>
#include <ListenerObject.h>

singleton class Router : ListenerObject
{
    int8 nextGameStateId;
    int8 currentGameStateId;
    GameState currentGameState;

    GameState getCurrentGameState();
    override bool onEvent(ListenerObject eventFirer, uint16 eventCode);
}

#endif
