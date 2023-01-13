// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleGameMode.h"
#include "PaperTileSet.h"
#include "PuzzleBoard.h"
#include "PuzzlePawn.h"
#include "PuzzlePiece.h"


APuzzleGameMode::APuzzleGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PuzzlePawnBPClass(TEXT("/Game/PuzzleTest/BP_PuzzlePawn"));
	if (PuzzlePawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PuzzlePawnBPClass.Class;
	}
}

void APuzzleGameMode::StartPlay()
{
	Super::StartPlay();

	auto World = GetWorld();
	if (World == nullptr)
		return;

	const int32 Size = 5;

	FName Path = TEXT("Class'/Game/PuzzleTest/BP_PuzzleBoard.BP_PuzzleBoard_C'");
	UClass* BP_PuzzleBoardClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *Path.ToString()));

	APuzzleBoard* Board = World->SpawnActorDeferred<APuzzleBoard>(BP_PuzzleBoardClass, FTransform());
	if (Board)
	{
		Board->SetSpawn(Size, 900.f, true);
		Board->FinishSpawning(FTransform());
	}

	APuzzlePawn* Player = Cast<APuzzlePawn>(World->GetFirstPlayerController()->GetPawn());
	if (Player == nullptr)
		return;

	if (Board == nullptr)
		return;

	Player->Init(Board);

	Path = TEXT("Class'/Game/PuzzleTest/BP_PuzzlePiece.BP_PuzzlePiece_C'");
	UClass* BP_PuzzlePieceClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *Path.ToString()));

	for (int32 i = 0; i < Size * Size; ++i)
	{
		APuzzlePiece* Piece = World->SpawnActorDeferred<APuzzlePiece>(BP_PuzzlePieceClass, FTransform());
		if (Piece)
		{
			Piece->Init(Board, i, Sprites[i]);
			Piece->FinishSpawning(FTransform());
		}

		if (Piece)
		{
			Board->AddPiece(Piece);
		}
	}

	APuzzlePiece* MainPiece = World->SpawnActorDeferred<APuzzlePiece>(BP_PuzzlePieceClass, FTransform());
	if (MainPiece)
	{
		MainPiece->Init(Board, 1000000, Sprites[Size * Size]);
		MainPiece->FinishSpawning(FTransform());
	}

	Board->SetMainPiece(MainPiece);
	Board->ShuffleBoard();
	if (Board->GetIsAI())
			Board->AStar();
}
