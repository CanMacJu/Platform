// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <stack>
#include "PortalInGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API UPortalInGameMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual bool Initialize() override;

	void SetGameInstance(class UPortalGameInstance* gameInstance) { GameInstance = gameInstance; };
	void SetCompleteGame();

	void OnInGameMenu();
	void Cancel();

	void Setup();
	void Teardown();

	void ClearMenu();

private:

	UFUNCTION()
	void OnClick_InGameMenu_MainMenu();
	UFUNCTION()
	void OnClick_InGameMenu_Setting();
	UFUNCTION()
	void OnClick_Cancel();

	UFUNCTION()
	void OnClick_Setting_Back();

	UFUNCTION()
	void OnClick_Check_Correct();


private:

	enum class eCheckMenuType
	{
		MAIN_MENU,

	};

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* InGameMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* SettingMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* CheckMenu;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_InGameMenu_Title;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_InGameMenu_MainMenu;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_InGameMenu_Setting;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_InGameMenu_Cancel;

	UPROPERTY(meta = (BindWidget))
	class USlider* Slider_PortalQuality;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Setting_Back;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock_Check_Title;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Check_Correct;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Check_Cancel;

	class UPortalGameInstance* GameInstance;

	std::stack<class UWidget*> MenuWidgets;

	eCheckMenuType CheckMenuType;
};
