// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuInterface.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "TPSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UTPSGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()
	
public:

	UTPSGameInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void LoadMenuWidget();

	UFUNCTION(BlueprintCallable)
	void LoadInGameMenu();

	UFUNCTION(BlueprintCallable)
	void OnInGameMenu();


	virtual void Host(const FString& InputServerName) override;

	virtual void Join(uint32 Index) override;

	virtual void LoadMainMenu() override;

	virtual void Quit() override;

	virtual void FindSessions() override;

private:

	void OnCreateSessionComplete(FName SessionName, bool Success);

	void OnDestroySessionComplete(FName SessionName, bool Success);

	void OnFindSessionsComplete(bool Success);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString = TEXT(""));

	void CreateSession();

public:

	void StartSession();

private:

	TSubclassOf<class UUserWidget> MenuClass;

	UPROPERTY()
	class UMainMenu* Menu;


	TSubclassOf<class UUserWidget> InGameMenuClass;

	UPROPERTY()
	class UInGameMenu* InGameMenu;

	class IOnlineSubsystem* OnlineSubsystem;

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	FString ServerName;
};
