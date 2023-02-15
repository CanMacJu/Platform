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
	
	virtual void Tick(float DeltaTime) override;

	void SetPortalMesh();
	void SetPortal(class APortal* portal);
	void ResetPortal();

private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PortalMesh;

	TWeakObjectPtr<class APortal> Portal;
};
