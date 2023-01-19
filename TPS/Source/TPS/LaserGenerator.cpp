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
#include "ReflectionCube.h"

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

	ReflectionCount = 5;
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
	Laser(Start, Direction * 10000, ReflectionCount);

	CompareReflectionCube();
	CompareLaserTrigger();
	DrawLaser();
}

void ALaserGenerator::Laser(FVector Start, FVector Direction, int32 _ReflectionCount)
{
	UWorld* World = GetWorld();

	{
		if (World == nullptr) return;

		if (MI_Mirror == nullptr) return;

		if (MI_Glass == nullptr) return;

		if (P_Laser == nullptr) return;
	}

	FHitResult HitResult;
	FCollisionQueryParams QueryParam = FCollisionQueryParams(NAME_None, true, this);
	bool Result = World->LineTraceSingleByChannel(HitResult, Start, Start + Direction, ECollisionChannel::ECC_GameTraceChannel7, QueryParam);


	AActor* HitActor = HitResult.GetActor();
	eHitType HitType;

	{
		if (Result == false)
		{
			HitType = eHitType::NONE;
			goto SWITCH;
		}

		if (Cast<APortal>(HitActor))
		{
			HitType = eHitType::PORTAL;
			goto SET_LASER;
		}

		if (HitResult.GetComponent()->GetMaterial(0) == MI_Mirror)
		{
			HitType = eHitType::MIRROR;
			goto SET_LASER;
		}

		if (SetLaserTrigger(Cast<APlatformTrigger>(HitActor)))
		{
			HitType = eHitType::TRIGGER;
			goto SET_LASER;
		}

		if (HitResult.GetComponent()->GetMaterial(0) == MI_Glass && AddReflectionCube(Cast<AReflectionCube>(HitActor)))
		{
			HitType = eHitType::REFLECTION_CUBE;
			goto SET_LASER;
		}

		// 추가할거는 여기에 추가


		// 무적권 얘는 맨 마지막
		HitType = eHitType::OTHER;
		goto SET_LASER;
	}

SET_LASER:
	LaserParticles.Add(UGameplayStatics::SpawnEmitterAttached(P_Laser, Muzzle));
	SourcePoints.Add(Start);
	EndPoints.Add(HitResult.ImpactPoint);

SWITCH:
	switch (HitType)
	{
	case eHitType::NONE:
		LaserParticles.Add(UGameplayStatics::SpawnEmitterAttached(P_Laser, Muzzle));
		SourcePoints.Add(Start);
		EndPoints.Add(Start + Direction);
		break;
	case eHitType::PORTAL:
	{
		APortal* Portal = Cast<APortal>(HitActor);
		if (Portal->LinkedPortal.IsValid())
		{
			FVector RelativeStartPoint = Portal->Arrow->GetComponentTransform().InverseTransformPosition(HitResult.ImpactPoint);
			Start = Portal->LinkedPortal->GetTransform().TransformPosition(RelativeStartPoint);

			FVector RelativeDirection = Portal->Arrow->GetComponentTransform().InverseTransformVector(Direction);
			Direction = Portal->LinkedPortal->GetTransform().TransformVector(RelativeDirection);

			Laser(Start, Direction, _ReflectionCount);
		}
		break;
	}
	case eHitType::MIRROR:
	{
		if (_ReflectionCount == 0) return;

		FVector ImpactNormal = HitResult.ImpactNormal;

		Start = HitResult.ImpactPoint;
		Direction = 2 * ImpactNormal * FVector::DotProduct(ImpactNormal, -1.f * Direction) + Direction;

		_ReflectionCount--;
		Laser(Start, Direction, _ReflectionCount);
		break;
	}
	case eHitType::TRIGGER:
		break;
	case eHitType::REFLECTION_CUBE:
	{
		LaserParticles.Add(UGameplayStatics::SpawnEmitterAttached(P_Laser, Muzzle));
		SourcePoints.Add(HitResult.ImpactPoint);
		EndPoints.Add(HitActor->GetActorLocation());

		LaserParticles.Add(UGameplayStatics::SpawnEmitterAttached(P_Laser, Muzzle));
		SourcePoints.Add(HitActor->GetActorLocation());
		EndPoints.Add(HitActor->GetActorLocation() + HitActor->GetActorForwardVector() * 50.f);

		Start = HitActor->GetActorLocation() + HitActor->GetActorForwardVector() * 50.f;
		Direction = HitActor->GetActorForwardVector() * 10000.f;
		Laser(Start, Direction, _ReflectionCount);
		break;
	}
	case eHitType::OTHER:
		break;
	default:
		
		break;
	}
}

bool ALaserGenerator::SetLaserTrigger(APlatformTrigger* laserTrigger)
{
	if (laserTrigger == nullptr || laserTrigger->IsLaserTrigger == false) return false;

	if (laserTrigger->GetIsLaserTriggerOn() == false)
		laserTrigger->LaserTriggerOn();
	CurrentLaserTrigger = laserTrigger;
	return true;
}

void ALaserGenerator::CompareLaserTrigger()
{
	if (CurrentLaserTrigger != PreviousLaserTrigger && PreviousLaserTrigger.IsValid())
		PreviousLaserTrigger->LaserTriggerOff();

	PreviousLaserTrigger = CurrentLaserTrigger;
	CurrentLaserTrigger.Reset();
}

void ALaserGenerator::DrawLaser()
{
	if ((LaserParticles.Num() == SourcePoints.Num() && SourcePoints.Num() == EndPoints.Num()) == false) return;

	for (int32 i = 0; i < LaserParticles.Num(); ++i)
	{
		LaserParticles[i]->SetBeamSourcePoint(0, SourcePoints[i], 0);
		LaserParticles[i]->SetBeamEndPoint(0, EndPoints[i]);
		LaserParticles[i]->SetRelativeScale3D(FVector(5.f, 5.f, 0.f));
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

bool ALaserGenerator::AddReflectionCube(AReflectionCube* reflectionCube)
{
	if (reflectionCube == nullptr) return false;

	for (TWeakObjectPtr<AReflectionCube> ReflectionCube : CurrentReflectionCubes)
	{
		if (reflectionCube == ReflectionCube)
			return false;
	}

	if (reflectionCube->GetIsCoreOn() == false)
		reflectionCube->CoreOn();
	CurrentReflectionCubes.Add(reflectionCube);
	return true;
}

void ALaserGenerator::CompareReflectionCube()
{
	for (TWeakObjectPtr<AReflectionCube> PreviousReflectionCube : PreviousReflectionCubes)
	{
		bool IsContinuously = false;
		for (TWeakObjectPtr<AReflectionCube> CurrentReflectionCube : CurrentReflectionCubes)
		{
			if (PreviousReflectionCube == CurrentReflectionCube)
			{
				IsContinuously = true;
				break;
			}
		}

		if (IsContinuously == false)
			PreviousReflectionCube->CoreOff();
	}

	PreviousReflectionCubes.Empty();
	for (TWeakObjectPtr<AReflectionCube> CurrentReflectionCube : CurrentReflectionCubes)
		PreviousReflectionCubes.Add(CurrentReflectionCube);
	CurrentReflectionCubes.Empty();
}
