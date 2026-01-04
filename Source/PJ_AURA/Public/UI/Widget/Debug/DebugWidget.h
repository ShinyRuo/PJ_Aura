#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/AuraUserWidget.h"

#include "DebugWidget.generated.h"


class UCanvasPanel;
class UOverlay;
class UScrollBox;
class UDebugWidgetRow;

UCLASS()
class PJ_AURA_API UDebugWidget : public UAuraUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief 从Lua脚本调用此函数来添加一个调试按钮
	 * @param ButtonText 按钮上显示的文本
	 * @param OnClickedFunc 按下按钮时要执行的Lua函数
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void AddDebugButton(const FString& ButtonText, const FString& OnClickedFuncName);

protected:

protected:
	////~ Begin UUserWidget Interface
	//virtual void NativeOnInitialized() override;
	//virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	////~ End UUserWidget Interface


	// 蓝图子类中要创建的行控件的类型
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	TSubclassOf<UDebugWidgetRow> DebugRowClass;

	// 将蓝图中的滚动框绑定到此变量
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> DebugContentBox;


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> DraggableOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> RootCanvasPanel;

private:
	/** 标记是否正在拖动窗口 */
	bool bIsDragging = false;

	/** 记录开始拖动时，鼠标相对于窗口左上角的偏移 */
	FVector2D DragOffset;
};