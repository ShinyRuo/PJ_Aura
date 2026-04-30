// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueData.h"
#include "DialogueContextManager.generated.h"

class UDialogueDataManager;
class APlayerController;

UCLASS(Blueprintable, BlueprintType)
class PJ_AURA_API UDialogueContextManager : public UObject
{
    GENERATED_BODY()

public:
    UDialogueContextManager();

    // Dialogue lifecycle
    UFUNCTION(BlueprintCallable, Category = "Dialogue Context")
    void StartDialogue(FName DialogueID, APlayerController* PlayerController);

    UFUNCTION(BlueprintCallable, Category = "Dialogue Context")
    void EndDialogue();

    UFUNCTION(BlueprintCallable, Category = "Dialogue Context")
    void SelectChoice(int32 ChoiceIndex);

    UFUNCTION(BlueprintCallable, Category = "Dialogue Context")
    void ContinueDialogue();

    UFUNCTION(BlueprintCallable, Category = "Dialogue Context")
    void SkipCurrentNode();

    // State queries
    UFUNCTION(BlueprintPure, Category = "Dialogue Context")
    bool IsInDialogue() const;

    UFUNCTION(BlueprintPure, Category = "Dialogue Context")
    FName GetCurrentDialogueID() const;

    UFUNCTION(BlueprintPure, Category = "Dialogue Context")
    int32 GetCurrentNodeID() const;

    UFUNCTION(BlueprintPure, Category = "Dialogue Context")
    const FDialogueNode* GetCurrentNode() const;

    UFUNCTION(BlueprintPure, Category = "Dialogue Context")
    TArray<FDialogueNode> GetCurrentChoices() const;

    UFUNCTION(BlueprintPure, Category = "Dialogue Context")
    FDialogueState GetDialogueState() const;

    // Choice management
    UFUNCTION(BlueprintPure, Category = "Dialogue Context")
    FText GetChoiceText(int32 ChoiceIndex) const;

    UFUNCTION(BlueprintPure, Category = "Dialogue Context")
    bool CanSelectChoice(int32 ChoiceIndex) const;

    // Node navigation
    UFUNCTION(BlueprintPure, Category = "Dialogue Context")
    const FDialogueNode* GetNextNode(int32 ChoiceIndex) const;

    UFUNCTION(BlueprintPure, Category = "Dialogue Context")
    const FDialogueNode* GetNextNodeFromTag(FGameplayTag TargetTag) const;

    // Progress tracking
    UFUNCTION(BlueprintPure, Category = "Dialogue Context")
    int32 GetDialogueProgress() const;

    UFUNCTION(BlueprintPure, Category = "Dialogue Context")
    float GetDialogueDuration() const;

    // Events
    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue Context")
    void OnDialogueStarted(FName DialogueID);

    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue Context")
    void OnDialogueEnded();

    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue Context")
    void OnNodeChanged(int32 PreviousNodeID, int32 CurrentNodeID);

    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue Context")
    void OnChoiceSelected(int32 ChoiceIndex);

    // Configuration
    UPROPERTY(EditAnywhere, Category = "Dialogue Context")
    UDialogueDataManager* DialogueDataManager;

    UPROPERTY(EditAnywhere, Category = "Dialogue Context")
    bool bDebugMode = false;

    UPROPERTY(EditAnywhere, Category = "Dialogue Context")
    float AutoProceedDelay = 2.0f;

    // Input handling
    UFUNCTION(BlueprintCallable, Category = "Dialogue Context")
    void HandleInput(EInputEvent InputEvent, int32 ControllerId);

protected:
    // Current dialogue state
    UPROPERTY()
    FDialogueState CurrentDialogueState;

    // Current dialogue tree
    UPROPERTY()
    const FDialogueTree* CurrentDialogueTree;

    // Timer for auto proceed
    FTimerHandle AutoProceedTimer;

    // Player controller reference
    UPROPERTY()
    APlayerController* PlayerController;

    // Dialogue start time
    FDateTime DialogueStartTime;

    // Internal functions
    void InitializeDialogue(FName DialogueID);
    void FinalizeDialogue();
    void HandleNodeComplete();
    void ScheduleAutoProceed();

    void CancelAutoProceed();
    bool ShouldAutoProceed(const FDialogueNode& Node) const;

    // Event triggers
    void TriggerOnDialogueStarted(FName DialogueID);
    void TriggerOnDialogueEnded();
    void TriggerOnNodeChanged(int32 PreviousNodeID, int32 CurrentNodeID);
    void TriggerOnChoiceSelected(int32 ChoiceIndex);

    // Logging
    void LogDebugMessage(const FString& Message) const;
    void LogErrorMessage(const FString& Message) const;

private:
    // Current node index in dialogue tree
    int32 CurrentNodeIndex = -1;

    // Choice validation
    bool ValidateChoice(int32 ChoiceIndex) const;

    // Check if choice is available
    bool IsChoiceAvailable(int32 ChoiceIndex, const FDialogueNode& CurrentNode) const;
};