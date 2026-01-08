// ALL CODE FOR  learning GAS


#include "UI/DragDrop/ItemDragDropOperation.h"

#include "Blueprint/UserWidget.h"

void UItemDragDropOperation::DragCancelled_Implementation(const FPointerEvent& PointerEvent)
{
	Super::DragCancelled_Implementation(PointerEvent);

	if (IsValid(SourceWidget))
	{
		SourceWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UItemDragDropOperation::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Dragged_Implementation(PointerEvent);
	if (IsValid(SourceWidget))
	{
		SourceWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UItemDragDropOperation::Drop_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Drop_Implementation(PointerEvent);
	if (IsValid(SourceWidget))
	{
		SourceWidget->SetVisibility(ESlateVisibility::Visible);
	}
}
