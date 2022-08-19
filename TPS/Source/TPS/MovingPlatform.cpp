// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_CUBE(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube_Chamfer.1M_Cube_Chamfer'"));
	if (SM_CUBE.Succeeded())
	{
		GetStaticMeshComponent()->SetStaticMesh(SM_CUBE.Object);
	}

	SetMobility(EComponentMobility::Movable);

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


	/*FVector GlobalTargetLocation = TargetLocation + GetActorLocation();
	Direction = (GlobalTargetLocation - GetActorLocation()).GetSafeNormal();*/

	//Direction = TargetLocation.GetSafeNormal();
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && ActiveTriggers > 0)
	{

		/*float JourneyLength = (GlobalTargetLocation - GlobalStartLocation).Size();
		float JourneyTravelled = (GetActorLocation() - GlobalStartLocation).Size();
		if (JourneyLength <= JourneyTravelled)
		{
			FVector TempVector = GlobalStartLocation;
			GlobalStartLocation = GlobalTargetLocation;
			GlobalTargetLocation = TempVector;
		}*/

		float TargetLength = FVector::Dist(GetActorLocation(), GlobalTargetLocation);

		FVector Location = GetActorLocation();
		Direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
		Location += Direction * Speed * DeltaTime;
		SetActorLocation(Location);

		float NextTickTargetLength = FVector::Dist(GetActorLocation(), GlobalTargetLocation);
		if (TargetLength <= NextTickTargetLength)
		{
			FVector TempVector = GlobalStartLocation;
			GlobalStartLocation = GlobalTargetLocation;
			GlobalTargetLocation = TempVector;
		}

	}
	

}

void AMovingPlatform::AddActiveTrigger()
{
	ActiveTriggers++;
}

void AMovingPlatform::RemoveActiveTrigger()
{
	if (ActiveTriggers > 0)
	{
		ActiveTriggers--;
	}
}
