#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReflectionCube.generated.h"

UCLASS()
class TPS_API AReflectionCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReflectionCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CoreOn();
	void CoreOff();

	bool GetIsCoreOn() const { return bIsCoreOn; }

private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CubeMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Glass;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Core;

	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* Arrow;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_Core_On;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* MI_Core_Off;

	bool bIsCoreOn;
};
