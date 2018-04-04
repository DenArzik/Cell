// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnBase.h"
#include "PlayerPawn.generated.h"

/**
 * 
 */
UCLASS()
class CELL_API APlayerPawn : public APawnBase
{
	GENERATED_BODY()

	class USpringArmComponent *SpringArm;
	class UCameraComponent *Camera;

	//Names for bindings
	const static FName MoveYBinding;
	const static FName MoveXBinding;
	const static FName ScrollBinding;
	const static FName CameraSecondModeBinding;
	const static FName CameraYBinding;
	const static FName CameraXBinding;
	const static FName CursorClickModeBinding;


	//Not to re-initialize trace many times
	FCollisionQueryParams TraceParams;
	FHitResult Hit;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	APlayerPawn(const FObjectInitializer &ObjectInitializer);

	//For getting mouse coords
	class APlayerController *Controller;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void Scroll(float DeltaTime);
	void CameraMove(float DeltaTime);

/////////////////////////////////////////////
	//Ill try to make smooth scroll
	//This will contain the rest of the previous scroll
	float ScrollRemaining;

	UPROPERTY(EditAnywhere, Category = Camera)
	float SmoothnessCoefficient;

	UPROPERTY(EditAnywhere, Category = Camera)
	float ScrollSpeed;
///////////////////////////////////////////////

	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraMoveSpeed;

	void CameraSecondModePressed();
	void CameraSecondModeReleased();

	bool bCameraSecondMode;
	bool bCameraReturned;
///////////////////////////////////////////////

	void CursorModePressed();
	void CursorModeReleased();

	bool bCursorClickedMode;
	FVector ToCursorDirection();
///////////////////////////////////////////////
//Movement Functions
private:
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void MoveToCursor();
public:
	void MoveTo(const FVector &Point);
	void AddBoost();
	void ReduceBoost();
};