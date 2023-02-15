// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"

#include "PlatformTrigger.generated.h"

UCLASS()
class TPS_API APlatformTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlatformTrigger();

protected:

	void ActiveLane();
	void InActiveLane();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void OnBeginOverlapTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlapTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void TimelineUpdate(float value);

	UFUNCTION()
	void TimelineFinish();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Border;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Switch;

	float SwitchHeight;

	FVector StartSwitchLocation;
	FVector FinishSwitchLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	UMaterialInterface* MI_TriggerOn;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	UMaterialInterface* MI_TriggerOff;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Trigger;

	UPROPERTY(EditInstanceOnly, Category = "BasicSetting")
	TArray<class ABasicPlatform*> PlaformsConnectedToTrigger;

	UPROPERTY(VisibleAnywhere)
	int32 OverlapedActorNum;



	UPROPERTY(VisibleAnywhere)
	class UTimelineComponent* SwitchTimeline;

	UPROPERTY(EditDefaultsOnly)
	class UCurveFloat* FloatCurve;

	FOnTimelineFloat UpdateFunction;
	FOnTimelineEvent FinishFunction;

	UPROPERTY(EditInstanceOnly, Category = "BasicSetting")
	TArray<class APlatformConnectLane*> PlatformConnectLanes;

	UPROPERTY(EditDefaultsOnly)
	class USoundCue* SC_On;

	UPROPERTY(EditDefaultsOnly)
	class USoundCue* SC_Off;
};
