// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnBase.generated.h"

class UStaticMeshComponent;
class USphereComponent;

class UPawnMovement;

enum FoodType
{
	Carnivorous,
	Herbovorous,
	Omnivorous 
};

UCLASS(/*abstract*/)
class CELL_API APawnBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawnBase(const FObjectInitializer &ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*UFUNCTION()
	virtual void OnConstruction(const FTransform& Transform) override;*/

	virtual UPawnMovementComponent *GetMovementComponent() const override;

	UFUNCTION()
	void OnMouthBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPawnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

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

	UPROPERTY(EditAnywhere)
	UPawnMovement *MovementComponent;

	/** Returns CollisionComponent subobject **/
	USphereComponent* GetCollisionComponent() const { return PawnCollision; }
	/** Returns MeshComponent subobject **/
	UStaticMeshComponent* GetMeshComponent() const { return MBody; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn")
	float Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn")
	float Experience;

	FoodType TypeOfFood;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn")
	int Level;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn")
	float Nutriment;

public:
	void Eat(float val);

};
