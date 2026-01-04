#include "UI/Widget/Debug/DebugWidget.h"
#include "UI/Widget/Debug/DebugWidgetRow.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "LuaEnv.h" 
#include "LuaValue.h"
#include "UnLuaLegacy.h"
#include "UnLuaModule.h"
#include "Blueprint/WidgetBlueprintLibrary.h" // 需要包含这个头文件
#include "Components/CanvasPanelSlot.h"      // 需要包含这个头文件
#include "Components/Overlay.h"
#include "Components/CanvasPanel.h"
#include "Components/Widget.h" // 包含此头文件以访问 MyWidget
//
//void UDebugWidget::NativeOnInitialized()
//{
//	Super::NativeOnInitialized();
//
//	if (RootCanvasPanel)
//	{
//		RootCanvasPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
//	}
//}
//
//FReply UDebugWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	// 只响应鼠标左键
//	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
//	{
//		// 将鼠标事件传递给 Slate，如果它处理了（比如点击了某个按钮），我们就返回它处理后的结果
//		FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
//		if (Reply.IsEventHandled())
//		{
//			return Reply;
//		}
//
//		// 如果 Slate 没处理，我们开始处理拖动逻辑
//		bIsDragging = true;
//		// 计算鼠标点击位置相对于Widget左上角的偏移
//		DragOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
//
//		// 返回一个已处理的 FReply，并请求捕获鼠标
//		// 这是标准的链式调用方法
//		return FReply::Handled().CaptureMouse(TakeWidget());
//
//	}
//
//	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
//}
//
//FReply UDebugWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	// 如果正在拖动
//	if (bIsDragging)
//	{
//		
//		// 获取鼠标在屏幕上的绝对位置
//		const FVector2D MousePosition = InMouseEvent.GetScreenSpacePosition();
//		// 计算窗口新的位置
//		const FVector2D NewPositionInScreen = MousePosition - DragOffset;
//
//		if (DraggableOverlay)
//		{
//			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(DraggableOverlay->Slot))
//			{
//				CanvasSlot->SetPosition(NewPositionInScreen);
//			}
//		}
//
//		// 返回一个已处理的回复，表示我们处理了此事件
//		return FReply::Handled();
//	}
//
//	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
//}
//
//FReply UDebugWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
//{
//	// 如果是鼠标左键松开，并且我们正在拖动
//	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsDragging)
//	{
//		FReply Reply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
//
//		bIsDragging = false;
//		return FReply::Handled().ReleaseMouseCapture();
//
//	}
//
//	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
//}

void UDebugWidget::AddDebugButton(const FString& ButtonText, const FString& OnClickedFuncName)
{

	if (!DebugRowClass || !DebugContentBox || OnClickedFuncName.IsEmpty())
	{
		return;
	}

	// 创建新的一行Widget
	UDebugWidgetRow* NewRow = CreateWidget<UDebugWidgetRow>(this, DebugRowClass);
	if (!NewRow)
	{
		return;
	}

	// 设置文本
	NewRow->Label->SetText(FText::FromString(ButtonText));

	// 绑定按钮的点击事件
	NewRow->OnActionButtonClicked.BindLambda([this, OnClickedFuncName]()
		{
			// 获取与此UObject关联的Lua环境
			const auto Env = IUnLuaModule::Get().GetEnv(this);
			if (Env)
			{
				// 2. 获取 Lua 状态机
				lua_State* L = Env->GetMainState();
				if (!L)
				{
					return;
				}

				// 3. 将 'this' (UDebugWidget 实例) 推送到 Lua 堆栈上，
				//    这会在堆栈顶部创建一个代表 'self' 的 table
				UnLua::Push(L, this);

				// 4. 从堆栈顶部的 table 创建一个 FLuaTable 对象
				UnLua::FLuaTable SelfTable(L, -1);

				// 5. 使用 'self' table 和函数名构造 FLuaFunction
				UnLua::FLuaFunction Func(L, SelfTable, TCHAR_TO_UTF8(*OnClickedFuncName));

				// 6. 清理堆栈 (弹出 self table)
				lua_pop(L, 1);

				if (Func.IsValid())
				{
					// 7. 调用成员函数
					Func.Call(this);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UnLua: Could not find member function '%s' on Lua object for '%s'"), *OnClickedFuncName, *this->GetName());
				}
			}
		});

	// 将新行添加到滚动框中
	DebugContentBox->AddChild(NewRow);
}