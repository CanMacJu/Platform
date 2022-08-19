// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

bool UInGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success)
		return false;

	Btn_InGameMenu_MainMenu->OnClicked.AddDynamic(this, &UInGameMenu::OnClick_InGameMenu_MainMenu);
	Btn_InGameMenu_Cancel->OnClicked.AddDynamic(this, &UInGameMenu::OnClick_InGameMenu_Cancel);
	Btn_ConfirmMenu_Confirm->OnClicked.AddDynamic(this, &UInGameMenu::OnClick_ConfirmMenu_Confirm);
	Btn_ConfirmMenu_Cancel->OnClicked.AddDynamic(this, &UInGameMenu::OnClick_ConfirmMenu_Cancel);

	IsActive = false;

	return true;
}

void UInGameMenu::NativeConstruct()
{
	Super::NativeConstruct();

}

void UInGameMenu::OnActive()
{
	if (InGameMenu == nullptr)
		return;
	MenuWidgets.push(InGameMenu);

	this->AddToViewport();

	auto PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr)
		return;

	this->bIsFocusable = true;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputModeData);

	PlayerController->bShowMouseCursor = true;
	IsActive = true;
}

void UInGameMenu::OnInactive()
{
	if (MenuWidgets.empty())
	{
		Teardown();
		IsActive = false;
	}
	else
	{
		auto MenuWidget = MenuWidgets.top();

		if (WidgetSwitcher_MenuSwitcher && MenuWidget)
		{
			WidgetSwitcher_MenuSwitcher->SetActiveWidget(MenuWidget);
		}
	}
}

void UInGameMenu::OnClick_InGameMenu_MainMenu()
{
	if (WidgetSwitcher_MenuSwitcher && ConfirmMenu)
	{
		MenuWidgets.push(ConfirmMenu);
		WidgetSwitcher_MenuSwitcher->SetActiveWidget(ConfirmMenu);
	}
}

void UInGameMenu::OnClick_InGameMenu_Cancel()
{
	Inactive();
}

void UInGameMenu::OnClick_ConfirmMenu_Confirm()
{
	if (Btn_ConfirmMenu_Confirm && MenuInterface)
	{
		Teardown();
		MenuInterface->LoadMainMenu();
	}
}

void UInGameMenu::OnClick_ConfirmMenu_Cancel()
{
	Inactive();
}

bool UInGameMenu::GetIsActive()
{
	return IsActive;
}

void UInGameMenu::Inactive()
{
	MenuWidgets.pop();

	this->OnInactive();
}
