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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool PortalA = true;

private:

	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* PortalBorder;

	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* PortalBody;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* PortalBoderA;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* PortalBoderB;

	UPROPERTY(VisibleInstanceOnly)
	TWeakObjectPtr<APortal> LinkedPortal;

};
