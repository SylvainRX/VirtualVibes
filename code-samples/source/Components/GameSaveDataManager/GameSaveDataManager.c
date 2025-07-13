#include <Clock.h>
#include <DebugMacros.h>
#include <Difficulty.h>
#include <KeypadManager.h>
#include <SRAMManager.h>
#include <Singleton.h>
#include <string.h>
#include <StringUtilities.h>
#include <Utilities.h>
#include <VUEngine.h>

#include "GameSaveDataManager.h"

void GameSaveDataManager::constructor()
{
    Base::constructor();

    #ifdef RESET_SRAM_DATA_ON_START
    GameSaveDataManager::setupInitialData(this);
    #endif
}

void GameSaveDataManager::destructor()
{
    Base::destructor();
}

void GameSaveDataManager::initialSetup()
{
    if(GameSaveDataManager::isInitialSetupDone(this) == false)
        GameSaveDataManager::setupInitialData(this);
}

void GameSaveDataManager::setupInitialData()
{
    GameSaveDataManager::setupFakeData(this);
    GameSaveDataManager::saveLastPlayerName(this, "");

    GameSaveDataManager::setSuccessParticleEnabled(this, true);
    GameSaveDataManager::setBackgroundAnimationEnabled(this, true);
    GameSaveDataManager::setNoteRumbleEnabled(this, true);
    GameSaveDataManager::setSuccessFailureRumbleEnabled(this, true);
    GameSaveDataManager::setMenuRumbleEnabled(this, true);
    GameSaveDataManager::setRumbleIntensity(this, 1);

    GameSaveDataManager::setInitialSetupAsDone(this);
}

void GameSaveDataManager::clearAll()
{
    GameSaveDataManager::saveLastPlayerName(this, "");
    for(uint8 songIndex = 0; songIndex < SONG_TITLES_COUNT; songIndex++)
        for(uint8 difficultyIndex = 0; difficultyIndex < 3; difficultyIndex++)
            GameSaveDataManager::clearPlayerScores(this, songIndex, difficultyIndex);
}

void GameSaveDataManager::saveLastPlayerName(char* lastPlayerName)
{
    char writtenString[PLAYER_NAME_LENGTH + 1];
    strncpy(writtenString, lastPlayerName, PLAYER_NAME_LENGTH);
    StringUtilities::trimLeadingAndTrailingSpaces(writtenString);
    SaveDataManager::setValue(this, (BYTE*)&writtenString, offsetof(struct GameSaveData, lastPlayerName), sizeof(writtenString));
}

void GameSaveDataManager::getLastPlayerName(char* lastPlayerName)
{
    char readString[PLAYER_NAME_LENGTH + 1];
    SaveDataManager::getValue(this, (BYTE*)&readString, offsetof(struct GameSaveData, lastPlayerName), sizeof(readString));
    if(readString[PLAYER_NAME_LENGTH] == '\0' && strlen(readString) != 0)
        strncpy(lastPlayerName, readString, PLAYER_NAME_LENGTH);
    else
        strncpy(lastPlayerName, "", PLAYER_NAME_LENGTH);
}

void GameSaveDataManager::setInitialSetupAsDone()
{
    SaveDataManager::setValue(this, (BYTE*)&DID_SETUP_FAKE_DATA_STRING, offsetof(struct GameSaveData, initialSetupFlag), sizeof(DID_SETUP_FAKE_DATA_STRING));
}

bool GameSaveDataManager::isInitialSetupDone()
{
    char initialSetupString[INITIAL_ACTIONS_FLAGS_STRING_SIZE];
    SaveDataManager::getValue(this, (BYTE*)&initialSetupString, offsetof(struct GameSaveData, initialSetupFlag), sizeof(initialSetupString));
    initialSetupString[INITIAL_ACTIONS_FLAGS_STRING_SIZE - 1] = '\0';
    return strcmp(initialSetupString, DID_SETUP_FAKE_DATA_STRING) == 0;
}

void GameSaveDataManager::setTutorialAsShown()
{
    SaveDataManager::setValue(this, (BYTE*)&DID_SHOW_TUTORIAL_STRING, offsetof(struct GameSaveData, tutorialFlag), sizeof(DID_SHOW_TUTORIAL_STRING));
}

bool GameSaveDataManager::wasTutorialShown()
{
    #ifndef DEMO_MODE
    char tutorialString[INITIAL_ACTIONS_FLAGS_STRING_SIZE];
    SaveDataManager::getValue(this, (BYTE*)&tutorialString, offsetof(struct GameSaveData, tutorialFlag), sizeof(tutorialString));
    tutorialString[INITIAL_ACTIONS_FLAGS_STRING_SIZE - 1] = '\0';
    return strcmp(tutorialString, DID_SHOW_TUTORIAL_STRING) == 0;
    #else
    return false;
    #endif
}

void GameSaveDataManager::savePlayerScore(uint8 songIndex, uint8 difficulty, char* playerName, uint32 score)
{
    GameSaveDataManager::loadPlayerScores(this, songIndex, difficulty);

    uint8 playerScoreIndex = 0;
    while(this->playerScores[playerScoreIndex].score >= score && playerScoreIndex < HIGHSCORE_COUNT)
        playerScoreIndex++;

    if(playerScoreIndex >= HIGHSCORE_COUNT)
        return;

    for(uint8 index = HIGHSCORE_COUNT - 1; index > playerScoreIndex; index--)
        this->playerScores[index] = this->playerScores[index - 1];

    strncpy(this->playerScores[playerScoreIndex].playerName, playerName, PLAYER_NAME_LENGTH);
    StringUtilities::trimLeadingAndTrailingSpaces(this->playerScores[playerScoreIndex].playerName);
    this->playerScores[playerScoreIndex].score = score;

    GameSaveDataManager::savePlayerScores(this, songIndex, difficulty);
    GameSaveDataManager::saveLastPlayerName(this, playerName);
}

PlayerScoreSaveData* GameSaveDataManager::getPlayerScore(uint8 songIndex, uint8 difficulty)
{
    GameSaveDataManager::loadPlayerScores(this, songIndex, difficulty);
    return this->playerScores;
}

bool GameSaveDataManager::isPlayerScoreHighscore(uint8 songIndex, uint8 difficulty, uint32 score)
{
    GameSaveDataManager::loadPlayerScores(this, songIndex, difficulty);
    uint8 lowestPlayerScoreindex = HIGHSCORE_COUNT - 1;
    while(this->playerScores[lowestPlayerScoreindex].score == 0 && lowestPlayerScoreindex > 0)
        lowestPlayerScoreindex--;
    return score > this->playerScores[lowestPlayerScoreindex].score;
}

void GameSaveDataManager::clearPlayerScores(uint8 songIndex, uint8 difficulty)
{
    for(uint8 index = 0; index < HIGHSCORE_COUNT; index++)
    {
        strncpy(this->playerScores[index].playerName, "", PLAYER_NAME_LENGTH);
        this->playerScores[index].score = 0;
    }

    GameSaveDataManager::savePlayerScores(this, songIndex, difficulty);
}

void GameSaveDataManager::loadPlayerScores(uint8 songIndex, uint8 difficulty)
{
    uint16 playerScoresOffset = GameSaveDataManager::playerScoresOffset(this, songIndex, difficulty);
    GameSaveDataManager::readPlayerScore(this, playerScoresOffset);
}

void GameSaveDataManager::readPlayerScore(uint16 playerScoresOffset)
{
    for(uint8 it = 0; it < HIGHSCORE_COUNT; it++)
    {
        uint16 playerScoreOffset = playerScoresOffset + (it * sizeof(struct PlayerScoreSaveData));
        uint16 playerScorePlayerNameOffset = playerScoreOffset + offsetof(struct PlayerScoreSaveData, playerName);
        uint16 playerScoreScoreOffset = playerScoreOffset + offsetof(struct PlayerScoreSaveData, score);
        char playerName[PLAYER_NAME_LENGTH + 1];
        SaveDataManager::getValue(this, (BYTE*)&playerName, playerScorePlayerNameOffset, sizeof(playerName));
        if(playerName[PLAYER_NAME_LENGTH] == '\0' && strlen(playerName) != 0)
            strncpy(this->playerScores[it].playerName, playerName, PLAYER_NAME_LENGTH);
        else
            strncpy(this->playerScores[it].playerName, "", PLAYER_NAME_LENGTH);
        SaveDataManager::getValue(this, (BYTE*)&this->playerScores[it].score, playerScoreScoreOffset, sizeof(this->playerScores[it].score));
    }
}

void GameSaveDataManager::savePlayerScores(uint8 songIndex, uint8 difficulty)
{
    uint16 playerScoresOffset = GameSaveDataManager::playerScoresOffset(this, songIndex, difficulty);
    GameSaveDataManager::writePlayerScore(this, playerScoresOffset);
}

void GameSaveDataManager::writePlayerScore(uint16 playerScoresOffset)
{
    for(uint8 index = 0; index < HIGHSCORE_COUNT; index++)
    {
        uint16 playerNameOffset = playerScoresOffset + offsetof(struct PlayerScoreSaveData, playerName) + (index * sizeof(struct PlayerScoreSaveData));
        uint16 scoreOffset = playerScoresOffset + offsetof(struct PlayerScoreSaveData, score) + (index * sizeof(struct PlayerScoreSaveData));
        char writtenPlayerName[PLAYER_NAME_LENGTH + 1];
        strncpy(writtenPlayerName, this->playerScores[index].playerName, PLAYER_NAME_LENGTH);
        SaveDataManager::setValue(this, (BYTE*)&writtenPlayerName, playerNameOffset, sizeof(writtenPlayerName));
        SaveDataManager::setValue(this, (BYTE*)&this->playerScores[index].score, scoreOffset, sizeof(this->playerScores[index].score));
    }
}

uint16 GameSaveDataManager::playerScoresOffset(uint8 songIndex, uint8 difficulty)
{
    uint16 songPlayerScoreOffset = offsetof(struct GameSaveData, songPlayerScores) + (songIndex * sizeof(struct SongPlayerScoreSaveData));
    switch(difficulty)
    {
        case kDifficultyEasy: return songPlayerScoreOffset + offsetof(struct SongPlayerScoreSaveData, easyPlayerScores);
        case kDifficultyNormal: return songPlayerScoreOffset + offsetof(struct SongPlayerScoreSaveData, normalPlayerScores);
        case kDifficultyHard: return songPlayerScoreOffset + offsetof(struct SongPlayerScoreSaveData, hardPlayerScores);
        default: return 0;
    }
}

#define MAX_FAKE_SCORE 1000
#define MIN_FAKE_SCORE 100
#define FAKE_PLAYER_COUNT 17

const char* FakePLayerNames[FAKE_PLAYER_COUNT] =
{
    "Gunpei",
    "Hrann",
    "JJ. R",
    "J. RP",
    "Kev YL",
    "Kiki",
    "Mr Cela",
    "Ren's",
    "R\xE9my B.",
    "Spotted",
    "SylvnRx",
    "Tayren",
    "Thrawn",
    "UFlGrt",
    "veve40",
    "Vivi.Mn",
    "YukiAvk",
};

void GameSaveDataManager::setupFakeData()
{
    for(uint8 songIndex = 0; songIndex < SONG_TITLES_COUNT; songIndex++)
        for(uint8 difficultyIndex = 0; difficultyIndex < 3; difficultyIndex++)
        {
            GameSaveDataManager::loadFakeData(this);
            GameSaveDataManager::savePlayerScores(this, songIndex, difficultyIndex);
        }
}

void GameSaveDataManager::loadFakeData()
{
    const char* fakePlayerNames[FAKE_PLAYER_COUNT];
    GameSaveDataManager::copyPlayerNames(fakePlayerNames, FakePLayerNames);

    GameSaveDataManager::shuffle(this, fakePlayerNames);

    PlayerScoreSaveData randomPlayerScores[FAKE_PLAYER_COUNT];
    GameSaveDataManager::fillRandomPlayerScores(this, fakePlayerNames, randomPlayerScores);

    GameSaveDataManager::sortPlayerScores(randomPlayerScores);

    for (uint8 i = FAKE_PLAYER_COUNT - HIGHSCORE_COUNT; i < FAKE_PLAYER_COUNT; i++)
    {
        uint8 index = i - (FAKE_PLAYER_COUNT - HIGHSCORE_COUNT);
        GameSaveDataManager::copyPlayerScore(&this->playerScores[index], &randomPlayerScores[i]);
    }
}

static void GameSaveDataManager::copyPlayerNames(const char** destination, const char** source)
{
    for (uint8 i = 0; i < FAKE_PLAYER_COUNT; i++)
    {
        destination[i] = source[i];
    }
}

void GameSaveDataManager::shuffle(const char** array)
{
    const char* temp;
    for (uint8 i = FAKE_PLAYER_COUNT - 1; i > 0; i--)
    {
        int32 j = Math::random(Math::randomSeed(), i);
        temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void GameSaveDataManager::fillRandomPlayerScores(const char** playerNames, PlayerScoreSaveData* playerScores)
{
    for (uint8 i = 0; i < FAKE_PLAYER_COUNT; i++)
    {
        strncpy(playerScores[i].playerName, playerNames[i], PLAYER_NAME_LENGTH);
        int32 score = Math::random(Math::randomSeed(), MAX_FAKE_SCORE - MIN_FAKE_SCORE) + MIN_FAKE_SCORE;
        playerScores[i].score = score > 0 ? score : 1;
    }
}

static void GameSaveDataManager::sortPlayerScores(PlayerScoreSaveData* playerScores)
{
    PlayerScoreSaveData temp;
    for (uint8 i = 0; i < FAKE_PLAYER_COUNT - 1; i++)
        for (uint8 j = 0; j < FAKE_PLAYER_COUNT - i - 1; j++)
            if (playerScores[j].score < playerScores[j + 1].score)
            {
                GameSaveDataManager::copyPlayerScore(&temp, &playerScores[j]);
                GameSaveDataManager::copyPlayerScore(&playerScores[j], &playerScores[j + 1]);
                GameSaveDataManager::copyPlayerScore(&playerScores[j + 1], &temp);
            }
}

static void GameSaveDataManager::copyPlayerScore(PlayerScoreSaveData* destination, PlayerScoreSaveData* source)
{
    strncpy(destination->playerName, source->playerName, PLAYER_NAME_LENGTH);
    destination->score = source->score;
}


// System Settings

bool GameSaveDataManager::isSuccessParticleEnabled()
{
    bool value = false;
    SaveDataManager::getValue(this, (BYTE*)&value, offsetof(struct GameSaveData, isSuccessParticleEnabled), sizeof(value));
    return value;
}

void GameSaveDataManager::setSuccessParticleEnabled(bool enabled)
{
    SaveDataManager::setValue(this, (BYTE*)&enabled, offsetof(struct GameSaveData, isSuccessParticleEnabled), sizeof(enabled));
}

bool GameSaveDataManager::isBackgroundAnimationEnabled()
{
    bool value = false;
    SaveDataManager::getValue(this, (BYTE*)&value, offsetof(struct GameSaveData, isBackgroundAnimationEnabled), sizeof(value));
    return value;
}

void GameSaveDataManager::setBackgroundAnimationEnabled(bool enabled)
{
    SaveDataManager::setValue(this, (BYTE*)&enabled, offsetof(struct GameSaveData, isBackgroundAnimationEnabled), sizeof(enabled));
}

bool GameSaveDataManager::isNoteRumbleEnabled()
{
    bool value = false;
    SaveDataManager::getValue(this, (BYTE*)&value, offsetof(struct GameSaveData, isNoteRumbleEnabled), sizeof(value));
    return value;
}

void GameSaveDataManager::setNoteRumbleEnabled(bool enabled)
{
    SaveDataManager::setValue(this, (BYTE*)&enabled, offsetof(struct GameSaveData, isNoteRumbleEnabled), sizeof(enabled));
}

bool GameSaveDataManager::isSuccessFailureRumbleEnabled()
{
    bool value = false;
    SaveDataManager::getValue(this, (BYTE*)&value, offsetof(struct GameSaveData, isSuccessFailureRumbleEnabled), sizeof(value));
    return value;
}

void GameSaveDataManager::setSuccessFailureRumbleEnabled(bool enabled)
{
    SaveDataManager::setValue(this, (BYTE*)&enabled, offsetof(struct GameSaveData, isSuccessFailureRumbleEnabled), sizeof(enabled));
}

bool GameSaveDataManager::isMenuRumbleEnabled()
{
    bool value = false;
    SaveDataManager::getValue(this, (BYTE*)&value, offsetof(struct GameSaveData, isMenuRumbleEnabled), sizeof(value));
    return value;
}

void GameSaveDataManager::setMenuRumbleEnabled(bool enabled)
{
    SaveDataManager::setValue(this, (BYTE*)&enabled, offsetof(struct GameSaveData, isMenuRumbleEnabled), sizeof(enabled));
}

uint8 GameSaveDataManager::getRumbleIntensity()
{
    uint8 value = 0;
    SaveDataManager::getValue(this, (BYTE*)&value, offsetof(struct GameSaveData, rumbleIntensity), sizeof(value));
    return value;
}

void GameSaveDataManager::setRumbleIntensity(uint8 intensity)
{
    SaveDataManager::setValue(this, (BYTE*)&intensity, offsetof(struct GameSaveData, rumbleIntensity), sizeof(intensity));
}
