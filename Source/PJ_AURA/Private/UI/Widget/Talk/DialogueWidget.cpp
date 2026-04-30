// ALL CODE FOR  learning GAS


#include "UI/Widget/Talk/DialogueWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "Talk/DialogueDataManager.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Animation/WidgetAnimation.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UDialogueWidget::UDialogueWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UDialogueWidget::SetNPCActor(AActor* InNPCActor)
{
    NPCActor = InNPCActor;
}

void UDialogueWidget::SetPlayerActor(AActor* InPlayerActor)
{
    PlayerActor = InPlayerActor;
}

void UDialogueWidget::SetDialogueID(FName InDialogueID)
{
    DialogueID = InDialogueID;
}

void UDialogueWidget::StartDialogue()
{
    // Initialize dialogue context
    DialogueContext.DialogueID = DialogueID;
    DialogueContext.NPCActor = NPCActor;
    DialogueContext.PlayerActor = PlayerActor;
    DialogueContext.NodeHistory.Empty();

    // Load dialogue data
    LoadDialogueData();
    UDialogueDataManager* DialogueDataManager = GetDialogueDataManagerPtr();
    // Start from beginning
    if (DialogueDataManager && DialogueDataManager->HasDialogueTree(DialogueID))
    {
    	FDialogueTree DialogueTree = DialogueDataManager->GetDialogueTree(DialogueID);
        if (DialogueTree.DialogueID != NAME_None)
        {
            MoveToNode(DialogueTree.StartNodeID);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DialogueWidget: Failed to find dialogue tree with ID: %s"), *DialogueID.ToString());
    }
}

void UDialogueWidget::EndDialogue()
{
    // 添加诊断日志
    UE_LOG(LogTemp, Warning, TEXT("DialogueWidget::EndDialogue called - Widget Name: %s, IsValid: %s, IsInViewport: %s"),
        *GetName(),
        IsValid(this) ? TEXT("true") : TEXT("false"),
        IsInViewport() ? TEXT("true") : TEXT("false"));
    // Play fade out animation if available
    if (FadeOutAnimation)
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueWidget::EndDialogue - Playing fade out animation"));
        PlayAnimation(FadeOutAnimation);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueWidget::EndDialogue - No fade out animation, proceeding with manual removal"));

        // Clear choice buttons
        ClearChoiceButtons();
        OnEndDialogueDelegate.Broadcast();
        //// 安全地移除Widget - 检查有效性后再移除
        //if (IsValid(this))
        //{
        //    if (IsInViewport())
        //    {
        //        UE_LOG(LogTemp, Warning, TEXT("DialogueWidget::EndDialogue - Widget is valid and in viewport, calling RemoveFromViewport"));
        //        RemoveFromParent();
        //    }
        //    else
        //    {
        //        UE_LOG(LogTemp, Warning, TEXT("DialogueWidget::EndDialogue - Widget is valid but not in viewport, skipping removal"));
        //    }
        //}
        //else
        //{
        //    UE_LOG(LogTemp, Error, TEXT("DialogueWidget::EndDialogue - Widget is not valid, cannot remove"));
        //}
        //// Restore player input
        //APlayerController* PlayerController = Cast<APlayerController>(PlayerActor->GetInstigatorController());
        //if (PlayerController)
        //{
        //    UE_LOG(LogTemp, Warning, TEXT("DialogueWidget::EndDialogue - Restoring player input"));
        //    FInputModeGameAndUI InputMode;
        //    InputMode.SetHideCursorDuringCapture(true);
        //    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        //    PlayerController->SetInputMode(InputMode);
        //    PlayerController->bShowMouseCursor = false;
        //}
        /*else
        {
            UE_LOG(LogTemp, Warning, TEXT("DialogueWidget::EndDialogue - PlayerController is null, cannot restore input"));
        }*/
    }
}

void UDialogueWidget::SelectChoice(int32 ChoiceIndex)
{
    if (ChoiceIndex >= 0 && ChoiceIndex < CurrentNode.Choices.Num())
    {
        const FDialogueChoice& Choice = CurrentNode.Choices[ChoiceIndex];

        // Process choice action if any
        if (!Choice.Action.IsEmpty())
        {
            ProcessNodeAction(Choice.Action);
        }

        // Move to next node
        MoveToNode(Choice.NextNodeID);
    }
}

void UDialogueWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind close button
    if (CloseButton)
    {
        CloseButton->OnClicked.AddDynamic(this, &UDialogueWidget::OnCloseButtonClicked);
    }

    // Start dialogue
    StartDialogue();

    // Play fade in animation if available
    if (FadeInAnimation)
    {
        PlayAnimation(FadeInAnimation);
    }
}

void UDialogueWidget::NativeDestruct()
{
    // Clear choice buttons
    ClearChoiceButtons();

    // Unbind close button
    if (CloseButton)
    {
        CloseButton->OnClicked.RemoveDynamic(this, &UDialogueWidget::OnCloseButtonClicked);
    }

    Super::NativeDestruct();
}

void UDialogueWidget::UpdateDialogueDisplay()
{
    // Update NPC name
    if (NPCNameText)
    {
        NPCNameText->SetText(CurrentNode.SpeakerName);
    }

    // Update dialogue text
    if (DialogueText)
    {
        DialogueText->SetText(CurrentNode.DialogueText);
    }
    UDialogueDataManager* DialogueDataManager = GetDialogueDataManagerPtr();
    // Update NPC portrait
    if (NPCPortraitImage && DialogueDataManager)
    {
        FDialogueTree DialogueTree = DialogueDataManager->GetDialogueTree(DialogueID);
        if (DialogueTree.DialogueID != NAME_None && DialogueTree.NPCPortrait)
        {
            NPCPortraitImage->SetBrushFromTexture(DialogueTree.NPCPortrait);
        }
    }

    // Create choice buttons
    CreateChoiceButtons();
}

void UDialogueWidget::CreateChoiceButtons()
{
    // Clear existing buttons
    ClearChoiceButtons();

    if (!ChoicesVerticalBox || CurrentNode.Choices.Num() == 0 || !ChoiceEntryClass)
    {
        return;
    }

    // Create a button for each choice
    for (int32 i = 0; i < CurrentNode.Choices.Num(); i++)
    {
        const FDialogueChoice& Choice = CurrentNode.Choices[i];

        // Check if choice condition is met
        if (!Choice.Condition.IsEmpty() && !CheckNodeCondition(Choice.Condition))
        {
            continue;
        }

        // Create button
        UDialogueChoiceEntry* ChoiceEntry = NewObject<UDialogueChoiceEntry>(this, ChoiceEntryClass);
        if (ChoiceEntry)
        {
            ChoiceEntry->OnChoiceSelected.AddDynamic(this, &UDialogueWidget::OnChoiceButtonClicked);
            ChoicesVerticalBox->AddChildToVerticalBox(ChoiceEntry);
            // Store button and its index
            ChoiceButtons.Add(ChoiceEntry);

            ChoiceEntry->InitializeChoice(i, Choice.ChoiceText);

        }
    }
}

void UDialogueWidget::ClearChoiceButtons()
{
    if (ChoicesVerticalBox)
    {
        ChoicesVerticalBox->ClearChildren();
    }

    // Clear button array
    ChoiceButtons.Empty();
}

void UDialogueWidget::LoadDialogueData()
{
    if (UDialogueDataManager* DialogueDataManager = GetDialogueDataManagerPtr())
    {
    	DialogueDataManager->LoadDialogueData();
    }
}

void UDialogueWidget::MoveToNode(int32 NodeID)
{
    UDialogueDataManager* DialogueDataManager = GetDialogueDataManagerPtr();
    if (!DialogueDataManager || !DialogueDataManager->HasDialogueTree(DialogueID))
    {
        return;
    }

     FDialogueNode Node = DialogueDataManager->GetDialogueNode(DialogueID, NodeID);
    if (Node.NodeID != 0)
    {
        // Check if node condition is met
        if (!Node.Condition.IsEmpty() && !CheckNodeCondition(Node.Condition))
        {
            UE_LOG(LogTemp, Warning, TEXT("DialogueWidget: Node condition not met for node %d"), NodeID);
            return;
        }

        CurrentNode = Node;
        DialogueContext.CurrentNodeID = NodeID;
        DialogueContext.NodeHistory.Add(NodeID);

        // Process node action if any
        if (!Node.Action.IsEmpty())
        {
            ProcessNodeAction(Node.Action);
        }

        // Update UI
        UpdateDialogueDisplay();

        // Check if this is the end of dialogue
        if (CurrentNode.Choices.Num() == 0)
        {
            UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Reached end of dialogue"));
            // Auto-end dialogue after a short delay
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
                this->EndDialogue();
                }, 2.0f, false);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DialogueWidget: Failed to find node with ID: %d"), NodeID);
    }
}

void UDialogueWidget::ProcessNodeAction(const FString& Action)
{
    // Parse and execute node action
    // This is a placeholder for action processing
    // You can implement specific actions like:
    // - Giving items to player
    // - Starting quests
    // - Changing game state
    // - Playing sounds/animations

    UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Processing action: %s"), *Action);

    // Example action parsing:
    if (Action.StartsWith("GiveItem:"))
    {
        // Parse item ID and give to player
        FString ItemID = Action.Right(Action.Len() - 9);
        // Implement item giving logic
    }
    else if (Action.StartsWith("StartQuest:"))
    {
        // Parse quest ID and start quest
        FString QuestID = Action.Right(Action.Len() - 10);
        // Implement quest starting logic
    }
}

bool UDialogueWidget::CheckNodeCondition(const FString& Condition)
{
    // Parse and check node condition
    // This is a placeholder for condition checking
    // You can implement specific conditions like:
    // - Player level requirements
    // - Item possession requirements
    // - Quest completion requirements
    // - Game state requirements

    UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Checking condition: %s"), *Condition);

    // Example condition parsing:
    if (Condition.StartsWith("PlayerLevel:"))
    {
        // Parse required level and check player level
        FString LevelStr = Condition.Right(Condition.Len() - 12);
        int32 RequiredLevel = FCString::Atoi(*LevelStr);
        // Implement level checking logic
        return true; // Placeholder
    }
    else if (Condition.StartsWith("HasItem:"))
    {
        // Parse item ID and check if player has it
        FString ItemID = Condition.Right(Condition.Len() - 8);
        // Implement item checking logic
        return true; // Placeholder
    }

    return true; // Default to true if condition is empty or unknown
}

UDialogueDataManager* UDialogueWidget::GetDialogueDataManagerPtr() const
{
    AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
    if (AuraGameMode)
    {
        if (UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance()))
        {
            return  AuraGI->GetDialogueDataManager();
        }
    }
    return nullptr;
}

void UDialogueWidget::OnCloseButtonClicked()
{
    EndDialogue();
}

void UDialogueWidget::OnChoiceButtonClicked(int32 Index)
{
	SelectChoice(Index);
}