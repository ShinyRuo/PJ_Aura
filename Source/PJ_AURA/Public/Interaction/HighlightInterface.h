#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HighlightInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHighlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PJ_AURA_API IHighlightInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** 当Actor需要被高亮时调用 */
	virtual void HighlightActor() = 0;

	/** 当Actor取消高亮时调用 */
	virtual void UnHighlightActor() = 0;
};