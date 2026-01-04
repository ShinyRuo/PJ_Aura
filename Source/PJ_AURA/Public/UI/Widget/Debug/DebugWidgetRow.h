#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DebugWidgetRow.generated.h"

class UTextBlock;
class UButton;


DECLARE_DELEGATE(FOnActionButtonClicked);

UCLASS()
class PJ_AURA_API UDebugWidgetRow : public UUserWidget
{
	GENERATED_BODY()

public:
	// 将蓝图中的控件绑定到C++变量
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Label;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ActionButton;

	// 标准委托，可以绑定Lambda
	FOnActionButtonClicked OnActionButtonClicked;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButtonClicked();
};