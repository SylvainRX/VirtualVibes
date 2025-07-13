#include <DebugMacros.h>
#include <GameEvents.h>
#include <LevelSetupGameState.h>
#include <FailureGameState.h>
#include <GameStateId.h>
#include <Singleton.h>
#include <ScoreConfig.h>
#include <SongLevelConfigBuilder.h>
#include <SongLevelGameState.h>
#include <SplashScreenGameState.h>
#include <GameSaveDataManager.h>
#include <HighscoresSaveConfig.h>
#include <HighscoreSaveGameState.h>
#include <ScoreGameState.h>
#include <ScoreManager.h>
#include <SongLevelConfig.h>
#include <SystemGameState.h>
#include <TutorialGameState.h>
#include <VUEngine.h>

#include "Router.h"

void Router::constructor()
{
    Base::constructor();

    this->nextGameStateId = kGameStateNone;
    this->currentGameStateId = kGameStateNone;

    #if defined(__RELEASE) && !defined(DEBUG_RELEASE)
    Router::setCurrentGameState(this, kGameStateSplashScreen);
    #else
	if(INITIAL_GAME_STATE != kGameStatePrecautionScreen)
    	Router::setCurrentGameState(this, INITIAL_GAME_STATE);
	else
		Router::setCurrentGameState(this, kGameStateSplashScreen);
    #endif
}

void Router::destructor()
{
    Base::destructor();
}

GameState Router::getCurrentGameState()
{
    return this->currentGameState;
}

void Router::enterGameState(uint8 gameStateId)
{
    Router::deinitGameState(this, this->currentGameStateId);
    Router::setCurrentGameState(this, gameStateId);
    VUEngine::changeState(this->currentGameState);
}

void Router::setCurrentGameState(uint8 gameStateId)
{
    switch(gameStateId)
    {
        case kGameStateSplashScreen: Router::initSplashScreenGameState(this); break;
        case kGameStateLevelSetup: Router::initLevelSetupGameState(this); break;
        case kGameStateSongLevel: Router::initSongLevelGameState(this); break;
        case kGameStateScore: Router::initScoreGameState(this); break;
        case kGameStateHighScoreSave: Router::initHighscoreSaveGameState(this); break;
        case kGameStateFailure: Router::initFailureGameState(this); break;
        case kGameStateSystem: Router::initSystemGameState(this); break;
        case kGameStateTutorial: Router::initTutorialGameState(this); break;
    }
    this->currentGameStateId = gameStateId;
    this->currentGameState = Router::gameStateFromId(this->currentGameStateId);
}

void Router::deinitGameState(uint8 gameStateId)
{
    switch(gameStateId)
    {
        case kGameStateSplashScreen: Router::deinitSplashScreenGameState(this); break;
        case kGameStateLevelSetup: Router::deinitLevelSetupGameState(this); break;
        case kGameStateSongLevel: Router::deinitSongLevelGameState(this); break;
        case kGameStateScore: Router::deinitScoreGameState(this); break;
        case kGameStateHighScoreSave: Router::deinitHighscoreSaveGameState(this); break;
        case kGameStateFailure: Router::deinitFailureGameState(this); break;
        case kGameStateSystem: Router::deinitSystemGameState(this); break;
        case kGameStateTutorial: Router::deinitTutorialGameState(this); break;
    }
}

static GameState Router::gameStateFromId(uint8 gameStateId)
{
    switch(gameStateId)
    {
        case kGameStateSplashScreen: return GameState::safeCast(SplashScreenGameState::getInstance());
        case kGameStateLevelSetup: return GameState::safeCast(LevelSetupGameState::getInstance());
        case kGameStateSongLevel: return GameState::safeCast(SongLevelGameState::getInstance());
        case kGameStateScore: return GameState::safeCast(ScoreGameState::getInstance());
        case kGameStateHighScoreSave: return GameState::safeCast(HighscoreSaveGameState::getInstance());
        case kGameStateFailure: return GameState::safeCast(FailureGameState::getInstance());
        case kGameStateSystem: return GameState::safeCast(SystemGameState::getInstance());
        case kGameStateTutorial: return GameState::safeCast(TutorialGameState::getInstance());
        default: return NULL;
    }
}

// Splash Screen

void Router::initSplashScreenGameState()
{
    SplashScreenGameState::addEventListener(
        SplashScreenGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateSplashScreenProceed
    );
}

void Router::deinitSplashScreenGameState()
{
    SplashScreenGameState::removeEventListener(
        SplashScreenGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateSplashScreenProceed
    );
}

void Router::onSplashScreenProceed()
{
    Router::enterGameState(this, kGameStateLevelSetup);
}

// Level Setup

void Router::initLevelSetupGameState()
{
    LevelSetupGameState::addEventListener(
        LevelSetupGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateLevelSetupIsDone
    );
    LevelSetupGameState::addEventListener(
        LevelSetupGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateLevelSetupOpenSystem
    );
}

void Router::deinitLevelSetupGameState()
{
    LevelSetupGameState::removeEventListener(
        LevelSetupGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateLevelSetupIsDone
    );
    LevelSetupGameState::removeEventListener(
        LevelSetupGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateLevelSetupOpenSystem
    );
}

void Router::onLevelSetupIsDone()
{
    LevelSetupState* levelSetupState = LevelSetupGameState::getState(LevelSetupGameState::getInstance());
    SongLevelConfig songLevelConfig = SongLevelConfigBuilder::build(
        levelSetupState->selectedSongIndex,
        levelSetupState->difficulty,
        levelSetupState->musicSpeed,
        levelSetupState->scrollingSpeed,
        levelSetupState->playMode
    );
    SongLevelGameState::configure(SongLevelGameState::getInstance(), songLevelConfig);

    if(GameSaveDataManager::wasTutorialShown(GameSaveDataManager::getInstance()))
        Router::enterGameState(this, kGameStateSongLevel);
    else
    {
        this->nextGameStateId = kGameStateSongLevel;
        TutorialGameState::setControlsType(TutorialGameState::getInstance(), kTutorialGameStateControlsTypeStart);
        Router::enterGameState(this, kGameStateTutorial);
        GameSaveDataManager::setTutorialAsShown(GameSaveDataManager::getInstance());
    }
}

void Router::onLevelSetupOpenSystem()
{
    Router::enterGameState(this, kGameStateSystem);
}

// Song Level

void Router::initSongLevelGameState()
{
    SongLevelGameState::addEventListener(
        SongLevelGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateSongLevelEndedWithSuccess
    );
    SongLevelGameState::addEventListener(
        SongLevelGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateSongLevelEndedWithFailure
    );
    SongLevelGameState::addEventListener(
        SongLevelGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateSongLevelDidExit
    );
}

void Router::deinitSongLevelGameState()
{
    SongLevelGameState::removeEventListener(
        SongLevelGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateSongLevelEndedWithSuccess
    );
    SongLevelGameState::removeEventListener(
        SongLevelGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateSongLevelEndedWithFailure
    );
    SongLevelGameState::removeEventListener(
        SongLevelGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateSongLevelDidExit
    );
}

void Router::onSongLevelSuccess()
{
    SongLevelConfig* songLevelConfig = SongLevelGameState::getConfig(SongLevelGameState::getInstance());
    uint32 score = ScoreManager::getScore(ScoreManager::getInstance());

    if(score == 0)
    {
        Router::enterGameState(this, kGameStateLevelSetup);
        return;
    }

    bool isPlayerScoreHighscore = GameSaveDataManager::isPlayerScoreHighscore(
        GameSaveDataManager::getInstance(),
        songLevelConfig->songIndex,
        songLevelConfig->difficulty,
        score
    );
    if(isPlayerScoreHighscore)
    {
        HighscoresSaveConfig highscoreSaveConfig =
        {
            .songIndex = songLevelConfig->songIndex,
            .difficulty = songLevelConfig->difficulty,
            .score = score,
            .greatestCombo = ScoreManager::getGreatestComboCount(ScoreManager::getInstance())
        };
        HighscoreSaveGameState::configure(HighscoreSaveGameState::getInstance(), highscoreSaveConfig);
        Router::enterGameState(this, kGameStateHighScoreSave);
    }
    else
    {
        ScoreConfig scoreConfig =
        {
            .score = score,
            .greatestCombo = ScoreManager::getGreatestComboCount(ScoreManager::getInstance())
        };
        ScoreGameState::configure(ScoreGameState::getInstance(), scoreConfig);
        Router::enterGameState(this, kGameStateScore);
    }
}

void Router::onSongLevelFailure()
{
    Router::enterGameState(this, kGameStateFailure);
}

void Router::onSongLevelExit()
{
    Router::enterGameState(this, kGameStateLevelSetup);
}

// Score

void Router::initScoreGameState()
{
    ScoreGameState::addEventListener(
        ScoreGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateScoreDone
    );
}

void Router::deinitScoreGameState()
{
    ScoreGameState::removeEventListener(
        ScoreGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateScoreDone
    );
}

void Router::onScoreDone()
{
    Router::enterGameState(this, kGameStateLevelSetup);
}

// Highscore Save

void Router::initHighscoreSaveGameState()
{
    HighscoreSaveGameState::addEventListener(
        HighscoreSaveGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateHighscoreSaveDone
    );
}

void Router::deinitHighscoreSaveGameState()
{
    HighscoreSaveGameState::removeEventListener(
        HighscoreSaveGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateHighscoreSaveDone
    );
}

void Router::onHighscoreSaveDone()
{
    Router::enterGameState(this, kGameStateLevelSetup);
}

// Failure

void Router::initFailureGameState()
{
    FailureGameState::addEventListener(
        FailureGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateFailureDone
    );
}

void Router::deinitFailureGameState()
{
    FailureGameState::removeEventListener(
        FailureGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateFailureDone
    );
}

void Router::onFailureDone()
{
    Router::enterGameState(this, kGameStateLevelSetup);
}

// System

void Router::initSystemGameState()
{
    SystemGameState::addEventListener(
        SystemGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateExitSystem
    );
    SystemGameState::addEventListener(
        SystemGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateShowTutorial
    );
}

void Router::deinitSystemGameState()
{
    SystemGameState::removeEventListener(
        SystemGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateExitSystem
    );
    SystemGameState::removeEventListener(
        SystemGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateShowTutorial
    );
}

void Router::onExitSystem()
{
    Router::enterGameState(this, kGameStateLevelSetup);
}

void Router::onSystemShowTutorial()
{
    this->nextGameStateId = kGameStateSystem;
    TutorialGameState::setControlsType(TutorialGameState::getInstance(), kTutorialGameStateControlsTypeBack);
    Router::enterGameState(this, kGameStateTutorial);
}

// Tutorial

void Router::initTutorialGameState()
{
    TutorialGameState::addEventListener(
        TutorialGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateTutorialDone
    );
}

void Router::deinitTutorialGameState()
{
    TutorialGameState::removeEventListener(
        TutorialGameState::getInstance(),
        ListenerObject::safeCast(this),
        kEventGameStateTutorialDone
    );
}

void Router::onTutorialDone()
{
    Router::enterGameState(this, this->nextGameStateId);
    this->nextGameStateId = kGameStateNone;
}

// Messages handling

bool Router::onEvent(ListenerObject eventFirer, uint16 eventCode)
{
    switch(eventCode)
    {
        case kEventGameStateSplashScreenProceed: Router::onSplashScreenProceed(this); return true;
        case kEventGameStateLevelSetupIsDone: Router::onLevelSetupIsDone(this); return true;
        case kEventGameStateLevelSetupOpenSystem: Router::onLevelSetupOpenSystem(this); return true;
        case kEventGameStateSongLevelEndedWithSuccess: Router::onSongLevelSuccess(this); return true;
        case kEventGameStateSongLevelEndedWithFailure: Router::onSongLevelFailure(this); return true;
        case kEventGameStateSongLevelDidExit: Router::onSongLevelExit(this); return true;
        case kEventGameStateScoreDone: Router::onScoreDone(this); return true;
        case kEventGameStateHighscoreSaveDone: Router::onHighscoreSaveDone(this); return true;
        case kEventGameStateFailureDone: Router::onFailureDone(this); return true;
        case kEventGameStateExitSystem: Router::onExitSystem(this); return true;
        case kEventGameStateShowTutorial: Router::onSystemShowTutorial(this); return true;
        case kEventGameStateTutorialDone: Router::onTutorialDone(this); return true;
    }
    return Base::onEvent(this, eventFirer, eventCode);
}
