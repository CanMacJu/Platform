// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformTrigger.h"
#include "Components/BoxComponent.h"
#include "MovingPlatform.h"
#include "Components/TimelineComponent.h"
#include "PlatformConnectLane.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

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
	Switch->SetRelativeScale3D(FVector(1.f, 1.f, 0.1f));

	Border = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BORDER"));
	Border->SetupAttachment(RootComponent);
	Border->SetRelativeLocation(FVector(0.f, 0.f, -10.f));
	Border->SetRelativeScale3D(FVector(1.2f, 1.2f, 0.1f));

	SwitchTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TIMELINE"));
}

void APlatformTrigger::ActiveLane()
{
	for (auto Lane : PlatformConnectLanes)
		Lane->SetTextureOn();
}

void APlatformTrigger::InActiveLane()
{
	for (auto Lane : PlatformConnectLanes)
		Lane->SetTextureOff();
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

	if (MI_TriggerOff)
		Switch->SetMaterial(0, MI_TriggerOff);
}

void APlatformTrigger::OnBeginOverlapTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlapedActorNum++;

	if (OverlapedActorNum > 1) return;

	if (MI_TriggerOn && SwitchTimeline)
	{
		Switch->SetMaterial(0, MI_TriggerOn);
		SwitchTimeline->Play();
		ActiveLane();
	}

	for (auto Platform : PlaformsConnectedToTrigger)
		Platform->AddActiveTrigger();

	if (SC_On)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SC_On, GetActorLocation());
}

void APlatformTrigger::OnEndOverlapTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlapedActorNum > 0)
		OverlapedActorNum--;

	if (OverlapedActorNum != 0) return;

	if (MI_TriggerOff && SwitchTimeline)
	{
		Switch->SetMaterial(0, MI_TriggerOff);
		SwitchTimeline->Reverse();
		InActiveLane();
	}

	for (auto Platform : PlaformsConnectedToTrigger)
		Platform->RemoveActiveTrigger();

	if (SC_Off)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SC_Off, GetActorLocation());
}

void APlatformTrigger::TimelineUpdate(float value)
{
	Switch->SetRelativeLocation(FMath::Lerp(StartSwitchLocation, FinishSwitchLocation, value));
}

void APlatformTrigger::TimelineFinish()
{
}



// Called every frame
void APlatformTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}