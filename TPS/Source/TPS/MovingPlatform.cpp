// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("SCENE"));
	RootComponent = Scene;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_CUBE(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube_Chamfer.1M_Cube_Chamfer'"));
	if (SM_CUBE.Succeeded())
		Mesh->SetStaticMesh(SM_CUBE.Object);
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetRelativeScale3D(FVector(2.f, 2.f, 0.5f));
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

	Init();
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && ActiveTriggers >= RequiredActiveTrigger)
	{
		FVector Location = GetActorLocation();
		Location += Direction * Speed * DeltaTime;
		SetActorLocation(Location);

		float DistanceFromStartToCurrent = FVector::Dist(Start, GetActorLocation());
		if (DistanceFromStartToTarget <= DistanceFromStartToCurrent)
		{
			SetActorLocation(Target);
			NextTargetSetting();
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

void AMovingPlatform::Init()
{
	for (FVector LocalTargetLocation : LocalTargetLocations)
		GlobalTargetLocations.Add(GetTransform().TransformPositionNoScale(LocalTargetLocation));

	NextTargetSetting();
}

void AMovingPlatform::NextTargetSetting()
{
	Start = GlobalTargetLocations[TargetIndex % LocalTargetLocations.Num()];
	TargetIndex++;
	Target = GlobalTargetLocations[TargetIndex % LocalTargetLocations.Num()];
	DistanceFromStartToTarget = FVector::Dist(Start, Target);
	Direction = (Target - Start).GetSafeNormal();
}
