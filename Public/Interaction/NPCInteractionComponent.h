// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/NPCInterface.h"
#include "NPCInteractionComponent.generated.h"

UENUM(BlueprintType)
enum class EDialogueState : uint8
{
    DS_None,
    DS_Hovering,
    DS_InConversation
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PJ_AURA_API UNPCInteractionComponent : public UActorComponent, public INPCInterface
{
    GENERATED_BODY()

public:
    UNPCInteractionComponent();

    // Component interface
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // INPCInterface interface
    virtual FName GetDialogueID_Implementation() const override;
    virtual bool IsInConversation_Implementation() const override;
    virtual void StartConversation_Implementation(AActor* PlayerActor) override;
    virtual void EndConversation_Implementation() override;
    virtual float GetInteractionRange_Implementation() const override;

    // Interaction detection
    UFUNCTION(BlueprintCallable, Category = "NPC Interaction")
    void CheckForHover();

    UFUNCTION(BlueprintCallable, Category = "NPC Interaction")
    void OnNPCClicked();

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Interaction")
    FName DialogueID = FName("DefaultNPC");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Interaction")
    float InteractionRange = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Interaction")
    float HoverCheckInterval = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Interaction")
    USoundBase* HoverSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Interaction")
    USoundBase* ClickSound;

    // State access
    UFUNCTION(BlueprintPure, Category = "NPC Interaction")
    EDialogueState GetDialogueState() const { return DialogueState; }

    // Events
    UFUNCTION(BlueprintNativeEvent, Category = "NPC Interaction")
    void OnDialogueStarted(AActor* PlayerActor);

    UFUNCTION(BlueprintNativeEvent, Category = "NPC Interaction")
    void OnDialogueEnded();

    UFUNCTION(BlueprintNativeEvent, Category = "NPC Interaction")
    void OnHoverStateChanged(bool IsHovering);

protected:
    // State variables
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC Interaction")
    EDialogueState DialogueState = EDialogueState::DS_None;

    // Timer for hover checking
    FTimerHandle HoverCheckTimer;

    // Current hovering actor (player)
    UPROPERTY()
    AActor* CurrentHoveringActor = nullptr;

    // Cached player controller
    UPROPERTY()
    APlayerController* CachedPlayerController = nullptr;

    // Cached pawn for interaction range check
    UPROPERTY()
    APawn* CachedPlayerPawn = nullptr;

    // Initialize references
    void InitializeReferences();

    // Check if player is within interaction range
    bool IsPlayerInRange() const;

    // Set interaction state
    void SetDialogueState(EDialogueState NewState);

    // Handle hover state changes
    void HandleHoverStateChanged(bool IsHovering);

    // Play appropriate sound effects
    void PlayHoverSound();
    void PlayClickSound();
};