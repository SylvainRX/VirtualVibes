#include <Camera.h>
#include <CameraEffectManager.h>
#include <DebugMacros.h>
#include <ForegroundArrowScheduler.h>
#include <FrameRate.h>
#include <GameEvents.h>
#include <GameSaveDataManager.h>
#include <HighscoreSaveGameState.h>
#include <HighscoresSaveConfig.h>
#include <KeypadManager.h>
#include <LaneFactory.h>
#include <Languages.h>
#include <LevelSetupGameState.h>
#include <LongArrowScheduler.h>
#include <Macros.h>
#include <Messages.h>
#include <MusicSpeed.h>
#include <PlayMode.h>
#include <Printer.h>
#include <ShortArrowScheduler.h>
#include <Singleton.h>
#include <SongLevelConfigBuilder.h>
#include <SongLevelFailureAnimation.h>
#include <SongLevelSuccessAnimation.h>
#include <Telegram.h>
#include <VIPManager.h>
#include <VUEngine.h>
#include <string.h>

#include "SongLevelGameState.h"

void SongLevelGameState::constructor()
{
    Base::constructor();

    this->dPad = NULL;
    this->leftDPadButton = NULL;
    this->rightDPadButton = NULL;
    this->upDPadButton = NULL;
    this->downDPadButton = NULL;
    this->shortArrowLeftLane = NULL;
    this->shortArrowRightLane = NULL;
    this->shortArrowUpLane = NULL;
    this->shortArrowDownLane = NULL;
    this->longArrowRightLane = NULL;
    this->scoreDisplay = NULL;
    this->songPlayer = NULL;

    this->isPlaying = false;
    this->levelEndEventCode = 0;

    ScoreManager::addEventListener(
        ScoreManager::getInstance(),
        ListenerObject::safeCast(this),
        kEventScoreManagerFailed
    );

    // Apply default config
    SongLevelGameState::configure(this, SongLevelConfigBuilder::buildDefaultConfig());
}

void SongLevelGameState::destructor()
{
    ScoreManager::removeEventListener(
        ScoreManager::getInstance(),
        ListenerObject::safeCast(this),
        kEventScoreManagerFailed
    );
    ArrowScheduler::endSchedule(this->shortArrowScheduler);
    ArrowScheduler::endSchedule(this->longArrowScheduler);
    ArrowScheduler::endSchedule(this->foregroundArrowScheduler);

    delete this->scoringEffectDisplay;

    delete this->shortArrowLeftLane;
    delete this->shortArrowRightLane;
    delete this->shortArrowUpLane;
    delete this->shortArrowDownLane;

    delete this->longArrowLeftLane;
    delete this->longArrowRightLane;
    delete this->longArrowUpLane;
    delete this->longArrowDownLane;

    delete this->foregroundArrowLeftLane;
    delete this->foregroundArrowRightLane;

    delete this->scoreDisplay;
    delete this->songPlayer;

    delete this->shortArrowScheduler;
    delete this->longArrowScheduler;
    delete this->foregroundArrowScheduler;

    delete this->gameDPadInputHandler;
    delete this->hapticPlayer;

    this->dPad = NULL;
    this->gameDPadInputHandler = NULL;
    this->leftDPadButton = NULL;
    this->rightDPadButton = NULL;
    this->upDPadButton = NULL;
    this->downDPadButton = NULL;
    this->shortArrowLeftLane = NULL;
    this->shortArrowRightLane = NULL;
    this->shortArrowUpLane = NULL;
    this->shortArrowDownLane = NULL;
    this->longArrowLeftLane = NULL;
    this->longArrowRightLane = NULL;
    this->longArrowUpLane = NULL;
    this->longArrowDownLane = NULL;
    this->foregroundArrowLeftLane = NULL;
    this->foregroundArrowRightLane = NULL;
    this->scoreDisplay = NULL;
    this->songPlayer = NULL;
    this->scoringEffectDisplay = NULL;

    Base::destructor();
}

void SongLevelGameState::enter(void* owner)
{
    Base::enter(this, owner);

    extern StageROMSpec SongLevelGameStageSpec;
    GameState::configureStage(this, (StageSpec*)&SongLevelGameStageSpec, NULL);

    #ifdef PRINT_FPS
    SongLevelGameState::sendMessageToSelf(this, kMessageDebugPrintFPS, 2000, 0);
    #endif
    SongLevelGameState::updateBackgroundAnimation(this);
    GameState::startClocks(GameState::safeCast(this));
    Camera::startEffect(Camera::getInstance(), kHide);
    Camera::startEffect(Camera::getInstance(),
        kFadeTo, // effect type
        0, // initial delay (in ms)
        NULL, // target brightness
        32, // delay between fading steps (in ms)
        NULL, // callback function
        NULL // callback scope
    );

    Printer::setCoordinates(0, 0, -5, -5);

    ScoreManager::reset(ScoreManager::getInstance());
    SongLevelGameState::setupHapticPlayer(this);
    this->shortArrowScheduler = (ArrowScheduler) new ShortArrowScheduler();
    this->longArrowScheduler = (ArrowScheduler) new LongArrowScheduler();
    this->foregroundArrowScheduler = (ArrowScheduler) new ForegroundArrowScheduler();
    SongLevelGameState::createDPad(this);
    SongLevelGameState::createLanes(this);
    this->scoreDisplay = new ScoreDisplay();
    this->songPlayer = new SongPlayer();
    this->scoringEffectDisplay = new ScoringEffectDisplay(this->stage);//, this->hapticPlayer);
    CountDownDisplay::start(this->stage, (ListenerObject)this);
}

void SongLevelGameState::exit(void* owner)
{
    Base::exit(this, owner);

    delete this;
}

void SongLevelGameState::suspend(void* owner)
{
    Camera::startEffect(Camera::getInstance(), kFadeOut, __FADE_DELAY);

    Base::suspend(this, owner);
}

void SongLevelGameState::resume(void* owner)
{
    Base::resume(this, owner);

    Camera::startEffect(Camera::getInstance(), kHide);
    Camera::startEffect(Camera::getInstance(),
        kFadeTo, // effect type
        0, // initial delay (in ms)
        NULL, // target brightness
        __FADE_DELAY, // delay between fading steps (in ms)
        NULL, // callback function
        NULL // callback scope
    );
}

void SongLevelGameState::configure(SongLevelConfig config)
{
    this->config = config;
}

SongLevelConfig* SongLevelGameState::getConfig()
{
    return &this->config;
}

void SongLevelGameState::processUserInput(const UserInput* userInput)
{
    // DPad left
    if((K_LL | K_RL) & userInput->pressedKey)
        SongLevelGameState::fireEvent(this, kEventDPadPressedLeft);
    else if((K_LL | K_RL) & userInput->releasedKey && !(K_LT & userInput->holdKey))
        SongLevelGameState::fireEvent(this, kEventDPadReleasedLeft);
    // DPad right
    if((K_LR | K_RR) & userInput->pressedKey)
        SongLevelGameState::fireEvent(this, kEventDPadPressedRight);
    else if((K_LR | K_RR) & userInput->releasedKey && !(K_RT & userInput->holdKey))
        SongLevelGameState::fireEvent(this, kEventDPadReleasedRight);
    // DPad up
    if((K_LU | K_RU) & userInput->pressedKey)
        SongLevelGameState::fireEvent(this, kEventDPadPressedUp);
    else if((K_LU | K_RU) & userInput->releasedKey)
        SongLevelGameState::fireEvent(this, kEventDPadReleasedUp);
    // DPad down
    if((K_LD | K_RD) & userInput->pressedKey)
        SongLevelGameState::fireEvent(this, kEventDPadPressedDown);
    else if((K_LD | K_RD) & userInput->releasedKey)
        SongLevelGameState::fireEvent(this, kEventDPadReleasedDown);
    // Left Trigger
    if (K_LT & userInput->pressedKey)
        SongLevelGameState::fireEvent(this, kEventDPadPressedLeftTrigger);
    else if (K_LT & userInput->releasedKey)
        SongLevelGameState::fireEvent(this, kEventDPadReleasedLeftTrigger);
    // Right Trigger
    if (K_RT & userInput->pressedKey)
        SongLevelGameState::fireEvent(this, kEventDPadPressedRightTrigger);
    else if (K_RT & userInput->releasedKey)
        SongLevelGameState::fireEvent(this, kEventDPadReleasedRightTrigger);

    if((K_SEL == userInput->pressedKey) || (K_B == userInput->pressedKey))
        SongLevelGameState::endLevelWithExit(this);
}

void SongLevelGameState::onCountDownOver()
{
    SongLevelGameState::createDPadInputHandler(this);
    KeypadManager::enable();
    SongLevelGameState::startLevel(this);
}

void SongLevelGameState::createDPad()
{
    extern ActorSpec DPadActorSpec;
    PositionedActor positionedActor = {&DPadActorSpec, {0, 0, DPAD_LAYER}, {0, 0, 0}, {1, 1, 1}, 0, NULL, NULL, NULL, false};
    this->dPad = DPad::safeCast(Stage::spawnChildActor(this->stage, (const PositionedActor* const)&positionedActor, true));
}

void SongLevelGameState::createLeftRightDPadButton()
{
    extern ActorSpec DPadLeftButtonSpec;
    PositionedActor leftDPadButtonPositionedActor = {&DPadLeftButtonSpec, LEFT_DPAD_BUTTON_POSITION, {0, 0, 0}, {1, 1, 1}, 0, NULL, NULL, NULL, false};
    this->leftDPadButton = DPadButton::safeCast(Stage::spawnChildActor(this->stage, (const PositionedActor* const)&leftDPadButtonPositionedActor, true));

    extern ActorSpec DPadRightButtonSpec;
    PositionedActor rightDPadButtonPositionedActor = {&DPadRightButtonSpec, RIGHT_DPAD_BUTTON_POSITION, {0, 0, 0}, {1, 1, 1}, 0, NULL, NULL, NULL, false};
    this->rightDPadButton = DPadButton::safeCast(Stage::spawnChildActor(this->stage, (const PositionedActor* const)&rightDPadButtonPositionedActor, true));
}

void SongLevelGameState::createUpDownDPadButton()
{
    extern ActorSpec DPadUpButtonSpec;
    PositionedActor upDPadButtonPositionedActor = {&DPadUpButtonSpec, UP_DPAD_BUTTON_POSITION, {0, 0, 0}, {1, 1, 1}, 0, NULL, NULL, NULL, false};
    this->upDPadButton = DPadButton::safeCast(Stage::spawnChildActor(this->stage, (const PositionedActor* const)&upDPadButtonPositionedActor, true));

    extern ActorSpec DPadDownButtonSpec;
    PositionedActor downDPadButtonPositionedActor = {&DPadDownButtonSpec, DOWN_DPAD_BUTTON_POSITION, {0, 0, 0}, {1, 1, 1}, 0, NULL, NULL, NULL, false};
    this->downDPadButton = DPadButton::safeCast(Stage::spawnChildActor(this->stage, (const PositionedActor* const)&downDPadButtonPositionedActor, true));
}

void SongLevelGameState::createDPadInputHandler()
{
    ListenerObject intputEventSenders[] =
    {
        (ListenerObject)this,
        (ListenerObject)this->shortArrowLeftLane,
        (ListenerObject)this->shortArrowRightLane,
        (ListenerObject)this->shortArrowUpLane,
        (ListenerObject)this->shortArrowDownLane,
        (ListenerObject)this->longArrowLeftLane,
        (ListenerObject)this->longArrowRightLane,
        (ListenerObject)this->longArrowUpLane,
        (ListenerObject)this->longArrowDownLane,
        (ListenerObject)this->foregroundArrowLeftLane,
        (ListenerObject)this->foregroundArrowRightLane,
        NULL
    };
    this->gameDPadInputHandler = new GameDPadInputHandler((GameDPadInputHandlerConfig){intputEventSenders, this->leftDPadButton, this->rightDPadButton, this->upDPadButton, this->downDPadButton, this->shortArrowLeftLane, this->shortArrowRightLane, this->shortArrowUpLane, this->shortArrowDownLane, this->longArrowLeftLane, this->longArrowRightLane, this->longArrowUpLane, this->longArrowDownLane, this->foregroundArrowLeftLane, this->foregroundArrowRightLane});
}

void SongLevelGameState::createLanes()
{
    int8 successLocationShift = 19;
    bool automaticPlay = this->config.playMode == kPlayModeAuto;

    this->shortArrowLeftLane = LaneFactory::makeShortArrowLeftLane(successLocationShift, this->config.scrollingSpeed, this->stage, this->shortArrowScheduler, automaticPlay);
    this->shortArrowRightLane = LaneFactory::makeShortArrowRightLane(successLocationShift, this->config.scrollingSpeed, this->stage, this->shortArrowScheduler, automaticPlay);
    this->shortArrowUpLane = LaneFactory::makeShortArrowUpLane(successLocationShift, this->config.scrollingSpeed, this->stage, this->shortArrowScheduler, automaticPlay);
    this->shortArrowDownLane = LaneFactory::makeShortArrowDownLane(successLocationShift, this->config.scrollingSpeed, this->stage, this->shortArrowScheduler, automaticPlay);

    this->longArrowLeftLane = LaneFactory::makeLongArrowLeftLane(successLocationShift, this->config.scrollingSpeed, this->stage, this->longArrowScheduler, automaticPlay);
    this->longArrowRightLane = LaneFactory::makeLongArrowRightLane(successLocationShift, this->config.scrollingSpeed, this->stage, this->longArrowScheduler, automaticPlay);
    this->longArrowUpLane = LaneFactory::makeLongArrowUpLane(successLocationShift, this->config.scrollingSpeed, this->stage, this->longArrowScheduler, automaticPlay);
    this->longArrowDownLane = LaneFactory::makeLongArrowDownLane(successLocationShift, this->config.scrollingSpeed, this->stage, this->longArrowScheduler, automaticPlay);

    this->foregroundArrowLeftLane = LaneFactory::makeForegroundArrowLeftForegroundLane(successLocationShift, this->config.scrollingSpeed, this->stage, this->foregroundArrowScheduler, automaticPlay);
    this->foregroundArrowRightLane = LaneFactory::makeForegroundArrowRightForegroundLane(successLocationShift, this->config.scrollingSpeed, this->stage, this->foregroundArrowScheduler, automaticPlay);
}

void SongLevelGameState::setupHapticPlayer()
{
    static const uint16 normalArrowEvents[] = {kEventScoreManagerScoredLeft, kEventScoreManagerScoredRight, kEventScoreManagerScoredUp, kEventScoreManagerScoredDown};
    static const uint16 foregroundArrowEvents[] = {kEventScoreManagerScoredForegroundLeft, kEventScoreManagerScoredForegroundRight};

    this->hapticPlayer = new HapticPlayer();

    HapticPlayer::playNormalArrowHapticFor(this->hapticPlayer, (ListenerObject)ScoreManager::getInstance(), normalArrowEvents, 4);
    HapticPlayer::playForegroundArrowHapticFor(this->hapticPlayer, (ListenerObject)ScoreManager::getInstance(), foregroundArrowEvents, 2);
}

void SongLevelGameState::startLevel()
{
    if(this->isPlaying)
        return;
    this->isPlaying = true;

    ScoreManager::configure(
        ScoreManager::getInstance(),
        this->config.playMode,
        this->config.musicSpeed,
        this->config.scrollingSpeed
    );

    SongPlayer::playLevelSong(
        this->songPlayer,
        *this->config.levelSong,
        this->config.musicSpeed,
        Lane::millisecondsToSuccessLocation(this->foregroundArrowRightLane),
        ListenerObject::safeCast(this)
    );

    ArrowScheduler::setupSchedule(
        this->shortArrowScheduler,
        this->config.arrowSchedule,
        (PlaybackTicksProvider){SongPlayer::getElapsedTicks, this->songPlayer},
        Lane::millisecondsToSuccessLocation(this->shortArrowRightLane),
        SongPlayer::getTargetTimerResolutionUS(this->songPlayer)
    );
    ArrowScheduler::setupSchedule(
        this->longArrowScheduler,
        this->config.arrowSchedule,
        (PlaybackTicksProvider){SongPlayer::getElapsedTicks, this->songPlayer},
        Lane::millisecondsToSuccessLocation(this->longArrowRightLane),
        SongPlayer::getTargetTimerResolutionUS(this->songPlayer)
    );
    ArrowScheduler::setupSchedule(
        this->foregroundArrowScheduler,
        this->config.arrowSchedule,
        (PlaybackTicksProvider){SongPlayer::getElapsedTicks, this->songPlayer},
        Lane::millisecondsToSuccessLocation(this->foregroundArrowRightLane),
        SongPlayer::getTargetTimerResolutionUS(this->songPlayer)
    );
    ArrowScheduler::run(this->shortArrowScheduler);
    ArrowScheduler::run(this->longArrowScheduler);
    ArrowScheduler::run(this->foregroundArrowScheduler);
}

void SongLevelGameState::endLevelWithSuccess()
{
    this->levelEndEventCode = kEventGameStateSongLevelEndedWithSuccess;
    SongLevelGameState::startSuccessAnimation(this);
}

void SongLevelGameState::endLevelWithFailure()
{
    this->levelEndEventCode = kEventGameStateSongLevelEndedWithFailure;
    SongLevelGameState::startFailureAnimation(this);
}

void SongLevelGameState::endLevelWithExit()
{
    this->levelEndEventCode = kEventGameStateSongLevelDidExit;
    SongLevelGameState::startExitAnimation(this);
}

void SongLevelGameState::stopBeforeExit()
{
    SongPlayer::stop(this->songPlayer);

    ArrowScheduler::endSchedule(this->shortArrowScheduler);
    ArrowScheduler::endSchedule(this->longArrowScheduler);
    ArrowScheduler::endSchedule(this->foregroundArrowScheduler);

    Lane::stop(this->shortArrowLeftLane);
    Lane::stop(this->shortArrowRightLane);
    Lane::stop(this->shortArrowUpLane);
    Lane::stop(this->shortArrowDownLane);
    Lane::stop(this->longArrowLeftLane);
    Lane::stop(this->longArrowRightLane);
    Lane::stop(this->longArrowUpLane);
    Lane::stop(this->longArrowDownLane);
    Lane::stop(this->foregroundArrowLeftLane);
    Lane::stop(this->foregroundArrowRightLane);
}

void SongLevelGameState::startSuccessAnimation()
{
    KeypadManager::disable();
    SongLevelGameState::stopBeforeExit(this);

    SongLevelSuccessAnimation::start(
        (DPadButton[]){
            this->leftDPadButton,
            this->rightDPadButton,
            this->upDPadButton,
            this->downDPadButton,
        },
        this->scoringEffectDisplay,
        ListenerObject::safeCast(this),
		this->hapticPlayer
    );
}

void SongLevelGameState::startFailureAnimation()
{
    KeypadManager::disable();
    SongLevelGameState::stopBeforeExit(this);

    DPadButton dPadButtons[] = {
        this->leftDPadButton,
        this->rightDPadButton,
        this->upDPadButton,
        this->downDPadButton,
    };

    SongLevelFailureAnimation::start(
        dPadButtons,
        this->dPad,
        ListenerObject::safeCast(this),
		this->hapticPlayer
    );
}

void SongLevelGameState::startExitAnimation()
{
    KeypadManager::disable();
    SongLevelGameState::stopBeforeExit(this);
    SongLevelGameState::startFadeOutAnimation(this);
}

void SongLevelGameState::startFadeOutAnimation()
{
    Camera::startEffect(
        Camera::getInstance(),
        kFadeTo, // effect type
        0, // initial delay (in ms)
        &(Brightness){0, 0, 0}, // target brightness
        __FADE_DELAY * 2, // delay between fading steps (in ms)
        ListenerObject::safeCast(this) // callback scope
    );
}

void SongLevelGameState::updateBackgroundAnimation()
{
    if(GameSaveDataManager::isBackgroundAnimationEnabled(GameSaveDataManager::getInstance()) == false)
    {
        Actor::stopAnimation(Actor::safeCast(Stage::getChildById(this->stage, 4)));
        Actor::stopAnimation(Actor::safeCast(Stage::getChildById(this->stage, 5)));
    }
}

bool SongLevelGameState::onEvent(ListenerObject eventFirer, uint16 eventCode)
{
    switch(eventCode)
    {
        case kEventCountDownDisplayStep1:
            SongLevelGameState::createLeftRightDPadButton(this);
            return true;
        case kEventCountDownDisplayStep2:
            SongLevelGameState::createUpDownDPadButton(this);
            return true;
        case kEventCountDownDisplayOver:
            SongLevelGameState::onCountDownOver(this);
            return true;

        case kEventScoreManagerFailed:
            SongLevelGameState::endLevelWithFailure(this);
            return true;
        case kEventSoundFinished:
            SongLevelGameState::endLevelWithSuccess(this);
            return true;

        case kEventSongLevelFailureAnimationComplete:
        case kEventSongLevelSuccessAnimationComplete:
            SongLevelGameState::startFadeOutAnimation(this);
            return true;
        case kEventEffectFadeOutComplete:
            SongLevelGameState::fireEvent(this, this->levelEndEventCode);
            return true;
    }
    return Base::onEvent(this, eventFirer, eventCode);
}

#ifdef PRINT_FPS
friend class FrameRate;

void SongLevelGameState::printFPS()
{
    SongLevelGameState::sendMessageToSelf(this, kMessageDebugPrintFPS, 500, 0);
    PRINT_TEXT("FPS:   ", 10, 1);
    PRINT_INT((float)FrameRate::getInstance()->totalFPS / FrameRate::getInstance()->seconds, 14, 1);
}
#endif

bool SongLevelGameState::handleMessage(Telegram telegram)
{
    #ifdef PRINT_FPS
    switch(Telegram::getMessage(telegram))
    {
        case kMessageDebugPrintFPS:
            SongLevelGameState::printFPS(this);
            return true;
    }
    #endif

    return Base::handleMessage(this, telegram);
}
