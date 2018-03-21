// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "TestFloatingPawnMovement.generated.h"

/**
 * 
 */
UCLASS()
class CELL_API UTestFloatingPawnMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()


	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
};
