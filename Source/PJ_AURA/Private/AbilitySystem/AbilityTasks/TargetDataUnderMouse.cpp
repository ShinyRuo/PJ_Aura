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
		 *��������� ��˳����  [client]UTargetDataUnderMouse::Activate() ->rep->[Server]UTargetDataUnderMouse::Activate()
		 * [client]SendMouseCursorData() ��ͬʱ [server] �ڸɱ�� ����֪��client�Ѿ�Activate�� 
		 *  [client]ServerSetReplicatedTargetData ��ͬʱ [server] rep��Activate�� ����AbilityTargetDataSetDelegate�ȴ�TargetData�ĵ���
		 *												 [server] �յ�TargetData�� ִ��OnTargetDataReplicatedCallback
		 * ����Ҳ�п���server���յ�TargetData �� ��rep��Activate ��Ϊ��֪������Э����tcp����udp��
		 * ����������SetDelegate֮�� ���һ���ǲ����Ѿ��յ�TargetData�� ���˾�ֱ��ִ��Delegate
		 */
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPreditionKey);
		if (!bCalledDelegate)
		{
			//�������ֱ��ִ��Delegate ��wait
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
	//�ѿͻ����ϼ�����������ݷ���server[��abilityCompnent��һ��map�������������] ȷ��ͬ�� ��Ȼ���˲�֪�����������ķŵ�
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
	//�����������server �Ǹ�����TargetData��map ���Զ������������
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
