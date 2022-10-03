// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "MenuInterface.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"
#include "ServerSlot.h"
#include "ServerRow.h"
#include "OnlineSessionSettings.h"

UMainMenu::UMainMenu()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_ServerRow(TEXT("/Game/MenuSystem/WBP_ServerRow.WBP_ServerRow_C"));
	if (WBP_ServerRow.Succeeded())
	{
		ServerRowClass = WBP_ServerRow.Class;
	}
}

bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success)
		return false;

	Btn_Quit->OnClicked.AddDynamic(this, &UMainMenu::OnClick_Quit);
	Btn_MainMenu_Host->OnClicked.AddDynamic(this, &UMainMenu::OnClick_MainMenu_Host);
	Btn_MainMenu_Join->OnClicked.AddDynamic(this, &UMainMenu::OnClick_MainMenu_Join);
	Btn_JoinMenu_Join->OnClicked.AddDynamic(this, &UMainMenu::OnClick_JoinMenu_Join);
	Btn_JoinMenu_Cancel->OnClicked.AddDynamic(this, &UMainMenu::OnClick_JoinMenu_Cancel);
	Btn_JoinMenu_Refresh->OnClicked.AddDynamic(this, &UMainMenu::OnClick_JoinMenu_Refresh);
	Btn_HostMenu_Host->OnClicked.AddDynamic(this, &UMainMenu::OnClick_HostMenu_Host);
	Btn_HostMenu_Cancel->OnClicked.AddDynamic(this, &UMainMenu::OnClick_HostMenu_Cancel);

	Btn_MainMenu_Join->SetIsEnabled(false);

	return true;
}

void UMainMenu::OnClick_Quit()
{
	if (MenuInterface)
	{
		MenuInterface->Quit();
	}
}

void UMainMenu::OnClick_MainMenu_Host()
{
	if (HostMenu && WidgetSwitcher_MenuSwitcher)
	{
		WidgetSwitcher_MenuSwitcher->SetActiveWidget(HostMenu);
	}
}

void UMainMenu::OnClick_MainMenu_Join()
{
	if (MenuInterface && WidgetSwitcher_MenuSwitcher && ScrollBox_JoinMenu_ServerList && JoinMenu && Btn_JoinMenu_Refresh)
	{
		WidgetSwitcher_MenuSwitcher->SetActiveWidget(JoinMenu);

		Btn_JoinMenu_Refresh->SetVisibility(ESlateVisibility::HitTestInvisible);
		ScrollBox_JoinMenu_ServerList->ClearChildren();
		MenuInterface->FindSessions();
		SelectedIndex.Reset();

		UServerRow* SeverRow = CreateWidget<UServerRow>(this, ServerRowClass);
		if (SeverRow)
		{
			SeverRow->SetLoadingText();
			ScrollBox_JoinMenu_ServerList->AddChild(SeverRow);
		}
	}
}

void UMainMenu::OnClick_JoinMenu_Join()
{
	/*if (MenuInterface && TextBox_JoinMenu_IPAdress)
	{
		FString IPAdress = TextBox_JoinMenu_IPAdress->GetText().ToString();
		MenuInterface->Join(IPAdress);
	}*/
	if (MenuInterface && SelectedIndex.IsSet())
	{
		UE_LOG(LogTemp, Error, TEXT("SelectedIndex is %d"), SelectedIndex.GetValue());
		MenuInterface->Join(SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SelectedIndex is not set"));
	}
}

void UMainMenu::OnClick_JoinMenu_Cancel()
{
	if (WidgetSwitcher_MenuSwitcher && MainMenu)
	{
		WidgetSwitcher_MenuSwitcher->SetActiveWidget(MainMenu);
	}
}

void UMainMenu::OnClick_JoinMenu_Refresh()
{
	if (MenuInterface && ScrollBox_JoinMenu_ServerList && Btn_JoinMenu_Refresh)
	{
		Btn_JoinMenu_Refresh->SetVisibility(ESlateVisibility::HitTestInvisible);
		ScrollBox_JoinMenu_ServerList->ClearChildren();
		MenuInterface->FindSessions();
		SelectedIndex.Reset();

		UServerRow* SeverRow = CreateWidget<UServerRow>(this, ServerRowClass);
		if (SeverRow)
		{
			SeverRow->SetLoadingText();
			ScrollBox_JoinMenu_ServerList->AddChild(SeverRow);
		}
	}
}

void UMainMenu::OnClick_HostMenu_Host()
{
	if (MenuInterface && EditableTextBox_HostMenu_ServerName)
	{
		FString ServerName = EditableTextBox_HostMenu_ServerName->GetText().ToString();
		MenuInterface->Host(ServerName);
	}
}

void UMainMenu::OnClick_HostMenu_Cancel()
{
	if (WidgetSwitcher_MenuSwitcher && MainMenu)
	{
		WidgetSwitcher_MenuSwitcher->SetActiveWidget(MainMenu);
	}
}

void UMainMenu::SetServerList(TArray<FServerData> ServerDatas)
{
	if (ScrollBox_JoinMenu_ServerList && Btn_JoinMenu_Refresh)
	{
		Btn_JoinMenu_Refresh->SetVisibility(ESlateVisibility::Visible);
		ScrollBox_JoinMenu_ServerList->ClearChildren();
		Rows.Empty();

		uint32 Index = 0;
		for (const FServerData& ServerData : ServerDatas)
		{
			UServerRow* ServerRow = CreateWidget<UServerRow>(this, ServerRowClass);
			if (ServerRow)
			{
				ServerRow->Text_ServerName->SetText(FText::FromString(ServerData.Name));
				ServerRow->Text_OwnerName->SetText(FText::FromString(ServerData.HostUsername));
				FText Players = FText::FromString(FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers));
				ServerRow->Text_Players->SetText(Players);
				ServerRow->Setup(this, Index);
				++Index;

				Rows.Emplace(ServerRow);
				ScrollBox_JoinMenu_ServerList->AddChild(ServerRow);
			}
		}
	}
}

void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
}

void UMainMenu::RowsResetColor()
{
	for (auto Row : Rows)
	{
		Row->ResetColor();
	}
}
