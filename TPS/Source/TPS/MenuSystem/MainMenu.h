// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MenuInterface.h"

#include "MainMenu.generated.h"

/**
 * 
 */
USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUsername;

};


UCLASS()
class TPS_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()
	
public:
	UMainMenu();

	virtual bool Initialize() override;

	void SetServerList(TArray<FServerData> ServerDatas);
	
	void SelectIndex(uint32 Index);

	void RowsResetColor();

private:

	UFUNCTION()
	void OnClick_Quit();

	UFUNCTION()
	void OnClick_MainMenu_Host();

	UFUNCTION()
	void OnClick_MainMenu_Join();

	UFUNCTION()
	void OnClick_JoinMenu_Join();

	UFUNCTION()
	void OnClick_JoinMenu_Cancel();

	UFUNCTION()
	void OnClick_JoinMenu_Refresh();

	UFUNCTION()
	void OnClick_HostMenu_Host();

	UFUNCTION()
	void OnClick_HostMenu_Cancel();

private:

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;
	 
	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu;



	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Quit;



	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_MainMenu_Host;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_MainMenu_Join;
	

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_JoinMenu_Join;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_JoinMenu_Cancel;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_JoinMenu_Refresh;


	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* EditableTextBox_HostMenu_ServerName;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_HostMenu_Host;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_HostMenu_Cancel;

	/*UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* TextBox_JoinMenu_IPAdress;*/

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ScrollBox_JoinMenu_ServerList;


	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher_MenuSwitcher;


	TSubclassOf<UUserWidget> ServerRowClass;
	
	TOptional<uint32> SelectedIndex;

	TArray<class UServerRow*> Rows;

};
