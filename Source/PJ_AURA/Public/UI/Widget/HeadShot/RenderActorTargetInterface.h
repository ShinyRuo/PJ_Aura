// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RenderActorTargetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URenderActorTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PJ_AURA_API IRenderActorTargetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FVector GetCaptureLocalPosition() const = 0;
	virtual FRotator GetCaptureLocalRotation() const = 0;
};
