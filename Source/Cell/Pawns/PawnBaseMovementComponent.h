// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PawnBaseMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class CELL_API UPawnBaseMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
public:
	UPawnBaseMovementComponent(const FObjectInitializer &ObjectInitializer);
	
};
