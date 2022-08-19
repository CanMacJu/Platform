// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "TPSGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	NumberOfPlayers++;
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	NumberOfPlayers--;
	if (NumberOfPlayers < 2 && GetWorldTimerManager().IsTimerActive(StartGameTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(StartGameTimerHandle);
	}
}

void ALobbyGameMode::StartGame()
{
	auto GameInstance = Cast<UTPSGameInstance>(GetGameInstance());
	if (GameInstance == nullptr)return;

	auto World = GetWorld();
	if (World == nullptr) return;

	GetWorldTimerManager().ClearTimer(StartGameTimerHandle);
	GameInstance->StartSession();

	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Platforms/Maps/Game?listen");
	
}

void ALobbyGameMode::ReadyGame()
{
	if (NumberOfPlayers >= 2)
	{
		GetWorldTimerManager().SetTimer(StartGameTimerHandle, this, &ALobbyGameMode::StartGame, 10.f, false);
	}
}
