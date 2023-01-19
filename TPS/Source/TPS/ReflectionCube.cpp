// Fill out your copyright notice in the Description page of Project Settings.


#include "ReflectionCube.h"
#include "Components/ArrowComponent.h"

// Sets default values
AReflectionCube::AReflectionCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE_MESH"));
	RootComponent = CubeMesh;
	CubeMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	RootComponent->SetMobility(EComponentMobility::Movable);
	CubeMesh->SetSimulatePhysics(true);

	Glass = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GLASS"));
	Glass->SetupAttachment(RootComponent);

	Core = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CORE"));
	Core->SetupAttachment(RootComponent);
	Core->SetCollisionProfileName(TEXT("NoCollision"));

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ARROW"));
	Arrow->SetupAttachment(RootComponent);
	Arrow->SetRelativeLocation(FVector(50.f , 0.f, 0.f));

	
}

// Called when the game starts or when spawned
void AReflectionCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AReflectionCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AReflectionCube::CoreOn()
{
	if (MI_Core_On)
		Core->SetMaterial(0, MI_Core_On);

	bIsCoreOn = true;
}

void AReflectionCube::CoreOff()
{
	if (MI_Core_Off)
		Core->SetMaterial(0, MI_Core_Off);

	bIsCoreOn = false;
}

