#include "UI/Widget/Debug/DebugWidgetRow.h"
#include "Components/Button.h"

void UDebugWidgetRow::NativeConstruct()
{
	Super::NativeConstruct();

	// 将按钮的动态委托绑定到一个UFUNCTION
	if (ActionButton)
	{
		ActionButton->OnClicked.AddDynamic(this, &UDebugWidgetRow::OnButtonClicked);
	}
}

void UDebugWidgetRow::OnButtonClicked()
{
	// 执行绑定到我们标准委托上的函数（即Lambda）
	OnActionButtonClicked.ExecuteIfBound();
}