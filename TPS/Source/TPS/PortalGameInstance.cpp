// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalGameInstance.h"
#include "PortalMainMenu.h"
#include "PortalInGameMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Portal.h"

UPortalGameInstance::UPortalGameInstance()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_PortalMainMenu(TEXT("WidgetBlueprint'/Game/Platforms/Menu/WBP_PortalMainMenu.WBP_PortalMainMenu_C'"));
	if (WBP_PortalMainMenu.Succeeded())
		MaineMenuClass = WBP_PortalMainMenu.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_PortalInGameMenu(TEXT("WidgetBlueprint'/Game/Platforms/Menu/WBP_PortalInGameMenu.WBP_PortalInGameMenu_C'"));
	if (WBP_PortalInGameMenu.Succeeded())
		InGameMenuClass = WBP_PortalInGameMenu.Class;

	PortalQuality = 0.4f;
}

void UPortalGameInstance::SetPortalQuality(float quality)
{
	PortalQuality = quality;
	OnChangePortalQualityDelegate.Broadcast(PortalQuality);
}

void UPortalGameInstance::LoadMainMenu()
{
	if (MaineMenuClass)
	{
		MainMenu = CreateWidget<UPortalMainMenu>(this, MaineMenuClass);
		if (MainMenu == nullptr)
			return;

		MainMenu->Setup();

		MainMenu->SetGameInstance(this);
	}
}

void UPortalGameInstance::LoadInGameMenu()
{
	if (InGameMenuClass)
	{
		InGameMenu = CreateWidget<UPortalInGameMenu>(this, InGameMenuClass);
		if (InGameMenu == nullptr)
			return;

		InGameMenu->SetGameInstance(this);
	}
}

void UPortalGameInstance::OnInGameMenu()
{
	if (InGameMenu)
		InGameMenu->OnInGameMenu();
}

void UPortalGameInstance::SetCompleteGame()
{
	if (InGameMenu)
		InGameMenu->SetCompleteGame();
}

