#ifndef SONG_LEVEL_GAME_STATE_H_
#define SONG_LEVEL_GAME_STATE_H_

#include <ArrowScheduler.h>
#include <CountDownDisplay.h>
#include <DPad.h>
#include <DPadButton.h>
#include <ForegroundArrowLane.h>
#include <GameDPadInputHandler.h>
#include <GameState.h>
#include <HapticPlayer.h>
#include <Lane.h>
#include <LongArrowLane.h>
#include <ScoreDisplay.h>
#include <ScoreManager.h>
#include <ScoringEffectDisplay.h>
#include <ShortArrowLane.h>
#include <SongLevelConfig.h>
#include <SongPlayer.h>

dynamic_singleton class SongLevelGameState : GameState
{
    SongLevelConfig config;
    DPad dPad;
    GameDPadInputHandler gameDPadInputHandler;
    HapticPlayer hapticPlayer;
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

    ArrowScheduler shortArrowScheduler;
    ArrowScheduler longArrowScheduler;
    ArrowScheduler foregroundArrowScheduler;

    ScoreDisplay scoreDisplay;
    SongPlayer songPlayer;
    ScoringEffectDisplay scoringEffectDisplay;
    bool isPlaying;

    uint16 levelEndEventCode;

    void configure(SongLevelConfig config);
    SongLevelConfig* getConfig();

    override void enter(void* owner);
    override void exit(void* owner);
    override void suspend(void* owner);
    override void resume(void* owner);
    override void processUserInput(const UserInput* userInput);
    override bool onEvent(ListenerObject eventFirer, uint16 eventCode);
    override bool handleMessage(Telegram telegram);
}

#endif
