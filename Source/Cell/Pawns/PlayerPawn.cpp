// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"
#include <Camera/CameraComponent.h>
#include <Components/InputComponent.h>
#include <GameFramework/SpringArmComponent.h>

#include "Pawns/PawnBaseMovementComponent.h"

#include <Engine.h>

#define MY_CHECK GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Cyan, "Check")

const FName APlayerPawn::MoveYBinding("MoveY");
const FName APlayerPawn::MoveXBinding("MoveX");
const FName APlayerPawn::ScrollBinding("Scroll");
const FName APlayerPawn::CameraSecondModeBinding("CameraSecondMode");
const FName APlayerPawn::CameraYBinding("CameraY");
const FName APlayerPawn::CameraXBinding("CameraX");
const FName APlayerPawn::CursorClickModeBinding("CursorClickMode");

APlayerPawn::APlayerPawn()
	: ScrollRemaining(0.0f)
	, SmoothnessCoefficient(50.0f)
	, ScrollSpeed(6000.0f)
	, bCameraSecondMode(false)
	, bCameraReturned(true)
	, CameraMoveSpeed(2000.0f)
	, CameraMoveRemaining(0.0f)
	, bCursorClickedMode(false)
	, Hit(ForceInit)
	, TraceParams(FName(TEXT("RV_Trace")), true)
	, RotationSpeed(5.0f)
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
	APawn::Controller = dynamic_cast<AController *>(Controller);
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Scroll(DeltaTime);
	
	if(bCameraSecondMode || !bCameraReturned)
		CameraMove(DeltaTime);
}


void APlayerPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	check(PlayerInputComponent);

	InputComponent->BindAxis(MoveYBinding, this, &APlayerPawn::MoveForward);
	InputComponent->BindAxis(MoveXBinding, this, &APlayerPawn::MoveRight);

	InputComponent->BindAxis(ScrollBinding);

	InputComponent->BindAction(CameraSecondModeBinding, IE_Pressed, this, &APlayerPawn::CameraSecondModePressed);
	InputComponent->BindAction(CameraSecondModeBinding, IE_Released, this, &APlayerPawn::CameraSecondModeReleased);

	InputComponent->BindAxis(CameraXBinding);
	InputComponent->BindAxis(CameraYBinding);

	InputComponent->BindAction(CursorClickModeBinding, IE_Pressed, this, &APlayerPawn::CursorModePressed);
	InputComponent->BindAction(CursorClickModeBinding, IE_Released, this, &APlayerPawn::CursorModeReleased);
}

void APlayerPawn::MoveForward(float AxisValue)
{
	if(AxisValue!=0)
		MovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
}
void APlayerPawn::MoveRight(float AxisValue)
{
	MovementComponent->AddInputVector(GetActorRightVector()*AxisValue);
}


APlayerPawn::MoveParams APlayerPawn::CalculateMove(float DeltaTime)
{
	FVector MoveDirection;
	// If we r into Cursor Clicked Mode we move in the direcion of cursor, otherwise to the inputcontroller direction
	if (bCursorClickedMode)
	{
		MoveDirection = ToCursorDirection();
	}
	else
	{
		const float ForwardValue = GetInputAxisValue(MoveYBinding);
		const float RightValue = GetInputAxisValue(MoveXBinding);
		// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
		MoveDirection = FVector(ForwardValue, RightValue, 0.f).GetClampedToMaxSize(1.0f);
	}

	// Calculate  movement
	FVector Movement = MoveDirection * MaxVelocity * DeltaTime;
	FRotator NewRotation = RootComponent->GetComponentRotation();
	NewRotation.Yaw -= ((RootComponent->GetComponentRotation()-Movement.Rotation())*RotationSpeed*DeltaTime).Yaw;
	

	// If non-zero size, move this actor
	if (Movement.SizeSquared() > 0.0f)
	{
		FHitResult Hit(1.f);
		RootComponent->MoveComponent(Movement, NewRotation, true, &Hit);

		if (Hit.IsValidBlockingHit())
		{
			const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
			Movement = FVector(0);//FVector::VectorPlaneProject(Movement, Normal2D) * (1.f - Hit.Time);
		}	
	}
	return { Movement, NewRotation };
}

void APlayerPawn::CursorModePressed()
{
	bCursorClickedMode = true;
}

void APlayerPawn::CursorModeReleased()
{
	bCursorClickedMode = false;
}

FVector APlayerPawn::ToCursorDirection()
{
	FVector Position, Direction;
	Controller->DeprojectMousePositionToWorld(Position, Direction);

	FVector CurrentPos = MBody->GetComponentLocation();

	//call GetWorld() from within an actor extending class
	GetWorld()->LineTraceSingleByChannel(
		Hit,        //result
		Position,    //start
		Position+Direction*SpringArm->TargetArmLength*2, //end
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