// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PJ_AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
protected:
	UFUNCTION(BlueprintImplementableEvent)

	void WidgetControllerSet();
	UFUNCTION(BlueprintPure)
	TArray<UAuraUserWidget*> GetAllChildAuraWidget();
	//UUserWidget蓝图设计界面 左下角时一个WidgetTree的UObject
	//WidgetTree仅有一个RootWidget 这就是为什么一个空的UUserWidget最开始只能拖一个控件进去
};
