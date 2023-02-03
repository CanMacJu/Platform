// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

UCLASS(config=Game)
class ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FPSCamera;

public:
	ATPSCharacter();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	//void OnInGameMenu();

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SpawnPortalA();
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SpawnPortalB();


	// Laser
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
	

	// Sound
	UPROPERTY(EditDefaultsOnly)
		class USoundCue* SC_PortalA;
	UPROPERTY(EditDefaultsOnly)
		class USoundCue* SC_PortalB;



	// GrabActor
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void GrabActor();
	bool IsGrab = false;
	FRotator GrabRotator;
	FVector GrabLocation;
	TWeakObjectPtr<class AReflectionCube> GrabedActor;

	// CameraFOVLerp
	void InitLerpSetting();
	FTimerHandle CameraFOVLerpTimerHandle;
	FTimerDelegate CameraFOVLerpTimerDelegate;
	void SetCameraFOVTimer(float startFOV, float targetFOV);
	void LerpCameraFOV(float startFOV, float targetFOV);
	float FOVCurrentTime;
	// GrabLocationRotationLerp
	FTimerHandle GrabLocAndRotTimerHandle;
	FTimerDelegate GrabLocAndRotTimerDelegate;
	void SetGrabLocAndRotTimer(FHitResult hitResult, FVector startLoc, FVector endLoc, FRotator startRot, FRotator endRot);
	void LerpGrabLocAndRot(FHitResult hitResult, FVector startLoc, FVector endLoc, FRotator startRot, FRotator endRot);
	float GrabCurrentTime;

	float IntervalTime;
	float LerpTime;

	void SetGrabSetting(FHitResult hitResult);




	// Camera (ÇÏ³ª¸¸ ¾¸)
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SwitchActiveCamera();
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void ActiveFPSCamera();
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void ActiveTPSCamera();
	bool IsFPS;

	UPROPERTY(VisibleInstanceOnly)
	TWeakObjectPtr<class APortal> PortalA;

	UPROPERTY(VisibleInstanceOnly)
	TWeakObjectPtr<class APortal> PortalB;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TeleportBox;

	UPROPERTY()
	class UPhysicsHandleComponent* PhysicsHandle;

	enum class eHitType
	{
		NONE,
		PORTAL,
		MIRROR,
		TRIGGER,

		OTHER
	};


public:

	float DirectionForward;
	float DirectionRight;

	UPROPERTY(VisibleInstanceOnly)
	TWeakObjectPtr<class UPrimitiveComponent> GrabedComponent;

	UPROPERTY(VisibleInstanceOnly)
	TWeakObjectPtr<class ALaserTrigger> LaserTrigger;


};

