// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserGenerator.generated.h"

UCLASS()
class TPS_API ALaserGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaserGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Muzzle;

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void Laser(FVector Start, FVector Direction, int32 _ReflectionCount);

	void ResetTrigger();

	TArray<class UParticleSystemComponent*> LaserParticles;
	TArray<FVector>SourcePoints;
	TArray<FVector>EndPoints;
	void DrawLaser();
	void ResetLaser();
	UPROPERTY(EditDefaultsOnly)
	int32 ReflectionCount;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_Mirror;
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* P_Laser;

	UPROPERTY(VisibleInstanceOnly)
	TWeakObjectPtr<class APlatformTrigger> LaserTrigger;

	enum class eHitType
	{
		NONE,
		PORTAL,
		MIRROR,
		TRIGGER,

		OTHER
	};
};
