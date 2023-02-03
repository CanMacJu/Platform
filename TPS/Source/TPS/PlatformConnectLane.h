// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformConnectLane.generated.h"

UCLASS()
class TPS_API APlatformConnectLane : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlatformConnectLane();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void SetTextureOn();
	void SetTextureOff();

private:

	virtual void OnConstruction(const FTransform& Transform) override;

private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere)
	class UDecalComponent* Decal;

	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* Arrow;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_Decal;

	UPROPERTY(VisibleAnywhere)
	class UMaterialInstanceDynamic* MID_Texture;

	UPROPERTY(EditDefaultsOnly)
	class UTexture2D* Texture_On;

	UPROPERTY(EditDefaultsOnly)
	class UTexture2D* Texture_Off;

};
