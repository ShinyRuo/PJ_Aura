// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/NPCInterface.h"
#include "UI/Widget/Talk/DialogueWidget.h"
#include "NPCInteractionComponent.generated.h"

UENUM(BlueprintType)
enum class EDialogueState : uint8
{
    DS_None,
    DS_Hovering,
    DS_InConversation
};

/**
 * NPC interaction component for handling player-NPC interactions
 */
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
    FName DialogueID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Interaction")
    float InteractionRange;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Interaction")
    bool bShowDebugVisuals;

    // State
    UPROPERTY(BlueprintReadOnly, Category = "NPC Interaction")
    EDialogueState CurrentState;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC Interaction")
    TSubclassOf<UDialogueWidget> DialogueWidgetClass;

protected:
    // Helper functions
    bool IsPlayerInRange(AActor* PlayerActor);
    void UpdateDebugVisuals();
    void SetDialogueState(EDialogueState NewState);

    // 当前激活的对话 Widget（如果本组件创建了它）
    UPROPERTY()
    TObjectPtr<UDialogueWidget> ActiveDialogueWidget;
};