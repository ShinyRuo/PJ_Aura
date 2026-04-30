// ALL CODE FOR  learning GAS


#include "Talk/DialogueDataManager.h"
#include "Engine/DataTable.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Talk/DialogueData.h"

// Static variable initialization
int32 UDialogueDataManager::NextNodeID = 1;

UDialogueDataManager::UDialogueDataManager()
{
    // Set default values
    bDebugMode = false;
}

void UDialogueDataManager::LoadDialogueData()
{
    if (DialogueDataTable)
    {
        LoadDialogueDataFromTable(DialogueDataTable);
        OnDialogueDataLoaded();
    }
}

void UDialogueDataManager::LoadDialogueDataFromTable(UDataTable* InDialogueDataTable)
{
    if (!InDialogueDataTable)
    {
        LogErrorMessage(TEXT("Dialogue data table is null"));
        return;
    }

    // Clear existing dialogue trees
    DialogueTrees.Empty();

    // Get all rows from data table
    TArray<FName> RowNames = InDialogueDataTable->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        FDialogueTree* DialogueTree = InDialogueDataTable->FindRow<FDialogueTree>(RowName, TEXT(""));
        if (DialogueTree)
        {
            // Validate dialogue tree
            if (ValidateDialogueTree(*DialogueTree))
            {
                DialogueTrees.Add(DialogueTree->DialogueID, *DialogueTree);
                LogDebugMessage(FString::Printf(TEXT("Loaded dialogue tree: %s"), *DialogueTree->DialogueID.ToString()));
            }
            else
            {
                LogErrorMessage(FString::Printf(TEXT("Invalid dialogue tree: %s"), *DialogueTree->DialogueID.ToString()));
                OnDialogueDataError(DialogueTree->DialogueID, TEXT("Invalid dialogue tree structure"));
            }
        }
    }
}

const FDialogueTree& UDialogueDataManager::GetDialogueTree(FName DialogueID) const
{
    const FDialogueTree* DialogueTree = DialogueTrees.Find(DialogueID);
    if (DialogueTree)
    {
        return *DialogueTree;
    }
    // Return a default empty dialogue tree if not found
	static FDialogueTree EmptyDialogueTree;
    return EmptyDialogueTree;
}

bool UDialogueDataManager::HasDialogueTree(FName DialogueID) const
{
    return DialogueTrees.Contains(DialogueID);
}

const FDialogueNode& UDialogueDataManager::GetDialogueNode(FName DialogueID, int32 NodeID) const
{
		const FDialogueTree& DialogueTree = GetDialogueTree(DialogueID);
		for (const FDialogueNode& Node : DialogueTree.Nodes)
        {
            if (Node.NodeID == NodeID)
            {
                return Node;
            }
        }
    // Return a default empty dialogue node if not found
	static FDialogueNode EmptyDialogueNode;
    return EmptyDialogueNode;
}

TArray<FDialogueNode> UDialogueDataManager::GetStartNodes(FName DialogueID) const
{
    TArray<FDialogueNode> StartNodes;
	FDialogueTree DialogueTree = GetDialogueTree(DialogueID);
    FDialogueNode StartNode = GetDialogueNode(DialogueID, DialogueTree.StartNodeID);
	StartNodes.Add(StartNode);
    return StartNodes;
}

TArray<FDialogueNode> UDialogueDataManager::GetChoiceNodes(FName DialogueID, int32 CurrentNodeID) const
{
    TArray<FDialogueNode> ChoiceNodes;
    FDialogueNode CurrentNode = GetDialogueNode(DialogueID, CurrentNodeID);
    for (const FDialogueChoice& Choice : CurrentNode.Choices)
    {
        FDialogueNode NextNode = GetDialogueNode(DialogueID, Choice.NextNodeID);
        if (NextNode.NodeID != 0)
        {
            ChoiceNodes.Add(NextNode);
        }
    }
    return ChoiceNodes;
}

bool UDialogueDataManager::ValidateDialogueTree(const FDialogueTree& Tree) const
{
    return ValidateTreeStructure(Tree) && ValidateNodeConnections(Tree);
}

bool UDialogueDataManager::ValidateDialogueNode(const FDialogueNode& Node) const
{
    // Check for valid node ID
    if (Node.NodeID <= 0)
    {
        return false;
    }

    // Check for non-empty dialogue text
    if (Node.DialogueText.IsEmpty())
    {
        return false;
    }

    // Check for valid choices
    for (const FDialogueChoice& Choice : Node.Choices)
    {
        if (Choice.ChoiceText.IsEmpty())
        {
            return false;
        }
        if (Choice.NextNodeID <= 0)
        {
            return false;
        }
    }

    return true;
}

void UDialogueDataManager::AddDialogueTree(const FDialogueTree& DialogueTree)
{
    if (ValidateDialogueTree(DialogueTree))
    {
        DialogueTrees.Add(DialogueTree.DialogueID, DialogueTree);
        LogDebugMessage(FString::Printf(TEXT("Added dialogue tree: %s"), *DialogueTree.DialogueID.ToString()));
    }
    else
    {
        LogErrorMessage(FString::Printf(TEXT("Failed to add invalid dialogue tree: %s"), *DialogueTree.DialogueID.ToString()));
    }
}

void UDialogueDataManager::RemoveDialogueTree(FName DialogueID)
{
    if (DialogueTrees.Remove(DialogueID) > 0)
    {
        LogDebugMessage(FString::Printf(TEXT("Removed dialogue tree: %s"), *DialogueID.ToString()));
    }
}

void UDialogueDataManager::ClearAllDialogueData()
{
    int32 Count = DialogueTrees.Num();
    DialogueTrees.Empty();
    LogDebugMessage(FString::Printf(TEXT("Cleared all dialogue data (%d trees)"), Count));
}

void UDialogueDataManager::OnDialogueDataLoaded_Implementation()
{
    // Blueprint implementation
}

void UDialogueDataManager::OnDialogueDataError_Implementation(FName DialogueID, const FString& ErrorMessage)
{
    // Blueprint implementation
}

void UDialogueDataManager::LoadDialogueTreesFromTable()
{
    LoadDialogueDataFromTable(DialogueDataTable);
}

FDialogueNode UDialogueDataManager::ParseDialogueNode(const FTableRowBase* RowData) const
{
    FDialogueNode Node;
    if (RowData)
    {
        // Parse node data from row
        // This would depend on actual structure of your data table
    }
    return Node;
}

bool UDialogueDataManager::ValidateTreeStructure(const FDialogueTree& Tree) const
{
    // Check for valid dialogue ID
    if (Tree.DialogueID.IsNone())
    {
        return false;
    }

    // Check for non-empty NPC name
    if (Tree.NPCName.IsEmpty())
    {
        return false;
    }

    // Check for valid start node ID
    if (Tree.StartNodeID <= 0)
    {
        return false;
    }

    // Check for at least one node
    if (Tree.Nodes.Num() == 0)
    {
        return false;
    }

    // Check all nodes
    for (const FDialogueNode& Node : Tree.Nodes)
    {
        if (!ValidateDialogueNode(Node))
        {
            return false;
        }
    }

    return true;
}

bool UDialogueDataManager::ValidateNodeConnections(const FDialogueTree& Tree) const
{
    // Check if start node exists
    bool bStartNodeFound = false;
    for (const FDialogueNode& Node : Tree.Nodes)
    {
        if (Node.NodeID == Tree.StartNodeID)
        {
            bStartNodeFound = true;
            break;
        }
    }
    if (!bStartNodeFound)
    {
        return false;
    }

    // Check if all choice nodes exist
    for (const FDialogueNode& Node : Tree.Nodes)
    {
        for (const FDialogueChoice& Choice : Node.Choices)
        {
            bool bNodeFound = false;
            for (const FDialogueNode& CheckNode : Tree.Nodes)
            {
                if (CheckNode.NodeID == Choice.NextNodeID)
                {
                    bNodeFound = true;
                    break;
                }
            }
            if (!bNodeFound)
            {
                return false;
            }
        }
    }

    return true;
}

void UDialogueDataManager::LogDebugMessage(const FString& Message) const
{
    if (bDebugMode)
    {
        UE_LOG(LogTemp, Log, TEXT("[DialogueManager] %s"), *Message);
    }
}

void UDialogueDataManager::LogErrorMessage(const FString& Message) const
{
    UE_LOG(LogTemp, Error, TEXT("[DialogueManager] %s"), *Message);
}