// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasicPlatform.h"
#include "MovingPlatform.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API AMovingPlatform : public ABasicPlatform
{
	GENERATED_BODY()
	
public:
	AMovingPlatform();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:

	virtual void AddActiveTrigger() override;
	virtual void RemoveActiveTrigger() override;

private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"));
	FVector TargetLocation;

	FVector GlobalTargetLocation;
	FVector GlobalStartLocation;

	FVector Direction;
};
