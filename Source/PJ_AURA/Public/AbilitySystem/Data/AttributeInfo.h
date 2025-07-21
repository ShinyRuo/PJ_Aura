// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AttributeInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraAttributeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription = FText();

	UPROPERTY( BlueprintReadOnly)
	float AttributeValue = 0.f;
};

/**
 * ���ڽ���tag��attribute����ϵ �����ʲ���
 */
UCLASS()
class PJ_AURA_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	FAuraAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound=false);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraAttributeInfo> AttributeInformation;
};
