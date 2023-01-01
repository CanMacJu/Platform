// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePiece.h"
#include "PaperSpriteComponent.h"
#include "PuzzleBoard.h"
#include "PaperSprite.h"
#include "Rendering/Texture2DResource.h"

APuzzlePiece::APuzzlePiece()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("SCENE"));
	RootComponent = Scene;
	Scene->SetMobility(EComponentMobility::Movable);

	GetRenderComponent()->SetRelativeRotation(FRotator(0.f, -90.f, -90.f));
	GetRenderComponent()->SetupAttachment(RootComponent);
	GetRenderComponent()->SetMobility(EComponentMobility::Movable);
	GetRenderComponent()->SetCollisionProfileName(TEXT("PuzzlePiece"));
}

void APuzzlePiece::Init(APuzzleBoard* Board, int32 Index, UPaperSprite* Sprite)
{
	PuzzleBoard = Board;
	CorrectIndex = Index;
	CurrentIndex = Index;

	GetRenderComponent()->SetSprite(Sprite);
}

bool APuzzlePiece::CheckIndex()
{
	return CorrectIndex == CurrentIndex;
}