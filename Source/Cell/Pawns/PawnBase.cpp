// Fill out your copyright notice in the Description page of Project Settings.

#include "PawnBase.h"
#include <Components/StaticMeshComponent.h>
#include <Components/InputComponent.h>
#include <Components/SphereComponent.h>
#include <Camera/CameraComponent.h>

#include "Pawns/PawnMovement.h"

#include "Food/FoodFlesh.h"
#include "Food/FoodHerb.h"

#include <Engine.h>


// Sets default values
APawnBase::APawnBase(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, Level(1)
	, Experience(0.0f)
	, Health(100.0f)
	, TypeOfFood(FoodType::Omnivorous)
	, Nutriment(60.0f)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bCanBeDamaged = true;

	PawnCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PawnCollision"));
	PawnCollision->InitSphereRadius(70.0f);
	PawnCollision->CanCharacterStepUpOn = ECB_No;
	PawnCollision->bShouldUpdatePhysicsVolume = true;
	PawnCollision->SetCanEverAffectNavigation(true);
	PawnCollision->bDynamicObstacle = true;

	PawnCollision->OnComponentBeginOverlap.AddDynamic(this, &APawnBase::OnPawnBeginOverlap);

	RootComponent = PawnCollision;

	MBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	MBody->SetupAttachment(PawnCollision);
	MBody->SetRelativeLocation(FVector(0.f));

	MouthCollision = CreateDefaultSubobject<USphereComponent>(TEXT("MouthCollision"));
	MouthCollision->SetupAttachment(MBody);

	MouthCollision->OnComponentBeginOverlap.AddDynamic(this, &APawnBase::OnMouthBeginOverlap);

	MovementComponent = CreateDefaultSubobject<UPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = PawnCollision;
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


void APawnBase::OnPawnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, "Overlapped");
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