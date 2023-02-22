// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalMainMenu.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Kismet/GameplayStatics.h"
#include "PortalGameInstance.h"

bool UPortalMainMenu::Initialize()
{
	bool success = Super::Initialize();
	if (success == false)
		return false;

	Btn_MainMenu_Start->OnClicked.AddDynamic(this, &UPortalMainMenu::OnClick_Start);
	Btn_MainMenu_Setting->OnClicked.AddDynamic(this, &UPortalMainMenu::OnClick_InGameMenu_Setting);
	Btn_MainMenu_Quit->OnClicked.AddDynamic(this, &UPortalMainMenu::OnClick_Quit);

	Btn_Setting_Back->OnClicked.AddDynamic(this, &UPortalMainMenu::OnClick_Setting_Back);

	return true;
}

void UPortalMainMenu::Setup()
{
	this->AddToViewport();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr)
		return;

	this->bIsFocusable = true;

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputModeData);

	/*FVector2D viewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(viewportSize);
	PlayerController->SetMouseLocation(viewportSize.X / 2, viewportSize.Y / 2);*/

	PlayerController->bShowMouseCursor = true;

}

void UPortalMainMenu::Teardown()
{
	this->RemoveFromParent();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr)
		return;

	this->bIsFocusable = false;

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);

	PlayerController->bShowMouseCursor = false;
}

void UPortalMainMenu::OnClick_Start()
{
	if (Btn_MainMenu_Start)
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Level1")));

	Teardown();
}

void UPortalMainMenu::OnClick_InGameMenu_Setting()
{
	if (MenuSwitcher && SettingMenu && Slider_PortalQuality && GameInstance)
	{
		float CurrentPortalQuality = GameInstance->GetPortalQuality();
		Slider_PortalQuality->SetValue(CurrentPortalQuality);

		MenuSwitcher->SetActiveWidget(SettingMenu);
	}

	
}

void UPortalMainMenu::OnClick_Quit()
{
	if (Btn_MainMenu_Quit)
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController == nullptr)
			return;

		PlayerController->ConsoleCommand("Quit");
	}
}

void UPortalMainMenu::OnClick_Setting_Back()
{
	if (MenuSwitcher && MainMenu && Slider_PortalQuality && GameInstance)
	{
		float protalQuality = Slider_PortalQuality->GetValue();
		GameInstance->SetPortalQuality(protalQuality);

		MenuSwitcher->SetActiveWidget(MainMenu);
	}
}

