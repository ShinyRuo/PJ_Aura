// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UInventoryWidgetController;
class USpellMenuWidgetController;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
class UAuraUserWidget;
struct FWidgetContorllerParams;
class UAbilitySystemComponent;
class UAttributeSet;
/**
 * 
 */
UCLASS()
class PJ_AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()
public:


	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetContorllerParams& WCParams);

	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetContorllerParams& WCParams);

	USpellMenuWidgetController* GetSpellMenuWidgetController(const FWidgetContorllerParams& WCParams);

	UInventoryWidgetController* GetInventoryWidgetController(const FWidgetContorllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

	UFUNCTION(BlueprintCallable)
	UAuraUserWidget* GetOverlayWidget() const;

	/*
	 * UI Pool
	 */
	 // 获取或创建 Widget
	UAuraUserWidget* GetOrCreateWidget(const FName& WidgetName, TSubclassOf<UAuraUserWidget> WidgetClass, int32 ZOrder = 0);

	// 通过 FName 获取 Widget
	UAuraUserWidget* GetWidgetByName(const FName& WidgetName) const;

	// 释放 Widget
	void ReleaseWidget(const FName& WidgetName);


private:
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> UOverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditAnywhere)	
	TSubclassOf<UInventoryWidgetController> InventoryWidgetControllerClass;

	// Widget 池，使用 FName 作为索引
	UPROPERTY()
	TMap<FName, TObjectPtr<UAuraUserWidget>> WidgetPool;

};
