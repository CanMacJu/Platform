// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePawn.h"
#include "Camera/CameraComponent.h"
#include "PuzzlePiece.h"
#include "PuzzleBoard.h"

// Sets default values
APuzzlePawn::APuzzlePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("SCENE"));
	RootComponent = Scene;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	Camera->SetupAttachment(RootComponent);
	Camera->SetWorldRotation(FRotator(-90.f, 0.f, 0.f));
	Camera->SetProjectionMode(ECameraProjectionMode::Orthographic);
	Camera->OrthoWidth = 1100;
}

// Called when the game starts or when spawned
void APuzzlePawn::BeginPlay()
{
	Super::BeginPlay();

	Controller = GetWorld()->GetFirstPlayerController();
	Controller->bShowMouseCursor = true;
}

// Called every frame
void APuzzlePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APuzzlePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


}

void APuzzlePawn::Init(APuzzleBoard* Board)
{
	PuzzleBoard = Board;
}

void APuzzlePawn::SelectPiece()
{
	if (PuzzleBoard->CanSelect() == false)
		return;

	FHitResult HitResult;
	bool Result = Controller->GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel8, false, HitResult);

	if (Result == false)
		return;

	APuzzlePiece* HitPiece = Cast<APuzzlePiece>(HitResult.Actor);
	if (HitPiece == nullptr)
		return;

	int32 HitPieceIndex = HitPiece->CurrentIndex;
	if (PuzzleBoard->CanMove(HitPieceIndex))
	{
		PuzzleBoard->SelectPiece(HitPieceIndex);
	}
}

