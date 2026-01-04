// ALL CODE FOR wangjunyang learning GAS


#include "UI/HUD/AuraHUD.h"

#include "Player/AuraPlayerState.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/InventoryWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetContorllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, UOverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbackToDependencies();
	}
	return OverlayWidgetController;
}

UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetContorllerParams& WCParams)
{
	if (AttributeMenuWidgetController == nullptr)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
		AttributeMenuWidgetController->BindCallbackToDependencies();
	}
	return AttributeMenuWidgetController;
}

USpellMenuWidgetController* AAuraHUD::GetSpellMenuWidgetController(const FWidgetContorllerParams& WCParams)
{
	if (SpellMenuWidgetController == nullptr)
	{
		SpellMenuWidgetController = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);
		SpellMenuWidgetController->SetWidgetControllerParams(WCParams);
		SpellMenuWidgetController->BindCallbackToDependencies();
	}
	return SpellMenuWidgetController;
}

UInventoryWidgetController* AAuraHUD::GetInventoryWidgetController(const FWidgetContorllerParams& WCParams)
{
	if (InventoryWidgetController == nullptr)
	{
		InventoryWidgetController = NewObject<UInventoryWidgetController>(this, InventoryWidgetControllerClass);
		InventoryWidgetController->SetWidgetControllerParams(WCParams);
		if (AAuraPlayerState* AuraPS = Cast<AAuraPlayerState>(WCParams.PlayerState))
		{
			InventoryWidgetController->Initialize(AuraPS->GetInventoryComponent());
		}
		InventoryWidgetController->BindCallbackToDependencies();
	}
	return InventoryWidgetController;	
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class Uninitialized,please fill out BP_AuraHUD"));
	checkf(UOverlayWidgetControllerClass,TEXT("Overlay Widget Controller Class Uninitialized,please fill out BP_AuraHUD"))

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);

	const FWidgetContorllerParams WidgetContorllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetContorllerParams);
	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitalValue();

	Widget->AddToViewport();
}


UAuraUserWidget* AAuraHUD::GetOrCreateWidget(const FName& WidgetName, TSubclassOf<UAuraUserWidget> WidgetClass)
{
	// 如果 Widget 已存在于池中，直接返回
	if (WidgetPool.Contains(WidgetName))
	{
		return WidgetPool[WidgetName];
	}

	// 创建新的 Widget
	UAuraUserWidget* NewWidget = CreateWidget<UAuraUserWidget>(GetWorld(), WidgetClass);
	if (NewWidget)
	{
		// 添加到池中
		NewWidget->WidgetName = WidgetName;
		WidgetPool.Add(WidgetName, NewWidget);
		NewWidget->AddToViewport();
	}

	return NewWidget;
}

UAuraUserWidget* AAuraHUD::GetWidgetByName(const FName& WidgetName) const
{
	// 从池中获取 Widget
	if (WidgetPool.Contains(WidgetName))
	{
		return WidgetPool[WidgetName];
	}

	return nullptr;
}

void AAuraHUD::ReleaseWidget(const FName& WidgetName)
{
	// 检查 Widget 是否存在于池中
	if (WidgetPool.Contains(WidgetName))
	{
		UAuraUserWidget* Widget = WidgetPool[WidgetName];
		if (Widget)
		{
			// 从视图中移除并销毁
			Widget->RemoveFromParent();
			Widget->ConditionalBeginDestroy();
		}

		// 从池中移除
		WidgetPool.Remove(WidgetName);
	}
}