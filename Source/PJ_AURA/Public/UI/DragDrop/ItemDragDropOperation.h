// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemDragDropOperation.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDropITem);

/**
 * 
 */
UCLASS()
class PJ_AURA_API UItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FOnDropITem OnDropItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag and Drop", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UUserWidget> SourceWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag and Drop", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UWidget> DropCheckWidget;

	virtual void DragCancelled_Implementation(const FPointerEvent& PointerEvent) override;

	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent) override;

	virtual void Drop_Implementation(const FPointerEvent& PointerEvent) override;
};
