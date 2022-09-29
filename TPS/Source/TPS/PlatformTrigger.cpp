// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformTrigger.h"
#include "Components/BoxComponent.h"
#include "MovingPlatform.h"
#include "Components/TimelineComponent.h"

// Sets default values
APlatformTrigger::APlatformTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Trigger->SetBoxExtent(FVector(50.f, 50.f, 15.f));
	RootComponent = Trigger;

	Switch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	Switch->SetupAttachment(RootComponent);

	Border = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BORDER"));
	Border->SetupAttachment(RootComponent);

	SwitchTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TIMELINE"));
}

// Called when the game starts or when spawned
void APlatformTrigger::BeginPlay()
{
	Super::BeginPlay();

	SwitchHeight = 10.f;
	StartSwitchLocation = Switch->GetRelativeLocation();
	FinishSwitchLocation = StartSwitchLocation - FVector(0.f, 0.f, SwitchHeight * 0.8f);

}

void APlatformTrigger::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &APlatformTrigger::OnBeginOverlapTrigger);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &APlatformTrigger::OnEndOverlapTrigger);

	UpdateFunction.BindUFunction(this, FName("TimelineUpdate"));
	FinishFunction.BindUFunction(this, FName("TimelineFinish"));

	if (SwitchTimeline && FloatCurve)
	{
		SwitchTimeline->AddInterpFloat(FloatCurve, UpdateFunction);
		SwitchTimeline->SetTimelineFinishedFunc(FinishFunction);
		SwitchTimeline->SetLooping(false);
		SwitchTimeline->SetPlayRate(3.f);
	}
}

void APlatformTrigger::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (M_TriggerOff)
	{
		Switch->SetMaterial(0, M_TriggerOff);
	}
}

void APlatformTrigger::OnBeginOverlapTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlapedActorNum++;

	if (M_TriggerOn && OverlapedActorNum > 0 && SwitchTimeline)
	{
		Switch->SetMaterial(0, M_TriggerOn);
		SwitchTimeline->Play();
	}

	for (auto Platform : PlaformsConnectedToTrigger)
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

	if (M_TriggerOff && OverlapedActorNum == 0 && SwitchTimeline)
	{
		Switch->SetMaterial(0, M_TriggerOff);
		SwitchTimeline->Reverse();
	}

	for (auto Platform : PlaformsConnectedToTrigger)
	{
		Platform->RemoveActiveTrigger();
	}
}

void APlatformTrigger::TimelineUpdate(float Value)
{
	Switch->SetRelativeLocation(FMath::Lerp(StartSwitchLocation, FinishSwitchLocation, Value));
}

void APlatformTrigger::TimelineFinish()
{
	UE_LOG(LogTemp, Warning, TEXT("Finish Timeline"));
}



// Called every frame
void APlatformTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlatformTrigger::LaserTriggerOn()
{
	if (M_TriggerOn && SwitchTimeline)
	{
		Switch->SetMaterial(0, M_TriggerOn);
		SwitchTimeline->Play();
	}

	for (auto Platform : PlaformsConnectedToTrigger)
	{
		Platform->AddActiveTrigger();
	}
}

void APlatformTrigger::LaserTriggerOff()
{
	if (M_TriggerOff && SwitchTimeline)
	{
		Switch->SetMaterial(0, M_TriggerOff);
		SwitchTimeline->Reverse();
	}

	for (auto Platform : PlaformsConnectedToTrigger)
	{
		Platform->RemoveActiveTrigger();
	}
}

