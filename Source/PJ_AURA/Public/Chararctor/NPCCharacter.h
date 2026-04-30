// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interaction/MiniMapInterface.h"
#include "Interaction/NPCInterface.h"
#include "Interaction/HighlightInterface.h"
#include "NPCCharacter.generated.h"

class UNPCInteractionComponent;
class USphereComponent;
class UWidgetComponent;

/**
 * Base class for dialogue NPCs
 * Inherits from Character and implements multiple interfaces
 */
UCLASS()
class PJ_AURA_API ANPCCharacter : public ACharacter, public IMiniMapInterface, public IHighlightInterface
{
    GENERATED_BODY()

public:
    ANPCCharacter();

    // BeginPlay
    virtual void BeginPlay() override;

    // Tick
    virtual void Tick(float DeltaTime) override;

    // IMiniMapInterface implementation
	virtual void GetMiniMapIcon_Implementation(UTexture2D*& OutMiniMapIcon, bool& OutIsPermanent, bool& OutShouldRemoveIcon, bool& OutIgnoreMapRotation) override;
    virtual void GetMiniMapLocationAndRotation_Implementation(FVector& OutLocation, FRotator& OutRotation) override;

    // IHighlightInterface implementation
    virtual void HighlightActor() override;
    virtual void UnHighlightActor() override;

    // Getters for components
    UFUNCTION(BlueprintPure, Category = "NPC")
    UNPCInteractionComponent* GetNPCInteractionComponent() const { return NPCInteractionComponent; }

protected:
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UNPCInteractionComponent* NPCInteractionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWidgetComponent* NameTagWidget;

    // Helper functions
    void SetupComponents();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
    TObjectPtr<UTexture2D>	MiniMapIcon;
};