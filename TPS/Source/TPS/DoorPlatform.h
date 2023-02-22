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

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

public:

	virtual void AddActiveTrigger() override;
	virtual void RemoveActiveTrigger() override;


private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DoorFrame;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Door;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_DoorFrameClose;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_DoorFrameOpen;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_DoorClose;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_DoorOpen;

	UPROPERTY(EditInstanceOnly, Category = "BasicSetting")
	bool mbIsAlwaysOpen;
};
