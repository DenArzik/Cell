// Fill out your copyright notice in the Description page of Project Settings.

#include "Food.h"
#include <Components/StaticMeshComponent.h>

// Sets default values
AFood::AFood()
	: Size(1.0)
	, Value(Size*20.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	//Mesh->OnComponentBeginOverlap.AddDynamic(this, &AFood::Overlap);
}

float AFood::GetSize()
{
	return Size;
}

float AFood::GetValue()
{
	return Value;
}

// Called when the game starts or when spawned
void AFood::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFood::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


