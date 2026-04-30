// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "DialogueChoiceEntry.h"
#include "Blueprint/UserWidget.h"
#include "Talk/DialogueData.h"
#include "DialogueWidget.generated.h"

class UTextBlock;
class UButton;
class UVerticalBox;
class UImage;
class UDialogueDataManager;
class UWidgetSwitcher;

DECLARE_MULTICAST_DELEGATE(FOnEndDialogue);


/**
 * Dialogue widget for NPC conversations
 */
UCLASS()
class PJ_AURA_API UDialogueWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UDialogueWidget(const FObjectInitializer& ObjectInitializer);

    // Setup
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void SetNPCActor(AActor* InNPCActor);

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void SetPlayerActor(AActor* InPlayerActor);

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void SetDialogueID(FName InDialogueID);

    // Dialogue flow
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void StartDialogue();

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void EndDialogue();

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void SelectChoice(int32 ChoiceIndex);

    // Getters
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    FName GetDialogueID() const { return DialogueID; }

    UFUNCTION(BlueprintPure, Category = "Dialogue")
    AActor* GetNPCActor() const { return NPCActor; }
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    AActor* GetPlayerActor() const { return PlayerActor; }

	FOnEndDialogue OnEndDialogueDelegate;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // Update UI
    void UpdateDialogueDisplay();
    void CreateChoiceButtons();
    void ClearChoiceButtons();

    

    // Dialogue management
    void LoadDialogueData();
    void MoveToNode(int32 NodeID);
    void ProcessNodeAction(const FString& Action);
    bool CheckNodeCondition(const FString& Condition);

    UDialogueDataManager* GetDialogueDataManagerPtr() const;

    // Widget components
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* NPCNameText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* DialogueText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UVerticalBox* ChoicesVerticalBox;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* CloseButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UImage* NPCPortraitImage;

    // Data
    UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
    AActor* NPCActor;

    UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
    AActor* PlayerActor;

    UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
    FName DialogueID;

    UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
    FMyDialogueContext DialogueContext;

    // Current state
    UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
    FDialogueNode CurrentNode;

    // Choice buttons
    UPROPERTY()
    TArray<UDialogueChoiceEntry*> ChoiceButtons;

    UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
    TSubclassOf<UDialogueChoiceEntry> ChoiceEntryClass;

    // Animation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    UWidgetAnimation* FadeInAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    UWidgetAnimation* FadeOutAnimation;

private:
    // Button click handlers
    UFUNCTION()
    void OnCloseButtonClicked();

    UFUNCTION()
    void OnChoiceButtonClicked(int32 Index);
};