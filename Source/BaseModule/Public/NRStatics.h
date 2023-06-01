// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRStatics.generated.h"

class APlayerController;

/**
 * 
 */
UCLASS()
class BASEMODULE_API UNRStatics : public UObject
{
	GENERATED_BODY()

public:
	static void AddSoftObjectPathToArray(const TSoftObjectPtr<UObject>& SoftObjectPtr, TArray<FSoftObjectPath>& PathArray);

	/**
	 * @brief 仅客户端函数, 从屏幕中心做射线检测
	 * @param PlayerController 玩家控制器
	 * @param TraceDistance 检测距离
	 * @param HitResult 射线检测结果
	 * @return 是否完成射线检测
	 */
	static bool CrosshairTrace(const APlayerController* PlayerController, float TraceDistance, FHitResult& HitResult);
};
