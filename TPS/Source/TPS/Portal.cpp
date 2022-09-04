// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "TPSCharacter.h"
#include "Components/CapsuleComponent.h"

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
}

void APortal::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (PortalA)
	{
		PortalBorder->SetMaterial(0, PortalBoderA);
	}
	else
	{
		PortalBorder->SetMaterial(0, PortalBoderB);
	}
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
	
	PortalBody->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnPortalBeginOverlap);
	PortalBody->OnComponentEndOverlap.AddDynamic(this, &APortal::OnPortalEndOverlap);
}

void APortal::LinkPortal(TWeakObjectPtr<APortal> LinkPortal)
{
	if (!LinkedPortal.IsValid() && LinkPortal.IsValid())
	{
		LinkedPortal = LinkPortal;
		LinkPortal->LinkPortal(this);
	}
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortal::OnPortalBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("OnPortalBeginOverlap"));

	auto Pawn = Cast<ATPSCharacter>(OtherActor);
	if (Pawn)
	{
		Pawn->GetCapsuleComponent()->SetCollisionProfileName(TEXT("PortalPawn"));
	}
}

void APortal::OnPortalEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Error, TEXT("OnPortalEndOverlap"));

	auto Pawn = Cast<ATPSCharacter>(OtherActor);
	if (Pawn)
	{
		Pawn->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	}
}

