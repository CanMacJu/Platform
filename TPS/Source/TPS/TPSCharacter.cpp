// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/InGameMenu.h"
#include "Portal.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "PortalWall.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneCaptureComponent2D.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PlatformTrigger.h"

//////////////////////////////////////////////////////////////////////////
// ATPSCharacter

ATPSCharacter::ATPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(32.f, 88.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	// set our turn rates for input
	BaseTurnRate = 1.f;
	BaseLookUpRate = 1.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, -1.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 1.f;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxAcceleration = 10000.f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->FallingLateralFriction = 1000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	FPSCamera->SetupAttachment(RootComponent);
	FPSCamera->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
	FPSCamera->bUsePawnControlRotation = true;
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	TeleportBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BOX"));
	TeleportBox->SetBoxExtent(FVector(1.5f, 1.5f, 60.f));
	TeleportBox->SetCollisionProfileName("OverlapAll");
	TeleportBox->SetupAttachment(RootComponent);

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Handle"));

	// Laser
	ReflectionCount = 5;
}

void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();


	ActiveFPSCamera();
}

void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsGrab)
	{
		GrabRotator = FRotator(0.f, GetActorRotation().Yaw, 0.f);
		GrabLocation = FPSCamera->GetComponentLocation() + FPSCamera->GetForwardVector() * 130.f;
		PhysicsHandle->SetTargetLocationAndRotation(GrabLocation, GrabRotator);
	}

	// Laser
	ResetLaser();

	FVector Start = FPSCamera->GetComponentLocation();
	FVector Direction = FPSCamera->GetForwardVector();
	Laser(Start, Direction * 10000, ReflectionCount);
	DrawLaser();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATPSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATPSCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATPSCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATPSCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATPSCharacter::OnResetVR);

}

void ATPSCharacter::SpawnPortalA()
{
	auto World = GetWorld();
	if (World == nullptr) return;

	if (IsFPS)
	{
		FHitResult HitResult;
		FVector Start = FPSCamera->GetComponentLocation();
		FVector End = Start + FPSCamera->GetForwardVector() * 10000.f;
		FCollisionQueryParams QueryParam = FCollisionQueryParams(NAME_None, false, this);

		bool Result = World->LineTraceSingleByChannel(OUT HitResult,
			Start,
			End,
			ECollisionChannel::ECC_GameTraceChannel1,
			QueryParam);

		FColor Color = Result ? FColor::Green : FColor::Red;
		DrawDebugLine(World, Start, End, Color, false, 3.f);

		if (Result && HitResult.IsValidBlockingHit())
		{
			APortalWall* PortalWall = Cast<APortalWall>(HitResult.GetActor());
			if (PortalWall == nullptr) return;

			if (PortalA.IsValid()) PortalA->Destroy();

			FTransform ClampTransform = PortalWall->ClampPortalPosition(HitResult.Location);

			FName Path = TEXT("Class'/Game/Portal/BP_Portal.BP_Portal_C'");
			UClass* BP_PortalClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *Path.ToString()));

			APortal* Portal = World->SpawnActorDeferred<APortal>(BP_PortalClass, ClampTransform);
			if (Portal)
			{
				PortalA = Portal;
				Portal->PortalA = true;
				Portal->LinkPortal(PortalB);
				Portal->FinishSpawning(ClampTransform);
			}
		}
	}
}

void ATPSCharacter::SpawnPortalB()
{
	auto World = GetWorld();
	if (World == nullptr) return;

	if (IsFPS)
	{
		FHitResult HitResult;
		FVector Start = FPSCamera->GetComponentLocation();
		FVector End = Start + FPSCamera->GetForwardVector() * 10000.f;
		FCollisionQueryParams QueryParam = FCollisionQueryParams(NAME_None, false, this);

		bool Result = World->LineTraceSingleByChannel(OUT HitResult,
			Start,
			End,
			ECollisionChannel::ECC_GameTraceChannel1,
			QueryParam);

		FColor Color = Result ? FColor::Green : FColor::Red;
		DrawDebugLine(World, Start, End, Color, false, 3.f);

		if (Result && HitResult.IsValidBlockingHit())
		{
			APortalWall* PortalWall = Cast<APortalWall>(HitResult.GetActor());
			if (PortalWall == nullptr) return;

			if (PortalB.IsValid()) PortalB->Destroy();

			FTransform ClampTransform = PortalWall->ClampPortalPosition(HitResult.Location);

			FName Path = TEXT("Class'/Game/Portal/BP_Portal.BP_Portal_C'");
			UClass* BP_PortalClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *Path.ToString()));

			APortal* Portal = World->SpawnActorDeferred<APortal>(BP_PortalClass, ClampTransform);
			if (Portal)
			{
				PortalB = Portal;
				Portal->PortalA = false;
				Portal->LinkPortal(PortalA);
				Portal->FinishSpawning(ClampTransform);
			}
		}
	}
}

void ATPSCharacter::Laser(FVector Start, FVector Direction, int32 _ReflectionCount)
{
	if (MI_Mirror == nullptr) return;

	if (P_Laser == nullptr) return;

	auto World = GetWorld();
	if (World == nullptr) return;

	FHitResult HitResult;
	FCollisionQueryParams QueryParam = FCollisionQueryParams(NAME_None, true, this);
	bool Result = World->LineTraceSingleByChannel(HitResult, Start, Start + Direction, ECollisionChannel::ECC_GameTraceChannel7, QueryParam);

	/*FColor Color = Result ? FColor::Green : FColor::Red;
	DrawDebugLine(World, Start, Start + Direction, Color);*/

	if (Result)
	{
		LaserParticles.Add(UGameplayStatics::SpawnEmitterAttached(P_Laser, FPSCamera));
		SourcePoints.Add(Start);
		EndPoints.Add(HitResult.ImpactPoint);

		auto PlatformTrigger = Cast<APlatformTrigger>(HitResult.GetActor());
		if (PlatformTrigger && PlatformTrigger->IsLaserTrigger && !LaserTrigger.IsValid())
		{
			LaserTrigger = PlatformTrigger;
			LaserTrigger->LaserTriggerOn();
		}
		else if (!PlatformTrigger && LaserTrigger.IsValid())
		{
			LaserTrigger->LaserTriggerOff();
			LaserTrigger.Reset();
		}

		if (!PlatformTrigger)
		{
			auto Portal = Cast<APortal>(HitResult.GetActor());
			if (Portal && Portal->LinkedPortal.IsValid())
			{
				FVector RelativeStartPoint = Portal->Arrow->GetComponentTransform().InverseTransformPosition(HitResult.ImpactPoint);
				Start = Portal->LinkedPortal->GetTransform().TransformPosition(RelativeStartPoint);

				FVector RelativeDirection = Portal->Arrow->GetComponentTransform().InverseTransformVector(Direction);
				Direction = Portal->LinkedPortal->GetTransform().TransformVector(RelativeDirection);

				Laser(Start, Direction, _ReflectionCount);
				return;
			}
		}

		if (_ReflectionCount == 0) return;

		if (HitResult.GetComponent()->GetMaterial(0) == MI_Mirror)
		{
			FVector ImpactNormal = HitResult.ImpactNormal;

			Start = HitResult.ImpactPoint;
			Direction = 2 * ImpactNormal * FVector::DotProduct(ImpactNormal, -1.f * Direction) + Direction;

			_ReflectionCount--;
			Laser(Start, Direction, _ReflectionCount);
		}
	}
	else
	{
		LaserParticles.Add(UGameplayStatics::SpawnEmitterAttached(P_Laser, FPSCamera));
		SourcePoints.Add(Start);
		EndPoints.Add(Start + Direction);
	}
}

void ATPSCharacter::DrawLaser()
{
	if ((LaserParticles.Num() == SourcePoints.Num() && SourcePoints.Num() == EndPoints.Num()) == false) return;

	for (int32 i = 0; i < LaserParticles.Num(); ++i)
	{
		LaserParticles[i]->SetBeamSourcePoint(0, SourcePoints[i], 0);
		LaserParticles[i]->SetBeamEndPoint(0, EndPoints[i]);
	}
}

void ATPSCharacter::ResetLaser()
{
	for (auto LaserParticle : LaserParticles)
	{
		LaserParticle->DestroyComponent();
	}
	LaserParticles.Empty();
	SourcePoints.Empty();
	EndPoints.Empty();
}

void ATPSCharacter::GrabActor()
{
	if (IsGrab == false)
	{
		FHitResult HitResult;
		FVector Start = FPSCamera->GetComponentLocation();
		FVector End = Start + FPSCamera->GetForwardVector() * 200.f;
		FCollisionQueryParams QueryParam = FCollisionQueryParams(NAME_None, false, this);
		bool Result = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_GameTraceChannel6, QueryParam);

		if (Result)
		{
			GrabLocation = FPSCamera->GetComponentLocation() + FPSCamera->GetForwardVector() * 130.f;
			GrabRotator = FRotator(0.f, GetActorRotation().Yaw, 0.f);
			GrabedComponent = HitResult.GetComponent();
			GrabedComponent->SetCollisionProfileName("GrabedActor");
			PhysicsHandle->GrabComponentAtLocationWithRotation(HitResult.GetComponent(), NAME_None, GrabLocation, GrabRotator);

			IsGrab = true;
		}
	}
	else
	{
		PhysicsHandle->ReleaseComponent();
		GrabedComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
		GrabedComponent.Reset();
		IsGrab = false;
	}
}

void ATPSCharacter::SwitchActiveCamera()
{
	IsFPS ? ActiveTPSCamera() : ActiveFPSCamera();
}

void ATPSCharacter::ActiveFPSCamera()
{
	GetMesh()->SetOwnerNoSee(true);
	FPSCamera->SetActive(true);
	FollowCamera->SetActive(false);
	this->bUseControllerRotationYaw = true;
	IsFPS = true;
}

void ATPSCharacter::ActiveTPSCamera()
{
	GetMesh()->SetOwnerNoSee(false);
	FollowCamera->SetActive(true);
	FPSCamera->SetActive(false);
	this->bUseControllerRotationYaw = false;
	IsFPS = false;
}

void ATPSCharacter::SwitchMouseCursor()
{
	IsShowMouse ? InActiveMouseCursor() : ActiveMouseCursor();
}

void ATPSCharacter::ActiveMouseCursor()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr)
		return;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputModeData);

	PlayerController->bShowMouseCursor = true;
	IsShowMouse = true;
}

void ATPSCharacter::InActiveMouseCursor()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr)
		return;

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);

	PlayerController->bShowMouseCursor = false;
	IsShowMouse = false;
}

//void ATPSCharacter::SetTeleportDelay()
//{
//	Teleportable = false;
//	GetWorldTimerManager().SetTimer(TeleportDelayTimerHandle, this, &ATPSCharacter::SetTeleportable, 0.1f, false);
//}
//
//void ATPSCharacter::SetTeleportable()
//{
//	UE_LOG(LogTemp, Error, TEXT("Timeline"));
//	Teleportable = true;
//	GetWorldTimerManager().ClearTimer(TeleportDelayTimerHandle);
//}

void ATPSCharacter::OnResetVR()
{
	// If TPS is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in TPS.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ATPSCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ATPSCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ATPSCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATPSCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATPSCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}

	DirectionForward = FMath::Clamp(Value, -1.f, 1.f);
}

void ATPSCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}

	DirectionRight = FMath::Clamp(Value, -1.f, 1.f);
}
