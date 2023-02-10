// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "Sound/SoundCue.h"

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

	FVector2D viewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(viewportSize);
	SceneCapture->TextureTarget->ResizeTarget(viewportSize.X * rederQuality, viewportSize.Y * rederQuality);
}

void APortal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (LinkedPortal.IsValid())
		LinkedPortal->ResetPortalMaterial();
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
			OtherComp->SetCollisionProfileName(TEXT("PortalPawn"));
	}
	else
	{
		OverlapedActors.AddUnique(OtherActor);

		// 캐릭터 말고 다른 것

	}

	
	
}

void APortal::OnPortalEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlapedCharacter == OtherActor)
	{
		bool bIsCapsuleComponent = FName(TEXT("PortalPawn")) == OtherComp->GetCollisionProfileName();
		if (bIsCapsuleComponent)
			OtherComp->SetCollisionProfileName(TEXT("Pawn"));

		OverlapedCharacter.Reset();
	}
	else
	{
		// 캐릭터 말고 다른 것

		OverlapedActors.Remove(OtherActor);
	}
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

		LinkedPortal->SceneCapture->CustomNearClippingPlane = FVector::Dist(GetActorLocation(), Character->FPSCamera->GetComponentLocation());
	}
}

void APortal::CheckPlayerTeleport()
{
	if (OverlapedCharacter.IsValid())
	{
		FVector velocity = OverlapedCharacter->GetCharacterMovement()->Velocity;
		FVector nextTickLocation = velocity * GetWorld()->GetDeltaSeconds() + OverlapedCharacter->GetActorLocation();
		FVector portalLocation = GetActorLocation();

		float DotOfPortalAndVelocity = FVector::DotProduct(velocity, GetActorForwardVector());
		float DotOfPortalAndNextTickLocation = FVector::DotProduct(GetActorForwardVector(), nextTickLocation - portalLocation);
		if (DotOfPortalAndVelocity < 0 && DotOfPortalAndNextTickLocation < 0)
		{
			FVector RelativeVelocity = Arrow->GetComponentTransform().InverseTransformVector(velocity);
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

