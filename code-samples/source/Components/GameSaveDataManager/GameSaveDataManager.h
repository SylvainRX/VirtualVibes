#ifndef GAME_SAVE_DATA_MANAGER_H_
#define GAME_SAVE_DATA_MANAGER_H_

#include <Macros.h>
#include <SaveDataManager.h>

#define INITIAL_ACTIONS_FLAGS_STRING_SIZE 16
#define DID_SETUP_FAKE_DATA_STRING "DID_SETUP_DATA_"
#define DID_SHOW_TUTORIAL_STRING   "DID_SHOW_TUTO__"

typedef struct PlayerScoreSaveData
{
    char playerName[PLAYER_NAME_LENGTH + 1];
    uint32 score;
} PlayerScoreSaveData;

typedef struct SongPlayerScoreSaveData
{
    PlayerScoreSaveData easyPlayerScores[HIGHSCORE_COUNT];
    PlayerScoreSaveData normalPlayerScores[HIGHSCORE_COUNT];
    PlayerScoreSaveData hardPlayerScores[HIGHSCORE_COUNT];
} SongPlayerScoreSaveData;

// this struct is never instantiated, its sole purpose is to determine offsets of its members.
// therefore it acts as kind of like a map of sram content.
typedef struct GameSaveData
{
    SaveData baseSaveData;

    char tutorialFlag[INITIAL_ACTIONS_FLAGS_STRING_SIZE];
    char lastPlayerName[PLAYER_NAME_LENGTH + 1];
    SongPlayerScoreSaveData songPlayerScores[SONG_TITLES_COUNT];
    bool isSuccessParticleEnabled;
    bool isBackgroundAnimationEnabled;
    bool isNoteRumbleEnabled;
    bool isSuccessFailureRumbleEnabled;
    bool isMenuRumbleEnabled;
    uint8 rumbleIntensity;

    // Leave at the end of the struct to automatically reset save data on update
    char initialSetupFlag[INITIAL_ACTIONS_FLAGS_STRING_SIZE];
} GameSaveData;

singleton class GameSaveDataManager : SaveDataManager
{
    PlayerScoreSaveData playerScores[HIGHSCORE_COUNT];

    void initialSetup();

    // Tutorial

    void setTutorialAsShown();
    bool wasTutorialShown();

    // Player Scores

    void clearAll();
    void setupFakeData();

    void getLastPlayerName(char* lastPlayerName);

    void savePlayerScore(uint8 songIndex, uint8 difficulty, char* playerName, uint32 score);
    PlayerScoreSaveData* getPlayerScore(uint8 songIndex, uint8 difficulty);
    bool isPlayerScoreHighscore(uint8 songIndex, uint8 songDifficulty, uint32 score);
    void clearPlayerScores(uint8 songIndex, uint8 difficulty);

    // System Settings

    bool isSuccessParticleEnabled();
    void setSuccessParticleEnabled(bool enabled);
    bool isBackgroundAnimationEnabled();
    void setBackgroundAnimationEnabled(bool enabled);
    bool isNoteRumbleEnabled();
    void setNoteRumbleEnabled(bool enabled);
    bool isSuccessFailureRumbleEnabled();
    void setSuccessFailureRumbleEnabled(bool enabled);
    bool isMenuRumbleEnabled();
    void setMenuRumbleEnabled(bool enabled);
    uint8 getRumbleIntensity();
    void setRumbleIntensity(uint8 intensity);
}

#endif
