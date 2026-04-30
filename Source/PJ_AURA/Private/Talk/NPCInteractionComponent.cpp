// ALL CODE FOR  learning GAS


#include "Talk/NPCInteractionComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

UNPCInteractionComponent::UNPCInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    // Set default values
    DialogueID = FName("DefaultNPC");
    InteractionRange = 200.0f;
    bShowDebugVisuals = false;
    CurrentState = EDialogueState::DS_None;
}

void UNPCInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UNPCInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Check for hover if not in conversation
    if (!IsInConversation_Implementation())
    {
        CheckForHover();
    }

    // Update debug visuals
    if (bShowDebugVisuals)
    {
        UpdateDebugVisuals();
    }
}

FName UNPCInteractionComponent::GetDialogueID_Implementation() const
{
    return DialogueID;
}

bool UNPCInteractionComponent::IsInConversation_Implementation() const
{
    return CurrentState == EDialogueState::DS_InConversation;
}

void UNPCInteractionComponent::StartConversation_Implementation(AActor* PlayerActor)
{
    // Check if already in conversation
    if (IsInConversation_Implementation())
    {
        return;
    }

    // Check if player is within interaction range
    if (!IsPlayerInRange(PlayerActor))
    {
        return;
    }

    // Create and show dialogue widget
    APlayerController* PlayerController = nullptr;
    if (APawn* Pawn = Cast<APawn>(PlayerActor))
    {
        PlayerController = Cast<APlayerController>(Pawn->GetController());
    }

    if (!PlayerController)
    {
        // 备用：Get first local player controller（单机）
        PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    }
	if (PlayerController && DialogueWidgetClass)
    {
        // 如果已经有一个对话 Widget，先清理（防止重复）
        if (IsValid(ActiveDialogueWidget))
        {
            ActiveDialogueWidget->RemoveFromParent();
            ActiveDialogueWidget = nullptr;
        }
        ActiveDialogueWidget = CreateWidget<UDialogueWidget>(PlayerController, DialogueWidgetClass);
        if (ActiveDialogueWidget)
        {
            ActiveDialogueWidget->SetNPCActor(GetOwner());
            ActiveDialogueWidget->SetPlayerActor(PlayerActor);
            ActiveDialogueWidget->SetDialogueID(DialogueID);
            ActiveDialogueWidget->AddToViewport();

            ActiveDialogueWidget->OnEndDialogueDelegate.AddUObject(this, &UNPCInteractionComponent::EndConversation_Implementation);

            // Lock player input
            PlayerController->SetInputMode(FInputModeUIOnly());
            PlayerController->bShowMouseCursor = true;

            // Update state
            SetDialogueState(EDialogueState::DS_InConversation);
        }
    }
}

void UNPCInteractionComponent::EndConversation_Implementation()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        // 直接移除由本组件创建的 Widget
        if (IsValid(ActiveDialogueWidget))
        {
            ActiveDialogueWidget->RemoveFromParent();
            ActiveDialogueWidget = nullptr;

            // 恢复玩家输入
            FInputModeGameAndUI InputMode;
            InputMode.SetHideCursorDuringCapture(true);
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PlayerController->SetInputMode(InputMode);
            PlayerController->bShowMouseCursor = false;
        }
    }

    // Update state
    SetDialogueState(EDialogueState::DS_None);
}

float UNPCInteractionComponent::GetInteractionRange_Implementation() const
{
    return InteractionRange;
}

void UNPCInteractionComponent::CheckForHover()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController || !GetOwner())
    {
        return;
    }

    // Check if player is within interaction range
    APawn* PlayerPawn = PlayerController->GetPawn();
    if (!PlayerPawn)
    {
        return;
    }

    if (!IsPlayerInRange(PlayerPawn))
    {
        SetDialogueState(EDialogueState::DS_None);
        return;
    }

    // Check if mouse is over the NPC
    FHitResult HitResult;
    PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
    if (HitResult.bBlockingHit && HitResult.GetActor() == GetOwner())
    {
        // Change mouse cursor to dialogue icon
        PlayerController->CurrentMouseCursor = EMouseCursor::Hand;
        SetDialogueState(EDialogueState::DS_Hovering);
    }
    else
    {
        // Reset mouse cursor
        PlayerController->CurrentMouseCursor = EMouseCursor::Default;
        SetDialogueState(EDialogueState::DS_None);
    }
}

void UNPCInteractionComponent::OnNPCClicked()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController || !GetOwner())
    {
        return;
    }

    // Check if player is within interaction range
    APawn* PlayerPawn = PlayerController->GetPawn();
    if (!PlayerPawn)
    {
        return;
    }

    if (!IsPlayerInRange(PlayerPawn))
    {
        return;
    }

    // Check if mouse is over the NPC
    FHitResult HitResult;
    PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
    if (HitResult.bBlockingHit && HitResult.GetActor() == GetOwner())
    {
        // Start conversation
        StartConversation_Implementation(PlayerPawn);
    }
}

bool UNPCInteractionComponent::IsPlayerInRange(AActor* PlayerActor)
{
    if (!PlayerActor || !GetOwner())
    {
        return false;
    }

    float Distance = FVector::Distance(PlayerActor->GetActorLocation(), GetOwner()->GetActorLocation());
    return Distance <= InteractionRange;
}

void UNPCInteractionComponent::UpdateDebugVisuals()
{
    if (!GetOwner())
    {
        return;
    }

    // Draw interaction range
    FColor RangeColor = FColor::Green;
    if (CurrentState == EDialogueState::DS_Hovering)
    {
        RangeColor = FColor::Yellow;
    }
    else if (CurrentState == EDialogueState::DS_InConversation)
    {
        RangeColor = FColor::Red;
    }

    DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), InteractionRange, 32, RangeColor, false, 0.0f, 0, 2.0f);
}

void UNPCInteractionComponent::SetDialogueState(EDialogueState NewState)
{
    if (CurrentState != NewState)
    {
        CurrentState = NewState;

        // Log state change
        FString StateName;
        switch (NewState)
        {
        case EDialogueState::DS_None:
            StateName = TEXT("None");
            break;
        case EDialogueState::DS_Hovering:
            StateName = TEXT("Hovering");
            break;
        case EDialogueState::DS_InConversation:
            StateName = TEXT("InConversation");
            break;
        }

        UE_LOG(LogTemp, Log, TEXT("NPC %s state changed to: %s"), *GetOwner()->GetName(), *StateName);
    }
}