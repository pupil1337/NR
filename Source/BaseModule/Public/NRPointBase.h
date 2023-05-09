// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/NavigationObjectBase.h"
#include "NRPointBase.generated.h"

class UArrowComponent;

UCLASS(Abstract)
class BASEMODULE_API ANRPointBase : public ANavigationObjectBase
{
	GENERATED_BODY()
	
public:
	ANRPointBase();
	
#if WITH_EDITORONLY_DATA
private:
	TObjectPtr<UArrowComponent> ArrowComponent;
#endif 
};
