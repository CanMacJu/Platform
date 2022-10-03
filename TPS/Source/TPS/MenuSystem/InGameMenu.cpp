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
	Btn_InGameMenu_Cancel->OnClicked.AddDynamic(this, &UInGameMenu::Cancel);
	Btn_ConfirmMenu_Confirm->OnClicked.AddDynamic(this, &UInGameMenu::LoadMainMenu);
	Btn_ConfirmMenu_Cancel->OnClicked.AddDynamic(this, &UInGameMenu::Cancel);
	Btn_CompleteMenu_Confirm->OnClicked.AddDynamic(this, &UInGameMenu::LoadMainMenu);
	Btn_CompleteMenu_Cancel->OnClicked.AddDynamic(this, &UInGameMenu::Cancel);

	IsActive = false;

	return true;
}

void UInGameMenu::NativeConstruct()
{
	Super::NativeConstruct();

}

void UInGameMenu::CallupInGameMenu()
{
	if (InGameMenu == nullptr)
		return;

	MenuWidgets.push(InGameMenu);
	if (WidgetSwitcher_MenuSwitcher && InGameMenu)
		WidgetSwitcher_MenuSwitcher->SetActiveWidget(InGameMenu);

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

void UInGameMenu::CancelInGameMenu()
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

void UInGameMenu::CompleteInGameMenu()
{
	if (WidgetSwitcher_MenuSwitcher && CompleteMenu)
	{
		if (IsActive)
		{
			int32 StackLength = MenuWidgets.size();
			for (int32 i = 0; i < StackLength; ++i)
			{
				this->Cancel();
			}
		}
		CallupInGameMenu();
		MenuWidgets.pop();
		MenuWidgets.push(CompleteMenu);
		WidgetSwitcher_MenuSwitcher->SetActiveWidget(CompleteMenu);
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

void UInGameMenu::LoadMainMenu()
{
	if (Btn_ConfirmMenu_Confirm && MenuInterface)
	{
		Teardown();
		MenuInterface->LoadMainMenu();
	}
}

bool UInGameMenu::GetIsActive()
{
	return IsActive;
}

void UInGameMenu::Cancel()
{
	MenuWidgets.pop();

	this->CancelInGameMenu();
}
