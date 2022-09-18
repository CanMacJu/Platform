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
	TeleportBox->SetBoxExtent(FVector(1.f, 1.f, 1.f));
	TeleportBox->SetCollisionProfileName("OverlapAll");
	TeleportBox->SetupAttachment(RootComponent);

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("Handle"));
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
		PhysicsHandle->SetTargetLocationAndRotation(GrabLocation, GrabRotator);
	}
	
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

void ATPSCharacter::GrabActor()
{
	if (IsGrab == false)
	{
		FHitResult HitResult;
		FVector Start = FPSCamera->GetComponentLocation();
		FVector End = Start + FPSCamera->GetForwardVector() * 200.f;
		FCollisionQueryParams QueryParam = FCollisionQueryParams(NAME_None, false, this);
		bool Result = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_GameTraceChannel1, QueryParam);

		if (Result)
		{
			GrabLocation = FPSCamera->GetComponentLocation() + FPSCamera->GetForwardVector() * 130.f;
			GrabRotator = FRotator(0.f, GetActorRotation().Yaw, 0.f);
			PhysicsHandle->GrabComponentAtLocationWithRotation(HitResult.GetComponent(), NAME_None, GrabLocation, GrabRotator);
			IsGrab = true;
			// 테스트
			// 테스트2
			// 테스트3
			// 테스트4
		}
	}
	else
	{
		PhysicsHandle->ReleaseComponent();
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
