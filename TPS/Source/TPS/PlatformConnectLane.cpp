// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformConnectLane.h"
#include "Engine/Texture2D.h"
#include "Components/DecalComponent.h"
#include "Components/ArrowComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetMaterialLibrary.h"

// Sets default values
APlatformConnectLane::APlatformConnectLane()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("SCENE"));
	RootComponent = Scene;

	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("DECAL"));
	Decal->SetupAttachment(RootComponent);
	Decal->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ARROW"));
	Arrow->SetupAttachment(RootComponent);
	Arrow->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
}

// Called when the game starts or when spawned
void APlatformConnectLane::BeginPlay()
{
	Super::BeginPlay();
	
	MID_Texture = Decal->CreateDynamicMaterialInstance();
}

void APlatformConnectLane::SetTextureOn()
{
	if (MID_Texture && Texture_On)
		MID_Texture->SetTextureParameterValue(FName(TEXT("LaneTexture")), Texture_On);
}

void APlatformConnectLane::SetTextureOff()
{
	if (MID_Texture && Texture_Off)
		MID_Texture->SetTextureParameterValue(FName(TEXT("LaneTexture")), Texture_Off);
}

void APlatformConnectLane::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (MI_Decal)
		Decal->SetDecalMaterial(MI_Decal);
}

