// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "Sound/SoundCue.h"
#include "GrabableActor.h"
#include "MirrorCube.h"
#include "PortalGameInstance.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("SCENE"));
	RootComponent = Scene;

	PortalBorder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PORTAL BORDER"));
	PortalBorder->SetRelativeRotation(FRotator(90.f, 180.f, 0.f));
	PortalBorder->SetCollisionProfileName(TEXT("PortalBorder"));
	PortalBorder->SetupAttachment(RootComponent);

	PortalBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PORTAL BODY"));
	PortalBody->SetCollisionProfileName(TEXT("Portal"));
	PortalBody->SetupAttachment(PortalBorder);

	PortalPlane = CreateDefaultSubobject<UBoxComponent>(TEXT("PORTAL PLANE"));
	PortalPlane->SetBoxExtent(FVector(1.f, 90.f, 124.5f));
	PortalPlane->SetCollisionProfileName(TEXT("PortalPlane"));
	PortalPlane->SetupAttachment(RootComponent);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SCENE CAPTURE"));
	SceneCapture->SetupAttachment(RootComponent);
	SceneCapture->bOverride_CustomNearClippingPlane = true;

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ARROW"));
	Arrow->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	Arrow->SetupAttachment(RootComponent);
}

void APortal::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (PortalA)
	{
		if (MI_PortalBoderA && RenderTargetB)
		{
			PortalBorder->SetMaterial(0, MI_PortalBoderA);
			SceneCapture->TextureTarget = RenderTargetB;
		}
	}
	else
	{
		if (MI_PortalBoderB && RenderTargetA)
		{
			PortalBorder->SetMaterial(0, MI_PortalBoderB);
			SceneCapture->TextureTarget = RenderTargetA;
		}
	}
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
	
	Character = Cast<ATPSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	PortalBody->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnPortalBeginOverlap);
	PortalBody->OnComponentEndOverlap.AddDynamic(this, &APortal::OnPortalEndOverlap);

	PortalPlane->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnPlaneBeginOverlap);
	PortalPlane->OnComponentEndOverlap.AddDynamic(this, &APortal::OnPlaneEndOverlap);

	GameInstance = Cast<UPortalGameInstance>(GetGameInstance());
	if (GameInstance)
		PortalQualityDelegateHandle = GameInstance->OnChangePortalQualityDelegate.AddUObject(this, &APortal::OnChangePortalQulity);

	if (GameInstance)
		OnChangePortalQulity(GameInstance->GetPortalQuality());
	else
		OnChangePortalQulity(0.4f);
}

void APortal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (LinkedPortal.IsValid())
		LinkedPortal->ResetPortalMaterial();

	GameInstance->OnChangePortalQualityDelegate.Remove(PortalQualityDelegateHandle);
}

void APortal::LinkPortal(TWeakObjectPtr<APortal> LinkPortal)
{
	if (!LinkedPortal.IsValid() && LinkPortal.IsValid())
	{
		LinkedPortal = LinkPortal;
		LinkPortal->LinkPortal(this);
	}

	SetPortalMaterial();
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckPlayerTeleport();
	CheckActorTeleport();

	SetCameraPosition();
}

void APortal::OnPortalBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (LinkedPortal.IsValid() == false)
		return;

	ATPSCharacter* Pawn = Cast<ATPSCharacter>(OtherActor);
	if (Pawn)
	{
		OverlapedCharacter = Pawn;

		bool bIsCapsuleComponent = FName(TEXT("Pawn")) == OtherComp->GetCollisionProfileName();
		if (bIsCapsuleComponent)
			OtherComp->SetCollisionProfileName(FName(TEXT("PortalPawn")));
	}
	else
	{
		AGrabableActor* GrabableActor = Cast<AGrabableActor>(OtherActor);
		if (GrabableActor)
		{
			OverlapedActors.AddUnique(GrabableActor);
			bool bIsPhysicsActor = FName(TEXT("PhysicsActor")) == OtherComp->GetCollisionProfileName();
			if (bIsPhysicsActor)
				OtherComp->SetCollisionProfileName(FName(TEXT("PortalPhysicsActor")));
		}
	}
	
	
}

void APortal::OnPortalEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlapedCharacter == OtherActor)
	{
		bool bIsCapsuleComponent = FName(TEXT("PortalPawn")) == OtherComp->GetCollisionProfileName();
		if (bIsCapsuleComponent)
			OtherComp->SetCollisionProfileName(FName(TEXT("Pawn")));

		OverlapedCharacter.Reset();
	}
	else
	{
		AGrabableActor* GrabableActor = Cast<AGrabableActor>(OtherActor);
		if (OverlapedActors.Remove(GrabableActor) != 0)
			OtherComp->SetCollisionProfileName(FName(TEXT("PhysicsActor")));
	}
}

void APortal::OnPlaneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (LinkedPortal.IsValid() == false)
		return;

	AMirrorCube* MirrorCube = Cast<AMirrorCube>(OtherActor);
	if (MirrorCube)
		MirrorCube->SetPortal(this);
}

void APortal::OnPlaneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMirrorCube* MirrorCube = Cast<AMirrorCube>(OtherActor);
	if (MirrorCube)
		MirrorCube->ResetPortal();
}

void APortal::OnChangePortalQulity(float quality)
{
	FVector2D viewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(viewportSize);
	SceneCapture->TextureTarget->ResizeTarget(viewportSize.X * quality, viewportSize.Y * quality);
}

void APortal::SetPortalMaterial()
{
	if (LinkedPortal.IsValid())
	{
		if (PortalA && MI_PortalBodyA)
			PortalBody->SetMaterial(0, MI_PortalBodyA);
		else if (MI_PortalBodyB)
			PortalBody->SetMaterial(0, MI_PortalBodyB);
	}
	else if (MI_PortalBodyDefault)
		PortalBody->SetMaterial(0, MI_PortalBodyDefault);
}

void APortal::ResetPortalMaterial()
{
	if (MI_PortalBodyDefault)
		PortalBody->SetMaterial(0, MI_PortalBodyDefault);
}

void APortal::SetCameraPosition()
{
	if (LinkedPortal.IsValid())
	{
		FVector RelativeCameraLocation = Arrow->GetComponentTransform().InverseTransformPositionNoScale(Character->FPSCamera->GetComponentLocation());
		LinkedPortal->SceneCapture->SetRelativeLocation(RelativeCameraLocation);

		FRotator CameraRotator = Character->FPSCamera->GetComponentRotation();
		FQuat Quat = FQuat(CameraRotator);
		FRotator RelativeCameraRotator = Arrow->GetComponentTransform().InverseTransformRotation(Quat).Rotator();
		LinkedPortal->SceneCapture->SetRelativeRotation(RelativeCameraRotator);

		LinkedPortal->SceneCapture->CustomNearClippingPlane = FVector::Dist(GetActorLocation(), Character->FPSCamera->GetComponentLocation()) - 20.f;
	}
}

void APortal::CheckPlayerTeleport()
{
	if (OverlapedCharacter.IsValid())
	{
		FVector velocity = OverlapedCharacter->GetCharacterMovement()->Velocity;
		FVector nextTickLocation = velocity * GetWorld()->GetDeltaSeconds() + OverlapedCharacter->GetActorLocation();
		FVector portalLocation = GetActorLocation();

		float DotOfPortalAndNextTickLocation = FVector::DotProduct(GetActorForwardVector(), nextTickLocation - portalLocation);
		if (DotOfPortalAndNextTickLocation < 0)
		{
			FVector RelativeVelocity = Arrow->GetComponentTransform().InverseTransformVector(velocity);
			if (RelativeVelocity.Size() < 300)
			{
				RelativeVelocity = RelativeVelocity.GetSafeNormal();
				RelativeVelocity *= 500.f;
			}
			OverlapedCharacter->GetMovementComponent()->Velocity = LinkedPortal->GetTransform().TransformVector(RelativeVelocity);

			FRotator Rot = LinkedPortal->GetActorRotation() - GetActorRotation();
			OverlapedCharacter->AddControllerYawInput((Rot.Yaw + 180.f) * 0.4f);

			FVector relativeTPLocation = Arrow->GetComponentTransform().InverseTransformPositionNoScale(nextTickLocation);
			FVector TPLocation = LinkedPortal->GetTransform().TransformPositionNoScale(relativeTPLocation);
			OverlapedCharacter->SetActorLocation(TPLocation);

			if (SC_PortalEnter)
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), SC_PortalEnter, LinkedPortal->GetActorLocation());
		}
	}
}

void APortal::CheckActorTeleport()
{
	for (AGrabableActor* OverlapedActor : OverlapedActors)
	{
		FVector velocity = OverlapedActor->GetVelocity();
		FVector nextTickLocation = velocity * GetWorld()->GetDeltaSeconds() + OverlapedActor->GetActorLocation();
		FVector portalLocation = GetActorLocation();

		float DotOfPortalAndNextTickLocation = FVector::DotProduct(GetActorForwardVector(), nextTickLocation - portalLocation);
		if (DotOfPortalAndNextTickLocation < 0)
		{
			FVector RelativeVelocity = Arrow->GetComponentTransform().InverseTransformVectorNoScale(velocity);
			velocity = LinkedPortal->GetTransform().TransformVectorNoScale(RelativeVelocity);
			if (velocity.Size() < 300)
			{
				velocity = velocity.GetSafeNormal();
				velocity *= 300.f;
			}
			OverlapedActor->SetVelocity(velocity);

			FRotator OverlapedActorRot = OverlapedActor->GetActorRotation();
			FQuat Quat = FQuat(OverlapedActorRot);
			FQuat RelativeQuat = Arrow->GetComponentTransform().InverseTransformRotation(Quat);
			FRotator Rotator = LinkedPortal->GetTransform().TransformRotation(RelativeQuat).Rotator();
			OverlapedActor->SetActorRelativeRotation(Rotator);

			FVector relativeTPLocation = Arrow->GetComponentTransform().InverseTransformPositionNoScale(nextTickLocation);
			FVector TPLocation = LinkedPortal->GetTransform().TransformPositionNoScale(relativeTPLocation);
			OverlapedActor->SetActorLocation(TPLocation);

			return;
		}
	}
}

