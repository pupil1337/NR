// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RunComponent.generated.h"


UCLASS()
class NR_API URunComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	float MaxRunSpeed = 600.0f;
	
public:
	URunComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

//~Begin This Class
	// 仅从LocallyControlled调用
	void Run(bool NewRun);

private:
	UPROPERTY(Transient, ReplicatedUsing=OnRep_Run)
	bool bRunning;

	UFUNCTION(Server, Reliable)
	void Server_Run(bool NewRun);
	
	UFUNCTION()
	void OnRep_Run(bool OldRunning);
};
