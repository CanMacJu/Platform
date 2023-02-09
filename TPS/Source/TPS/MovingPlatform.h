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

	void Init();
	void NextTargetSetting();

private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* Ptl_Lane;

	TArray<class UParticleSystemComponent*> MovingLaneParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicSetting", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(EditInstanceOnly, meta = (MakeEditWidget = "true"), Category = "BasicSetting");
	TArray<FVector> LocalTargetLocations;
	TArray<FVector> GlobalTargetLocations;

	UPROPERTY(EditInstanceOnly, Category = "BasicSetting")
	int32 RequiredActiveTrigger = 1;

	FVector Start;
	FVector Target;
	int32 TargetIndex;
	float DistanceFromStartToTarget;
	FVector Direction;
};
