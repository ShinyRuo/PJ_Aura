// ALL CODE FOR  learning GAS


#include "UI/Widget/Talk/DialogueChoiceEntry.h"

void UDialogueChoiceEntry::InitializeChoice(int32 InChoiceIndex, const FText& InChoiceText)
{
	ChoiceIndex = InChoiceIndex;

	if (ChoiceText)
	{
		ChoiceText->SetText(InChoiceText);
	}
}

void UDialogueChoiceEntry::NativeConstruct()
{
	Super::NativeConstruct();

	if (ChoiceButton)
	{
		ChoiceButton->OnClicked.AddDynamic(this, &UDialogueChoiceEntry::HandleChoiceButtonClicked);
	}
}

void UDialogueChoiceEntry::NativeDestruct()
{
	if (ChoiceButton)
	{
		ChoiceButton->OnClicked.RemoveDynamic(this, &UDialogueChoiceEntry::HandleChoiceButtonClicked);
	}

	Super::NativeDestruct();
}

void UDialogueChoiceEntry::HandleChoiceButtonClicked()
{
	// 广播被选中的索引（接收方在 DialogueWidget 中绑定并调用 SelectChoice）
	OnChoiceSelected.Broadcast(ChoiceIndex);
}

