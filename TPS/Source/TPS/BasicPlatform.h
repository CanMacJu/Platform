// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasicPlatform.generated.h"

UCLASS()
class TPS_API ABasicPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasicPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:	

	virtual void AddActiveTrigger();
	virtual void RemoveActiveTrigger();

protected:

	UPROPERTY(EditAnywhere, Category = "BasicSetting")
	int32 ActiveTriggers = 1;



};
