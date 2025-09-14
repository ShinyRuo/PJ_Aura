// ALL CODE FOR wangjunyang learning GAS


#include "UI/Widget/AuraUserWidget.h"
#include <functional>

#include "Components/PanelWidget.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}

TArray<UAuraUserWidget*> UAuraUserWidget::GetAllChildAuraWidget()
{
    UWidget* RootWidget = GetRootWidget();
    TArray<UAuraUserWidget*> OutWidgets;

    // 定义递归lambda函数，注意需要捕获OutWidgets
    std::function<void(UWidget*)> RecursiveCollectChildWidgets = [&](UWidget* CurWidget)
        {
            if (!CurWidget) return;

            // 尝试将当前控件的拥有者转换为UAuraUserWidget
            UAuraUserWidget* AuraWidget = Cast<UAuraUserWidget>(CurWidget);
            if (AuraWidget && AuraWidget != this) // 排除自身
            {
                OutWidgets.AddUnique(AuraWidget); // 避免重复添加
            }

            // 如果是面板控件，递归处理其子控件
            UPanelWidget* PanelWidget = Cast<UPanelWidget>(CurWidget);
            if (PanelWidget)
            {
                for (int32 i = 0; i < PanelWidget->GetChildrenCount(); ++i)
                {
                    UWidget* ChildWidget = PanelWidget->GetChildAt(i);
                    RecursiveCollectChildWidgets(ChildWidget); // 递归遍历子控件
                }
            }
        };

    // 从根控件开始遍历
    if (RootWidget)
    {
        RecursiveCollectChildWidgets(RootWidget);
    }

    return OutWidgets;
}
