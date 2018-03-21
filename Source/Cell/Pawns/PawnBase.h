// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/"
#include "PawnBase.generated.h"

class UStaticMeshComponent;
class USphereComponent;

class UPawnBaseMovementComponent;

enum FoodType
{
	Carnivorous,
	Herbovorous,
	Omnivorous 
};

UCLASS(/*abstract*/)
class CELL_API APawnBase : public ABasePawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawnBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*UFUNCTION()
	virtual void OnConstruction(const FTransform& Transform) override;*/

	virtual UPawnMovementComponent *GetMovementComponent() const override;

	UFUNCTION()
	void OnMouthBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent *MBody;

	UPROPERTY(EditAnywhere)
	USphereComponent *MouthCollision;

	UPROPERTY(EditAnywhere)
	USphereComponent *PawnCollision;

	UPawnMovementComponent *MovementComponent;
	
	UPROPERTY(EditAnywhere)
	float MaxVelocity;

protected:
	float Health;
	float Experience;
	FoodType TypeOfFood;
	int8 Level;

	FVector CurrentVelocity;

	float Nutriment;

public:
	void Eat(float val);

};
