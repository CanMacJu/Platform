// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStartTrigger.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LobbyGameMode.h"

// Sets default values
AGameStartTrigger::AGameStartTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Trigger->SetBoxExtent(FVector(50.f, 50.f, 15.f));
	RootComponent = Trigger;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	Mesh->SetupAttachment(RootComponent);

}

void AGameStartTrigger::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AGameStartTrigger::OnBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AGameStartTrigger::OnEndOverlap);
}

// Called when the game starts or when spawned
void AGameStartTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGameStartTrigger::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (MI_TriggerOff)
	{
		Mesh->SetMaterial(0, MI_TriggerOff);
	}
}

void AGameStartTrigger::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlapActorNum++;

	if (MI_TriggerOn)
	{
		Mesh->SetMaterial(0, MI_TriggerOn);
	}

	auto LobbyGameMode = Cast<ALobbyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (LobbyGameMode == nullptr) return;

	LobbyGameMode->ReadyGame();
}

void AGameStartTrigger::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OverlapActorNum > 0)
	{
		OverlapActorNum--;
	}

	if (MI_TriggerOff && OverlapActorNum == 0)
	{
		Mesh->SetMaterial(0, MI_TriggerOff);
	}
	
}

// Called every frame
void AGameStartTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

