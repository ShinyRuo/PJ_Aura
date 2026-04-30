// ALL CODE FOR  learning GAS


#include "Game/AuraGameInstance.h"

void UAuraGameInstance::Init()
{
	Super::Init();
	if (DialogueDataManagerClass)
	{
		DialogueDataManager = NewObject<UDialogueDataManager>(this, DialogueDataManagerClass);
	}
}

UDialogueDataManager* UAuraGameInstance::GetDialogueDataManager()
{
		return DialogueDataManager;
}
