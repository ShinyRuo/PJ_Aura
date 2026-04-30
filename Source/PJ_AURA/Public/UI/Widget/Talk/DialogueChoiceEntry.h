// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "DialogueChoiceEntry.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoiceSelected, int32, ChoiceIndex);

/**
 * 单个对话选择项条目（包含按钮＋文本），用于 DialogueWidget 的动态生成。
 * 设计：在 Widget Blueprint 中使用该类作为父类，并将 Button/TextBlock 绑定到对应变量（BindWidget）。
 */
UCLASS()
class PJ_AURA_API UDialogueChoiceEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	// 初始化该条目（必须在创建后调用）
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void InitializeChoice(int32 InChoiceIndex, const FText& InChoiceText);

	// 绑定按钮点击后会广播该委托
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnChoiceSelected OnChoiceSelected;

protected:
	// 绑定到 Blueprint Widget 中的 Button（在 UMG 中用同名控件绑定）
	UPROPERTY(meta = (BindWidget))
	UButton* ChoiceButton;

	// 绑定到 Blueprint Widget 中的 TextBlock
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ChoiceText;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	// 本条目对应的选择索引
	int32 ChoiceIndex = INDEX_NONE;

	// 按钮点击回调
	UFUNCTION()
	void HandleChoiceButtonClicked();
};
