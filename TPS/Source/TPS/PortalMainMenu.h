// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PortalMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UPortalMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual bool Initialize() override;

	void Setup();
	void Teardown();

	void SetGameInstance(class UPortalGameInstance* gameInstance) { GameInstance = gameInstance; };


private:
	UFUNCTION()
	void OnClick_Start();
	UFUNCTION()
	void OnClick_InGameMenu_Setting();
	UFUNCTION()
	void OnClick_Quit();

	UFUNCTION()
	void OnClick_Setting_Back();


private:

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;


	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* SettingMenu;


	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_MainMenu_Start;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_MainMenu_Setting;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_MainMenu_Quit;

	UPROPERTY(meta = (BindWidget))
	class USlider* Slider_PortalQuality;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Setting_Back;



public:

	class UPortalGameInstance* GameInstance;

};
