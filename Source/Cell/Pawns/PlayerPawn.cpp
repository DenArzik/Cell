// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"
#include <Camera/CameraComponent.h>
#include <Components/InputComponent.h>
#include <GameFramework/SpringArmComponent.h>

#include "Pawns/PawnMovement.h"

#include <Engine.h>

#define MY_CHECK GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Cyan, "Check")

const FName APlayerPawn::MoveYBinding("MoveY");
const FName APlayerPawn::MoveXBinding("MoveX");
const FName APlayerPawn::ScrollBinding("Scroll");
const FName APlayerPawn::CameraSecondModeBinding("CameraSecondMode");
const FName APlayerPawn::CameraYBinding("CameraY");
const FName APlayerPawn::CameraXBinding("CameraX");
const FName APlayerPawn::CursorClickModeBinding("CursorClickMode");

APlayerPawn::APlayerPawn(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, ScrollRemaining(0.0f)
	, SmoothnessCoefficient(50.0f)
	, ScrollSpeed(6000.0f)
	, bCameraSecondMode(false)
	, bCameraReturned(true)
	, CameraMoveSpeed(2000.0f)
	, bCursorClickedMode(false)
	, Hit(ForceInit)
	, TraceParams(FName(TEXT("RV_Trace")), true)
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 1500.f;
	SpringArm->bAbsoluteRotation = true;
	SpringArm->RelativeRotation = FRotator(-70.0f, 0.0f, 0.0f);
	SpringArm->bDoCollisionTest = false;	//Useless in our case

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->bAbsoluteRotation = true;
	Camera->SetWorldRotation(FRotator(-70.0, 0.0f, 0.0f));

	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();


	Controller = GetWorld()->GetFirstPlayerController();
	Controller->bShowMouseCursor = true;
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Scroll(DeltaTime);
	
	if(bCameraSecondMode || !bCameraReturned)
		CameraMove(DeltaTime);

	if (bCursorClickedMode)
		MoveToCursor();

	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, FString::SanitizeFloat(MovementComponent->GetTurnBoost()));
	GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, FString::SanitizeFloat(MovementComponent->GetTurnSpeed()));
}


void APlayerPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis(MoveYBinding, this, &APlayerPawn::MoveForward);
	PlayerInputComponent->BindAxis(MoveXBinding, this, &APlayerPawn::MoveRight);
	
	PlayerInputComponent->BindAxis(ScrollBinding);
	
	PlayerInputComponent->BindAction(CameraSecondModeBinding, IE_Pressed, this, &APlayerPawn::CameraSecondModePressed);
	PlayerInputComponent->BindAction(CameraSecondModeBinding, IE_Released, this, &APlayerPawn::CameraSecondModeReleased);
	
	PlayerInputComponent->BindAxis(CameraXBinding);
	PlayerInputComponent->BindAxis(CameraYBinding);
	
	PlayerInputComponent->BindAction(CursorClickModeBinding, IE_Pressed, this, &APlayerPawn::CursorModePressed);
	PlayerInputComponent->BindAction(CursorClickModeBinding, IE_Released, this, &APlayerPawn::CursorModeReleased);

	PlayerInputComponent->BindAction("AddBoost", IE_Pressed, this, &APlayerPawn::AddBoost);
	PlayerInputComponent->BindAction("ReduceBoost", IE_Pressed, this, &APlayerPawn::ReduceBoost);
}

void APlayerPawn::AddBoost()
{
	MovementComponent->SetTurnBoost(MovementComponent->GetTurnBoost() + 0.1f);
}
void APlayerPawn::ReduceBoost()
{
	MovementComponent->SetTurnBoost(MovementComponent->GetTurnBoost() - 0.1f);
}


void APlayerPawn::MoveForward(float AxisValue)
{
	if (AxisValue != 0)
	{
		FVector WorldForwardVector(1.f, 0.f, 0.f);
		MovementComponent->AddInputVector(WorldForwardVector * AxisValue);
	}
}
void APlayerPawn::MoveRight(float AxisValue)
{
	if (AxisValue != 0)
	{
		FVector WorldRightVector(0.f, 1.f, 0.f);
		MovementComponent->AddInputVector(WorldRightVector * AxisValue);
	}	
}

void APlayerPawn::MoveToCursor()
{
	FVector Direction = ToCursorDirection();
	MovementComponent->AddInputVector(Direction);
}

void APlayerPawn::MoveTo(const FVector &Point)
{
	const FVector Direction = Point - GetActorLocation();
	MovementComponent->AddInputVector(Direction);
}

FVector APlayerPawn::ToCursorDirection()
{
	FVector WorldLocation, WorldDirection;
	Controller->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	FVector CurrentPos = MBody->GetComponentLocation();

	//call GetWorld() from within an actor extending class
	GetWorld()->LineTraceSingleByChannel(
		Hit,        //result
		WorldLocation,    //start
		WorldLocation + WorldDirection * SpringArm->TargetArmLength * 2, //end
		ECC_Pawn, //collision channel
		TraceParams
	);

	if (Hit.bBlockingHit)
	{
		Hit.Location.Z = CurrentPos.Z;
		return (Hit.Location - CurrentPos).GetClampedToMaxSize(1.0f);
	}

	return FVector(0);
}

void APlayerPawn::CursorModePressed()
{
	bCursorClickedMode = true;
}

void APlayerPawn::CursorModeReleased()
{
	bCursorClickedMode = false;
}


void APlayerPawn::Scroll(float DeltaTime)
{
	//Getting the scroll value
	float deltaArm = GetInputAxisValue(ScrollBinding);
	//if there was a scroll, or if there is unscrolled distance
	if (deltaArm != 0 || fabs(ScrollRemaining)>0.1)
	{
		//add to "unscrolled distance" thar value (value can be zero)
		ScrollRemaining += GetInputAxisValue(ScrollBinding);
		//calculate the small part of that value (which is gonna be scrolled now)
		float scrollReduce = ScrollRemaining/SmoothnessCoefficient;
		//offset camera by that value
		SpringArm->TargetArmLength = FMath::Clamp((SpringArm->TargetArmLength + scrollReduce * DeltaTime * ScrollSpeed), 400.0f, 2000.0f);
		//remove scrolled part from the remaining
		ScrollRemaining -= scrollReduce;
	}
}


void APlayerPawn::CameraSecondModePressed()
{
	bCameraSecondMode = true;
	bCameraReturned = false;
}
void APlayerPawn::CameraSecondModeReleased()
{
	bCameraSecondMode = false;
}


void APlayerPawn::CameraMove(float DeltaTime)																
{	
	if (bCameraSecondMode)
	{
		float ForwardValue = GetInputAxisValue(CameraXBinding);
		float RightValue = GetInputAxisValue(CameraYBinding);
		const FVector MoveDirection = -FVector(RightValue, ForwardValue, 0.f).GetClampedToMaxSize(1.0f);
		SpringArm->AddRelativeLocation(MoveDirection * CameraMoveSpeed * DeltaTime);
		return;
	}
	else
	{
		SpringArm->SetRelativeLocation(FVector(0));
		bCameraReturned = true;
	}
}