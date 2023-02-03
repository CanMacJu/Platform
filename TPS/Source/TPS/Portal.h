// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

UCLASS()
class TPS_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

	virtual void OnConstruction(const FTransform& Transform) override;

	void LinkPortal(TWeakObjectPtr<APortal> LinkPortal);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UFUNCTION()
	void OnPortalBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPortalEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetPortalMaterial();

public:

	void ResetPortalMaterial();

	void SetCameraPosition();

	bool PortalA;

private:

	UPROPERTY()
	USceneComponent* Scene;

	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* PortalBorder;

	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* PortalBody;


	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_PortalBoderA;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_PortalBoderB;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_PortalBodyA;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_PortalBodyB;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_PortalBodyDefault;


	UPROPERTY(VisibleInstanceOnly)
	TWeakObjectPtr<class ATPSCharacter> Character;


	UPROPERTY(EditDefaultsOnly)
	class UTextureRenderTarget2D* RenderTargetA;

	UPROPERTY(EditDefaultsOnly)
	class UTextureRenderTarget2D* RenderTargetB;

	UPROPERTY(EditDefaultsOnly)
	class USceneCaptureComponent2D* SceneCapture;

	UPROPERTY(EditDefaultsOnly)
	class USoundCue* SC_PortalEnter;

public:

	UPROPERTY(VisibleInstanceOnly)
	TWeakObjectPtr<APortal> LinkedPortal;

	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* Arrow;

};