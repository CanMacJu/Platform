// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_CUBE(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube_Chamfer.1M_Cube_Chamfer'"));
	if (SM_CUBE.Succeeded())
	{
		Mesh->SetStaticMesh(SM_CUBE.Object);
	}

	//SetMobility(EComponentMobility::Movable);

	Speed = 100.f;
}

void AMovingPlatform::BeginPlay() 
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}

	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
	GlobalStartLocation = GetActorLocation();
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && ActiveTriggers > 0)
	{
		float TargetLength = FVector::Dist(GetActorLocation(), GlobalTargetLocation);

		FVector Location = GetActorLocation();
		Direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
		Location += Direction * Speed * DeltaTime;
		SetActorLocation(Location);

		float NextTickTargetLength = FVector::Dist(GetActorLocation(), GlobalTargetLocation);
		if (TargetLength < NextTickTargetLength)
		{
			Swap(GlobalStartLocation, GlobalTargetLocation);
		}
	}
}

void AMovingPlatform::AddActiveTrigger()
{
	Super::AddActiveTrigger();

}

void AMovingPlatform::RemoveActiveTrigger()
{
	Super::RemoveActiveTrigger();

}
