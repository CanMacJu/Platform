// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API AMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	AMovingPlatform();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:

	void AddActiveTrigger();
	void RemoveActiveTrigger();

private:

	UPROPERTY(EditAnywhere)
		int32 ActiveTriggers = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"));
	FVector TargetLocation;

	FVector GlobalTargetLocation;
	FVector GlobalStartLocation;

	FVector Direction;
};
