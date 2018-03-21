// Fill out your copyright notice in the Description page of Project Settings.

#include "PawnBase.h"
#include <Components/StaticMeshComponent.h>
#include <Components/InputComponent.h>
#include <Components/SphereComponent.h>
#include <Camera/CameraComponent.h>

#include <GameFramework/PawnMovementComponent.h>
#include <GameFramework/FloatingPawnMovement.h>
#include "Pawns/PawnBaseMovementComponent.h"
#include "TestFloatingPawnMovement.h"

#include "Food/FoodFlesh.h"
#include "Food/FoodHerb.h"

#include <Engine.h>


// Sets default values
APawnBase::APawnBase()
	: Level(1)
	, Experience(0.0f)
	, Health(100.0f)
	, TypeOfFood(FoodType::Omnivorous)
	, MaxVelocity(200.0f)
	, Nutriment(60.0f)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PawnCollision"));
	PawnCollision->InitSphereRadius(50.0f);
	PawnCollision->CanCharacterStepUpOn = ECB_No;
	PawnCollision->bShouldUpdatePhysicsVolume = true;
	PawnCollision->SetCanEverAffectNavigation(false);
	PawnCollision->bDynamicObstacle = true;

	RootComponent = PawnCollision;

	MBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));

	MouthCollision = CreateDefaultSubobject<USphereComponent>(TEXT("MouthCollision"));
	MouthCollision->SetupAttachment(MBody);

	MouthCollision->OnComponentBeginOverlap.AddDynamic(this, &APawnBase::OnMouthBeginOverlap);

	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UTestFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;	
}

// Called when the game starts or when spawned
void APawnBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UPawnMovementComponent  *APawnBase::GetMovementComponent() const
{
	return MovementComponent;
}

void APawnBase::OnMouthBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//If we Overlapped Food
	if (OtherActor->GetClass()->IsChildOf(AFood::StaticClass()))
	{
		switch (TypeOfFood)
		{
			case Carnivorous:
			{
				if (OtherActor->GetClass()->IsChildOf(AFoodFlesh::StaticClass()))
					Eat(dynamic_cast<AFoodFlesh *>(OtherActor)->GetValue());
				OtherActor->Destroy();
				break;
			}
			case Herbovorous:
			{
				if (OtherActor->GetClass()->IsChildOf(AFoodHerb::StaticClass()))
					Eat(dynamic_cast<AFoodHerb *>(OtherActor)->GetValue());
				OtherActor->Destroy();
				break;
			}				
			case Omnivorous:
			{
				if (OtherActor->GetClass()->IsChildOf(AFoodHerb::StaticClass()))
					Eat(dynamic_cast<AFoodHerb *>(OtherActor)->GetValue());
				else
					Eat(dynamic_cast<AFoodFlesh *>(OtherActor)->GetValue());
				OtherActor->Destroy();
			}
		}//end of switch
	}
}


void APawnBase::Eat(float val)
{
	if (Nutriment + val > 100.0f)
		 Nutriment = 100.0f;
	else Nutriment += val;
}


/*void OnConstruction(const FTransform& Transform)
{

}*/