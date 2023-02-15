// Fill out your copyright notice in the Description page of Project Settings.


#include "MirrorCube.h"

AMirrorCube::AMirrorCube()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = Mesh;
	Mesh->SetCollisionProfileName(FName(TEXT("PhysicsActor")));
	Mesh->SetSimulatePhysics(true);
}

void AMirrorCube::SetVelocity(FVector velocity)
{
	Mesh->SetPhysicsLinearVelocity(velocity);
}
