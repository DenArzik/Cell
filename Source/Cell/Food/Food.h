// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Food.generated.h"

UCLASS(abstract)
class CELL_API AFood : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float Size;

	UPROPERTY(EditAnywhere)
	float Value;

	//void Overlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Sets default values for this actor's properties
	AFood();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent *Mesh;

	float GetSize();

	float GetValue();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
