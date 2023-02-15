// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlatformTrigger.h"
#include "LaserTrigger.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ALaserTrigger : public APlatformTrigger
{
	GENERATED_BODY()
	
private:

	bool bIsLaserTriggerOn;

public:

	ALaserTrigger();

	bool GetIsLaserTriggerOn() const { return bIsLaserTriggerOn; };

	void LaserTriggerOn();
	void LaserTriggerOff();
};
