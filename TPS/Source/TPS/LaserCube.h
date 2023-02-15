// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabableActor.h"
#include "LaserCube.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ALaserCube : public AGrabableActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ALaserCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CoreOn();
	void CoreOff();

	bool GetIsCoreOn() const { return bIsCoreOn; }

	virtual void SetVelocity(FVector velocity) override;

private:

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* CubeMesh;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Glass;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Core;

	UPROPERTY(VisibleAnywhere)
		class UArrowComponent* Arrow;

	UPROPERTY(EditDefaultsOnly)
		class UMaterialInterface* MI_Core_On;

	UPROPERTY(EditDefaultsOnly)
		class UMaterialInterface* MI_Core_Off;

	bool bIsCoreOn;
};
