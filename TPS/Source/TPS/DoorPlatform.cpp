// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorPlatform.h"
#include "Components/TimelineComponent.h"

ADoorPlatform::ADoorPlatform()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = Mesh;

	Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TIMELINE"));

}

void ADoorPlatform::AddActiveTrigger()
{
	Super::AddActiveTrigger();

	UE_LOG(LogTemp, Error, TEXT("ADoorPlatform::AddActiveTrigger"));
}

void ADoorPlatform::RemoveActiveTrigger()
{
	Super::RemoveActiveTrigger();

	UE_LOG(LogTemp, Error, TEXT("ADoorPlatform::RemoveActiveTrigger"));
}
