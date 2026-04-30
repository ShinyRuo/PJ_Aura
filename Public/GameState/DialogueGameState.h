// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "DialogueGameState.generated.h"

class UDialogueDataManager;
class UDialogueContextManager;
class UWidget;

UCLASS()
class PJ_AURA_API ADialogueGameState : public AGameState
{
    GENERATED_BODY()

public:
    ADialogueGameState();

    // GameState interface
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

    // Dialogue system management
    UFUNCTION(BlueprintCallable, Category = "Dialogue Game State")
    void InitializeDialogueSystem();

    UFUNCTION(BlueprintCallable, Category = "Dialogue Game State")
    UDialogueDataManager* GetDialogueDataManager() const;

    UFUNCTION(BlueprintCallable, Category = "Dialogue Game State")
    UDialogueContextManager* GetDialogueContextManager() const;

    // Dialogue UI management
    UFUNCTION(BlueprintCallable, Category = "Dialogue Game State")
    void ShowDialogueBox();

    UFUNCTION(BlueprintCallable, Category = "Dialogue Game State")
    void HideDialogueBox();

    UFUNCTION(BlueprintCallable, Category = "Dialogue Game State")
    bool IsDialogueBoxVisible() const;

    // Dialogue state queries
    UFUNCTION(BlueprintPure, Category = "Dialogue Game State")
    bool IsAnyDialogueActive() const;

    UFUNCTION(BlueprintPure, Category = "Dialogue Game State")
    TArray<FName> GetActiveDialogueIDs() const;

    // Input handling
    UFUNCTION(BlueprintCallable, Category = "Dialogue Game State")
    void HandleDialogueInput(EInputEvent InputEvent, int32 ControllerId);

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Game State")
    UDialogueDataManager* DialogueDataManager;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Game State")
    TSubclassOf<UUserWidget> DialogueBoxWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Game State")
    bool bEnableDialogueSystem = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue Game State")
    bool bDebugMode = false;

protected:
    // Dialogue system components
    UPROPERTY()
    UDialogueDataManager* InternalDialogueDataManager;

    UPROPERTY()
    UDialogueContextManager* InternalDialogueContextManager;

    // UI components
    UPROPERTY()
    UUserWidget* DialogueBoxWidget;

    // Active dialogues tracking
    UPROPERTY()
    TArray<FName> ActiveDialogueIDs;

    // Helper functions
    void CreateDialogueComponents();
    void SetupDialogueSystem();
    void SetupInputBindings();

    void CleanupDialogueSystem();
    void CleanupInputBindings();

    // Event handlers
    void OnDialogueStarted_Native(FName DialogueID);
    void OnDialogueEnded_Native();
    void OnNodeChanged_Native(int32 PreviousNodeID, int32 CurrentNodeID);
    void OnChoiceSelected_Native(int32 ChoiceIndex);

    // UI management
    void CreateDialogueBox();
    void UpdateDialogueBoxUI();
    void HandleDialogueBoxClosed();

    // Logging
    void LogDebugMessage(const FString& Message) const;
    void LogErrorMessage(const FString& Message) const;

private:
    // Input action mappings
    UPROPERTY()
    UInputAction* ConfirmAction;

    UPROPERTY()
    UInputAction* ContinueAction;

    UPROPERTY()
    UInputAction* CancelAction;

    // Dialogue state tracking
    bool bHasActiveDialogue = false;
};