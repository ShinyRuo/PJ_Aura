// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionManager.generated.h"

class UCursorManagerComponent;
class UNPCInteractionComponent;

UCLASS()
class PJ_AURA_API AInteractionManager : public AActor
{
    GENERATED_BODY()

public:
    AInteractionManager();

    // Component references
    UPROPERTY(VisibleAnywhere BlueprintReadOnly)
    TObjectPtr<UCursorManagerComponent> CursorManager;

    // Actor interface
    virtual void BeginPlay() override;

    // Input handling
    void OnLeftMouseButtonClicked();

    // Hover detection using ray casting
    void CheckHoverUnderCursor();

    // Add/remove NPCs from hover tracking
    void RegisterNPCForHover(UNPCInteractionComponent* NPCComponent);
    void UnregisterNPCFromHover(UNPCInteractionComponent* NPCComponent);

protected:
    // NPC interaction components we're tracking
    TSet<TObjectPtr<UNPCInteractionComponent>> TrackedNPCComponents;

    // Currently hovered NPC
    TObjectPtr<UNPCInteractionComponent> CurrentlyHoveredNPC;

    // Helper functions
    UNPCInteractionComponent* GetHoveredNPCUnderCursor();
    bool IsRayHittingNPC(FHitResult& OutHit) const;

    // Callback for hover timer
    UFUNCTION()
    void OnHoverTimer();

    // Timer for hover detection
    FTimerHandle HoverCheckTimer;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction Manager")
    float HoverCheckInterval = 0.02f; // 50fps for smooth hover detection

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction Manager")
    float MaxInteractionRange = 1000.0f; // Maximum distance for interaction

private:
    // Cached player controller
    TObjectPtr<APlayerController> PlayerController;

    // Cached player pawn
    TObjectPtr<APawn> PlayerPawn;
};