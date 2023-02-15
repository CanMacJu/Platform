// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabableActor.h"
#include "MirrorCube.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API AMirrorCube : public AGrabableActor
{
	GENERATED_BODY()
	
public:

	AMirrorCube();

	virtual void SetVelocity(FVector velocity) override;
	
private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
};
