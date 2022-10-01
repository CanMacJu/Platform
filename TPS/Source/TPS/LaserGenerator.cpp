// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "Portal.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "TPSCharacter.h"
#include "Components/ArrowComponent.h"
#include "PlatformTrigger.h"

// Sets default values
ALaserGenerator::ALaserGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("SCENE"));
	RootComponent = Scene;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	Mesh->SetupAttachment(RootComponent);

	Muzzle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MUZZLE"));
	Muzzle->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALaserGenerator::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALaserGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ResetLaser();

	FVector Start = Muzzle->GetComponentLocation();
	FVector Direction = Muzzle->GetForwardVector();
	Laser(Start, Direction * 10000, 5);
	DrawLaser();
}

void ALaserGenerator::Laser(FVector Start, FVector Direction, int32 ReflectionCount)
{
	if (MI_Mirror == nullptr) return;

	if (P_Laser == nullptr) return;

	auto World = GetWorld();
	if (World == nullptr) return;

	FHitResult HitResult;
	FCollisionQueryParams QueryParam = FCollisionQueryParams(NAME_None, true, this);
	bool Result = World->LineTraceSingleByChannel(HitResult, Start, Start + Direction, ECollisionChannel::ECC_GameTraceChannel7, QueryParam);

	/*FColor Color = Result ? FColor::Green : FColor::Red;
	DrawDebugLine(World, Start, Start + Direction, Color);*/

	if (Result)
	{
		LaserParticles.Add(UGameplayStatics::SpawnEmitterAttached(P_Laser, Muzzle));
		SourcePoints.Add(Start);
		EndPoints.Add(HitResult.ImpactPoint);

		auto PlatformTrigger = Cast<APlatformTrigger>(HitResult.GetActor());
		if (PlatformTrigger && PlatformTrigger->IsLaserTrigger && !LaserTrigger.IsValid())
		{
			LaserTrigger = PlatformTrigger;
			LaserTrigger->LaserTriggerOn();
		}
		else if (!PlatformTrigger && LaserTrigger.IsValid())
		{
			LaserTrigger->LaserTriggerOff();
			LaserTrigger.Reset();
		}

		if (!PlatformTrigger)
		{
			auto Portal = Cast<APortal>(HitResult.GetActor());
			if (Portal && Portal->LinkedPortal.IsValid())
			{
				FVector RelativeStartPoint = Portal->Arrow->GetComponentTransform().InverseTransformPosition(HitResult.ImpactPoint);
				Start = Portal->LinkedPortal->GetTransform().TransformPosition(RelativeStartPoint);

				FVector RelativeDirection = Portal->Arrow->GetComponentTransform().InverseTransformVector(Direction);
				Direction = Portal->LinkedPortal->GetTransform().TransformVector(RelativeDirection);

				Laser(Start, Direction, ReflectionCount);
				return;
			}
		}

		if (ReflectionCount == 0) return;

		if (HitResult.GetComponent()->GetMaterial(0) == MI_Mirror)
		{
			FVector ImpactNormal = HitResult.ImpactNormal;

			Start = HitResult.ImpactPoint;
			Direction = 2 * ImpactNormal * FVector::DotProduct(ImpactNormal, -1.f * Direction) + Direction;

			ReflectionCount--;
			Laser(Start, Direction, ReflectionCount);
		}
	}
	else
	{
		LaserParticles.Add(UGameplayStatics::SpawnEmitterAttached(P_Laser, Muzzle));
		SourcePoints.Add(Start);
		EndPoints.Add(Start + Direction);
	}
}

void ALaserGenerator::DrawLaser()
{
	if ((LaserParticles.Num() == SourcePoints.Num() && SourcePoints.Num() == EndPoints.Num()) == false) return;

	for (int32 i = 0; i < LaserParticles.Num(); ++i)
	{
		LaserParticles[i]->SetBeamSourcePoint(0, SourcePoints[i], 0);
		LaserParticles[i]->SetBeamEndPoint(0, EndPoints[i]);
	}
}

void ALaserGenerator::ResetLaser()
{
	for (auto LaserParticle : LaserParticles)
	{
		LaserParticle->DestroyComponent();
	}
	LaserParticles.Empty();
	SourcePoints.Empty();
	EndPoints.Empty();
}