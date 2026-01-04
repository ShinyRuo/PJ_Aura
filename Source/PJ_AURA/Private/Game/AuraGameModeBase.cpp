// ALL CODE FOR wangjunyang learning GAS


#include "Game/AuraGameModeBase.h"

#include "Game/AuraGameInstance.h"
#include "Game/ItemManager.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->LoadSlotName,LoadSlot->SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->LoadSlotName, LoadSlot->SlotIndex);
	}

	USaveGame* SaveGameObj = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObj);
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayName();
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;
	LoadScreenSaveGame->SaveSlotStatus = Taken;

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->LoadSlotName, SlotIndex);
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObj = nullptr;
	if(UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObj = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObj = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObj);
	return LoadScreenSaveGame;
}

void AAuraGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

ULoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameSaveData() const
{
	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());
	const	FString InGameLoadSlotName = AuraGI->LoadSlotName;
	const	int32 InGameLoadSlotIndex = AuraGI->LoadSlotIndex;
	return GetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex);
}

void AAuraGameModeBase::SaveInGameProgressData(ULoadScreenSaveGame* SaveObject) const
{
	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());
	const	FString InGameLoadSlotName = AuraGI->LoadSlotName;
	const	int32 InGameLoadSlotIndex = AuraGI->LoadSlotIndex;
	AuraGI->PlayerStartTag = SaveObject->PlayerStartTag;
	UGameplayStatics::SaveGameToSlot(SaveObject, InGameLoadSlotName, InGameLoadSlotIndex);
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	const FString SlotName = Slot->LoadSlotName;
	const int32 SlotIndex = Slot->SlotIndex;
	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, Maps.FindChecked(Slot->GetMapName()));
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
	if (!Actors.IsEmpty() && AuraGI)
	{
		AActor* SelectedActor = Actors[0];
		for (AActor* Actor : Actors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				if (PlayerStart->PlayerStartTag == AuraGI->PlayerStartTag)
				{
					SelectedActor = PlayerStart;
					break;
				}
			}
		}
		return SelectedActor;
	}
	return nullptr;
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Maps.Add(DefaultMapName, DefaultMap);

}
