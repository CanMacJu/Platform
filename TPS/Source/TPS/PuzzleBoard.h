// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <vector>

#include "PuzzleBoard.generated.h"


UCLASS()
class TPS_API APuzzleBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APuzzleBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SelectPiece(int32 _SelectIndex);
	bool CanMove(int32 Index);
	bool CanSelect() { return !IsMovePiece; };

	void SetSpawn(int32 _Size, float _SwapSpeed, bool _IsAI);

	void AddPiece(class APuzzlePiece* Piece) { Pieces.Push(Piece); };

	void SetPieceLocation();
	void SetMainPiece(class APuzzlePiece* _MainPiece);

	void SetRandIndex(int32 randNum);
	void ShuffleBoard();

	bool GetIsAI() { return IsAI; };
	void AStar();
	bool IsReset;

private:
	
	enum DIR
	{
		UP = 0,
		RIGHT = 1,
		DOWN = 2,
		LEFT = 3,

		DIR_COUNT = 4
	};

	void Init();

	void RefreshBoard();
	void UpdatePieceData();
	bool CheckCorrect();

	void MovePiece(int32 From, int32 To);

	float GetPieceSize() { return 450.f / Size; };

	std::vector<int32> GetIndexData();
	int32 GetHeuristic(std::vector<int32> IndexDatas);
	bool GetIsCorrect(std::vector<int32> IndexDatas);
	int32 GetNextIndex(int32 Index, int32 DIR);

private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Board;

	std::vector<FVector> Dir =
	{
		FVector(-1.f, 0.f, 0.f),
		FVector(0.f, -1.f, 0.f),
		FVector( 1.f, 0.f, 0.f),
		FVector(0.f,  1.f, 0.f)
	};

	std::vector<int32> Path;
	bool IsAI;
	int32 ResetCount;

	bool IsMovePiece;
	int32 BlankIndex;
	int32 SelectIndex;
	
	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true"), Category = "PuzzleSetting")
	TArray<class APuzzlePiece*> Pieces;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true"), Category = "PuzzleSetting")
	class APuzzlePiece* MainPiece;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true"), Category = "PuzzleSetting")
	int32 Size = 1;

	UPROPERTY(EditInstanceOnly, meta = (AllowPrivateAccess = "true"), Category = "PuzzleSetting")
	float SwapSpeed = 500.f;
	
	UPROPERTY(VisibleAnywhere)
	class APuzzlePawn* Player;

	TArray<FVector> IndexLocation;
};
