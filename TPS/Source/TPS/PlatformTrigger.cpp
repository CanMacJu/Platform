// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformTrigger.h"
#include "Components/BoxComponent.h"
#include "MovingPlatform.h"

// Sets default values
APlatformTrigger::APlatformTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Trigger->SetBoxExtent(FVector(50.f, 50.f, 15.f));
	RootComponent = Trigger;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	Mesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void APlatformTrigger::BeginPlay()
{
	Super::BeginPlay();

}

void APlatformTrigger::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &APlatformTrigger::OnBeginOverlapTrigger);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &APlatformTrigger::OnEndOverlapTrigger);

}

void APlatformTrigger::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (M_TriggerOff)
	{
		Mesh->SetMaterial(0, M_TriggerOff);
	}
}

void APlatformTrigger::OnBeginOverlapTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlapedActorNum++;

	if (M_TriggerOn && OverlapedActorNum > 0)
	{
		Mesh->SetMaterial(0, M_TriggerOn);
	}

	for (auto Platform : PlaformsToTrigger)
	{
		Platform->AddActiveTrigger();
	}

}

void APlatformTrigger::OnEndOverlapTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlapedActorNum > 0)
	{
		OverlapedActorNum--;
	}

	if (M_TriggerOff && OverlapedActorNum == 0)
	{
		Mesh->SetMaterial(0, M_TriggerOff);
	}

	for (auto Platform : PlaformsToTrigger)
	{
		Platform->RemoveActiveTrigger();
	}
}



// Called every frame
void APlatformTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

