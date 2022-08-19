// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameStartTrigger.generated.h"

UCLASS()
class TPS_API AGameStartTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameStartTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

private:

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY()
	class UBoxComponent* Trigger;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_TriggerOn;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_TriggerOff;

	int32 OverlapActorNum = 0;
};
