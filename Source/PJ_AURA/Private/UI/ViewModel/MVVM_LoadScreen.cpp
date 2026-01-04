// ALL CODE FOR  learning GAS


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->LoadSlotName = FString("LoadSlot_0");
	LoadSlot_0->SlotIndex = 0;
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->LoadSlotName = FString("LoadSlot_1");
	LoadSlot_1->SlotIndex = 1;
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->LoadSlotName = FString("LoadSlot_2");
	LoadSlot_2->SlotIndex = 2;
	LoadSlots.Add(0, LoadSlot_0);
	LoadSlots.Add(1, LoadSlot_1);
	LoadSlots.Add(2, LoadSlot_2);
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
	if(AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		//LoadSlots[Slot]->PlayName = EnteredName;
		LoadSlots[Slot]->SetPlayName(EnteredName); //¿Ø¼þÏìÓ¦
		LoadSlots[Slot]->SetMapName(AuraGameMode->DefaultMapName);
		LoadSlots[Slot]->LoadSlotStatus = Taken;
		LoadSlots[Slot]->PlayerStartTag = AuraGameMode->DefaultPlayerStartTag;
		LoadSlots[Slot]->SetPlayerLevel(1);
		AuraGameMode->SaveSlotData(LoadSlots[Slot], Slot);
		LoadSlots[Slot]->InitializeSlot();

		if (UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance()))
		{
			AuraGI->LoadSlotName = LoadSlots[Slot]->LoadSlotName;
			AuraGI->LoadSlotIndex = LoadSlots[Slot]->SlotIndex;
			AuraGI->PlayerStartTag = AuraGameMode->DefaultPlayerStartTag;
		}
	}
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(ESaveSlotStatus::EnterName); //ÇÐ»»switcher
	LoadSlots[Slot]->LoadSlotStatus = ESaveSlotStatus::EnterName;
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	SelectedSlotIndex = Slot;
	SlotSelected.Broadcast(Slot);
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		if (LoadSlot.Key == Slot)
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(false);
		}
		else
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(true);
		}
	}
}

void UMVVM_LoadScreen::DeleteSlotButtonPressed()
{
	UMVVM_LoadSlot* SelectedSlot = GetSelectedSlot();
	if (IsValid(SelectedSlot))
	{
		AAuraGameModeBase::DeleteSlot(SelectedSlot->LoadSlotName, SelectedSlot->SlotIndex);
		SelectedSlot->LoadSlotStatus = Vacant;
		SelectedSlot->InitializeSlot();
		SelectedSlot->EnableSelectSlotButton.Broadcast(true);
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (AuraGameMode && SelectedSlotIndex >= 0)
	{
		if (UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance()))
		{
			AuraGI->PlayerStartTag = LoadSlots[SelectedSlotIndex]->PlayerStartTag;
			AuraGI->LoadSlotName = LoadSlots[SelectedSlotIndex]->LoadSlotName;
			AuraGI->LoadSlotIndex = LoadSlots[SelectedSlotIndex]->SlotIndex;
		}
		AuraGameMode->TravelToMap(LoadSlots[SelectedSlotIndex]);
	}
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetSelectedSlot() const
{
	return GetLoadSlotViewModelByIndex(SelectedSlotIndex);
}

void UMVVM_LoadScreen::LoadData()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (const TTuple<int32,UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		ULoadScreenSaveGame* SaveObject = AuraGameMode->GetSaveSlotData(LoadSlot.Value->LoadSlotName, LoadSlot.Key);
		LoadSlot.Value->SetPlayName(SaveObject->PlayerName);
		LoadSlot.Value->SetMapName(SaveObject->MapName);
		LoadSlot.Value->LoadSlotStatus = SaveObject->SaveSlotStatus;
		LoadSlot.Value->PlayerStartTag = SaveObject->PlayerStartTag;
		LoadSlot.Value->SetPlayerLevel(SaveObject->PlayerLevel);
		LoadSlot.Value->InitializeSlot();
	}
}
