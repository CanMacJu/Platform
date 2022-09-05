// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PortalBorder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PORTAL BORDER"));
	PortalBorder->SetRelativeRotation(FRotator(90.f, 180.f, 0.f));
	PortalBorder->SetCollisionProfileName(TEXT("PortalBorder"));
	RootComponent = PortalBorder;

	PortalBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PORTAL BODY"));
	PortalBody->SetCollisionProfileName(TEXT("Portal"));
	PortalBody->SetupAttachment(RootComponent);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SCENE CAPTURE"));
	SceneCapture->SetRelativeRotation(FRotator(90.f, 180.f, 0.f));
	SceneCapture->SetupAttachment(RootComponent);
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
	
	Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

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

}

void APortal::OnPortalBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("OnPortalBeginOverlap"));

	auto Pawn = Cast<ACharacter>(OtherActor);
	if (Pawn && LinkedPortal.IsValid())
	{
		Pawn->GetCapsuleComponent()->SetCollisionProfileName(TEXT("PortalPawn"));
	}
}

void APortal::OnPortalEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Error, TEXT("OnPortalEndOverlap"));

	auto Pawn = Cast<ACharacter>(OtherActor);
	if (Pawn && LinkedPortal.IsValid())
	{
		Pawn->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
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

