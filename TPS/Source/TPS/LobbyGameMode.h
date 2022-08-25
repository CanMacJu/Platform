// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ALobbyGameMode : public ATPSGameMode
{
	GENERATED_BODY()
	
protected:

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;

private:

	void StartGame();

public:

	void ReadyGame();

private:

	int32 NumberOfPlayers = 0;

	FTimerHandle StartGameTimerHandle;

	int32 minimumPlayerNum = 1;
};
