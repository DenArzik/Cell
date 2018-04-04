// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PawnMovement.generated.h"

/**
 * 
 */
UCLASS()
class CELL_API UPawnMovement : public UPawnMovementComponent
{
	GENERATED_BODY()

	static const float s_MinTurnSpeed;
	static const float s_MaxTurnBoost;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void BeginPlay() override;
public:
	UPawnMovement(const FObjectInitializer &ObjectInitializer);
protected:
	virtual bool ResolvePenetrationImpl(const FVector& Adjustment, const FHitResult& Hit, const FQuat& NewRotation) override;

	/** Update Velocity based on input. Also applies gravity. */
	virtual void ApplyControlInputToVelocity(float DeltaTime);

public:
	/** Maximum velocity magnitude allowed for the controlled Pawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PawnMovement)
	float MaxSpeed;

	/** Acceleration applied by input (rate of change of velocity) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PawnMovement)
	float Acceleration;

	/** Deceleration applied when there is no input (rate of change of velocity) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PawnMovement)
	float Deceleration;

	/** Set to true when a position correction is applied. Used to avoid recalculating velocity when this occurs. */
	UPROPERTY(Transient)
	uint8 bPositionCorrected : 1;


private:
	/**
	* Setting affecting extra force applied when changing direction, making turns have less drift and become more responsive.
	* Velocity magnitude is not allowed to increase, that only happens due to normal acceleration. It may decrease with large direction changes.
	* Larger values apply extra force to reach the target direction more quickly, while a zero value disables any extra turn force.
	*/
	UPROPERTY(VisibleAnywhere, Category = PawnMovement)
	float TurnBoost;

	UPROPERTY(VisibleAnywhere,  Category = PawnMovement)
	float TurnSpeed;

public:
	float GetTurnSpeed() const;
	void SetTurnSpeed(float speed);

	float GetTurnBoost() const;
	void SetTurnBoost(float boost);


private:
	FRotator CurrentRotation;
	void CalculateRotation(const FRotator &RotateTo, float DeltaTime);
};
