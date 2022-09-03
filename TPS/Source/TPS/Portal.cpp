// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PortalBorder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PORTAL BORDER"));
	PortalBorder->SetRelativeRotation(FRotator(90.f, 180.f, 0.f));
	RootComponent = PortalBorder;

	PortalBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PORTAL BODY"));
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
	
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

