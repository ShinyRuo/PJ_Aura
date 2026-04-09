// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MiniMapInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMiniMapInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PJ_AURA_API IMiniMapInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    // --- GetMiniMapIcon ---
   /**
    * 获取小地图图标及相关标志
    * @param OutMiniMapIcon        输出的小地图图标纹理
    * @param OutIsPermanent        是否永久显示
    * @param OutShouldRemoveIcon   是否需要移除图标
    * @param OutIgnoreMapRotation  是否忽略地图旋转
    */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MiniMap")
    void GetMiniMapIcon(
        UTexture2D*& OutMiniMapIcon,
        bool& OutIsPermanent,
        bool& OutShouldRemoveIcon,
        bool& OutIgnoreMapRotation
    );

    // --- GetMiniMapLocationAndRotation ---
    /**
     * 获取小地图中对象的位置和朝向
     * @param OutLocation   输出世界位置
     * @param OutRotation   输出朝向
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MiniMap")
    void GetMiniMapLocationAndRotation(
        FVector& OutLocation,
        FRotator& OutRotation
    );
};
