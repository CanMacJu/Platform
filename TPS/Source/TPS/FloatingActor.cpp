// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingActor.h"
#include "Components/TimelineComponent.h"

// Sets default values
AFloatingActor::AFloatingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = Mesh;

	FloatingTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TIMELINE"));

}

// Called when the game starts or when spawned
void AFloatingActor::BeginPlay()
{
	Super::BeginPlay();
	
	GlobalStartLocation = GetActorLocation();
	GlobalTargetLocation = GlobalStartLocation + TargetLocation;

	if (FloatingTimeline)
	{
		FloatingTimeline->Play();
	}
}

void AFloatingActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UpdateFunction.BindUFunction(this, FName("TimelineUpdate"));
	FinishedFunction.BindUFunction(this, FName("TimelineFinish"));

	if (FloatingTimeline)
	{
		FloatingTimeline->AddInterpFloat(FloatCurve, UpdateFunction);
		FloatingTimeline->SetTimelineFinishedFunc(FinishedFunction);
		FloatingTimeline->SetLooping(false);
	}
}

// Called every frame
void AFloatingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloatingActor::TimelineUpdate(float Value)
{
	SetActorLocation(FMath::Lerp(GlobalStartLocation, GlobalTargetLocation, Value));
}

void AFloatingActor::TimelineFinish()
{
	/*if (FloatingTimeline->GetPlaybackPosition() == 2.0f)
	{
		FloatingTimeline->Reverse();
		UE_LOG(LogTemp, Warning, TEXT("Finish Timeline"));
	}
	else if (FloatingTimeline->GetPlaybackPosition() == 0.f)
	{
		FloatingTimeline->Play();
		UE_LOG(LogTemp, Error, TEXT("Play Timeline"));
	}*/

	
	FloatingTimeline->PlayFromStart();
}

