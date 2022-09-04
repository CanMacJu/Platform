// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalWall.h"

// Sets default values
APortalWall::APortalWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	Mesh->SetCollisionProfileName(TEXT("PortalWall"));
	RootComponent = Mesh;

	Width = 100.f;
	Height = 100.f;
}

// Called when the game starts or when spawned
void APortalWall::BeginPlay()
{
	Super::BeginPlay();
	

}

FTransform APortalWall::ClampPortalPosition(FVector Location)
{
	FTransform ClampTransform;
	FVector ClampLocation;
	FVector LocalLocation = GetTransform().InverseTransformPositionNoScale(Location);

	ClampLocation.X = 1.f;
	ClampLocation.Y = FMath::Clamp(FMath::Abs(LocalLocation.Y), 0.f, Width / 2 - 90.f);
	ClampLocation.Z = FMath::Clamp(FMath::Abs(LocalLocation.Z), 0.f, Height / 2 - 124.5f);
	
	if (LocalLocation.Y < 0)
	{
		ClampLocation.Y *= -1.f;
	}

	if (LocalLocation.Z < 0)
	{
		ClampLocation.Z *= -1.f;
	}

	ClampLocation = GetTransform().TransformPositionNoScale(ClampLocation);

	ClampTransform = FTransform(GetActorRotation(), ClampLocation);

	return ClampTransform;
}

// Called every frame
void APortalWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

