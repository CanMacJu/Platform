// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PortalGameInstance.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FPortalQualityDelegate, float)
/**
 * 
 */
UCLASS()
class TPS_API UPortalGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPortalGameInstance();

	void SetPortalQuality(float quality);
	float GetPortalQuality() const { return PortalQuality; };

private:

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void LoadMainMenu();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void LoadInGameMenu();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void OnInGameMenu();

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SetCompleteGame();

private:

	TSubclassOf<UUserWidget> MaineMenuClass;
	TSubclassOf<UUserWidget> InGameMenuClass;

	UPROPERTY()
	class UPortalMainMenu* MainMenu;
	UPROPERTY()
	class UPortalInGameMenu* InGameMenu;


	float PortalQuality;

public:

	FPortalQualityDelegate OnChangePortalQualityDelegate;

};
