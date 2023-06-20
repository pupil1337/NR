// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Types/NRWeaponTypes.h"
#include "Table/Weapon/NRWeaponInformation.h"
#include "NRStatics.generated.h"

struct FStreamableHandle;
class APlayerController;

/**
 * 
 */
UCLASS()
class NR_API UNRStatics : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * @brief 增加某个资源的路径到数组中
	 * @param SoftObjectPtr 需要加载的资源软引用
	 * @param OutTargetsToStream out路径数组
	 */
	static void AddSoftObjectPathToArray(const TSoftObjectPtr<UObject>& SoftObjectPtr, TArray<FSoftObjectPath>& OutTargetsToStream);

	/**
	 * @brief 异步加载资源
	 * @param OutStreamableHandle StreamableHandle引用
	 * @param TargetsToStream 需要加载的资源路径列表
	 * @param DelegateToCall 加载完成回调(TargetsToStream为Empty则不会调用)
	 */
	static void RequestAsyncLoad(TSharedPtr<FStreamableHandle>& OutStreamableHandle, const TArray<FSoftObjectPath>& TargetsToStream, const FStreamableDelegate& DelegateToCall = FStreamableDelegate());

	/**
	 * @brief 设置骨骼网格体是否开启第一人称FOV+穿模修复
	 * @param Mesh 需要设置的Mesh
	 * @param bEnable 是否开启第一人称修复 (开启则默认需要FOV修复)
	 * @param bSeparate 是否开启穿模修复
	 */
	static void SetFPS_SeparateFOV(UMeshComponent* Mesh, bool bEnable, bool bSeparate = false);
	
	/**
	 * @brief 仅客户端函数, 从屏幕中心做射线检测
	 * @param PlayerController 玩家控制器
	 * @param TraceDistance 检测距离
	 * @param HitResult 射线检测结果
	 * @return 是否完成射线检测
	 */
	static bool CrosshairTrace(const APlayerController* PlayerController, float TraceDistance, FHitResult& HitResult);

	/**
	 * @brief 根据武器型号获取武器信息
	 * @param WeaponType 武器型号
	 * @return 武器信息表行
	 */
	static FNRWeaponInformationRow* GetWeaponInformationRow(ENRWeaponType WeaponType);
};
