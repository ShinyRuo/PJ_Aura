// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NPCInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UNPCInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for NPCs that can be interacted with
 */
class PJ_AURA_API INPCInterface
{
    GENERATED_BODY()

public:
    /** Get the dialogue ID for this NPC */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NPC Interaction")
    FName GetDialogueID() const;

    /** Check if NPC is currently in conversation */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NPC Interaction")
    bool IsInConversation() const;

    /** Start conversation with player */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NPC Interaction")
    void StartConversation(AActor* PlayerActor);

    /** End conversation */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NPC Interaction")
    void EndConversation();

    /** Get maximum interaction range */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NPC Interaction")
    float GetInteractionRange() const;
};