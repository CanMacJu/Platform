// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorPlatform.h"
#include "Components/TimelineComponent.h"

ADoorPlatform::ADoorPlatform()
{
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	RootComponent = DoorFrame;

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(RootComponent);
}

void ADoorPlatform::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (MI_DoorFrameClose && MI_DoorFrameOpen && MI_DoorClose && MI_DoorOpen)
	{
		for (int32 i = 0; i < DoorFrame->GetMaterials().Num(); ++i)
		{
			DoorFrame->SetMaterial(i, MI_DoorFrameClose);
		}
		Door->SetMaterial(0, MI_DoorClose);
	}
}

void ADoorPlatform::AddActiveTrigger()
{
	Super::AddActiveTrigger();

	if (MI_DoorFrameOpen && MI_DoorOpen)
	{
		for (int32 i = 0; i < DoorFrame->GetMaterials().Num(); ++i)
		{
			DoorFrame->SetMaterial(i, MI_DoorFrameOpen);
		}
		Door->SetMaterial(0, MI_DoorOpen);
		Door->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ADoorPlatform::RemoveActiveTrigger()
{
	Super::RemoveActiveTrigger();

	if (ActiveTriggers > 0) return;

	if (MI_DoorFrameClose && MI_DoorClose)
	{
		for (int32 i = 0; i < DoorFrame->GetMaterials().Num(); ++i)
		{
			DoorFrame->SetMaterial(i, MI_DoorFrameClose);
		}
		Door->SetMaterial(0, MI_DoorClose);
		Door->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}