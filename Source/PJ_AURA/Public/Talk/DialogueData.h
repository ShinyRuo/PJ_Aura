// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "DialogueData.generated.h"

class UAnimMontage;
class USoundBase;
class UTexture2D;

/**
 * Dialogue choice structure
 */
USTRUCT(BlueprintType)
struct FDialogueChoice : public FTableRowBase
{
    GENERATED_BODY()

    /** Text displayed for the choice */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText ChoiceText;

    /** ID of the node this choice leads to */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    int32 NextNodeID;

    /** Optional condition for this choice to be available */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FString Condition;

    /** Optional action to execute when this choice is selected */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FString Action;
};

/**
 * Dialogue node structure
 */
USTRUCT(BlueprintType)
struct FDialogueNode : public FTableRowBase
{
    GENERATED_BODY()

    /** Unique node ID */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    int32 NodeID;

    /** Speaker name */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText SpeakerName;

    /** Dialogue text */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (MultiLine = true))
    FText DialogueText;

    /** Choices available to player */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FDialogueChoice> Choices;

    /** Optional animation to play during this dialogue */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    UAnimMontage* Animation;

    /** Optional sound to play during this dialogue */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    USoundBase* VoiceOver;

    /** Optional condition for this node to be available */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FString Condition;

    /** Optional action to execute when this node is displayed */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FString Action;
};

/**
 * Dialogue tree structure
 */
USTRUCT(BlueprintType)
struct FDialogueTree : public FTableRowBase
{
    GENERATED_BODY()

    /** Unique dialogue tree ID */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName DialogueID;

    /** NPC name */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText NPCName;

    /** NPC portrait */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    UTexture2D* NPCPortrait;

    /** All nodes in this dialogue tree */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FDialogueNode> Nodes;

    /** ID of the starting node */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    int32 StartNodeID;

    /** Optional background music */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    USoundBase* BackgroundMusic;

    /** Optional ambient sound */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    USoundBase* AmbientSound;
};

/**
 * Dialogue context structure
 */
USTRUCT(BlueprintType)
struct FMyDialogueContext : public FTableRowBase
{
    GENERATED_BODY()

    /** Current dialogue tree ID */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName DialogueID;

    /** Current node ID */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    int32 CurrentNodeID;

    /** NPC actor */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    AActor* NPCActor;

    /** Player actor */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    AActor* PlayerActor;

    /** Dialogue history */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<int32> NodeHistory;
};