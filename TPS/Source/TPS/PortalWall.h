// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalWall.generated.h"

UCLASS()
class TPS_API APortalWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalWall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	FTransform ClampPortalPosition(FVector Location);


	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Width;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Height;

	
};
