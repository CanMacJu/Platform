// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicPlatform.h"
#include "Components/TimelineComponent.h"
#include "DoorPlatform.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ADoorPlatform : public ABasicPlatform
{
	GENERATED_BODY()
	
public:

	ADoorPlatform();

public:

	virtual void AddActiveTrigger() override;
	virtual void RemoveActiveTrigger() override;


private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY()
	class UTimelineComponent* Timeline;

	UPROPERTY(EditAnywhere)
	class UCurveFloat* CurveFloat;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector TargetLocation;

	FVector GlobalStartLocation;
	FVector GlobalTargetLocation;

	FOnTimelineFloat UpdateFunction;
	FOnTimelineEvent FinishFunction;

};
