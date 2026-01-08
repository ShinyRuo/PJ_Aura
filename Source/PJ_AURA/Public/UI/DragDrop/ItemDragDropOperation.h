// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class PJ_AURA_API UItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag and Drop", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UUserWidget> SourceWidget;

	virtual void DragCancelled_Implementation(const FPointerEvent& PointerEvent) override;

	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent) override;

	virtual void Drop_Implementation(const FPointerEvent& PointerEvent) override;
};
