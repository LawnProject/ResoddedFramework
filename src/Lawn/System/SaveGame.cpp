#include "Music.h"
#include "SaveGame.h"
#include "LegacySaveGame.h"


static const char SAVE_HEADER_MAGIC[16] = "RESODDED_SAVEV1";


bool LawnLoadGame(Board *theBoard, const std::string &theFilePath)
{
	return LegacySaveGame::LawnLoadGame(theBoard, theFilePath);
}

//0x4820D0
bool LawnSaveGame(Board *theBoard, const std::string &theFilePath)
{
	return LegacySaveGame::LawnSaveGame(theBoard, theFilePath);
}
