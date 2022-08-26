// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "FloatingActor.generated.h"

UCLASS()
class TPS_API AFloatingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UFUNCTION()
	void TimelineUpdate(float Value);

	UFUNCTION()
	void TimelineFinish();

private:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY()
	class UTimelineComponent* FloatingTimeline;

	UPROPERTY(EditAnywhere)
	class UCurveFloat* FloatCurve;


	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector TargetLocation;

	FVector GlobalStartLocation;

	FVector GlobalTargetLocation;

	FOnTimelineFloat UpdateFunction;
	FOnTimelineEvent FinishedFunction;
};
