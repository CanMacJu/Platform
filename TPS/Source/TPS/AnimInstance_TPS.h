// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstance_TPS.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UAnimInstance_TPS : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UAnimInstance_TPS();

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void FootIK(float DeltaTime);

	TTuple<bool, float> CapsuleDistance(FName SocketName, ACharacter* Char);

	TTuple<bool, float, FVector> FootLineTrace(FName SocketName, ACharacter* Char);


private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "state", meta = (AllowPrivateAccess = "true"))
	bool IsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "state", meta = (AllowPrivateAccess = "true"))
	float Velocity;

	UPROPERTY()
	class ACharacter* Character;

	UPROPERTY()
	TArray<AActor*> IgnoreActors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
		float Displacement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
		float IKInterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
		FRotator RRot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
		FRotator LRot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
		float RIK;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
		float LIK;

	bool IsMove;
};
