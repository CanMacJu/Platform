// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalInGameMenu.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "PortalGameInstance.h"

bool UPortalInGameMenu::Initialize()
{
	bool success = Super::Initialize();
	if (success == false)
		return false;

	Btn_InGameMenu_MainMenu->OnClicked.AddDynamic(this, &UPortalInGameMenu::OnClick_InGameMenu_MainMenu);
	Btn_InGameMenu_Setting->OnClicked.AddDynamic(this, &UPortalInGameMenu::OnClick_InGameMenu_Setting);
	Btn_InGameMenu_Cancel->OnClicked.AddDynamic(this, &UPortalInGameMenu::OnClick_Cancel);

	Btn_Setting_Back->OnClicked.AddDynamic(this, &UPortalInGameMenu::OnClick_Setting_Back);

	Btn_Check_Correct->OnClicked.AddDynamic(this, &UPortalInGameMenu::OnClick_Check_Correct);
	Btn_Check_Cancel->OnClicked.AddDynamic(this, &UPortalInGameMenu::OnClick_Cancel);

	return true;
}

void UPortalInGameMenu::SetCompleteGame()
{
	TextBlock_InGameMenu_Title->SetText(FText::FromString(TEXT("Complete Level")));
	ClearMenu();
	OnInGameMenu();
}

void UPortalInGameMenu::OnInGameMenu()
{
	if (MenuWidgets.empty())
	{
		MenuWidgets.push(InGameMenu);
		Setup();
	}
	else
	{
		Cancel();
	}
}

void UPortalInGameMenu::Cancel()
{
	MenuWidgets.pop();
	if (MenuWidgets.empty())
	{
		Teardown();
		return;
	}

	UWidget* Widget = MenuWidgets.top();
	MenuSwitcher->SetActiveWidget(Widget);
}

void UPortalInGameMenu::Setup()
{
	this->AddToViewport();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr)
		return;

	this->bIsFocusable = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(this->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);

	FVector2D viewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(viewportSize);
	PlayerController->SetMouseLocation(viewportSize.X / 2, viewportSize.Y / 2);

	PlayerController->bShowMouseCursor = true;
}

void UPortalInGameMenu::Teardown()
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

void UPortalInGameMenu::ClearMenu()
{
	int32 stackSize = MenuWidgets.size();
	for (int32 i = 0; i < stackSize; ++i)
		Cancel();
}

void UPortalInGameMenu::OnClick_InGameMenu_MainMenu()
{
	CheckMenuType = eCheckMenuType::MAIN_MENU;
	TextBlock_Check_Title->SetText(FText::FromString(TEXT("Go To MainMenu?")));
	MenuSwitcher->SetActiveWidget(CheckMenu);
	MenuWidgets.push(CheckMenu);
}

void UPortalInGameMenu::OnClick_InGameMenu_Setting()
{
	float CurrentPortalQuality = GameInstance->GetPortalQuality();
	Slider_PortalQuality->SetValue(CurrentPortalQuality);

	MenuSwitcher->SetActiveWidget(SettingMenu);
	MenuWidgets.push(SettingMenu);
}

void UPortalInGameMenu::OnClick_Cancel()
{
	Cancel();
}

void UPortalInGameMenu::OnClick_Setting_Back()
{
	float portalQuality = Slider_PortalQuality->GetValue();
	GameInstance->SetPortalQuality(portalQuality);

	Cancel();
}

void UPortalInGameMenu::OnClick_Check_Correct()
{
	switch (CheckMenuType)
	{
	case eCheckMenuType::MAIN_MENU:
		ClearMenu();
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("PortalMainMenu")));
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Check 'CheckMenuType'"));
		break;
	}

}
