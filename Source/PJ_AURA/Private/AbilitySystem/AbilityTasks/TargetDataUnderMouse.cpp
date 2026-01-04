// ALL CODE FOR  learning GAS


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "PJ_AURA/PJ_AURA.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	const bool bIsLocallyContrlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyContrlled)
	{
		SendMouseCursorData();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPreditionKey = GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPreditionKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
		/*
		 *正常情况下 的顺序是  [client]UTargetDataUnderMouse::Activate() ->rep->[Server]UTargetDataUnderMouse::Activate()
		 * [client]SendMouseCursorData() 的同时 [server] 在干别的 还不知道client已经Activate了 
		 *  [client]ServerSetReplicatedTargetData 的同时 [server] rep到Activate了 设置AbilityTargetDataSetDelegate等待TargetData的到来
		 *												 [server] 收到TargetData了 执行OnTargetDataReplicatedCallback
		 * 但是也有可能server先收到TargetData 再 被rep到Activate 因为不知道发包协议是tcp还是udp？
		 * 所有这里在SetDelegate之后 检查一下是不是已经收到TargetData了 有了就直接执行Delegate
		 */
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPreditionKey);
		if (!bCalledDelegate)
		{
			//如果不是直接执行Delegate 就wait
			SetWaitingOnRemotePlayerData();
		}
	}

}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	//无论服务器是否验证 TargetData，FScopedPredictionWindow 都是必要的。
	//它的核心价值是为客户端的预测性操作提供上下文标记，
	//确保 GAS 系统能正确管理 “临时预测状态” 与 “服务器权威状态” 的对齐，
	//避免因网络延迟、状态残留或未来需求变更导致的一致性问题。在 GAS 客户端预测逻辑中，这是一种 “最佳实践”，应始终遵循。

	// 这里server不做验证 这个targetdata的操作 不会因为验证而回滚
	// server会收到targetdata数据而缓存 （虽然下面server调用ConsumeClientReplicatedTargetData清空了 因为不需要缓存）
	// 假如说server 需要验证targetdata 或者说不验证 只缓存 在后面继续用
	// 那么这里的ScopedPrediction 就很重要
	// 这里使用的构造方法 是生成了一个新的预测key 这个预测key 是在ability激活后生成的预测key之后的 如果回滚这个ability 那么根据key的调用链 也会回滚这个PredictionWindow
	// 回滚的操作 就是把client这里预测的targetdata清理掉
	// 另外 也可以直接在构造的时候传入 ability激活时生成的预测key 那么回滚的时候 所有的这个预测key的操作都会回滚
	// 回滚 什么操作呢 ？ 能回滚 ASC里预设好的 能预测的操作 能预测就能回滚
	// ScopedPrediction构造的时候 会设置ASC的预测key ScopedPrediction析构的时候 会设置ASC的预测key为上一个key
	// ASC在预测模式下 执行能预测的操作时 会关联这个操作 与 预测key 当然 回退的操作ASC也准备好了 还有一些数据
	// 在需要回退的时候 ASC根据预测key 找到回退操作 根据之前保存的数据 加上 固定好的操作 回退这个key改变的所有属性

	// 这里只是通过ServerSetReplicatedTargetData rpc机制把targetdata传给server了 server甚至不保存 一拿到就扔了
	// 它到底用不用预测key 我还是觉得不需要 这里只是用TargetData便利的机制 让server能同步鼠标点击数据 与预测是无关的
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Target, false, CursorHit);
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	DataHandle.Add(Data);
	Data->HitResult = CursorHit;
	//把客户端上计算的坐标数据发给server[由abilityCompnent的一个map来保存这个数据] 确保同步 不然别人不知道你的鼠标往哪放的
	//AbilityTargetDataMap
	AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(), 
		AbilitySystemComponent->ScopedPredictionKey);
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,FGameplayTag ActivationTag)
{
	//这个方法告诉server 那个保持TargetData的map 可以丢掉这个数据了
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
