// Fill out your copyright notice in the Description page of Project Settings.


#include "MirrorCube.h"
#include "Portal.h"
#include "Components/ArrowComponent.h"

AMirrorCube::AMirrorCube()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = Mesh;
	Mesh->SetCollisionProfileName(FName(TEXT("PhysicsActor")));
	Mesh->SetSimulatePhysics(true);

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PORTALMESH"));
	PortalMesh->SetCollisionProfileName(FName(TEXT("NoCollision")));
	PortalMesh->SetupAttachment(RootComponent);
	PortalMesh->SetHiddenInGame(true);
	PortalMesh->SetGenerateOverlapEvents(false);
}

void AMirrorCube::SetVelocity(FVector velocity)
{
	Mesh->SetPhysicsLinearVelocity(velocity);
}

void AMirrorCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetPortalMesh();
}

void AMirrorCube::SetPortalMesh()
{
	if (Portal.IsValid())
	{
		FVector RootLocation = GetActorLocation();
		FVector RelativeLocation = Portal->Arrow->GetComponentTransform().InverseTransformPositionNoScale(RootLocation);
		FVector GlobalLocation = Portal->LinkedPortal->GetTransform().TransformPositionNoScale(RelativeLocation);

		FQuat RootQuat = FQuat(GetActorRotation());
		FQuat RelativeQuat = Portal->Arrow->GetComponentTransform().InverseTransformRotation(RootQuat);
		FQuat GlobalQuat = Portal->LinkedPortal->GetTransform().TransformRotation(RelativeQuat);
		FRotator GlobalRotator = GlobalQuat.Rotator();

		PortalMesh->SetWorldLocationAndRotation(GlobalLocation, GlobalRotator);
	}
}

void AMirrorCube::SetPortal(APortal* portal)
{
	Portal = portal;
	PortalMesh->SetHiddenInGame(false);
}

void AMirrorCube::ResetPortal()
{
	Portal.Reset();
	PortalMesh->SetHiddenInGame(true);
}
