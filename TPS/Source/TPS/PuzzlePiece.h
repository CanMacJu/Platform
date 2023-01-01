// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "PuzzlePiece.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API APuzzlePiece : public APaperSpriteActor
{
	GENERATED_BODY()
public:

	APuzzlePiece();

	void Init(class APuzzleBoard* Board, int32 Index, class UPaperSprite* Sprite);
	bool CheckIndex();

private:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Scene;

public:

	class APuzzleBoard* PuzzleBoard;

	UPROPERTY(VisibleInstanceOnly)
	int32 CurrentIndex;

	UPROPERTY(VisibleInstanceOnly)
	int32 CorrectIndex;

};
