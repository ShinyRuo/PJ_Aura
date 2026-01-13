// ALL CODE FOR  learning GAS


#include "UI/DragDrop/ItemDragDropOperation.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UItemDragDropOperation::DragCancelled_Implementation(const FPointerEvent& PointerEvent)
{

	// 首先进行安全检查
	if (!IsValid(SourceWidget) || !IsValid(DropCheckWidget))
	{
		// 如果关键的Widget无效，则执行默认的取消操作
		if (IsValid(SourceWidget))
		{
			SourceWidget->SetVisibility(ESlateVisibility::Visible);
		}
		Super::DragCancelled_Implementation(PointerEvent);
		return;
	}

	// 1. 获取 DropCheckWidget 的屏幕空间几何信息
	const FGeometry& WidgetGeometry = DropCheckWidget->GetCachedGeometry();

	// 2. 根据几何信息创建一个屏幕空间的矩形区域 (FSlateRect)
	const FSlateRect WidgetRect = WidgetGeometry.GetLayoutBoundingRect();

	// 3. 获取鼠标在拖拽取消时的屏幕空间位置
	const FVector2D MousePosition = PointerEvent.GetScreenSpacePosition();

	// 4. 判断鼠标位置是否在矩形区域内
	if (WidgetRect.ContainsPoint(MousePosition))
	{
		// 鼠标在 DropCheckWidget 区域内，这是常规的取消操作
		// 恢复原始物品的显示
		SourceWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		// 鼠标在 DropCheckWidget 区域外，执行“丢弃”操作
		OnDropItem.Broadcast();
	}

}

void UItemDragDropOperation::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Dragged_Implementation(PointerEvent);
	if (IsValid(SourceWidget))
	{
		// 拖动开始时立即隐藏，而不是在拖动过程中反复设置
		if (SourceWidget->GetVisibility() == ESlateVisibility::Visible)
		{
			SourceWidget->SetVisibility(ESlateVisibility::Hidden);
		}
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
