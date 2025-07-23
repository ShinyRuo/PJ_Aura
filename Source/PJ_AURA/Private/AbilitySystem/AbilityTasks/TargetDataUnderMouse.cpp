// ALL CODE FOR  learning GAS


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

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

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
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
