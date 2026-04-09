// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "AuraEnemySpawnPoint.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemySpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class PJ_AURA_API AAuraEnemySpawnVolume : public AActor,public ISaveInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties

	AAuraEnemySpawnVolume();

	/* Save Interface */
	virtual void LoadActor_Implementation() override;
	/* Save Interface 结束 */

	//当前怪物生成体积是否已经生成过敌人
	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//配置当前的生成体积所需生成的敌人
	UPROPERTY(EditAnywhere)
	TArray<AAuraEnemySpawnPoint*> SpawnPoints;
private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Box;

};
