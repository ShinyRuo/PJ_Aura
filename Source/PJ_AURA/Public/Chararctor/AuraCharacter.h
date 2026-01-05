// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Chararctor/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

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
	virtual void SaveProgress_Implementation(const FName& CheckPointTag) override;
	virtual void PickUpItem_Implementation(UItem* PickUpItem) override;
	virtual void DropItem_Implementation(UItem* DropItem) override;
	/** end Player Interface*/

	UFUNCTION(BlueprintImplementableEvent)
	void OnRotatingCamera(float DeltaX, float DeltaY);
	UFUNCTION(BlueprintImplementableEvent)
	void AddMove(float ForwardScale, float RightScale);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> NiagaraPlayComponent;

	UFUNCTION(BlueprintCallable)
	void OnPickUpItemBegin(APickUpItem* ItemToPickUp);


protected:
	virtual void InitAbilityActorInfo() override;

	UFUNCTION(NetMulticast,Reliable)
	void MulticastLevelUpParticles() const;

private:
	TObjectPtr<UCameraComponent> TopDownCameraComponent;


};
