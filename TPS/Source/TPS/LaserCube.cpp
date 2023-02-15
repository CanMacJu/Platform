// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserCube.h"
#include "Components/ArrowComponent.h"

ALaserCube::ALaserCube()
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
	Glass->SetCollisionProfileName(TEXT("OverlapAll"));

	Core = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CORE"));
	Core->SetupAttachment(RootComponent);
	Core->SetCollisionProfileName(TEXT("NoCollision"));

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ARROW"));
	Arrow->SetupAttachment(RootComponent);
	Arrow->SetRelativeLocation(FVector(50.f, 0.f, 0.f));


}

// Called when the game starts or when spawned
void ALaserCube::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ALaserCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALaserCube::CoreOn()
{
	if (MI_Core_On)
		Core->SetMaterial(0, MI_Core_On);

	bIsCoreOn = true;
}

void ALaserCube::CoreOff()
{
	if (MI_Core_Off)
		Core->SetMaterial(0, MI_Core_Off);

	bIsCoreOn = false;
}

void ALaserCube::SetVelocity(FVector velocity)
{
	CubeMesh->SetPhysicsLinearVelocity(velocity);
}
