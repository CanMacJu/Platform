// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserTrigger.h"
#include "BasicPlatform.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"

ALaserTrigger::ALaserTrigger()
{
	Trigger->SetCollisionProfileName(FName(TEXT("LaserTrigger")));
}

void ALaserTrigger::LaserTriggerOn()
{
	if (MI_TriggerOn && SwitchTimeline)
	{
		Switch->SetMaterial(0, MI_TriggerOn);
		SwitchTimeline->Play();
		bIsLaserTriggerOn = true;
	}

	ActiveLane();

	for (ABasicPlatform* Platform : PlaformsConnectedToTrigger)
		Platform->AddActiveTrigger();

	if (SC_On)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SC_On, GetActorLocation());
}

void ALaserTrigger::LaserTriggerOff()
{
	if (MI_TriggerOff && SwitchTimeline)
	{
		Switch->SetMaterial(0, MI_TriggerOff);
		SwitchTimeline->Reverse();
		bIsLaserTriggerOn = false;
	}

	InActiveLane();

	for (ABasicPlatform* Platform : PlaformsConnectedToTrigger)
		Platform->RemoveActiveTrigger();

	if (SC_Off)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SC_Off, GetActorLocation());
}