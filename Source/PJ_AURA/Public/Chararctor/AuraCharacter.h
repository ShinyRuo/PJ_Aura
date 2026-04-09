// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Chararctor/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class APickUpItem;
class ULoadScreenSaveGame;
class UCameraComponent;
class UNiagaraComponent;
class UInventoryComponent;
/**
 * 
 */
UCLASS()
class PJ_AURA_API AAuraCharacter : public AAuraCharacterBase,public IPlayerInterface
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Stunned() override;
	virtual void OnRep_Burned() override;
	void LoadProgress();
	/** Combat Interface*/
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	/** end Combat Interface*/

	/** Player Interface*/
	virtual void AddToExp_Implementation(int32 InExp) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetExp_Implementation() override;
	virtual int32 FindLevelForExp_Implementation(int32 InExp) override;
	virtual int32 GetAttributePointsRewards_Implementation(int32 Level) const override;
	virtual int32 GetSpellPointsRewards_Implementation(int32 Level) const override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayLevel) override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual int32 GetSpellPoints_Implementation() const override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual void ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial) override;
	virtual void HideMagicCircle_Implementation() override;
	virtual void SaveProgress_Implementation(const FName& CheckPointTag, const FString& DestinationMapAssetName = FString("")) override;
	virtual void PickUpItem_Implementation(UItem* PickUpItem) override;
	virtual void DropItem_Implementation(UItem* DropItem) override;
	/** end Player Interface*/

	UFUNCTION(BlueprintImplementableEvent)
	void OnRotatingCamera(float DeltaX, float DeltaY);
	UFUNCTION(BlueprintImplementableEvent)
	void AddMove(float ForwardScale, float RightScale);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> NiagaraPlayComponent;

	UFUNCTION(Server,Reliable,BlueprintCallable)
	void OnPickUpItemBegin(APickUpItem* ItemToPickUp);

	void SaveInventory(ULoadScreenSaveGame* SaveData);
	void LoadInventory(const ULoadScreenSaveGame* SaveData);

	void ApplyEquipmentAttributeGE_OnInit();
	UFUNCTION()
	void OnEquipmentUpdate_ApplyEffectModifiers();


	UPROPERTY(Transient) // Transient 表示不序列化，避免存档问题
	FActiveGameplayEffectHandle EquipmentAttributeGEHandle;

protected:
	virtual void InitAbilityActorInfo() override;

	UFUNCTION(NetMulticast,Reliable)
	void MulticastLevelUpParticles() const;

	//角色死亡后持续时间，用于表现角色死亡
	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;

	//声明一个计时器，用于角色死亡后一定时间处理后续逻辑
	FTimerHandle DeathTimer;

private:
	TObjectPtr<UCameraComponent> TopDownCameraComponent;


};
