// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueData.h"
#include "DialogueDataManager.generated.h"

class UDataTable;
class UAssetManager;

UCLASS(Blueprintable, BlueprintType)
class PJ_AURA_API UDialogueDataManager : public UObject
{
    GENERATED_BODY()

public:
    UDialogueDataManager();

    // Dialogue data loading
    UFUNCTION(BlueprintCallable, Category = "Dialogue Manager")
    void LoadDialogueData();

    UFUNCTION(BlueprintCallable, Category = "Dialogue Manager")
    void LoadDialogueDataFromTable(UDataTable* DialogueDataTable);

    // Dialogue tree access
    UFUNCTION(BlueprintPure, Category = "Dialogue Manager")
    const FDialogueTree* GetDialogueTree(FName DialogueID) const;

    UFUNCTION(BlueprintPure, Category = "Dialogue Manager")
    bool HasDialogueTree(FName DialogueID) const;

    // Dialogue node operations
    UFUNCTION(BlueprintPure, Category = "Dialogue Manager")
    const FDialogueNode* GetDialogueNode(FName DialogueID, int32 NodeID) const;

    UFUNCTION(BlueprintPure, Category = "Dialogue Manager")
    TArray<FDialogueNode> GetStartNodes(FName DialogueID) const;

    UFUNCTION(BlueprintPure, Category = "Dialogue Manager")
    TArray<FDialogueNode> GetChoiceNodes(FName DialogueID, int32 CurrentNodeID) const;

    // Validation
    UFUNCTION(BlueprintCallable, Category = "Dialogue Manager")
    bool ValidateDialogueTree(FName DialogueID) const;

    UFUNCTION(BlueprintCallable, Category = "Dialogue Manager")
    bool ValidateDialogueNode(const FDialogueNode& Node) const;

    // Data management
    UFUNCTION(BlueprintCallable, Category = "Dialogue Manager")
    void AddDialogueTree(const FDialogueTree& DialogueTree);

    UFUNCTION(BlueprintCallable, Category = "Dialogue Manager")
    void RemoveDialogueTree(FName DialogueID);

    UFUNCTION(BlueprintCallable, Category = "Dialogue Manager")
    void ClearAllDialogueData();

    // Configuration
    UPROPERTY(EditAnywhere, Category = "Dialogue Manager")
    UDataTable* DialogueDataTable;

    UPROPERTY(EditAnywhere, Category = "Dialogue Manager")
    bool bDebugMode = false;

    // Events
    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue Manager")
    void OnDialogueDataLoaded();

    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue Manager")
    void OnDialogueDataError(FName DialogueID, const FString& ErrorMessage);

protected:
    // Dialogue trees storage
    UPROPERTY()
    TMap<FName, FDialogueTree> DialogueTrees;

    // Cached asset manager
    UPROPERTY()
    TObjectPtr<UAssetManager> AssetManager;

    // Load dialogue trees from data table
    void LoadDialogueTreesFromTable();

    // Parse dialogue node from row data
    FDialogueNode ParseDialogueNode(const FTableRowBase* RowData) const;

    // Validate dialogue tree structure
    bool ValidateTreeStructure(const FDialogueTree& Tree) const;

    // Validate node connections
    bool ValidateNodeConnections(const FDialogueTree& Tree) const;

    // Helper functions
    void LogDebugMessage(const FString& Message) const;
    void LogErrorMessage(const FString& Message) const;

private:
    // Dialogue node ID counter
    static int32 NextNodeID;
};