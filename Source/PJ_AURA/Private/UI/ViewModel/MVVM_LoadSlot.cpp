// ALL CODE FOR  learning GAS


#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot()
{
	SetWidgetSwitcherIndex.Broadcast(LoadSlotStatus.GetIntValue());
}

void UMVVM_LoadSlot::SetPlayName(FString InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayName, InPlayerName); // broadcast widget bind 的控件
}

void UMVVM_LoadSlot::SetMapName(FString InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName); // broadcast
}
void UMVVM_LoadSlot::SetPlayerLevel(int32 InLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, InLevel);
}


