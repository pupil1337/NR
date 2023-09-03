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
	 * @brief 异步加载资源, 如果传入StreamableHandle持有引用则释放
	 * @param OutStreamableHandle StreamableHandle引用
	 * @param TargetsToStream 需要加载的资源路径列表
	 * @param DelegateToCall 加载完成回调(TargetsToStream为Empty则不会调用)
	 * @param Priority 加载优先级 Default = 0, Height(立即加载) = 100
	 * @param bManageActiveHandle 是否由FStreamManager持有StreamableHandle
	 * @param bStartStalled 是否停滞状态启动Handle, 如果true 则需要手动调用StartStalledHandle()
	 * @param DebugName DebugName
	 */
	static void RequestAsyncLoad(OUT TSharedPtr<FStreamableHandle>& OutStreamableHandle, const TArray<FSoftObjectPath>& TargetsToStream, const FStreamableDelegate& DelegateToCall = FStreamableDelegate(), TAsyncLoadPriority Priority = 0, bool bManageActiveHandle = false, bool bStartStalled = false, const FString& DebugName = TEXT("ArrayDelegate"));
	
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
	 * 模拟圆锥扫描，返回圆锥内的Actor(近似值)
	 * @param World World
	 * @param OutHits 返回的HitResults
	 * @param Start 圆锥顶点
	 * @param End 圆锥底面中心
	 * @param Angle 圆锥角度 (底面半径/高=Tan(Angle°))
	 * @param ProfileName 碰撞预设
	 * @param Params 碰撞参数
	 * @param FindActorNum 最多查找到Actor数量 (<=0为不限制)
	 * @param bDebug bDebug
	 * @param DebugLifeTime DebugLifeTime
	 */
	static void ConeTraceMultiByProfile(const UWorld* World, OUT TArray<FHitResult>& OutHits, const FVector& Start, const FVector& End, float Angle, FName ProfileName, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, int32 FindActorNum = -1, bool bDebug = false, float DebugLifeTime = 3.0f);

	/**
	 * @brief 根据武器型号获取武器信息
	 * @param WeaponType 武器型号
	 * @return 武器信息表行
	 */
	static FNRWeaponInformationRow* GetWeaponInformationRow(ENRWeaponType WeaponType);
};
