/**
 * My Project
 *
 * © Sylvain Roux
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this source code.
 */


//---------------------------------------------------------------------------------------------------------
//                                                 INCLUDES
//---------------------------------------------------------------------------------------------------------

#include <AdjustmentScreenState.h>
#include <DebugMacros.h>
#include <GameStateId.h>
#include <PrecautionScreenState.h>
#include <Router.h>
#include <SplashScreenState.h>
#include <VUEngine.h>

GameState game(void)
{
    #if defined(__RELEASE) && !defined(DEBUG_RELEASE)
    SplashScreenState::setNextState(
        SplashScreenState::safeCast(PrecautionScreenState::getInstance()),
        GameState::safeCast(AdjustmentScreenState::getInstance())
    );
    SplashScreenState::setNextState(
        SplashScreenState::safeCast(AdjustmentScreenState::getInstance()),
        GameState::safeCast(Router::getCurrentGameState(Router::getInstance()))
    );
    return GameState::safeCast(PrecautionScreenState::getInstance());
    #else
    if(INITIAL_GAME_STATE == kGameStatePrecautionScreen)
    {
        SplashScreenState::setNextState(
            SplashScreenState::safeCast(PrecautionScreenState::getInstance()),
            GameState::safeCast(AdjustmentScreenState::getInstance())
        );
        SplashScreenState::setNextState(
            SplashScreenState::safeCast(AdjustmentScreenState::getInstance()),
            GameState::safeCast(Router::getCurrentGameState(Router::getInstance()))
        );
        return GameState::safeCast(PrecautionScreenState::getInstance());
    }
    else
        return GameState::safeCast(Router::getCurrentGameState(Router::getInstance()));
    #endif
}
