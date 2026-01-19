// ALL CODE FOR  learning GAS


#include "Game/LoadScreenSaveGame.h"

FSavedMap ULoadScreenSaveGame::GetSavedMapWithMapName(const FString& InMapName) const
{
    for (const FSavedMap& SavedMap : SavedMaps)
    {
        if (SavedMap.MapAssetName == InMapName)
        {
            return SavedMap;
        }
    }
    return FSavedMap();
}

bool ULoadScreenSaveGame::HasMap(const FString& InMapName) const
{
    for (const FSavedMap& SavedMap : SavedMaps)
    {
        if (SavedMap.MapAssetName == InMapName)
        {
            return true;
        }
    }
    return false;
}
