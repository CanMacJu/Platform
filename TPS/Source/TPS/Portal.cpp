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
}

void APortal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (LinkedPortal.IsValid())
	{
		LinkedPortal->ResetPortalMaterial();
	}
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

	SetCameraPosition();

	FVector2D CurrentViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(CurrentViewportSize);

	if (PortalA)
	{
		if (RenderTargetB)
		{
			RenderTargetB->ResizeTarget(CurrentViewportSize.X, CurrentViewportSize.Y);
		}
	}
	else
	{
		if (RenderTargetA)
		{
			RenderTargetA->ResizeTarget(CurrentViewportSize.X, CurrentViewportSize.Y);
		}
	}
}

void APortal::OnPortalBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (LinkedPortal.IsValid() == false)
		return;

	auto Pawn = Cast<ATPSCharacter>(OtherActor);
	if (Pawn)
	{
		bool CapsuleComponent = TEXT("Pawn") == OtherComp->GetCollisionProfileName();
		if (CapsuleComponent)
		{
			Pawn->GetCapsuleComponent()->SetCollisionProfileName(TEXT("PortalPawn"));
		}
		else
		{
			FRotator Rotator = LinkedPortal->GetActorRotation() - GetActorRotation();
			Pawn->AddControllerYawInput((180.f + Rotator.Yaw) * 0.4f);


			/*FRotator VelocityRotator = FRotator(0, Rotator.Yaw + 180.f, 0);
			FVector OriginVelocity = Pawn->GetMovementComponent()->Velocity;
			Pawn->GetMovementComponent()->Velocity = VelocityRotator.RotateVector(OriginVelocity);*/

			FVector RelativeVelocity = Arrow->GetComponentTransform().InverseTransformVector(Pawn->GetMovementComponent()->Velocity);
			Pawn->GetMovementComponent()->Velocity = LinkedPortal->GetTransform().TransformVector(RelativeVelocity);

			float Ydiff = GetTransform().InverseTransformPositionNoScale(Pawn->GetActorLocation()).Y;
			Pawn->SetActorLocation(LinkedPortal->GetActorLocation() + LinkedPortal->GetActorForwardVector() * 7 + LinkedPortal->GetActorRightVector() * Ydiff * -1.f);
		}
	}
}

void APortal::OnPortalEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto Pawn = Cast<ATPSCharacter>(OtherActor);
	if (Pawn && LinkedPortal.IsValid())
	{
		bool CapsuleComponent = TEXT("PortalPawn") == OtherComp->GetCollisionProfileName();
		if (CapsuleComponent)
		{
			Pawn->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
		}
	}
}

void APortal::SetPortalMaterial()
{
	if (LinkedPortal.IsValid())
	{
		if (PortalA && MI_PortalBodyA)
		{
			PortalBody->SetMaterial(0, MI_PortalBodyA);
		}
		else if (MI_PortalBodyB)
		{
			PortalBody->SetMaterial(0, MI_PortalBodyB);
		}
	}
	else if (MI_PortalBodyDefault)
	{
		PortalBody->SetMaterial(0, MI_PortalBodyDefault);
	}
}

void APortal::ResetPortalMaterial()
{
	if (MI_PortalBodyDefault)
	{
		PortalBody->SetMaterial(0, MI_PortalBodyDefault);
	}
}

void APortal::SetCameraPosition()
{
	if (LinkedPortal.IsValid())
	{
		FVector LocalPosition = GetTransform().InverseTransformPosition(Character->FPSCamera->GetComponentLocation());

		FVector Position = FVector(-LocalPosition.X, -LocalPosition.Y, LocalPosition.Z);
		LinkedPortal->SceneCapture->SetRelativeLocation(Position);


		FRotator CameraRotator = Character->FPSCamera->GetComponentRotation();

		FQuat Quat = FQuat(CameraRotator);

		FQuat A = GetTransform().InverseTransformRotation(Quat);

		FRotator Rotator = FRotator(A.Rotator().Pitch, A.Rotator().Yaw - 180.f, 0.f);

		LinkedPortal->SceneCapture->SetRelativeRotation(Rotator);

		LinkedPortal->SceneCapture->CustomNearClippingPlane = FVector::Dist(GetActorLocation(), Character->FPSCamera->GetComponentLocation());
	}
}

