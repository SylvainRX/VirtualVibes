#include <LevelSetupData.h>
#include <LevelSong.h>

extern LevelSongROM MysticQuestLevelSong;
extern LevelSongROM OracleOfSeasonsLevelSong;

const char* SongTitles[] =
{
    "MYSTIC QUEST",
    "ORACLE OF SEASONS",
    "LOST IN THE RHYTHM",
	NULL,
};

const LevelSong* LevelSongs[] =
{
	&MysticQuestLevelSong,
	&OracleOfSeasonsLevelSong,
	&OracleOfSeasonsLevelSong,
	NULL,
};

LevelSetupData UniqueLevelSetupData =
{
	.songTitles = SongTitles,
	.levelSongs = LevelSongs,
	.playerScores = {{"", 0}, {"", 0}, {"", 0}, {"", 0}, {"", 0}, {"", 0}, {"", 0}, {"", 0}, {"", 0}, {"", 0}},
};
