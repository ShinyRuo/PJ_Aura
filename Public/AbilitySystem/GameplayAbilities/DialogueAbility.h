// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "GameplayAbility.h"
#include "GameplayTagAssetInterface.h"
#include "DialogueAbility.generated.h"

class UDialogueContextManager;
class UNPCInteractionComponent;

UCLASS()
class PJ_AURA_API UDialogueAbility : public UGameplayAbility, public IGameplayTagAssetInterface
{
    GENERATED_BODY()

public:
    UDialogueAbility();

    // Ability interface
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    // IGameplayTagAssetInterface interface
    virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

    // Dialogue integration
    UFUNCTION(BlueprintCallable, Category = "Dialogue Ability")
    void StartDialogueWithNPC(UNPCInteractionComponent* NPCComponent);

    UFUNCTION(BlueprintCallable, Category = "Dialogue Ability")
    void EndCurrentDialogue();

    // State queries
    UFUNCTION(BlueprintPure, Category = "Dialogue Ability")
    bool IsInDialogue() const;

    UFUNCTION(BlueprintPure, Category = "Dialogue Ability")
    FName GetCurrentDialogueID() const;

    UFUNCTION(BlueprintPure, Category = "Dialogue Ability")
    int32 GetCurrentNodeID() const;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Ability")
    FName DialogueID = FName("DefaultDialogue");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Ability")
    FGameplayTag StartedDialogueTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Ability")
    FGameplayTag InDialogueTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Ability")
    FGameplayTag FinishedDialogueTag;

    // Input handling
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Ability")
    FGameplayTag InputChoiceConfirmTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Ability")
    FGameplayTag InputContinueTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Ability")
    FGameplayTag InputCancelTag;

    // Cooldown
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue Ability")
    FGameplayAbilityCooldown CooldownGameplayEffect;

    // Event callbacks
    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue Ability")
    void OnDialogueStarted(FName DialogueID);

    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue Ability")
    void OnDialogueEnded();

    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue Ability")
    void OnNodeChanged(int32 PreviousNodeID, int32 CurrentNodeID);

    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue Ability")
    void OnChoiceSelected(int32 ChoiceIndex);

protected:
    // Dialogue manager reference
    UPROPERTY()
    UDialogueContextManager* DialogueContextManager;

    // Current NPC component being interacted with
    UPROPERTY()
    UNPCInteractionComponent* CurrentNPCComponent;

    // Helper functions
    void HandleInputEvent(const FGameplayTag& InputTag);
    void ApplyCooldown();
    void RemoveCooldown();

    // Tag management
    void ApplyDialogueTags();
    void RemoveDialogueTags();

    // Input binding
    void BindInputActions();
    void UnbindInputActions();

    // Debug
    UPROPERTY(EditAnywhere, Category = "Dialogue Ability")
    bool bDebugMode = false;

private:
    bool bIsInDialogue = false;
};